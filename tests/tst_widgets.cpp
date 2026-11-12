// SPDX-License-Identifier: MPL-2.0
#include "KetActivityIndicator.h"
#include "KetActivityRow.h"
#include "KetAnimationClock.h"
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
#include <QPaintEvent>
#include <QProgressBar>
#include <QSignalSpy>
#include <QStyleOptionMenuItem>
#include <QTabBar>
#include <QTest>
#include <QTextEdit>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWindow>

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
        // Inside the border, with its own transparent style sheet over any app style sheet.
        QVERIFY(edit->y() >= 1 && edit->geometry().bottom() < editable.height() - 1);
        QVERIFY(edit->styleSheet().contains(QStringLiteral("transparent")));
        QCOMPARE(edit->palette().color(QPalette::Text), editable.recipe().text);

        Checkbox check(QStringLiteral("Follow system appearance"));
        QVERIFY(!check.isChecked());
        QCOMPARE(check.recipe().mark.alpha(), 0);
        check.click();
        QVERIFY(check.isChecked());
        QCOMPARE(check.recipe().background, Tokens::color(u"accent", Theme::Light));
        // Wrapped, a narrow toggle grows down instead of asking for its label's width.
        QCOMPARE(check.minimumSizeHint(), check.sizeHint());
        QVERIFY(!check.hasHeightForWidth());
        check.setWordWrap(true);
        QVERIFY(check.minimumSizeHint().width() < check.sizeHint().width());
        QCOMPARE(check.heightForWidth(check.sizeHint().width()), check.sizeHint().height());
        QVERIFY(check.heightForWidth(check.minimumSizeHint().width()) > check.sizeHint().height());

        // In a layout the wrapped toggle gets the height its width needs, at every zoom.
        for (const qreal zoom : {1.0, 1.25, 1.5}) {
            Appearance::instance().setZoom(zoom);
            QWidget column;
            auto* layout = new QVBoxLayout(&column);
            auto* wrapped = new Checkbox(
                QStringLiteral(
                    "The leader writes requirements before the developers implement them."),
                &column);
            wrapped->setWordWrap(true);
            layout->addWidget(wrapped);
            layout->addStretch();
            column.resize(220, 400);
            column.show();
            QVERIFY(QTest::qWaitForWindowExposed(&column));
            QTRY_COMPARE(wrapped->height(), wrapped->heightForWidth(wrapped->width()));
            QVERIFY(wrapped->height() > wrapped->sizeHint().height());
        }
        Appearance::instance().setZoom(1.0);

        // At exactly that height the whole label is painted: drawn taller, nothing more
        // of it shows below.
        Checkbox paragraph(QStringLiteral("Paragraph gyp descenders wrap several words gyp"));
        paragraph.setWordWrap(true);
        const int fitted = paragraph.heightForWidth(145);
        paragraph.resize(145, fitted + 12);
        QImage taller(paragraph.size(), QImage::Format_ARGB32_Premultiplied);
        taller.fill(Qt::transparent);
        paragraph.render(&taller, {}, {}, QWidget::DrawChildren);
        int below = 0;
        for (int y = fitted; y < taller.height(); ++y)
            for (int x = 0; x < taller.width(); ++x)
                below += qAlpha(taller.pixel(x, y)) > 0;
        QCOMPARE(below, 0);

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

        // Stacked, the label is a line above the control and the row needs less width.
        const int beside = row.minimumSizeHint().width();
        row.resize(beside + 40, row.sizeHint().height());
        row.show();
        QVERIFY(QTest::qWaitForWindowExposed(&row));
        auto* label = row.findChild<QLabel*>();
        QVERIFY(label->geometry().right() < field->geometry().left());
        row.setStacked(true);
        QVERIFY(row.isStacked());
        QVERIFY(row.minimumSizeHint().width() < beside);
        row.resize(field->minimumSizeHint().width(), row.sizeHint().height());
        QTRY_VERIFY(label->geometry().bottom() < field->geometry().top());
        QCOMPARE(field->width(), row.width());
        row.setStacked(false);
        row.resize(beside + 40, row.sizeHint().height());
        QTRY_VERIFY(label->geometry().right() < field->geometry().left());
        QCOMPARE(label->width(), qRound(fieldRowRecipe(Theme::Light).labelWidth));
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

    void toastsCanFollowAnExpiredToast() {
        // An expired toast deletes itself; the next message must not delete it again.
        QWidget window;
        window.resize(800, 600);
        QPointer<Toast> expired = Toast::showMessage(&window, QStringLiteral("Opened"));
        expired->deleteLater();
        QTRY_VERIFY(expired.isNull());
        QVERIFY(!window.property("_ket_toast").value<QObject*>());
        Toast* next = Toast::showMessage(&window, QStringLiteral("Opened again"));
        QCOMPARE(window.property("_ket_toast").value<QObject*>(), next);
        QVERIFY(next->isVisibleTo(&window));
    }

    void toastsKeepTheirToneAndFitTheWindow() {
        QWidget window;
        window.resize(800, 600);
        // A failure is not shown as a success.
        Toast* neutral =
            Toast::showMessage(&window, QStringLiteral("Unable to fork"), ToastTone::Neutral);
        QCOMPARE(neutral->recipe().iconName, QStringLiteral("info"));
        QCOMPARE(neutral->recipe().border, Tokens::color(u"panel-border", Theme::Light));
        QCOMPARE(Toast::showMessage(&window, QStringLiteral("Saved"))->recipe().iconName,
                 QStringLiteral("success"));

        // A long message wraps inside the card's widest width and the window's margins.
        const QString longText = QStringLiteral("Unable to fork the conversation because the "
                                                "worktree changed while the request was pending. "
                                                "Try again from the conversation menu.");
        // A URL or path has no space to break at, and still breaks inside the card.
        const QString unbroken =
            QStringLiteral("https://relay.ketplus.example/v2/conversations/") + QString(96, u'a') +
            QStringLiteral("/fork?worktree=/Users/someone/dev/a-very-long-worktree-name");
        for (const QString& text : {longText, unbroken})
            for (const int width : {800, 320}) {
                window.resize(width, 600);
                Toast* toast = Toast::showMessage(&window, text, ToastTone::Neutral);
                const QRect card = toast->geometry(); // with its shadow
                QVERIFY2(card.left() >= 0 && card.right() < width,
                         qPrintable(QStringLiteral("%1 at %2").arg(text.left(8)).arg(width)));
                QVERIFY(toast->height() >
                        Toast::showMessage(&window, QStringLiteral("Saved"))->height());
            }
    }

    void overlaysStayAboveWindowContainers() {
        // Without a native child the overlays stay drawn into the window's surface.
        QWidget plain;
        plain.resize(800, 600);
        plain.show();
        QVERIFY(QTest::qWaitForWindowExposed(&plain));
        Dialog flat(QStringLiteral("Rename"), &plain);
        flat.setBody(new QWidget);
        flat.open();
        QVERIFY(!flat.testAttribute(Qt::WA_NativeWindow));
        QVERIFY(!Toast::showMessage(&plain, QStringLiteral("Saved"))
                     ->testAttribute(Qt::WA_NativeWindow));

        // A window container (as the Agent Workspace's QQuickView) is a native child window
        // above the window's surface, so the overlays become native children raised above it.
        QWidget window;
        window.resize(800, 600);
        auto* layout = new QVBoxLayout(&window);
        auto* container = QWidget::createWindowContainer(new QWindow, &window);
        layout->addWidget(container);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        QVERIFY(container->isVisible());
        Dialog dialog(QStringLiteral("Rename"), &window);
        dialog.setBody(new QWidget);
        dialog.open();
        QVERIFY(dialog.testAttribute(Qt::WA_NativeWindow));
        QVERIFY(dialog.internalWinId() != 0);
        Toast* toast = Toast::showMessage(&window, QStringLiteral("Saved"));
        QVERIFY(toast->testAttribute(Qt::WA_NativeWindow));
        QVERIFY(toast->isVisible());
    }

    void overlaysLeaveTheirSiblingsAlone() {
        // The page around a window container is the overlay's sibling. Had it become native, its
        // own window would cover the container's (the Agent Workspace showed blank).
        QWidget window;
        window.resize(800, 600);
        auto* layout = new QVBoxLayout(&window);
        auto* page = new QWidget;
        auto* pageLayout = new QVBoxLayout(page);
        auto* quick = new QWindow;
        pageLayout->addWidget(QWidget::createWindowContainer(quick, page));
        layout->addWidget(page);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        Toast* toast = Toast::showMessage(&window, QStringLiteral("Ready"));
        QVERIFY(toast->testAttribute(Qt::WA_NativeWindow));
        Dialog dialog(QStringLiteral("Rename"), &window);
        dialog.setBody(new QWidget);
        dialog.open();
        QVERIFY(dialog.testAttribute(Qt::WA_NativeWindow));
        QCoreApplication::processEvents();
        QCOMPARE(page->internalWinId(), WId(0));
        QCOMPARE(quick->parent(), window.windowHandle());
        QVERIFY(!QCoreApplication::testAttribute(Qt::AA_DontCreateNativeWidgetSiblings));
    }

    void chromelessDialogsShowTheirOwnPage() {
        QWidget window;
        window.resize(900, 700);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        Dialog dialog(QStringLiteral("Connect your phone"), &window);
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(0, 0, 0, 0);
        auto* line = new QLabel(QStringLiteral("Scan the code"));
        layout->addWidget(line);
        auto* more = new QLabel(QStringLiteral("Compare the code on both screens"));
        more->hide();
        layout->addWidget(more);
        dialog.setBody(page);
        dialog.setChromeless(true);
        dialog.open();
        // No header band and no padding: the page fills the card.
        for (auto* label : dialog.card()->findChildren<QLabel*>())
            QVERIFY(label->text() != dialog.title() || !label->isVisible());
        QCOMPARE(page->geometry().topLeft(), QPoint(0, 0));
        QCOMPARE(page->width(), dialog.card()->width());
        QCOMPARE(dialog.card()->accessibleName(), QStringLiteral("Connect your phone"));
        // The card follows the page's height as its state changes.
        const int before = dialog.card()->height();
        more->show();
        QTRY_VERIFY(dialog.card()->height() > before);
        QCOMPARE(dialog.card()->geometry().center().y(), window.rect().center().y());
    }

    void dialogsTakeTheMocksWidth() {
        QWidget window;
        window.resize(1200, 800);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        Dialog dialog(QStringLiteral("Repository & worktree"), &window);
        dialog.setBody(new QWidget);
        dialog.setCardWidth(800);
        dialog.open();
        QCOMPARE(dialog.card()->width(), 800);
        QCOMPARE(dialog.card()->geometry().center().x(), window.rect().center().x());
        // A narrow window keeps the recipe's margin on both sides.
        window.resize(600, 800);
        const int margin = qRound(dialog.recipe().margin);
        QCOMPARE(dialog.card()->width(), 600 - 2 * margin);
        dialog.setCardWidth(0);
        window.resize(1200, 800);
        QCOMPARE(dialog.card()->width(), qRound(dialog.recipe().width));
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

    void navItemsRingOnlyForKeyboardFocus() {
        QWidget window;
        auto* first = new NavItem(QStringLiteral("Storage"), QStringLiteral("layers"), &window);
        auto* second = new NavItem(QStringLiteral("Updates"), QStringLiteral("download"), &window);
        second->move(0, 40);
        window.show();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        first->setFocus(Qt::MouseFocusReason);
        QTRY_VERIFY(first->hasFocus());
        QCOMPARE(first->recipe().focusRing.alpha(), 0); // a click selects without a ring
        first->setFocus(Qt::OtherFocusReason);
        QCOMPARE(first->recipe().focusRing.alpha(), 0); // nor does a window opening on it
        second->setFocus(Qt::TabFocusReason);
        QTRY_VERIFY(second->hasFocus());
        QVERIFY(second->recipe().focusRing.alpha() > 0);
        QCOMPARE(first->recipe().focusRing.alpha(), 0);
        first->setFocus(Qt::BacktabFocusReason);
        QTRY_VERIFY(first->hasFocus());
        QVERIFY(first->recipe().focusRing.alpha() > 0);
        QCOMPARE(second->recipe().focusRing.alpha(), 0);
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

    // Working rows and activity bars step together on one 20 Hz clock, which runs only while
    // something shown needs it, and a spinner step repaints the icon rather than the row.
    void workingRowsStepOnTheSharedClock() {
        AnimationClock& clock = AnimationClock::instance();
        QCOMPARE(AnimationClock::intervalMs, 50);
        QWidget host;
        auto* layout = new QVBoxLayout(&host);
        auto* first = new ConversationRow(QStringLiteral("First"));
        auto* second = new ConversationRow(QStringLiteral("Second"));
        auto* bars = new ActivityIndicator;
        layout->addWidget(first);
        layout->addWidget(second);
        layout->addWidget(bars);
        first->setStatus(ConversationStatus::Working);
        second->setStatus(ConversationStatus::Working);
        host.resize(320, 160);
        host.show();
        QVERIFY(QTest::qWaitForWindowExposed(&host));
        QCOMPARE(clock.subscriberCount(), 3);
        QVERIFY(clock.isRunning());

        QList<QRect> painted;
        struct Recorder : QObject {
            QList<QRect>* rects;
            explicit Recorder(QList<QRect>* r) : rects(r) {}
            bool eventFilter(QObject*, QEvent* event) override {
                if (event->type() == QEvent::Paint)
                    rects->append(static_cast<QPaintEvent*>(event)->rect());
                return false;
            }
        } recorder(&painted);
        QCoreApplication::processEvents();
        first->installEventFilter(&recorder);
        clock.stepForTest();
        QTRY_VERIFY(!painted.isEmpty());
        for (const QRect& rect : painted) {
            QVERIFY(first->indicatorRect().contains(rect));
            QVERIFY(rect.width() * rect.height() * 8 < first->width() * first->height());
        }
        first->removeEventFilter(&recorder);

        bars->setPreviewTime(0);
        QCOMPARE(clock.subscriberCount(), 2);
        first->setStatus(ConversationStatus::Completed);
        QCOMPARE(clock.subscriberCount(), 1);
        second->hide();
        QCOMPARE(clock.subscriberCount(), 0);
        QVERIFY(!clock.isRunning());
        second->show();
        QCOMPARE(clock.subscriberCount(), 1);
        delete second;
        QCOMPARE(clock.subscriberCount(), 0);
        QVERIFY(!clock.isRunning());
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

    void lockedTagsAreFixed() {
        Tag plain(QStringLiteral("Developer"), QColor(0x2f6fde));
        Tag locked(QStringLiteral("Developer"), QColor(0x2f6fde));
        locked.setFilterable(true);
        locked.setLocked(true);
        QVERIFY(!locked.isFilterable());
        QCOMPARE(locked.recipe().iconCode, QStringLiteral("lock"));
        QCOMPARE(locked.accessibleDescription(), QStringLiteral("Locked"));
        // Room for the lock, and the same tint hovered or not.
        QVERIFY(locked.sizeHint().width() > plain.sizeHint().width());
        locked.setPreviewHovered(false);
        QCOMPARE(locked.recipe().background,
                 lockedTagRecipe(QColor(0x2f6fde), Theme::Light).background);
        locked.resize(locked.sizeHint());
        QSignalSpy clicked(&locked, &QAbstractButton::clicked);
        QTest::mouseClick(&locked, Qt::LeftButton, {}, locked.rect().center());
        QCOMPARE(clicked.count(), 0);
        QVERIFY(!locked.grab().isNull());
        // A locked chip on a conversation row is wider by its lock.
        ConversationRow row(QStringLiteral("Cart"));
        row.resize(280, row.sizeHint().height());
        row.setTags({{QStringLiteral("Developer"), QColor(0x2f6fde), true},
                     {QStringLiteral("web"), QColor(0x22aa66)}});
        QCOMPARE(row.visibleTagCount(), 2);
        QVERIFY(!row.grab().isNull());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    WidgetsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_widgets.moc"
