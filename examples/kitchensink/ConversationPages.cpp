// SPDX-License-Identifier: MPL-2.0
#include "ConversationPages.h"

#include "KetActivityRow.h"
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetComposer.h"
#include "KetConversationRow.h"
#include "KetIcons.h"
#include "KetTranscript.h"
#include "KitchenSinkCommon.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPainter>
#include <QQuickWidget>
#include <QVBoxLayout>

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
    return quickView(QStringLiteral("ConversationSamples"), parent,
                     {{QStringLiteral("mode"), mode}});
}

class Pane final : public QWidget {
  public:
    Pane(QStringView surface, QWidget* parent) : QWidget(parent), surface_(surface.toString()) {
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        const Theme theme = Appearance::instance().theme();
        painter.fillRect(rect(), Tokens::color(surface_, theme));
        painter.setPen(Tokens::color(u"panel-border", theme));
        painter.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
    }

  private:
    QString surface_;
};

// The mock's tag palette (conversation-tags.css), light then dark.
QColor tagColor(int index) {
    static const QColor light[] = {QColor(0x2f6fde), QColor(0x23875a), QColor(0x7a55d6),
                                   QColor(0xb7791f)};
    static const QColor dark[] = {QColor(0x7aa7ff), QColor(0x5fcf95), QColor(0xb39dff),
                                  QColor(0xf0b45a)};
    return Appearance::instance().dark() ? dark[index] : light[index];
}

QWidget* sidebar(QWidget* parent) {
    auto* pane = new Pane(u"sidebar-bg", parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(10, 10, 10, 10);
    column->setSpacing(2);
    auto* group = new QButtonGroup(pane);
    const auto add = [&](const QString& title, const QString& number, ConversationStatus status) {
        auto* row = new ConversationRow(title, pane);
        row->setNumber(number);
        row->setStatus(status);
        group->addButton(row);
        column->addWidget(row);
        return row;
    };
    add(QStringLiteral("Navigation rail polish"), QStringLiteral("4821"),
        ConversationStatus::Working)
        ->setTags({{QStringLiteral("ui"), tagColor(0)}});
    auto* selected = add(QStringLiteral("Startup time regression"), QStringLiteral("1307"),
                         ConversationStatus::Completed);
    selected->setTags({{QStringLiteral("perf"), tagColor(3)},
                       {QStringLiteral("review"), tagColor(1)},
                       {QStringLiteral("account"), tagColor(2)},
                       {QStringLiteral("ui"), tagColor(0)}});
    selected->setChecked(true);
    add(QStringLiteral("Atlas bundle validation"), QStringLiteral("5590"),
        ConversationStatus::NeedsInput);
    add(QStringLiteral("Sync conflict on resume"), QStringLiteral("7142"),
        ConversationStatus::Error);
    add(QStringLiteral("Release checklist"), QStringLiteral("2208"), ConversationStatus::Stopped)
        ->setPreviewState(InteractionState{true, false, true, false});
    add(QStringLiteral("Draft a new conversation"), QStringLiteral("9031"),
        ConversationStatus::NotStarted);
    column->addStretch();
    pane->setFixedSize(300, 340);
    return pane;
}

QWidget* composer(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    // Idle; busy with guidance typed (the circle sends); busy and empty (the circle stops).
    for (const auto& [busy, typed] :
         {std::pair{false, false}, std::pair{true, true}, std::pair{true, false}}) {
        auto* field = new Composer(host);
        auto* attach = new IconButton(Icons::icon(u"plus", IconWeight::Regular),
                                      QStringLiteral("Add images or text files"), field);
        attach->setGlyphSize(composerRecipe(false, Appearance::instance().theme()).attachIconSize);
        field->leadingTools()->addWidget(attach);
        auto* mode = new Button(QStringLiteral("Ask before edits"), ButtonVariant::Ghost, field);
        mode->setIcon(Icons::icon(u"shield"));
        mode->setControlSize(ControlSize::Small);
        field->leadingTools()->addWidget(mode);
        auto* model = new Button(QStringLiteral("Claude · Opus"), ButtonVariant::Ghost, field);
        model->setControlSize(ControlSize::Small);
        field->trailingTools()->addWidget(model);
        if (typed)
            field->setText(QStringLiteral("Also keep the +N chip when tags overflow."));
        field->setBusy(busy);
        field->setFixedWidth(620);
        column->addWidget(field);
    }
    return host;
}

QWidget* transcript(QWidget* parent) {
    auto* pane = new Pane(u"panel-bg", parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(20, 16, 20, 16);
    column->setSpacing(0);
    column->addWidget(new DayDivider(QStringLiteral("Today"), pane));
    column->addWidget(new UserMessage(
        QStringLiteral("Make the sidebar rows two lines: the title, then the number and the "
                       "tags. Hide whole tags that do not fit and count them in +N."),
        pane));
    auto* assistant = new AssistantMessage(QStringLiteral("Claude"), QStringLiteral("10:42"), pane);
    auto* body = new QWidget;
    auto* bodyColumn = new QVBoxLayout(body);
    bodyColumn->setContentsMargins(0, 0, 0, 0);
    bodyColumn->setSpacing(2);
    auto* read = new ActivityRow(QStringLiteral("Read 3 files"), QStringLiteral("file"), body);
    read->setDetail(QStringLiteral("ConversationRow.qml, conversation-tags.css"));
    bodyColumn->addWidget(read);
    auto* edit =
        new ActivityRow(QStringLiteral("Edited ConversationRow.qml"), QStringLiteral("edit"), body);
    edit->setExpandable(true);
    edit->setPreviewState(InteractionState{true, false, true, false});
    bodyColumn->addWidget(edit);
    auto* run = new ActivityRow(QStringLiteral("Ran ctest"), QStringLiteral("terminal"), body);
    run->setDetail(QStringLiteral("7 passed"));
    bodyColumn->addWidget(run);
    bodyColumn->addSpacing(8);
    bodyColumn->addWidget(new ReadingText(
        QStringLiteral("Rows now show the number and as many whole tags as fit, with +N for the "
                       "rest. Without tags the meta line shows the status."),
        body));
    assistant->setBody(body);
    column->addWidget(assistant);
    pane->setFixedWidth(720);
    return pane;
}

} // namespace

QWidget* conversationPage() {
    PageFrame frame = makePage(
        QStringLiteral("Conversation"),
        QStringLiteral("Sidebar conversation rows, the composer and transcript blocks, from "
                       "ket::conversationRowRecipe, composerRecipe, userMessageRecipe, "
                       "assistantMessageRecipe, dayDividerRecipe and activityRowRecipe."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Conversation rows"), frame.page));
    frame.body->addWidget(
        toolkits(sidebar(frame.page), quick(QStringLiteral("rows"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Composer · live"), frame.page));
    frame.body->addWidget(
        toolkits(composer(frame.page), quick(QStringLiteral("composer"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Transcript"), frame.page));
    frame.body->addWidget(toolkits(transcript(frame.page),
                                   quick(QStringLiteral("transcript"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
