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

    void ketIconPicksTheMaster() {
        std::unique_ptr<QObject> root = create(
            QByteArrayLiteral("import QtQuick\nimport Ket\n"
                              "KetIcon { name: KetTheme.iconForLucide(\"x\"); size: 17 }\n"));
        auto* icon = qobject_cast<QQuickItem*>(root.get());
        QVERIFY(icon);
        QCOMPARE(icon->implicitWidth(), 17.0);
        QCOMPARE(icon->property("drawSize").toReal(), 16.0);
        QCOMPARE(icon->property("name").toString(), QStringLiteral("close"));
        auto* image = icon->childItems().value(0);
        QVERIFY(image);
        QCOMPARE(image->property("source").toUrl(),
                 QUrl(QStringLiteral("qrc:/ket-symbol/light/16/close.svg")));
        QCOMPARE(image->property("color").value<QColor>(),
                 Tokens::color(u"text-main", Theme::Light));
        Appearance::instance().setZoom(1.25); // 21 px box: the 20 px master
        QCOMPARE(image->property("source").toUrl(),
                 QUrl(QStringLiteral("qrc:/ket-symbol/light/20/close.svg")));
    }

    void formControlsMatchTheWidgetsRecipes() {
        std::unique_ptr<QObject> field =
            create(QByteArrayLiteral("import QtQuick\nimport Ket\n"
                                     "KetTextField { leadingIcon: \"search\"; invalid: true }\n"));
        auto* fieldItem = qobject_cast<QQuickItem*>(field.get());
        QVERIFY(fieldItem);
        const FieldRecipe fieldR = fieldRecipe({}, true, Theme::Light);
        QCOMPARE(fieldItem->implicitHeight(), fieldR.height);
        QCOMPARE(fieldItem->property("leftPadding").toReal(),
                 fieldR.paddingInline + fieldR.iconSize + fieldR.spacing);
        auto* background = fieldItem->property("background").value<QQuickItem*>();
        QCOMPARE(
            background->property("border").value<QObject*>()->property("color").value<QColor>(),
            fieldR.border);

        std::unique_ptr<QObject> check = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\nKetCheckbox { text: \"Follow\"; checked: true }\n"));
        auto* checkItem = qobject_cast<QQuickItem*>(check.get());
        QVERIFY(checkItem);
        auto* indicator = checkItem->property("indicator").value<QQuickItem*>();
        QCOMPARE(indicator->property("color").value<QColor>(),
                 toggleRecipe(ToggleKind::Checkbox, true, {}, Theme::Light).background);

        std::unique_ptr<QObject> choice = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetChoice { text: \"Read only\"; kind: KetTheme.ToggleKind.Radio; checked: true }\n"));
        auto* choiceItem = qobject_cast<QQuickItem*>(choice.get());
        QVERIFY(choiceItem);
        auto* card = choiceItem->property("background").value<QQuickItem*>();
        QCOMPARE(card->property("color").value<QColor>(),
                 choiceRecipe(true, {}, Theme::Light).background);

        std::unique_ptr<QObject> row = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "KetFieldRow { label: \"Font\"; help: \"Help\"; KetSelect { model: [\"A\"] } }\n"));
        QVERIFY(qobject_cast<QQuickItem*>(row.get()));
        QVERIFY(row->property("implicitWidth").toReal() > fieldRowRecipe(Theme::Light).labelWidth);
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
    void overlaysLoadAndDrawTheirRecipes() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { width: 900; height: 700\n"
            "  property alias banner: banner; property alias toast: toast\n"
            "  property alias dialog: dialog; property alias menu: menu; property alias tip: tip\n"
            "  KetBanner { id: banner; width: 400; tone: KetTheme.BannerTone.Danger;"
            "              title: \"Sync failed\"; actionText: \"Retry\" }\n"
            "  KetToast { id: toast; text: \"Saved\" }\n"
            "  KetDialog { id: dialog; title: \"Rename\"\n"
            "    KetTextField {}\n"
            "    actions: [ KetButton { text: \"Save\" } ] }\n"
            "  KetMenu { id: menu\n"
            "    KetMenuItem { text: \"Open\" }\n"
            "    KetMenuItem { text: \"Delete\"; danger: true }\n"
            "    KetMenuSeparator {}\n"
            "    KetMenuItem { text: \"Pin\"; checkable: true; description: \"Keep it on top.\" } "
            "}\n"
            "  KetToolTip { id: tip; text: \"Copy\" }\n"
            "}\n"));
        QVERIFY(root);
        auto* banner = root->property("banner").value<QQuickItem*>();
        QCOMPARE(banner->property("color").value<QColor>(),
                 Tokens::color(u"danger-bg", Theme::Light));
        QCOMPARE(banner->property("radius").toReal(), 6.0);

        QObject* dialog = root->property("dialog").value<QObject*>();
        QCOMPARE(dialog->property("width").toReal(), 500.0);
        QCOMPARE(dialog->property("modal").toBool(), true);

        QObject* menu = root->property("menu").value<QObject*>();
        QVERIFY(menu->property("implicitWidth").toReal() >= 210);
        QCOMPARE(menu->property("hasCheckableItems").toBool(), true);

        QObject* tip = root->property("tip").value<QObject*>();
        auto* tipBackground = tip->property("background").value<QQuickItem*>();
        QCOMPARE(tipBackground->property("color").value<QColor>(),
                 Tokens::color(u"accent", Theme::Light));

        Appearance::instance().setZoom(1.25);
        QCOMPARE(dialog->property("width").toReal(), 625.0);
    }

    void navigationAndStatusLoad() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { width: 800; height: 600\n"
            "  property alias nav: nav; property alias segments: segments\n"
            "  property alias tabs: tabs; property alias progress: progress\n"
            "  property alias activity: activity; property alias empty: empty\n"
            "  KetNavItem { id: nav; text: \"Files\"; iconName: \"folder\"; count: 3;"
            "               checked: true }\n"
            "  KetKbd { text: \"⌘K\" }\n"
            "  KetSegmentedControl { id: segments; options: [\"Claude\", \"Codex\"] }\n"
            "  KetTabBar { id: tabs\n"
            "    KetTabButton { text: \"README.md\"; iconName: \"file\"; closable: true }\n"
            "    KetTabButton { text: \"main.cpp\" } }\n"
            "  KetProgressBar { id: progress; value: 0.4 }\n"
            "  KetActivityIndicator { id: activity }\n"
            "  KetEmptyState { id: empty; iconName: \"success\"; title: \"Clean\";"
            "                  message: \"Nothing to commit.\"; actionText: \"Back\" }\n"
            "}\n"));
        QVERIFY(root);
        auto* nav = root->property("nav").value<QQuickItem*>();
        QCOMPARE(nav->implicitHeight(), 34.0);
        auto* navBackground = nav->property("background").value<QQuickItem*>();
        QCOMPARE(navBackground->property("color").value<QColor>(),
                 Tokens::color(u"accent-subtle", Theme::Light));
        auto* segments = root->property("segments").value<QQuickItem*>();
        QMetaObject::invokeMethod(segments, "select", Q_ARG(QVariant, 1));
        QCOMPARE(segments->property("currentIndex").toInt(), 1);
        QCOMPARE(root->property("tabs").value<QQuickItem*>()->implicitHeight(), 41.0);
        QCOMPARE(root->property("progress").value<QQuickItem*>()->implicitHeight(), 6.0);
        auto* activity = root->property("activity").value<QQuickItem*>();
        QCOMPARE(activity->implicitWidth(), 15.0);
        QVERIFY(root->property("empty").value<QQuickItem*>()->implicitHeight() > 35);
    }

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
