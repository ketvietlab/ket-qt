// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetConversationRecipes.h"
#include "KetRecipes.h"

#include <QDirIterator>
#include <QFile>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QSignalSpy>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>
#include <cmath>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

// QML warnings fail the test that caused them (cleanup() checks the count).
int qmlWarnings = 0;
QtMessageHandler previousHandler = nullptr;

void countQmlWarnings(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    if (type != QtDebugMsg && type != QtInfoMsg &&
        (message.contains(QLatin1String("qrc:/")) || message.contains(QLatin1String(".qml:"))))
        ++qmlWarnings;
    previousHandler(type, context, message);
}

} // namespace

class QuickTest final : public QObject {
    Q_OBJECT
  private slots:
    // The attached Basic.ToolTip ignores the kit's recipe and shows Qt's black box.
    void componentsUseKetToolTip() {
        int files = 0;
        QDirIterator it(QStringLiteral(":/qt/qml/Ket"), {QStringLiteral("*.qml")}, QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QFile file(it.next());
            QVERIFY(file.open(QIODevice::ReadOnly));
            ++files;
            QVERIFY2(!file.readAll().contains("ToolTip.visible"), qPrintable(file.fileName()));
        }
        QVERIFY(files > 50);
    }

    void initTestCase() { previousHandler = qInstallMessageHandler(countQmlWarnings); }

    void cleanup() {
        const int warnings = std::exchange(qmlWarnings, 0);
        QCOMPARE(warnings, 0);
    }

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
        QCOMPARE(button->property("glyphSize").toReal(), recipe.iconSize);
        button->setProperty("glyphSize", 20);
        QCOMPARE(button->implicitWidth(), recipe.height);
        button->setProperty("glyphSize", recipe.iconSize);
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

    void lockedTagMatchesTheWidgetsRecipe() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { property alias locked: locked; property alias plain: plain\n"
            "  property alias row: row\n"
            "  KetTag { id: plain; text: \"Leader\"; tagColor: \"#2f6fde\" }\n"
            "  KetTag { id: locked; text: \"Leader\"; tagColor: \"#2f6fde\"; locked: true;\n"
            "    filterable: true }\n"
            "  KetConversationRow { id: row; width: 280; text: \"Cart\"\n"
            "    tags: [{ label: \"Developer\", color: \"#2f6fde\", locked: true }] }\n"
            "}\n"));
        QVERIFY(root);
        auto* locked = root->property("locked").value<QQuickItem*>();
        auto* plain = root->property("plain").value<QQuickItem*>();
        const TagRecipe recipe = lockedTagRecipe(QColor(0x2f6fde), Theme::Light);
        QCOMPARE(locked->property("color").value<QColor>(), recipe.background);
        QCOMPARE(locked->implicitHeight(), recipe.height);
        QCOMPARE(locked->implicitWidth(),
                 plain->implicitWidth() + recipe.iconSize + recipe.iconGap);
        auto* row = root->property("row").value<QQuickItem*>();
        QCOMPARE(row->property("visibleTagCount").toInt(), 1);
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

    void overlaysLoadAndDrawTheirRecipes() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { width: 900; height: 700\n"
            "  property alias banner: banner; property alias toast: toast\n"
            "  property alias dialog: dialog; property alias menu: menu; property alias tip: tip\n"
            "  property alias section: section; property alias full: full\n"
            "  KetBanner { id: banner; width: 400; tone: KetTheme.BannerTone.Danger;"
            "              title: \"Sync failed\"; actionText: \"Retry\" }\n"
            "  KetToast { id: toast; text: \"Saved\" }\n"
            "  KetDialog { id: dialog; title: \"Rename\"\n"
            "    KetTextField {}\n"
            "    actions: [ KetButton { text: \"Save\" } ] }\n"
            "  KetMenu { id: menu\n"
            "    KetMenuSection { id: section; text: \"Mode\" }\n"
            "    KetMenuItem { text: \"Open\" }\n"
            "    KetMenuItem { id: full; text: \"Full access\"; warning: true }\n"
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
        auto* section = root->property("section").value<QQuickItem*>();
        QCOMPARE(section->implicitHeight(), 24.0);
        QObject* full = root->property("full").value<QObject*>();
        QCOMPARE(full->property("recipe").toMap().value("text").value<QColor>(),
                 Tokens::color(u"warning", Theme::Light));

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
        // Overflowing tabs scroll inside the bar instead of drawing over the tools beside it.
        auto* tabList = root->property("tabs")
                            .value<QQuickItem*>()
                            ->property("contentItem")
                            .value<QQuickItem*>();
        QVERIFY(tabList && tabList->clip());
        QCOMPARE(root->property("progress").value<QQuickItem*>()->implicitHeight(), 6.0);
        auto* activity = root->property("activity").value<QQuickItem*>();
        QCOMPARE(activity->implicitWidth(), 15.0);
        QVERIFY(root->property("empty").value<QQuickItem*>()->implicitHeight() > 35);
    }

    // In a pane narrower than the recipe's message width, the message wraps inside the
    // pane like the title does, instead of drawing past both edges.
    void emptyStateMessageFitsANarrowPane() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { width: 800; height: 600\n"
            "  property alias empty: empty; property alias natural: natural\n"
            "  KetEmptyState { id: empty; width: 200; iconName: \"search\"; title: \"No results\";"
            "    message: \"Nothing in ketplus-desktop matches this query. Check the spelling, "
            "or search for a shorter word.\" }\n"
            "  KetEmptyState { id: natural; iconName: \"search\"; title: \"No results\";"
            "    message: \"Nothing in ketplus-desktop matches this query. Check the spelling, "
            "or search for a shorter word.\"; actionText: \"Clear search\" }\n"
            "}\n"));
        QVERIFY(root);
        const auto messageOf = [](QQuickItem* state) -> QQuickItem* {
            for (auto* item : state->findChildren<QQuickItem*>())
                if (item->property("text").toString().startsWith(u"Nothing in"))
                    return item;
            return nullptr;
        };
        auto* empty = root->property("empty").value<QQuickItem*>();
        QQuickItem* message = messageOf(empty);
        QVERIFY(message);
        QVERIFY2(message->width() <= empty->width(), qPrintable(QString::number(message->width())));
        QVERIFY(message->property("lineCount").toInt() > 1);
        // It uses the pane's width, not just the width of the title or the action.
        QVERIFY2(message->width() > empty->width() * 0.8,
                 qPrintable(QString::number(message->width())));

        // Without a width, the state keeps its natural size: the message at the recipe's
        // maximum width, not squeezed to the width of the title or the action.
        auto* natural = root->property("natural").value<QQuickItem*>();
        QQuickItem* naturalMessage = messageOf(natural);
        QVERIFY(naturalMessage);
        const qreal maxWidth =
            natural->property("recipe").toMap().value("messageMaxWidth").toReal();
        QVERIFY(maxWidth > 200);
        QCOMPARE(natural->width(), maxWidth);
        QCOMPARE(naturalMessage->width(), maxWidth);
        QVERIFY(naturalMessage->property("lineCount").toInt() <= 3);
    }

    void conversationBlocksLoad() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Item { width: 800; height: 600\n"
            "  property alias row: row; property alias composer: composer\n"
            "  property alias user: user; property alias step: step\n"
            "  KetConversationRow { id: row; width: 320; text: \"Navigation\"; number: \"4821\"\n"
            "    status: KetTheme.ConversationStatus.Working\n"
            "    tags: [{label: \"ui\", color: \"#2f6fde\"}, {label: \"review\", color: "
            "\"#23875a\"}] }\n"
            "  property int stops: 0; property string sent\n"
            "  KetComposer { id: composer; width: 600; onStopRequested: stops++;\n"
            "    onSubmitted: text => sent = text }\n"
            "  property alias tooled: tooled\n"
            "  KetComposer { id: tooled; width: 600\n"
            "    leadingTools: [ KetIconButton { text: \"Add\"; iconName: \"plus\" } ] }\n"
            "  KetUserMessage { id: user; width: 700; text: \"Two lines please.\" }\n"
            "  KetAssistantMessage { width: 600; name: \"Claude\"; time: \"10:42\";"
            "                        text: \"Done.\" }\n"
            "  KetDayDivider { width: 600; text: \"Today\" }\n"
            "  KetActivityRow { id: step; text: \"Read 3 files\"; iconName: \"file\";"
            "                   expandable: true }\n"
            "}\n"));
        QVERIFY(root);
        auto* row = root->property("row").value<QQuickItem*>();
        QCOMPARE(row->implicitHeight(), 48.0);
        QCOMPARE(row->property("visibleTagCount").toInt(), 2);
        auto* user = root->property("user").value<QQuickItem*>();
        auto* bubble = user->property("bubble").value<QQuickItem*>();
        QCOMPARE(bubble->property("color").value<QColor>(),
                 userMessageRecipe(Theme::Light).background);
        QVERIFY(bubble->x() > 0);
        // The glyphs sit in the middle of the line box, as in CSS: half the leading above.
        auto* label = bubble->childItems().value(0);
        QVERIFY(label);
        QVERIFY(label->y() > userMessageRecipe(Theme::Light).paddingBlock);
        auto* composer = root->property("composer").value<QQuickItem*>();
        // Without tools the toolbar row hides; with one it adds its 40 px.
        QVERIFY(composer->implicitHeight() >= 48);
        QVERIFY(composer->implicitHeight() < 48 + 40);
        auto* tooled = root->property("tooled").value<QQuickItem*>();
        QVERIFY(tooled->implicitHeight() >= 48 + 40);
        // One action circle: it stops the turn while busy with nothing typed, then sends.
        auto* action = composer->property("actionButton").value<QQuickItem*>();
        QCOMPARE(action->width(), action->height());
        QVERIFY(!action->isEnabled());
        composer->setProperty("busy", true);
        QVERIFY(action->isEnabled());
        QVERIFY(action->property("look").toMap().value(QStringLiteral("stop")).toBool());
        QMetaObject::invokeMethod(action, "clicked");
        QCOMPARE(root->property("stops").toInt(), 1);
        composer->setProperty("text", QStringLiteral("Also check Linux"));
        QVERIFY(!action->property("look").toMap().value(QStringLiteral("stop")).toBool());
        QMetaObject::invokeMethod(action, "clicked");
        QCOMPARE(root->property("sent").toString(), QStringLiteral("Also check Linux"));
        QCOMPARE(root->property("stops").toInt(), 1);
        QCOMPARE(root->property("step").value<QQuickItem*>()->implicitHeight(), 32.0);
    }

    // The placeholder starts where the caret and the first typed line do, and that line sits
    // in the middle of its line box, as in CSS.
    void composerPlaceholderSitsOnTheCaretLine() {
        std::unique_ptr<QObject> root = create(
            QByteArrayLiteral("import QtQuick\nimport Ket\n"
                              "Item { width: 600; height: 200; property alias composer: composer\n"
                              "  KetComposer { id: composer; width: 600 } }\n"));
        QVERIFY(root);
        auto* composer = root->property("composer").value<QQuickItem*>();
        auto* input = composer->findChild<QQuickItem*>(QStringLiteral("composerInput"));
        auto* placeholder = composer->findChild<QQuickItem*>(QStringLiteral("composerPlaceholder"));
        QVERIFY(input && placeholder);
        QVERIFY(placeholder->isVisible());
        QTRY_COMPARE(placeholder->y(), input->property("cursorRectangle").toRectF().y());
        const QRectF empty = input->property("cursorRectangle").toRectF();
        QCOMPARE(placeholder->height(), empty.height());
        const ComposerRecipe recipe = composerRecipe(false, Theme::Light);
        const qreal lineBox = std::floor(recipe.textPixels * recipe.lineHeight);
        QVERIFY(qAbs(empty.center().y() - (recipe.textPaddingBlock + lineBox / 2)) <= 1.0);
        input->setProperty("text", QStringLiteral("Make it so"));
        input->setProperty("cursorPosition", 0);
        QVERIFY(!placeholder->isVisible());
        QCOMPARE(input->property("cursorRectangle").toRectF().y(), empty.y());
    }

    // Looping indicators step on one shared clock that runs only while one of them is
    // showing, so an idle window does not redraw at the display rate.
    void loopingIndicatorsShareOneClock() {
        std::unique_ptr<QObject> root = create(
            QByteArrayLiteral("import QtQuick\nimport Ket\n"
                              "Item { property alias row: row; property alias bars: bars\n"
                              "  property real clock: KetTheme.animationClock\n"
                              "  KetConversationRow { id: row; width: 320; text: \"Navigation\"\n"
                              "    status: KetTheme.ConversationStatus.Working }\n"
                              "  KetActivityIndicator { id: bars }\n"
                              "}\n"));
        QVERIFY(root);
        auto* row = root->property("row").value<QQuickItem*>();
        auto* bars = root->property("bars").value<QQuickItem*>();
        QVERIFY(row->property("holdsClock").toBool());
        QVERIFY(bars->property("holdsClock").toBool());
        const qreal start = root->property("clock").toReal();
        QTRY_VERIFY(root->property("clock").toReal() > start);
        // A visible window behind another application keeps the same steps, so its
        // spinners do not stutter.
        QVERIFY(QGuiApplication::applicationState() != Qt::ApplicationActive);
        QSignalSpy steps(root.get(), SIGNAL(clockChanged()));
        QTest::qWait(500);
        QVERIFY2(steps.size() >= 7,
                 qPrintable(QStringLiteral("%1 steps in 500 ms").arg(steps.size())));
        row->setVisible(false);
        bars->setProperty("running", false);
        QVERIFY(!row->property("holdsClock").toBool());
        QVERIFY(!bars->property("holdsClock").toBool());
        const qreal stopped = root->property("clock").toReal();
        QTest::qWait(400);
        QCOMPARE(root->property("clock").toReal(), stopped);
    }

    void richTranscriptBlocksLoad() {
        std::unique_ptr<QObject> root =
            create(QByteArrayLiteral("import QtQuick\nimport Ket\n"
                                     "Item { width: 800; height: 600\n"
                                     "  property alias view: view; property alias ring: ring\n"
                                     "  KetMarkdown { id: view; width: 600\n"
                                     "    markdown: \"### Plan\\n\\nBody "
                                     "**bold**.\\n\\n```sh\\nctest\\n```\\n\\n> Quote\" }\n"
                                     "  KetUsageRing { id: ring; value: 0.9 }\n"
                                     "}\n"));
        QVERIFY(root);
        auto* view = root->property("view").value<QQuickItem*>();
        QVERIFY(view->implicitHeight() > 0);
        QCOMPARE(view->property("markdown").toString().count(QLatin1Char('\n')), 8);
        auto* ring = root->property("ring").value<QQuickItem*>();
        QCOMPARE(ring->implicitWidth(), 19.0);
        QCOMPARE(ring->property("recipe").toMap().value(QStringLiteral("arc")).value<QColor>(),
                 Tokens::color(u"warning", Theme::Light));
    }

    void composerCompletesAgentMentions() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Window { width: 800; height: 600; visible: true\n"
            "  property alias composer: composer; property string lastQuery: \"-\"\n"
            "  KetComposer { id: composer; y: 400; width: 600\n"
            "    onMentionQuery: query => { lastQuery = query;\n"
            "      mentionItems = [{number: \"4821\", title: \"Navigation\", place: \"ketplus\"}] "
            "} }\n"
            "}\n"));
        QVERIFY(root);
        auto* composer = root->property("composer").value<QQuickItem*>();
        composer->setProperty("text", QStringLiteral("Ask @agent/48"));
        auto* picker = composer->property("mentionPicker").value<QObject*>();
        QVERIFY(picker);
        composer->setProperty("cursorPosition", 13);
        QCOMPARE(root->property("lastQuery").toString(), QStringLiteral("48"));
        QVERIFY(picker->property("visible").toBool());
        const QVariant option = QVariantMap{{QStringLiteral("number"), QStringLiteral("4821")}};
        QMetaObject::invokeMethod(composer, "applyMention", Q_ARG(QVariant, option));
        QCOMPARE(composer->property("text").toString(), QStringLiteral("Ask @agent/4821 "));
        QVERIFY(!picker->property("visible").toBool());
    }

    void chatExtrasLoadAndBehave() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Window { width: 900; height: 700; visible: true\n"
            "  property alias composer: composer; property alias gallery: gallery\n"
            "  property alias group: group; property alias popover: popover\n"
            "  property alias anchor: anchor; property int removed: -1\n"
            "  KetComposer { id: composer; y: 500; width: 600\n"
            "    attachments: [{ name: \"a.md\" }, { name: \"b.png\", icon: \"image\" }]\n"
            "    onAttachmentRemoved: (index, name) => removed = index }\n"
            "  KetActivityGroup { id: group; width: 600; text: \"Explored\"\n"
            "    KetActivityRow { width: 500; text: \"Read\"; iconName: \"file\" }\n"
            "    KetGallery { id: gallery; width: 500; maxVisible: 2\n"
            "      images: [{ caption: \"1\" }, { caption: \"2\" }, { caption: \"3\" }] } }\n"
            "  KetSuggestion { y: 200; width: 400; text: \"Review\"; caption: \"Risky first\"; "
            "iconName: \"branch\" }\n"
            "  KetQuestionCard { y: 260; width: 400; question: \"Which?\"; step: \"1 / 2\"\n"
            "    footer: [KetButton { text: \"Next\" }]\n"
            "    KetChoice { width: 300; text: \"One\"; kind: KetTheme.ToggleKind.Radio } }\n"
            "  KetMeter { y: 420; width: 300; label: \"Weekly\"; valueText: \"32%\"; fraction: "
            "0.32; compact: true }\n"
            "  KetAttachmentChip { y: 460; name: \"loose.txt\" }\n"
            "  Rectangle { id: anchor; x: 700; y: 600; width: 30; height: 30 }\n"
            "  KetPopover { id: popover; title: \"Context & limits\"\n"
            "    footer: [KetButton { text: \"Compact session\" }]\n"
            "    KetPopoverText { text: \"Claude · Opus\" }\n"
            "    KetPopoverDivider {}\n"
            "    KetMeter { width: parent.width; label: \"5-hour usage\"; valueText: \"18%\" } }\n"
            "}\n"));
        QVERIFY(root);
        auto* composer = root->property("composer").value<QQuickItem*>();
        QMetaObject::invokeMethod(composer, "removeAttachment", Q_ARG(QVariant, 1));
        QCOMPARE(root->property("removed").toInt(), 1);
        QCOMPARE(composer->property("attachments").toList().size(), 1);

        auto* gallery = root->property("gallery").value<QQuickItem*>();
        QCOMPARE(gallery->property("shown").toList().size(), 2);
        QVERIFY(gallery->property("limited").toBool());
        gallery->setProperty("expanded", true);
        QCOMPARE(gallery->property("shown").toList().size(), 3);

        auto* group = root->property("group").value<QQuickItem*>();
        const qreal closed = group->implicitHeight();
        QCOMPARE(closed, 36.0);
        group->setProperty("expanded", true);
        QTRY_VERIFY(group->implicitHeight() > closed); // Column lays out on polish

        QObject* popover = root->property("popover").value<QObject*>();
        auto* anchor = root->property("anchor").value<QQuickItem*>();
        QMetaObject::invokeMethod(popover, "openAbove",
                                  Q_ARG(QVariant, QVariant::fromValue(anchor)));
        QVERIFY(popover->property("opened").toBool() || popover->property("visible").toBool());
        QCOMPARE(popover->property("width").toReal(), 340.0);
        QCOMPARE(popover->property("x").toReal() + 340.0, 730.0);
        QCOMPARE(popover->property("y").toReal() + popover->property("height").toReal() + 12,
                 600.0);
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
