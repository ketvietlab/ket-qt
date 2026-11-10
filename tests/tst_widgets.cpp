// SPDX-License-Identifier: MPL-2.0
#include "KetActivityIndicator.h"
#include "KetActivityRow.h"
#include "KetAppearance.h"
#include "KetBadge.h"
#include "KetBanner.h"
#include "KetButton.h"
#include "KetChoice.h"
#include "KetComposer.h"
#include "KetConversationRow.h"
#include "KetDialog.h"
#include "KetEmptyState.h"
#include "KetFieldRow.h"
#include "KetIcon.h"
#include "KetIcons.h"
#include "KetMenuPainter.h"
#include "KetNavItem.h"
#include "KetSegmentedControl.h"
#include "KetSelect.h"

#include "KetStyle.h"
#include "KetTag.h"
#include "KetTextField.h"
#include "KetToast.h"
#include "KetToggle.h"
#include "KetTranscript.h"
#include <QLineEdit>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QSignalSpy>
#include <QStyleOptionMenuItem>
#include <QTabBar>
#include <QTest>
#include <QTextEdit>
#include <QToolTip>

using namespace ket;

class WidgetsTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void styleIsFusionBasedOnEveryPlatform() {
        auto* style = qobject_cast<Style*>(QApplication::style());
        QVERIFY(style);
        QCOMPARE(style->baseStyle()->name().toLower(), QStringLiteral("fusion"));
    }

    void paletteFollowsAppearance() {
        QCOMPARE(QApplication::palette().color(QPalette::Window),
                 Tokens::color(u"page-bg", Theme::Light));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(QApplication::palette().color(QPalette::Window),
                 Tokens::color(u"page-bg", Theme::Dark));
        QCOMPARE(QApplication::palette().color(QPalette::Text),
                 Tokens::color(u"input-text", Theme::Dark));
    }

    void buttonHeightsComeFromTokens() {
        Button button(QStringLiteral("Save"), ButtonVariant::Primary);
        QCOMPARE(button.sizeHint().height(), 32);
        button.setControlSize(ControlSize::Small);
        QCOMPARE(button.sizeHint().height(), 28);
        Appearance::instance().setZoom(1.25);
        QCOMPARE(button.sizeHint().height(), 35); // 28 * 1.25 = 35
    }

    void buttonRecipeTracksInteraction() {
        Button button(QStringLiteral("Delete"), ButtonVariant::Danger);
        QCOMPARE(button.recipe().text, Tokens::color(u"danger", Theme::Light));
        button.setEnabled(false);
        QCOMPARE(button.recipe().opacity, 0.48);
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(button.recipe().border, Tokens::color(u"danger-border", Theme::Dark));
    }

    void buttonRendersWithoutWarnings() {
        Button button(QStringLiteral("Continue"), ButtonVariant::Primary);
        button.resize(button.sizeHint());
        const QImage image = button.grab().toImage();
        QVERIFY(!image.isNull());
        // Below the label, the middle of a primary button is its fill colour.
        const QColor centre = image.pixelColor(image.width() / 2, image.height() - 4);
        QCOMPARE(centre.rgb(), Tokens::color(u"action-primary-bg", Theme::Light).rgb());
    }

    void badgeSizesToText() {
        Badge narrow(QStringLiteral("UI"), BadgeTone::Accent);
        Badge wide(QStringLiteral("Performance"), BadgeTone::Accent);
        QCOMPARE(narrow.sizeHint().height(), qRound(narrow.recipe().height));
        QVERIFY(wide.sizeHint().width() > narrow.sizeHint().width());
        QCOMPARE(narrow.recipe().text, Tokens::color(u"accent-text", Theme::Light));
    }

    void iconsTakeTheTextColour() {
        QPixmap glyph(16, 16);
        glyph.fill(Qt::black);
        IconButton button(QIcon(glyph), QStringLiteral("Close"));
        QCOMPARE(button.sizeHint(), QSize(30, 30));
        QCOMPARE(button.accessibleName(), QStringLiteral("Close"));
        QCOMPARE(button.toolTip(), QStringLiteral("Close"));
        button.resize(button.sizeHint());
        const QImage image = button.grab().toImage();
        QCOMPARE(image.pixelColor(image.width() / 2, image.height() / 2).rgb(),
                 Tokens::color(u"text-secondary", Theme::Light).rgb());
        Appearance::instance().setZoom(1.25);
        QCOMPARE(button.sizeHint(), QSize(38, 38)); // round(30 * 1.25)
        button.setGlyphSize(20);
        QCOMPARE(button.recipe().iconSize, 20.0);
        QCOMPARE(button.sizeHint(), QSize(38, 38)); // the glyph grows, not the button
    }

    void ketSymbolIconsFollowTheButtonRecipe() {
        IconButton button(Icons::icon(u"close"), QStringLiteral("Close"));
        button.resize(button.sizeHint());
        const QImage image = button.grab().toImage();
        const QColor secondary = Tokens::color(u"text-secondary", Theme::Light);
        // A light 1 px stroke has no fully opaque pixel at 1x; its core is within a few levels.
        const auto near = [&secondary](const QColor& pixel) {
            return qAbs(pixel.red() - secondary.red()) <= 12 &&
                   qAbs(pixel.green() - secondary.green()) <= 12 &&
                   qAbs(pixel.blue() - secondary.blue()) <= 12;
        };
        bool tinted = false;
        for (int y = 0; y < image.height() && !tinted; ++y) {
            for (int x = 0; x < image.width() && !tinted; ++x)
                tinted = near(image.pixelColor(x, y));
        }
        QVERIFY(tinted);

        Icon icon(QStringLiteral("folder"), 17);
        QCOMPARE(icon.sizeHint(), QSize(17, 17));
        QCOMPARE(icon.color(), Tokens::color(u"text-main", Theme::Light));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(icon.color(), Tokens::color(u"text-main", Theme::Dark));
        icon.setColor(QColor(Qt::red));
        QCOMPARE(icon.color(), QColor(Qt::red));
    }

    void textFieldsDrawTheFieldRecipe() {
        TextField field;
        field.setPlaceholderText(QStringLiteral("Search conversations"));
        QCOMPARE(field.sizeHint().height(), 34);
        QCOMPARE(field.palette().color(QPalette::PlaceholderText),
                 Tokens::color(u"input-placeholder", Theme::Light));
        const int plain = field.textMargins().left();
        field.setLeadingIcon(QStringLiteral("search"));
        QCOMPARE(field.textMargins().left(), plain + 14 + 9);
        field.resize(240, field.sizeHint().height());
        const QImage image = field.grab().toImage();
        QCOMPARE(image.pixelColor(image.width() - 6, image.height() / 2).rgb(),
                 Tokens::color(u"input-bg", Theme::Light).rgb());
        field.setInvalid(true);
        QCOMPARE(field.recipe().border, Tokens::color(u"danger", Theme::Light));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(field.palette().color(QPalette::Text), Tokens::color(u"input-text", Theme::Dark));
    }

    void selectsAndTogglesFollowTheirRecipes() {
        Select select;
        select.addItems({QStringLiteral("Default · 760 px"), QStringLiteral("Wide · 864 px")});
        QCOMPARE(select.sizeHint().height(), 34);
        select.resize(select.sizeHint());
        QVERIFY(!select.grab().isNull());

        // ket-qt#43: editable, the line edit alone draws the text, inside the text rect.
        Select editable;
        editable.addItems({QStringLiteral("feature/offline-cache")});
        editable.setEditable(true);
        editable.resize(240, editable.sizeHint().height());
        editable.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editable));
        QVERIFY(!editable.grab().isNull());
        auto* edit = editable.lineEdit();
        QVERIFY(edit != nullptr);
        QVERIFY(!edit->hasFrame());
        QCOMPARE(edit->text(), QStringLiteral("feature/offline-cache"));
        const int padding = qRound(Appearance::instance().scaled(editable.recipe().paddingInline));
        QCOMPARE(edit->x(), padding);
        QVERIFY(edit->geometry().right() < editable.width() - padding);
        QCOMPARE(edit->palette().color(QPalette::Base), QColor(Qt::transparent));
        QCOMPARE(edit->palette().color(QPalette::Text), editable.recipe().text);

        Checkbox check(QStringLiteral("Follow system appearance"));
        QVERIFY(!check.isChecked());
        QCOMPARE(check.recipe().mark.alpha(), 0);
        check.click();
        QVERIFY(check.isChecked());
        QCOMPARE(check.recipe().background, Tokens::color(u"accent", Theme::Light));

        QWidget group;
        auto* first = new Radio(QStringLiteral("Workspace write"), &group);
        auto* second = new Radio(QStringLiteral("Read only"), &group);
        first->click();
        second->click();
        QVERIFY(!first->isChecked());
        QVERIFY(second->isChecked());
    }

    void choicesAndFieldRows() {
        Choice choice(QStringLiteral("Check for updates automatically"),
                      QStringLiteral("Once a day while KetPlus is open."));
        QVERIFY(choice.isCheckable());
        QCOMPARE(choice.recipe().border, Tokens::color(u"panel-border", Theme::Light));
        choice.click();
        QCOMPARE(choice.recipe().background, Tokens::color(u"accent-subtle", Theme::Light));
        QVERIFY(choice.heightForWidth(200) > choice.heightForWidth(600));

        auto* field = new TextField;
        FieldRow row(QStringLiteral("Interface font"), field, QStringLiteral("Help"));
        row.setError(QStringLiteral("Required"));
        QVERIFY(field->isInvalid());
        row.setError({});
        QVERIFY(!field->isInvalid());
    }

    void bannersWrapAndTakeAnAction() {
        Banner banner(BannerTone::Warning, QStringLiteral("Router unavailable"),
                      QStringLiteral("Requests use the direct connection until it returns."));
        QCOMPARE(banner.recipe().background, Tokens::color(u"warning-bg", Theme::Light));
        QVERIFY(!banner.action());
        Button* retry = banner.setAction(QStringLiteral("Retry"), QStringLiteral("refresh"));
        QVERIFY(retry && banner.action() == retry);
        QVERIFY(banner.heightForWidth(260) > banner.heightForWidth(700));
        banner.resize(500, banner.heightForWidth(500));
        QVERIFY(!banner.grab().isNull());
    }

    void toastsSitAtTheBottomAndExpire() {
        QWidget window;
        window.resize(800, 600);
        Toast* toast = Toast::showMessage(&window, QStringLiteral("Saved"));
        QCOMPARE(toast->parentWidget(), &window);
        QVERIFY(toast->isVisibleTo(&window));
        QCOMPARE(toast->geometry().center().x(), window.rect().center().x());
        QVERIFY(toast->geometry().bottom() < window.height() - 30);
        QPointer<Toast> first = toast;
        Toast* second = Toast::showMessage(&window, QStringLiteral("Saved again"));
        QVERIFY(second != first);
        QTRY_VERIFY(first.isNull()); // the earlier toast is replaced
    }

    void dialogsCoverTheWindowAndTrapFocus() {
        QWidget window;
        window.resize(900, 700);
        auto* outside = new QLineEdit(&window);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        outside->setFocus();

        Dialog dialog(QStringLiteral("Rename"), &window);
        auto* name = new TextField;
        dialog.setBody(name);
        Button* cancel = dialog.addButton(QStringLiteral("Cancel"));
        Button* save = dialog.addButton(QStringLiteral("Save"), ButtonVariant::Primary);
        QSignalSpy accepted(&dialog, &Dialog::accepted);
        QSignalSpy rejected(&dialog, &Dialog::rejected);
        connect(save, &QAbstractButton::clicked, &dialog, &Dialog::accept);

        dialog.open();
        QCOMPARE(dialog.geometry(), window.rect());
        QCOMPARE(dialog.card()->width(), 500);
        QCOMPARE(dialog.card()->geometry().center(), window.rect().center());
        QTRY_COMPARE(QApplication::focusWidget(), name);
        for (int i = 0; i < 6; ++i) {
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QVERIFY(dialog.card()->isAncestorOf(QApplication::focusWidget()));
        }
        QTest::keyClick(QApplication::focusWidget(), Qt::Key_Escape);
        QCOMPARE(rejected.count(), 1);
        QVERIFY(!dialog.isOpen());
        QCOMPARE(QApplication::focusWidget(), outside);

        dialog.open();
        save->click();
        QCOMPARE(accepted.count(), 1);
        dialog.open();
        QTest::mouseClick(&dialog, Qt::LeftButton, {}, QPoint(5, 5));
        QCOMPARE(rejected.count(), 2);
        Q_UNUSED(cancel);
    }

    void menusUseTheMenuRecipe() {
        QMenu menu;
        menu.addAction(QStringLiteral("Open"));
        QAction* remove = menu.addAction(QStringLiteral("Delete"));
        Style::setDanger(remove);
        QAction* pin = menu.addAction(QStringLiteral("Pin to sidebar"));
        pin->setCheckable(true);
        Style::setDescription(pin, QStringLiteral("Keep it above recent conversations."));
        menu.addSeparator();
        QAction* full = menu.addAction(QStringLiteral("Full access"));
        Style::setWarning(full);
        menu.insertSection(remove, QStringLiteral("Mode"));
        menu.ensurePolished();
        QVERIFY(menu.testAttribute(Qt::WA_TranslucentBackground));
        QVERIFY(menu.sizeHint().width() >= 210);
        QVERIFY(menu.sizeHint().width() <= 330);

        QStyleOptionMenuItem option;
        option.initFrom(&menu);
        option.menuItemType = QStyleOptionMenuItem::Normal;
        option.text = QStringLiteral("Open");
        option.menuHasCheckableItems = true;
        QCOMPARE(detail::menuItemSize(&option, &menu).height(), 32);
        option.text = QStringLiteral("Pin to sidebar");
        QVERIFY(detail::menuItemSize(&option, &menu).height() > 32);
        option.menuItemType = QStyleOptionMenuItem::Separator;
        option.text.clear();
        QCOMPARE(detail::menuItemSize(&option, &menu).height(), 11);
        option.text = QStringLiteral("Mode"); // QMenu::addSection
        QCOMPARE(detail::menuItemSize(&option, &menu).height(), 24);
        QVERIFY(!menu.grab().isNull());
    }

    void tooltipsUseTheAccent() {
        QCOMPARE(QToolTip::palette().color(QPalette::ToolTipBase),
                 Tokens::color(u"accent", Theme::Light));
        QCOMPARE(QApplication::style()->styleHint(QStyle::SH_ToolTip_WakeUpDelay), 600);
        QVERIFY(QApplication::style()->styleHint(QStyle::SH_ComboBox_Popup));
    }

    void navItemsSelectAndShowTrailingText() {
        QButtonGroup group;
        NavItem chat(QStringLiteral("Conversations"), QStringLiteral("chat"));
        NavItem files(QStringLiteral("Files"), QStringLiteral("folder"));
        group.addButton(&chat);
        group.addButton(&files);
        chat.setChecked(true);
        QCOMPARE(chat.recipe().background, Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(files.recipe().background.alpha(), 0);
        files.click();
        QVERIFY(!chat.isChecked());
        QCOMPARE(chat.sizeHint().height(), 34);
        const int plain = files.sizeHint().width();
        files.setCount(12);
        QVERIFY(files.sizeHint().width() > plain);
        files.setCount(std::nullopt);
        files.setShortcutText(QStringLiteral("⌘K"));
        QVERIFY(files.sizeHint().width() > plain);
        files.resize(240, 34);
        QVERIFY(!files.grab().isNull());
        Kbd kbd(QStringLiteral("⌘⇧P"));
        QVERIFY(kbd.sizeHint().width() > 0);
    }

    void segmentedControlsMoveWithKeys() {
        SegmentedControl control(
            {QStringLiteral("Claude"), QStringLiteral("Codex"), QStringLiteral("Cursor")});
        QSignalSpy changed(&control, &SegmentedControl::currentChanged);
        control.resize(control.sizeHint());
        QCOMPARE(control.currentIndex(), 0);
        QTest::keyClick(&control, Qt::Key_Right);
        QCOMPARE(control.currentIndex(), 1);
        QTest::keyClick(&control, Qt::Key_End);
        QCOMPARE(control.currentIndex(), 2);
        QTest::keyClick(&control, Qt::Key_Right);
        QCOMPARE(control.currentIndex(), 2);
        QTest::mouseClick(&control, Qt::LeftButton, {}, control.segmentRect(0).center());
        QCOMPARE(control.currentIndex(), 0);
        QCOMPARE(changed.count(), 3);
        QVERIFY(control.segmentRect(1).left() > control.segmentRect(0).right());
    }

    void tabBarsAndProgressBarsUseTheStyle() {
        QTabBar bar;
        bar.addTab(QStringLiteral("NavigationRail.qml"));
        bar.addTab(Icons::icon(u"file"), QStringLiteral("README.md"));
        bar.setTabsClosable(true);
        QCOMPARE(bar.sizeHint().height(), 41);
        QVERIFY(!bar.grab().isNull());

        QProgressBar progress;
        progress.setRange(0, 100);
        progress.setValue(40);
        QCOMPARE(progress.sizeHint().height(), 6);
        progress.resize(200, 6);
        const QImage image = progress.grab().toImage();
        const QColor fill = image.pixelColor(image.width() / 8, image.height() / 2);
        QCOMPARE(fill, Tokens::color(u"accent", Theme::Light));
        const QColor track = image.pixelColor(image.width() * 7 / 8, image.height() / 2);
        QCOMPARE(track, Tokens::color(u"border-default", Theme::Light));
    }

    void activityAndEmptyStates() {
        ActivityIndicator activity;
        QCOMPARE(activity.sizeHint(), QSize(15, 10));
        activity.setPreviewTime(0);
        QVERIFY(!activity.grab().isNull());

        EmptyState empty(QStringLiteral("success"), QStringLiteral("Your worktree is clean."),
                         QStringLiteral("There are no uncommitted changes in this snapshot."));
        QCOMPARE(empty.message(),
                 QStringLiteral("There are no uncommitted changes in this snapshot."));
        QVERIFY(!empty.action());
        empty.setAction(QStringLiteral("Back to editor"), QStringLiteral("arrow-left"));
        QVERIFY(empty.action());
        empty.resize(600, 400);
        QVERIFY(!empty.grab().isNull());
    }

    void conversationRowsFitWholeTags() {
        ConversationRow row(QStringLiteral("Navigation rail polish"));
        row.setNumber(QStringLiteral("4821"));
        row.setStatus(ConversationStatus::Working);
        QCOMPARE(row.sizeHint().height(), 48);
        row.resize(320, 48);
        QCOMPARE(row.visibleTagCount(), 0);
        row.setTags({{QStringLiteral("ui"), QColor(0x2f6fde)},
                     {QStringLiteral("review"), QColor(0x23875a)},
                     {QStringLiteral("perf"), QColor(0xb7791f)},
                     {QStringLiteral("account"), QColor(0x7a55d6)}});
        const int wide = row.visibleTagCount();
        QVERIFY(wide >= 3);
        row.resize(170, 48);
        const int narrow = row.visibleTagCount();
        QVERIFY(narrow < wide);
        QVERIFY(narrow >= 0);
        QVERIFY(!row.grab().isNull());
        QCOMPARE(row.statusLabel(), QStringLiteral("Working"));
        row.setStatusLabel(QStringLiteral("Đang làm"));
        QCOMPARE(row.statusLabel(), QStringLiteral("Đang làm"));
    }

    void composerToolbarHidesWhileEmpty() {
        Composer composer;
        composer.resize(600, 200);
        composer.show();
        QCoreApplication::processEvents();
        const int bare = composer.sizeHint().height();
        auto* attach = new IconButton(Icons::icon(u"plus"), QStringLiteral("Add"));
        composer.leadingTools()->addWidget(attach);
        QCoreApplication::processEvents();
        QVERIFY(composer.sizeHint().height() >= bare + 40);
        delete attach;
        QCoreApplication::processEvents();
        QCOMPARE(composer.sizeHint().height(), bare);
    }

    void composersSendOnEnterAndGrow() {
        Composer composer;
        QSignalSpy submitted(&composer, &Composer::submitted);
        composer.resize(600, composer.sizeHint().height());
        composer.show();
        QVERIFY(QTest::qWaitForWindowExposed(&composer));
        QVERIFY(!composer.sendButton()->isEnabled());
        const int oneLine = composer.editor()->height();
        QCOMPARE(oneLine, 48);
        composer.editor()->setFocus();
        QTest::keyClicks(composer.editor(), QStringLiteral("Hello"));
        QVERIFY(composer.sendButton()->isEnabled());
        QTest::keyClick(composer.editor(), Qt::Key_Return, Qt::ShiftModifier);
        QTest::keyClicks(composer.editor(), QStringLiteral("there"));
        QTest::keyClick(composer.editor(), Qt::Key_Return, Qt::ShiftModifier);
        QTest::keyClicks(composer.editor(), QStringLiteral("again"));
        QVERIFY(composer.editor()->height() > oneLine);
        QCOMPARE(composer.recipe().border, Tokens::color(u"accent-border", Theme::Light));
        QTest::keyClick(composer.editor(), Qt::Key_Return);
        QCOMPARE(submitted.count(), 1);
        QCOMPARE(submitted.first().first().toString(), QStringLiteral("Hello\nthere\nagain"));
        QVERIFY(composer.text().isEmpty());
        // One circle: while busy with nothing typed it stops the turn, otherwise it sends.
        QSignalSpy stopped(&composer, &Composer::stopRequested);
        composer.setBusy(true);
        QVERIFY(composer.sendButton()->isEnabled());
        QCOMPARE(composer.sendButton()->accessibleName(), QStringLiteral("Stop turn"));
        QTest::mouseClick(composer.sendButton(), Qt::LeftButton);
        QCOMPARE(stopped.count(), 1);
        QTest::keyClicks(composer.editor(), QStringLiteral("Also check Linux"));
        QCOMPARE(composer.sendButton()->accessibleName(), QStringLiteral("Send guidance"));
        QTest::mouseClick(composer.sendButton(), Qt::LeftButton);
        QCOMPARE(submitted.count(), 2);
        QCOMPARE(stopped.count(), 1);
        QCOMPARE(composer.sendButton()->width(), composer.sendButton()->height());
        // The user's bubble is tinted, not filled with the accent.
        QCOMPARE(userMessageRecipe(Theme::Light).background,
                 Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(userMessageRecipe(Theme::Light).text, Tokens::color(u"text-main", Theme::Light));
    }

    void transcriptBlocksWrapAndAlign() {
        UserMessage user(QStringLiteral("Make the sidebar rows two lines and show the tags."));
        user.resize(700, user.heightForWidth(700));
        QVERIFY(user.bubbleRect().right() >= 698);
        QVERIFY(user.bubbleRect().width() <= qRound(700 * 0.8));
        UserMessage longMessage(QString(QStringLiteral("word ")).repeated(80));
        QVERIFY(longMessage.heightForWidth(300) > longMessage.heightForWidth(700));

        AssistantMessage assistant(QStringLiteral("Claude"), QStringLiteral("10:42"));
        assistant.setText(QStringLiteral("The rows now wrap their tags."));
        QVERIFY(qobject_cast<ReadingText*>(assistant.body()));
        assistant.resize(600, 200);
        QVERIFY(!assistant.grab().isNull());

        DayDivider day(QStringLiteral("Today"));
        QVERIFY(day.sizeHint().height() > 22);
        ActivityRow step(QStringLiteral("Read 3 files"), QStringLiteral("file"));
        step.setExpandable(true);
        QVERIFY(step.isCheckable());
        QCOMPARE(step.sizeHint().height(), 32);
        step.click();
        QVERIFY(step.isChecked());
    }

    void tagElidesAtTheRecipeWidth() {
        Tag tag(QStringLiteral("A very long tag label that overflows"), QColor(0x2f6fde));
        QCOMPARE(tag.sizeHint().width(), qRound(tag.recipe().maxWidth));
        QCOMPARE(tag.sizeHint().height(), 18);
        QVERIFY(!tag.recipe().background.isValid() || tag.recipe().background.alpha() < 255);
        tag.setPreviewHovered(true);
        QCOMPARE(tag.recipe().background.alpha(), qRound(0.22 * 255));
        Tag more(QStringLiteral("+2"));
        more.setOverflow(true);
        QCOMPARE(more.recipe().text, Tokens::color(u"text-muted", Theme::Light));
        QVERIFY(more.sizeHint().width() < tag.sizeHint().width());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    WidgetsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_widgets.moc"
