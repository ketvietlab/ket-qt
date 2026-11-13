// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetRecipes.h"
#include "KetStyle.h"
#include "KetTokens.h"

#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QSignalSpy>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>
#include <atomic>
#include <thread>

Q_IMPORT_QML_PLUGIN(KetPlugin)
using namespace ket;

namespace {
class PaintCounter final : public QObject {
  public:
    int count{0};

  protected:
    bool eventFilter(QObject*, QEvent* event) override {
        if (event->type() == QEvent::Paint)
            ++count;
        return false;
    }
};
} // namespace

class OverridesTest final : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase() { Style::install(*qobject_cast<QApplication*>(qApp)); }
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().clearColorOverrides(Theme::Light);
        Appearance::instance().clearColorOverrides(Theme::Dark);
    }
    void cleanup() {
        Appearance::instance().clearColorOverrides(Theme::Light);
        Appearance::instance().clearColorOverrides(Theme::Dark);
    }
    void defaultAndClearMatchEveryPinnedToken() {
        QFile file(QFINDTESTDATA("../tokens/ket-tokens.json"));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto tokens =
            QJsonDocument::fromJson(file.readAll()).object().value("tokens").toObject();
        QCOMPARE(Tokens::names().size(), tokens.size());
        for (const auto theme : {Theme::Light, Theme::Dark}) {
            const auto compareDefaults = [&] {
                const auto map = Tokens::map(theme);
                for (auto it = tokens.begin(); it != tokens.end(); ++it) {
                    const auto entry = it.value().toObject();
                    const auto value =
                        entry.value(theme == Theme::Light ? "light" : "dark").toString();
                    QCOMPARE(Tokens::raw(it.key(), theme), value);
                    if (entry.value("kind").toString() == QStringLiteral("Color")) {
                        QCOMPARE(Tokens::color(it.key(), theme), QColor::fromString(value));
                        QCOMPARE(map.value(Tokens::camelCase(it.key())).value<QColor>(),
                                 QColor::fromString(value));
                    }
                }
            };
            compareDefaults();
            const auto rejected = Appearance::instance().setColorOverrides(
                theme, {{"page-bg", QColor(1, 2, 3, 4)}, {"accent", QColor(4, 5, 6)}});
            QVERIFY(rejected.isEmpty());
            Appearance::instance().clearColorOverrides(theme);
            compareDefaults();
        }
    }
    void validatesAndReplacesOneBranch() {
        const auto names = Tokens::names();
        const auto dark = Tokens::color(u"page-bg", Theme::Dark);
        const auto accent = Tokens::color(u"accent", Theme::Light);
        QSignalSpy changed(&Appearance::instance(), &Appearance::themeChanged);
        const QColor alpha(10, 20, 30, 40);
        const auto rejected = Appearance::instance().setColorOverrides(
            Theme::Light, {{"page-bg", alpha},
                           {"accent", QColor(1, 2, 3)},
                           {"invented-token", QColor(4, 5, 6)},
                           {"control-height-md", QColor(7, 8, 9)},
                           {"text-main", QColor()}});
        QCOMPARE(changed.count(), 1);
        QCOMPARE(rejected, QStringList({"control-height-md", "invented-token", "text-main"}));
        QCOMPARE(Tokens::names(), names);
        QVERIFY(!Tokens::contains(u"invented-token"));
        QCOMPARE(Tokens::px(u"control-height-md"), 32.0);
        QCOMPARE(Tokens::color(u"page-bg", Theme::Light), alpha);
        QCOMPARE(Tokens::color(u"page-bg", Theme::Dark), dark);
        QCOMPARE(QColor::fromString(Tokens::raw(u"page-bg", Theme::Light)), alpha);
        QCOMPARE(Tokens::map(Theme::Light).value("pageBg").value<QColor>(), alpha);
        const auto next =
            Appearance::instance().setColorOverrides(Theme::Light, {{"page-bg", alpha}});
        QVERIFY(next.isEmpty());
        QCOMPARE(changed.count(), 2);
        QCOMPARE(Tokens::color(u"accent", Theme::Light), accent); // replaced, not merged
        Appearance::instance().clearColorOverrides(Theme::Light);
        QCOMPARE(changed.count(), 3);
        Appearance::instance().clearColorOverrides(
            Theme::Light); // explicit no-op still one publication
        QCOMPARE(changed.count(), 4);
        const auto inactive =
            Appearance::instance().setColorOverrides(Theme::Dark, {{"page-bg", alpha}});
        QVERIFY(inactive.isEmpty());
        QCOMPARE(changed.count(), 5);
        QCOMPARE(changed.last().first().value<Theme>(), Theme::Light);
    }
    void qmlBindingsUpdateWithoutRecreation() {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData(R"(
            import QtQuick
            import Ket
            Item {
                id: root
                property color fill: KetTheme.t.pageBg
                property color buttonFill: KetTheme.buttonRecipe(KetTheme.dark, KetTheme.zoom, 0, 1, false, false, true, false).background
                property int changes: 0
                property string preservedState: "kept"
                Connections { target: KetTheme; function onThemeChanged() { ++root.changes } }
            }
        )",
                          QUrl("qrc:/overrides-test.qml"));
        QScopedPointer<QObject> object(component.create());
        QVERIFY2(object, qPrintable(component.errorString()));
        const auto* identity = object.data();
        const QColor page(15, 25, 35), button(45, 55, 65);
        const auto rejected = Appearance::instance().setColorOverrides(
            Theme::Light, {{"page-bg", page}, {"action-primary-bg", button}});
        QVERIFY(rejected.isEmpty());
        QCOMPARE(object->property("fill").value<QColor>(), page);
        QCOMPARE(object->property("buttonFill").value<QColor>(), button);
        QCOMPARE(object->property("changes").toInt(), 1);
        QCOMPARE(object.data(), identity);
        QCOMPARE(object->property("preservedState").toString(), QStringLiteral("kept"));
        Appearance::instance().clearColorOverrides(Theme::Light);
        QCOMPARE(object->property("fill").value<QColor>(), Tokens::color(u"page-bg", Theme::Light));
        QCOMPARE(object->property("changes").toInt(), 2);
    }
    void widgetsRepaintAndPaletteUpdates() {
        Button button(QStringLiteral("Live"), ButtonVariant::Primary);
        button.resize(button.sizeHint());
        button.show();
        QTest::qWait(20);
        const auto before = button.grab().toImage();
        const auto* identity = &button;
        PaintCounter paints;
        button.installEventFilter(&paints);
        const QColor fill(12, 34, 56), page(65, 43, 21);
        const auto rejected = Appearance::instance().setColorOverrides(
            Theme::Light, {{"action-primary-bg", fill}, {"page-bg", page}});
        QVERIFY(rejected.isEmpty());
        QTRY_VERIFY(paints.count > 0);
        const auto after = button.grab().toImage();
        QVERIFY(before != after);
        QCOMPARE(&button, identity);
        QCOMPARE(
            buttonRecipe(ButtonVariant::Primary, ControlSize::Medium, {}, Theme::Light).background,
            fill);
        QCOMPARE(qApp->palette().color(QPalette::Window), page);
        QCOMPARE(button.text(), QStringLiteral("Live"));
    }
    void concurrentReadersSeeValidColors() {
        const QColor first(1, 2, 3, 4), second(5, 6, 7, 8);
        const auto initial =
            Appearance::instance().setColorOverrides(Theme::Light, {{"page-bg", first}});
        QVERIFY(initial.isEmpty());
        std::atomic<bool> done{false}, valid{true};
        std::thread reader([&] {
            while (!done.load()) {
                const auto color = Tokens::color(u"page-bg", Theme::Light);
                if (color != first && color != second)
                    valid = false;
            }
        });
        for (int i = 0; i < 100; ++i) {
            const auto rejected = Appearance::instance().setColorOverrides(
                Theme::Light, {{"page-bg", i % 2 ? first : second}});
            if (!rejected.isEmpty())
                valid = false;
        }
        done = true;
        reader.join();
        QVERIFY(valid.load());
    }
};
QTEST_MAIN(OverridesTest)
#include "tst_color_overrides.moc"
