// SPDX-License-Identifier: MPL-2.0
#include "SidebarTreePages.h"

#include "KetAppearance.h"
#include "KetSidebarTree.h"
#include "KitchenSinkCommon.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPainter>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

// The sidebar's surface, with its border.
class SidebarPane final : public QWidget {
  public:
    explicit SidebarPane(QWidget* parent) : QWidget(parent) {
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
        connect(&Appearance::instance(), &Appearance::zoomChanged, this, &SidebarPane::applyZoom);
        auto* column = new QVBoxLayout(this);
        column->setSpacing(0);
        applyZoom();
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        const Theme theme = Appearance::instance().theme();
        painter.fillRect(rect(), Tokens::color(u"sidebar-bg", theme));
        painter.setPen(Tokens::color(u"sidebar-border", theme));
        painter.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
    }

  private:
    // The sidebar's width and inset, at the interface zoom.
    void applyZoom() {
        const Appearance& appearance = Appearance::instance();
        const int inset = qRound(appearance.scaled(Tokens::px(u"space-2")));
        layout()->setContentsMargins(inset, inset, inset, inset);
        setFixedWidth(qRound(appearance.scaled(Tokens::px(u"sidebar-width"))) + 2 * inset);
    }
};

SidebarRow* row(const QString& title, const QString& number, ConversationStatus status, int level,
                QButtonGroup* group) {
    auto* item = new SidebarRow(title);
    item->setNumber(number);
    item->setStatus(status);
    item->setLevel(level);
    group->addButton(item);
    return item;
}

// The tree of the mock: a repository with its conversations and a team whose decision
// waits, then a folded repository with something waiting.
QWidget* tree(QWidget* parent) {
    auto* pane = new SidebarPane(parent);
    auto* column = static_cast<QVBoxLayout*>(pane->layout());
    auto* group = new QButtonGroup(pane);

    auto* repository = new SidebarRepositoryHeader(QStringLiteral("ketplus"), pane);
    repository->setConversationCount(6);
    repository->setSelected(true);
    auto* parts = new SidebarTreeGuide(0, pane);
    auto* section = new SidebarSectionLabel(QStringLiteral("Conversations"));
    section->setCount(2);
    parts->addPart(section);
    SidebarRow* open = row(QStringLiteral("Fix the transcript anchors"), QStringLiteral("2035"),
                           ConversationStatus::Working, 0, group);
    open->setChecked(true);
    open->setWorktree(QStringLiteral("fix/transcript-anchors"));
    parts->addPart(open);
    SidebarRow* asks = row(QStringLiteral("Release notes for 1.4"), QStringLiteral("2110"),
                           ConversationStatus::NeedsInput, 0, group);
    asks->setStatusDetail(QStringLiteral("Allow git push?"));
    parts->addPart(asks);

    auto* team = new SidebarTeamBranch(QStringLiteral("Mobile Refactor"));
    team->setMemberCount(3);
    team->setDecisionCount(1);
    parts->addPart(team);
    auto* members = new SidebarTreeGuide(1);
    SidebarRow* leader = row(QStringLiteral("Lead Mobile Refactor"), QStringLiteral("9692"),
                             ConversationStatus::AwaitingReply, 1, group);
    leader->setRole(QStringLiteral("Leader"));
    members->addPart(leader);
    SidebarRow* done = row(QStringLiteral("ADR and contract for teams"), QStringLiteral("3651"),
                           ConversationStatus::Completed, 1, group);
    done->setUnread(true);
    done->setRole(QStringLiteral("Developer"));
    done->setWorktree(QStringLiteral("team/3/t-3"));
    members->addPart(done);
    SidebarRow* failed = row(QStringLiteral("Reviewer"), QStringLiteral("6913"),
                             ConversationStatus::Error, 1, group);
    failed->setRole(QStringLiteral("Reviewer"));
    members->addPart(failed);
    parts->addPart(members);
    QObject::connect(team, &SidebarTeamBranch::toggled, members, &QWidget::setVisible);
    QObject::connect(repository, &QAbstractButton::toggled, parts, &QWidget::setVisible);

    auto* other = new SidebarRepositoryHeader(QStringLiteral("ket-qt"), pane);
    other->setDivided(true);
    other->setConversationCount(2);
    other->setWaitingCount(1);
    other->setExpanded(false);

    column->addWidget(repository);
    column->addWidget(parts);
    column->addWidget(other);
    column->addStretch(1);
    return pane;
}

// Every state of each part, at rest, hovered, focused, selected and disabled.
QWidget* states(QWidget* parent) {
    auto* pane = new SidebarPane(parent);
    auto* column = static_cast<QVBoxLayout*>(pane->layout());
    auto* group = new QButtonGroup(pane);
    group->setExclusive(false);
    const InteractionState hover{true, false, true, false};
    const InteractionState focus{false, false, true, true};

    for (const auto& [label, state] : {std::pair{"Hovered", hover}, std::pair{"Focused", focus}}) {
        auto* header = new SidebarRepositoryHeader(QString::fromLatin1(label), pane);
        header->setConversationCount(1);
        header->setPreviewState(state);
        column->addWidget(header);
    }
    for (const auto& [label, selected, state] :
         {std::tuple{"Team, hovered", false, hover}, std::tuple{"Team, focused", false, focus},
          std::tuple{"Team, selected", true, InteractionState{}}}) {
        auto* team = new SidebarTeamBranch(QString::fromLatin1(label), pane);
        team->setMemberCount(2);
        team->setSelected(selected);
        team->setPreviewState(state);
        column->addWidget(team);
    }
    const struct {
        const char* title;
        ConversationStatus status;
        std::optional<InteractionState> state;
        bool checked;
        bool unread;
    } rows[] = {
        {"Not started", ConversationStatus::NotStarted, std::nullopt, false, false},
        {"Hovered", ConversationStatus::Stopped, hover, false, false},
        {"Focused", ConversationStatus::Working, focus, false, false},
        {"Selected", ConversationStatus::Completed, std::nullopt, true, false},
        {"Finished, unread", ConversationStatus::Completed, std::nullopt, false, true},
        {"Disabled", ConversationStatus::NotStarted, InteractionState{false, false, false, false},
         false, false},
    };
    for (const auto& sample : rows) {
        SidebarRow* item =
            row(QString::fromLatin1(sample.title), QStringLiteral("1024"), sample.status, 0, group);
        item->setChecked(sample.checked);
        item->setUnread(sample.unread);
        item->setPreviewState(sample.state);
        if (sample.state && !sample.state->enabled)
            item->setEnabled(false);
        column->addWidget(item);
    }
    column->addStretch(1);
    return pane;
}

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("SidebarTreeSamples"), parent,
                     {{QStringLiteral("mode"), mode}});
}

QWidget* pair(QWidget* widgets, QWidget* quickView, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-6")));
    for (const auto& [title, content] :
         {std::pair{"Qt Widgets", widgets}, std::pair{"Qt Quick", quickView}}) {
        auto* column = new QVBoxLayout;
        column->setSpacing(qRound(Tokens::px(u"space-2")));
        column->addWidget(new TokenLabel(QString::fromLatin1(title),
                                         QStringLiteral("text-secondary"),
                                         QStringLiteral("text-sm"), 600, host));
        column->addWidget(content, 0, Qt::AlignLeft | Qt::AlignTop);
        column->addStretch(1);
        row->addLayout(column);
    }
    row->addStretch(1);
    return host;
}

} // namespace

QWidget* sidebarTreePage() {
    PageFrame frame = makePage(
        QStringLiteral("Sidebar tree"),
        QStringLiteral("A repository header, a team branch, a section label and one-line "
                       "conversation rows, with guide lines under each chevron. Every part "
                       "places its mark and text in the columns of its level "
                       "(ket::sidebarTreeColumns), so the text edges hold at every zoom. "
                       "Colours and metrics come from ket::sidebar*Recipe and "
                       "Profile::sidebarTree."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Tree"), frame.page));
    frame.body->addWidget(
        pair(tree(frame.page), quick(QStringLiteral("tree"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("States"), frame.page));
    frame.body->addWidget(
        pair(states(frame.page), quick(QStringLiteral("states"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
