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
        QCOMPARE(badge->implicitHeight(), Tokens::px(u"line-sm") + 2);
    }

    void iconButtonMatchesTheWidgetsRecipe() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetIconButton { text: \"Close\"; previewState: ({ hovered: true }) }\n"));
        auto* button = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(button);
        const ButtonRecipe recipe =
            iconButtonRecipe(ControlSize::Medium, {true, false, true, false}, Theme::Light);
        QCOMPARE(button->implicitWidth(), recipe.height);
        QCOMPARE(button->implicitHeight(), recipe.height);
        auto* background = button->property("background").value<QQuickItem*>();
        QCOMPARE(background->property("color").value<QColor>(), recipe.background);
        auto* icon = button->property("contentItem").value<QQuickItem*>();
        QCOMPARE(icon->property("color").value<QColor>(), recipe.text);
    }

    void tagMatchesTheWidgetsRecipe() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetTag { text: \"A very long tag label that overflows\"; tagColor: \"#2f6fde\" }\n"));
        auto* tag = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(tag);
        const TagRecipe recipe = tagRecipe(QColor(0x2f6fde), false, false, Theme::Light);
        QCOMPARE(tag->implicitHeight(), recipe.height);
        QCOMPARE(tag->implicitWidth(), recipe.maxWidth);
        QCOMPARE(tag->property("color").value<QColor>(), recipe.background);
        tag->setProperty("previewHovered", true);
        QCOMPARE(tag->property("color").value<QColor>(),
                 tagRecipe(QColor(0x2f6fde), false, true, Theme::Light).background);
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
