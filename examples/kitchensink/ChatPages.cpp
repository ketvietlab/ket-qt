// SPDX-License-Identifier: MPL-2.0
#include "ChatPages.h"

#include "KetActivityGroup.h"
#include "KetActivityRow.h"
#include "KetAppearance.h"
#include "KetAttachmentChip.h"
#include "KetButton.h"
#include "KetChoice.h"
#include "KetComposer.h"
#include "KetGallery.h"
#include "KetIcons.h"
#include "KetMentionPicker.h"
#include "KetMeter.h"
#include "KetPopover.h"
#include "KetQuestionCard.h"
#include "KetSuggestion.h"
#include "KetTextField.h"
#include "KitchenSinkCommon.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QPainter>
#include <QQuickWidget>

namespace ket::kitchensink {
namespace {

QWidget* toolkits(QWidget* widgets, QWidget* quick, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-2")));
    for (const auto& [title, content] :
         {std::pair{"Qt Widgets", widgets}, std::pair{"Qt Quick", quick}}) {
        column->addWidget(new TokenLabel(QString::fromLatin1(title),
                                         QStringLiteral("text-secondary"),
                                         QStringLiteral("text-sm"), 600, host));
        column->addWidget(content, 0, Qt::AlignLeft | Qt::AlignTop);
        column->addSpacing(qRound(Tokens::px(u"space-3")));
    }
    return host;
}

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("ChatSamples"), parent, {{QStringLiteral("mode"), mode}});
}

// The usage panel from the mock's K.usagePanel(), shared by the static and live popovers.
void fillUsage(Popover* popover) {
    popover->addNote(QStringLiteral("Claude · Opus"));
    auto* context = new Meter(QStringLiteral("Current context window"), popover);
    context->setValueText(QStringLiteral("24%"));
    context->setFraction(0.24);
    context->setCaption(QStringLiteral("32,768 of 136,533 tokens · sample provider snapshot"));
    popover->body()->addSpacing(qRound(Appearance::instance().scaled(12)));
    popover->body()->addWidget(context);
    popover->addDivider();
    popover->addHeading(QStringLiteral("Account limits"));
    for (const auto& [label, value] :
         {std::pair{"5-hour usage", "18%"}, std::pair{"Weekly usage", "32%"}}) {
        auto* meter = new Meter(QString::fromLatin1(label), popover);
        meter->setValueText(QString::fromLatin1(value));
        popover->body()->addSpacing(qRound(Appearance::instance().scaled(12)));
        popover->body()->addWidget(meter);
    }
    popover->body()->addSpacing(qRound(Appearance::instance().scaled(12)));
    popover->addNote(QStringLiteral("Account limits belong to the selected provider, not this "
                                    "conversation."));
    auto* compact = new Button(QStringLiteral("Compact session"), ButtonVariant::Ghost, popover);
    compact->setIcon(Icons::icon(u"layers"));
    popover->addFooterWidget(compact);
}

QWidget* usage(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-8")));
    auto* sample = new Popover(QStringLiteral("Context & limits"), host);
    fillUsage(sample);
    sample->setFixedSize(sample->sizeHint());
    row->addWidget(sample, 0, Qt::AlignTop);

    // Click the ring to open the same panel above it.
    auto* live = new QWidget(host);
    auto* liveColumn = new QVBoxLayout(live);
    liveColumn->setContentsMargins(0, 0, 0, 0);
    liveColumn->addStretch(1);
    liveColumn->addWidget(new TokenLabel(QStringLiteral("Click the ring"),
                                         QStringLiteral("text-muted"), QStringLiteral("text-sm"),
                                         400, live));
    auto* ring = new UsageRing(live);
    ring->setValue(0.24);
    liveColumn->addWidget(ring, 0, Qt::AlignRight);
    auto* popover = new Popover(QStringLiteral("Context & limits"), live);
    popover->hide();
    fillUsage(popover);
    QObject::connect(ring, &QAbstractButton::clicked, popover, [ring, popover] {
        if (popover->isVisible())
            popover->dismiss();
        else
            popover->openAbove(ring);
    });
    live->setFixedSize(220, sample->sizeHint().height());
    row->addWidget(live, 0, Qt::AlignBottom);
    row->addStretch(1);
    return host;
}

class Sidebar final : public QWidget {
  public:
    using QWidget::QWidget;

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        const Theme theme = Appearance::instance().theme();
        painter.fillRect(rect(), Tokens::color(u"sidebar-bg", theme));
        painter.setPen(Tokens::color(u"sidebar-border", theme));
        painter.drawLine(width() - 1, 0, width() - 1, height());
    }
};

QWidget* quota(QWidget* parent) {
    auto* pane = new Sidebar(parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(19, 12, 19, 12);
    column->setSpacing(16);
    for (const auto& [value, fraction] :
         {std::pair{"32%", 0.32}, std::pair{"86%", 0.86}, std::pair{"97%", 0.97}}) {
        auto* meter = new Meter(QStringLiteral("Weekly usage"), pane);
        meter->setCompact(true);
        meter->setValueText(QString::fromLatin1(value));
        meter->setFraction(fraction);
        meter->setCaption(QStringLiteral("Resets Monday 09:00"));
        column->addWidget(meter);
    }
    pane->setFixedWidth(280);
    return pane;
}

QWidget* attachments(QWidget* parent) {
    auto* field = new Composer(parent);
    field->addAttachment(QStringLiteral("design-notes.md"));
    field->addAttachment(QStringLiteral("sidebar-two-lines.png"), QStringLiteral("image"));
    field->addAttachment(
        QStringLiteral("a-very-long-attachment-name-that-elides-in-the-middle.txt"));
    auto* attach =
        new IconButton(Icons::icon(u"plus"), QStringLiteral("Add images or text files"), field);
    field->leadingTools()->addWidget(attach);
    int next = 1;
    QObject::connect(attach, &QAbstractButton::clicked, field, [field, next]() mutable {
        field->addAttachment(QStringLiteral("notes-%1.md").arg(next++));
    });
    field->setFixedWidth(620);
    return field;
}

QWidget* suggestions(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(8);
    const struct {
        const char* title;
        const char* caption;
        const char* icon;
    } items[] = {{"Review the open pull request", "Summarise risky changes first", "branch"},
                 {"Find why startup is slow", "Profile the first 2 seconds", "search"},
                 {"Write release notes", "From the commits since the last tag", "file"}};
    for (const auto& item : items) {
        auto* suggestion =
            new Suggestion(QString::fromLatin1(item.title), QString::fromLatin1(item.caption),
                           QString::fromLatin1(item.icon), host);
        column->addWidget(suggestion);
    }
    static_cast<Suggestion*>(column->itemAt(1)->widget())
        ->setPreviewState(InteractionState{true, false, true, false});
    host->setFixedWidth(490);
    return host;
}

QWidget* question(QWidget* parent) {
    auto* card =
        new QuestionCard(QStringLiteral("How much detail should each status show?"), parent);
    card->setStep(QStringLiteral("1 / 2"));
    auto* group = new QButtonGroup(card);
    for (const char* title : {"Status and last update", "Status only"}) {
        auto* choice = new Choice(QString::fromLatin1(title), {}, ToggleKind::Radio, card);
        group->addButton(choice);
        card->body()->addWidget(choice);
    }
    group->buttons().constFirst()->setChecked(true);
    auto* other = new TextField(card);
    other->setPlaceholderText(QStringLiteral("Or enter another answer…"));
    card->body()->addWidget(other);
    card->footer()->addStretch(1);
    card->footer()->addWidget(new Button(QStringLiteral("Next"), ButtonVariant::Primary, card));
    card->setFixedWidth(620);
    return card;
}

QList<GalleryImage> screenshots() {
    QList<GalleryImage> images;
    for (const char* name : {"sidebar", "editor", "settings", "dialog", "terminal"})
        images.append({QPixmap(QStringLiteral(":/kitchensink/images/screenshot-%1.png")
                                   .arg(QString::fromLatin1(name))),
                       QStringLiteral("%1.png").arg(QString::fromLatin1(name))});
    return images;
}

QWidget* activity(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(0);
    auto* open = new ActivityGroup(QStringLiteral("Explored 4 files, ran 2 commands"), host);
    auto* read =
        new ActivityRow(QStringLiteral("Read ConversationRow.qml"), QStringLiteral("file"), open);
    open->addRow(read);
    auto* run = new ActivityRow(QStringLiteral("Ran ctest"), QStringLiteral("terminal"), open);
    run->setDetail(QStringLiteral("8 passed"));
    open->addRow(run);
    auto* shots =
        new ActivityRow(QStringLiteral("Captured 5 screenshots"), QStringLiteral("image"), open);
    open->addRow(shots);
    auto* gallery = new Gallery(open);
    gallery->setImages(screenshots());
    gallery->setMaxVisible(3);
    open->addRow(gallery);
    open->setExpanded(true);
    column->addWidget(open);
    auto* closed = new ActivityGroup(QStringLiteral("Edited 2 files"), host);
    closed->addRow(
        new ActivityRow(QStringLiteral("Edited KetComposer.cpp"), QStringLiteral("edit"), closed));
    closed->setPreviewState(InteractionState{true, false, true, false});
    column->addWidget(closed);
    host->setFixedWidth(620);
    return host;
}

} // namespace

QWidget* chatPage() {
    PageFrame frame = makePage(
        QStringLiteral("Chat extras"),
        QStringLiteral("The usage popover and meters, attachment chips, welcome suggestions, the "
                       "question card and activity groups with screenshots, from "
                       "ket::popoverRecipe, meterRecipe, attachmentChipRecipe, suggestionRecipe, "
                       "questionCardRecipe, activityGroupRecipe and galleryRecipe."));
    const auto section = [&](const char* title, QWidget* widgets, const char* mode) {
        frame.body->addWidget(sectionTitle(QString::fromUtf8(title), frame.page));
        frame.body->addWidget(
            toolkits(widgets, quick(QString::fromLatin1(mode), frame.page), frame.page));
    };
    section("Usage popover · live", usage(frame.page), "usage");
    section("Sidebar quota", quota(frame.page), "quota");
    section("Composer attachments · live", attachments(frame.page), "attachments");
    section("Suggestions", suggestions(frame.page), "suggestions");
    section("Question card", question(frame.page), "question");
    section("Activity group", activity(frame.page), "activity");
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
