// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetRecipes.h"

#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

class QuickTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void buttonMatchesTheWidgetsRecipe() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetButton { text: \"Save\"; variant: KetTheme.ButtonVariant.Primary }\n"));
        auto* button = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(button);
        const ButtonRecipe recipe =
            buttonRecipe(ButtonVariant::Primary, ControlSize::Medium, {}, Theme::Light);
        QCOMPARE(button->implicitHeight(), recipe.height);
        auto* background = button->property("background").value<QQuickItem*>();
        QVERIFY(background);
        QCOMPARE(background->property("color").value<QColor>(), recipe.background);
        QCOMPARE(background->property("radius").toReal(), recipe.radius);
    }

    void buttonFollowsThemeAndZoom() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetButton { text: \"Cancel\"; controlSize: KetTheme.ControlSize.Small }\n"));
        auto* button = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(button);
        QCOMPARE(button->implicitHeight(), 28.0);
        Appearance::instance().setMode(Appearance::Mode::Dark);
        auto* background = button->property("background").value<QQuickItem*>();
        QCOMPARE(background->property("color").value<QColor>(),
                 Tokens::color(u"interactive-bg", Theme::Dark));
        Appearance::instance().setZoom(1.25);
        QCOMPARE(button->implicitHeight(), 35.0);
    }

    void badgeUsesToneTokens() {
        std::unique_ptr<QObject> root = create(
            QByteArrayLiteral("import QtQuick\nimport Ket\n"
                              "KetBadge { text: \"Bug\"; tone: KetTheme.BadgeTone.Danger }\n"));
        auto* badge = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(badge);
        QCOMPARE(badge->property("color").value<QColor>(),
                 Tokens::color(u"danger-bg", Theme::Light));
        QCOMPARE(badge->implicitHeight(), Tokens::px(u"line-sm"));
    }

    void themeExposesTokenMap() {
        std::unique_ptr<QObject> root =
            create(QByteArrayLiteral("import QtQuick\nimport Ket\n"
                                     "Item { property color page: KetTheme.t.pageBg; property "
                                     "string source: KetTheme.source }\n"));
        QVERIFY(root);
        QCOMPARE(root->property("page").value<QColor>(), Tokens::color(u"page-bg", Theme::Light));
        QVERIFY(root->property("source").toString().contains(QStringLiteral("0.1.44")));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(root->property("page").value<QColor>(), Tokens::color(u"page-bg", Theme::Dark));
    }

  private:
    std::unique_ptr<QObject> create(const QByteArray& source) {
        QQmlComponent component(&engine_);
        component.setData(source, QUrl()); // a local URL loads imports synchronously
        if (component.isError())
            qWarning().noquote() << component.errorString();
        return std::unique_ptr<QObject>(component.create());
    }

    QQmlEngine engine_;
};

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    QuickTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_quick.moc"
