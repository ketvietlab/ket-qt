// SPDX-License-Identifier: MPL-2.0
// Chat extras: the recipes and the Qt Widgets controls for the usage popover and meters,
// attachment chips, suggestions, question cards, activity groups and galleries.
#include "KetActivityGroup.h"
#include "KetActivityRow.h"
#include "KetAppearance.h"
#include "KetAttachmentChip.h"
#include "KetButton.h"
#include "KetComposer.h"
#include "KetGallery.h"
#include "KetMeter.h"
#include "KetPopover.h"
#include "KetQuestionCard.h"
#include "KetStyle.h"
#include "KetSuggestion.h"

#include <QAbstractButton>
#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>

using namespace ket;

class ChatTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void recipesFollowTheKetPlusMock() {
        const PopoverRecipe popover = popoverRecipe(Theme::Light);
        QCOMPARE(popover.width, 340.0);
        QCOMPARE(popover.padding, 18.0);
        QCOMPARE(popover.border, Tokens::color(u"border-default", Theme::Light));
        QCOMPARE(meterRecipe(true, 0.3, Theme::Light).trackHeight, 4.0);
        QCOMPARE(meterRecipe(false, 0.3, Theme::Light).trackHeight, 6.0);
        QCOMPARE(meterRecipe(false, 0.3, Theme::Light).fill,
                 Tokens::color(u"accent", Theme::Light));
        QCOMPARE(meterRecipe(false, 0.85, Theme::Light).fill,
                 Tokens::color(u"warning", Theme::Light));
        QCOMPARE(meterRecipe(false, 0.96, Theme::Light).fill,
                 Tokens::color(u"danger", Theme::Light));
        QCOMPARE(meterRecipe(true, 0, Theme::Light).label,
                 Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(attachmentChipRecipe(false, Theme::Light).background,
                 Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(attachmentChipRecipe(true, Theme::Light).remove,
                 Tokens::color(u"accent-text", Theme::Light));
        const InteractionState hovered{true, false, true, false};
        QCOMPARE(suggestionRecipe(hovered, Theme::Light).border,
                 Tokens::color(u"accent-border", Theme::Light));
        QCOMPARE(suggestionRecipe({}, Theme::Light).border,
                 Tokens::color(u"panel-border", Theme::Light));
        QCOMPARE(questionCardRecipe(Theme::Light).padding, 17.0);
        QCOMPARE(activityGroupRecipe({}, Theme::Light).height, 36.0);
        QCOMPARE(activityGroupRecipe(hovered, Theme::Light).background,
                 Tokens::color(u"surface-hover", Theme::Light));
        QCOMPARE(galleryRecipe(false, Theme::Light).tileSize, 88.0);
        QCOMPARE(galleryRecipe(true, Theme::Light).border,
                 Tokens::color(u"accent-border", Theme::Light));
    }

    void popoversOpenAboveTheirAnchorAndClose() {
        QWidget window;
        window.resize(800, 600);
        auto* anchor = new QPushButton(QStringLiteral("Usage"), &window);
        anchor->setGeometry(600, 500, 40, 30);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        Popover popover(QStringLiteral("Context & limits"));
        popover.addNote(QStringLiteral("Claude · Opus"));
        popover.addDivider();
        popover.addHeading(QStringLiteral("Account limits"));
        popover.addFooterWidget(new Button(QStringLiteral("Compact session")));
        QSignalSpy closed(&popover, &Popover::closed);
        popover.openAbove(anchor);
        QVERIFY(popover.isVisible());
        QCOMPARE(popover.parentWidget(), &window);
        const QRect surface = popover.geometry().marginsRemoved(popover.shadowMargins());
        QCOMPARE(surface.width(), 340);
        QCOMPARE(surface.right(), anchor->geometry().right());
        QCOMPARE(surface.bottom() + 1 + 12, anchor->geometry().top());
        QTest::keyClick(&popover, Qt::Key_Escape);
        QVERIFY(!popover.isVisible());
        QCOMPARE(closed.count(), 1);
        popover.openAbove(anchor);
        QTest::mouseClick(&window, Qt::LeftButton, {}, QPoint(20, 20));
        QVERIFY(!popover.isVisible());
        popover.openAbove(anchor);
        popover.findChild<IconButton*>()->click();
        QVERIFY(!popover.isVisible());
        QCOMPARE(closed.count(), 3);
    }

    void metersHideTheTrackWithoutAFraction() {
        Meter meter(QStringLiteral("Weekly usage"));
        meter.setValueText(QStringLiteral("32%"));
        const int plain = meter.heightForWidth(300);
        meter.setFraction(0.32);
        QCOMPARE(meter.heightForWidth(300), plain + 8 + 6 + 4);
        meter.setCompact(true);
        QCOMPARE(meter.heightForWidth(300), plain + 8 + 4 + 4);
        meter.setCaption(QStringLiteral("Resets Monday"));
        QVERIFY(meter.heightForWidth(300) > plain + 16);
        meter.setFraction(3);
        QCOMPARE(meter.fraction(), 1.0);
        meter.resize(300, meter.heightForWidth(300));
        QVERIFY(!meter.grab().isNull());
    }

    void composersAddAndRemoveAttachments() {
        Composer composer;
        composer.resize(600, composer.sizeHint().height());
        composer.show();
        QSignalSpy removed(&composer, &Composer::attachmentRemoved);
        const int before = composer.sizeHint().height();
        composer.addAttachment(QStringLiteral("design-notes.md"));
        AttachmentChip* image =
            composer.addAttachment(QStringLiteral("shot.png"), QStringLiteral("image"));
        QCOMPARE(composer.attachments(),
                 QStringList({QStringLiteral("design-notes.md"), QStringLiteral("shot.png")}));
        QVERIFY(composer.sizeHint().height() > before);
        QCOMPARE(image->sizeHint().height(), 20);
        image->resize(image->sizeHint());
        QTest::mouseClick(image, Qt::LeftButton, {}, image->removeRect().center());
        QCOMPARE(removed.count(), 1);
        QCOMPARE(removed.first().at(0).toInt(), 1);
        QCOMPARE(removed.first().at(1).toString(), QStringLiteral("shot.png"));
        auto* notes = composer.findChildren<AttachmentChip*>().constFirst();
        notes->setFocus();
        QTest::keyClick(notes, Qt::Key_Delete);
        QCOMPARE(removed.count(), 2);
        QVERIFY(composer.attachments().isEmpty());
    }

    void longAttachmentNamesElideAtTheMaxWidth() {
        AttachmentChip chip(QString(200, QLatin1Char('a')));
        QCOMPARE(chip.sizeHint().width(), 220);
    }

    void suggestionsAndQuestionCardsLayOut() {
        Suggestion suggestion(QStringLiteral("Review the open pull request"),
                              QStringLiteral("Summarise risky changes first"),
                              QStringLiteral("branch"));
        QSignalSpy clicked(&suggestion, &QAbstractButton::clicked);
        suggestion.resize(490, suggestion.sizeHint().height());
        QVERIFY(suggestion.sizeHint().height() >= 2 * 11 + 17);
        QTest::mouseClick(&suggestion, Qt::LeftButton);
        QCOMPARE(clicked.count(), 1);
        QVERIFY(!suggestion.grab().isNull());

        QuestionCard card(QStringLiteral("How much detail should each status show?"));
        card.setStep(QStringLiteral("1 / 2"));
        card.body()->addWidget(new QLabel(QStringLiteral("answer")));
        card.footer()->addWidget(new Button(QStringLiteral("Next")));
        card.resize(620, card.sizeHint().height());
        QCOMPARE(card.step(), QStringLiteral("1 / 2"));
        QCOMPARE(card.layout()->contentsMargins().left(), 18); // padding plus the border
        QVERIFY(!card.grab().isNull());
    }

    void activityGroupsToggleTheirRows() {
        ActivityGroup group(QStringLiteral("Explored 4 files"));
        auto* row = new ActivityRow(QStringLiteral("Read file"), QStringLiteral("file"));
        group.addRow(row);
        group.show();
        QVERIFY(!row->isVisible());
        QSignalSpy toggled(&group, &ActivityGroup::toggled);
        group.summary()->click();
        QVERIFY(group.isExpanded());
        QVERIFY(row->isVisible());
        QCOMPARE(toggled.count(), 1);
        QCOMPARE(group.summary()->sizeHint().height(), 36);
        group.setExpanded(false);
        QVERIFY(!row->isVisible());
    }

    void galleriesLimitAndExpand() {
        Gallery gallery;
        QList<GalleryImage> images;
        for (int i = 0; i < 5; ++i) {
            QPixmap pixmap(40, 30);
            pixmap.fill(Qt::gray);
            images.append({pixmap, QStringLiteral("shot-%1.png").arg(i)});
        }
        gallery.setImages(images);
        gallery.setMaxVisible(3);
        QCOMPARE(gallery.tiles().size(), 4); // three thumbnails and +2
        QSignalSpy activated(&gallery, &Gallery::activated);
        gallery.resize(620, gallery.heightForWidth(620));
        qobject_cast<QAbstractButton*>(gallery.tiles().at(1))->click();
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.first().first().toInt(), 1);
        qobject_cast<QAbstractButton*>(gallery.tiles().last())->click();
        QVERIFY(gallery.isExpanded());
        QCOMPARE(gallery.tiles().size(), 5);
        // Five 92 px tiles (88 plus the ring) with 8 px gaps fit one row of 620 px.
        QVERIFY(gallery.heightForWidth(620) < gallery.heightForWidth(200));
        QVERIFY(!gallery.grab().isNull());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    ChatTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_chat.moc"
