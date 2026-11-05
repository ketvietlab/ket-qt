// SPDX-License-Identifier: MPL-2.0
#include "NavigationPages.h"

#include "KetActivityIndicator.h"
#include "KetAppearance.h"
#include "KetEmptyState.h"
#include "KetIcons.h"
#include "KetNavItem.h"
#include "KetSegmentedControl.h"
#include "KitchenSinkCommon.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QProgressBar>
#include <QQuickWidget>
#include <QTabBar>
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
    return quickView(QStringLiteral("NavigationSamples"), parent, {{QStringLiteral("mode"), mode}});
}

// A surface with a border, standing in for the sidebar or an editor pane.
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

QWidget* sidebar(QWidget* parent) {
    auto* pane = new Pane(u"sidebar-bg", parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(10, 10, 10, 10);
    column->setSpacing(4);
    auto* group = new QButtonGroup(pane);
    const auto add = [&](const QString& text, const QString& icon) {
        auto* item = new NavItem(text, icon, pane);
        group->addButton(item);
        column->addWidget(item);
        return item;
    };
    add(QStringLiteral("New conversation"), QStringLiteral("compose"))
        ->setShortcutText(QStringLiteral("⌘N"));
    add(QStringLiteral("Search"), QStringLiteral("search"))->setShortcutText(QStringLiteral("⌘K"));
    add(QStringLiteral("Conversations"), QStringLiteral("chat"))->setChecked(true);
    add(QStringLiteral("Worktrees"), QStringLiteral("folder"))->setCount(4);
    add(QStringLiteral("Atlas"), QStringLiteral("map"))
        ->setPreviewState(InteractionState{true, false, true, false});
    add(QStringLiteral("Archived"), QStringLiteral("archive"))->setEnabled(false);
    column->addStretch();
    pane->setFixedSize(260, 260);
    return pane;
}

QWidget* tabs(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));

    auto* row = new QHBoxLayout;
    row->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* providers = new SegmentedControl(
        {QStringLiteral("Claude"), QStringLiteral("Codex"), QStringLiteral("Cursor")}, host);
    providers->setPreviewHovered(2);
    row->addWidget(providers);
    auto* views = new SegmentedControl({QStringLiteral("Map"), QStringLiteral("List")}, host);
    views->setCurrentIndex(1);
    row->addWidget(views);
    auto* disabled = new SegmentedControl({QStringLiteral("On"), QStringLiteral("Off")}, host);
    disabled->setEnabled(false);
    row->addWidget(disabled);
    row->addStretch();
    column->addLayout(row);

    auto* expanding = new SegmentedControl({QStringLiteral("Claude"), QStringLiteral("Codex"),
                                            QStringLiteral("Cursor"), QStringLiteral("Gemini")},
                                           host);
    expanding->setExpanding(true);
    expanding->setFixedWidth(460);
    column->addWidget(expanding);

    auto* bar = new QTabBar(host);
    bar->setDocumentMode(true);
    bar->setExpanding(false);
    bar->setTabsClosable(true);
    bar->addTab(Icons::icon(u"file-code"), QStringLiteral("NavigationRail.qml"));
    bar->addTab(Icons::icon(u"file"), QStringLiteral("README.md"));
    bar->addTab(Icons::icon(u"file-code"), QStringLiteral("ThemeManager.cpp"));
    bar->setCurrentIndex(0);
    QObject::connect(bar, &QTabBar::tabCloseRequested, bar, &QTabBar::removeTab);
    auto* strip = new Pane(u"panel-bg-subtle", host);
    auto* stripLayout = new QHBoxLayout(strip);
    stripLayout->setContentsMargins(0, 0, 0, 0);
    stripLayout->addWidget(bar);
    stripLayout->addStretch();
    strip->setFixedWidth(620);
    column->addWidget(strip);
    return host;
}

QWidget* status(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* grid = new QGridLayout(host);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(qRound(Tokens::px(u"space-4")));
    grid->setVerticalSpacing(qRound(Tokens::px(u"space-3")));
    const struct {
        const char* label;
        int value;
        int maximum;
        bool enabled;
    } bars[] = {{"0%", 0, 100, true},
                {"40%", 40, 100, true},
                {"100%", 100, 100, true},
                {"Busy", 0, 0, true},
                {"Disabled", 60, 100, false}};
    int row = 0;
    for (const auto& sample : bars) {
        grid->addWidget(new TokenLabel(QString::fromLatin1(sample.label),
                                       QStringLiteral("text-muted"), QStringLiteral("text-xs"), 500,
                                       host),
                        row, 0);
        auto* bar = new QProgressBar(host);
        bar->setRange(0, sample.maximum);
        bar->setValue(sample.value);
        bar->setTextVisible(false);
        bar->setEnabled(sample.enabled);
        bar->setFixedWidth(320);
        grid->addWidget(bar, row++, 1);
    }
    grid->addWidget(new TokenLabel(QStringLiteral("Working"), QStringLiteral("text-muted"),
                                   QStringLiteral("text-xs"), 500, host),
                    row, 0);
    auto* working = new QHBoxLayout;
    working->setSpacing(qRound(Tokens::px(u"space-4")));
    working->addWidget(new ActivityIndicator(host));
    auto* frozen = new ActivityIndicator(host);
    frozen->setPreviewTime(400); // mid-animation for screenshots
    working->addWidget(frozen);
    auto* accent = new ActivityIndicator(host);
    accent->setColor(Tokens::color(u"accent", Appearance::instance().theme()));
    working->addWidget(accent);
    working->addStretch();
    grid->addLayout(working, row++, 1);

    auto* pane = new Pane(u"panel-bg", host);
    auto* paneLayout = new QVBoxLayout(pane);
    auto* empty =
        new EmptyState(QStringLiteral("success"), QStringLiteral("Your worktree is clean."),
                       QStringLiteral("There are no uncommitted changes in this snapshot."), pane);
    empty->setAction(QStringLiteral("Back to editor"), QStringLiteral("arrow-left"));
    paneLayout->addWidget(empty);
    pane->setFixedSize(560, 300);
    grid->addWidget(pane, row, 0, 1, 2);
    return host;
}

} // namespace

QWidget* navigationPage() {
    PageFrame frame = makePage(
        QStringLiteral("Navigation and status"),
        QStringLiteral("Sidebar rows, kbd, segmented controls, tab strips, progress, the working "
                       "indicator and empty states, from ket::navItemRecipe, segmentRecipe, "
                       "tabRecipe, progressRecipe, activityRecipe and emptyStateRecipe. "
                       "ket::Style draws every top QTabBar and horizontal QProgressBar."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Sidebar rows"), frame.page));
    frame.body->addWidget(
        toolkits(sidebar(frame.page), quick(QStringLiteral("nav"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Segmented controls and tabs"), frame.page));
    frame.body->addWidget(
        toolkits(tabs(frame.page), quick(QStringLiteral("tabs"), frame.page), frame.page));
    frame.body->addWidget(
        sectionTitle(QStringLiteral("Progress, activity and empty state"), frame.page));
    frame.body->addWidget(
        toolkits(status(frame.page), quick(QStringLiteral("status"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
