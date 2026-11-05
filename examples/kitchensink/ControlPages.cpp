// SPDX-License-Identifier: MPL-2.0
#include "ControlPages.h"

#include "KetAppearance.h"
#include "KetAvatar.h"
#include "KetButton.h"
#include "KetChrome.h"
#include "KetIcons.h"
#include "KetRouteTabs.h"
#include "KetStepSlider.h"
#include "KetTreeItem.h"
#include "KitchenSinkCommon.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QPainter>
#include <QQuickWidget>
#include <QSlider>

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
    return quickView(QStringLiteral("ControlSamples"), parent, {{QStringLiteral("mode"), mode}});
}

class Surface final : public QWidget {
  public:
    Surface(QStringView token, QWidget* parent) : QWidget(parent), token_(token.toString()) {}

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        const Theme theme = Appearance::instance().theme();
        painter.fillRect(rect(), Tokens::color(token_, theme));
        painter.setPen(Tokens::color(u"panel-border", theme));
        painter.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
    }

  private:
    QString token_;
};

QWidget* sliders(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(20);
    auto* effort = new StepSlider(QStringLiteral("Reasoning effort"), host);
    effort->setSteps({QStringLiteral("low"), QStringLiteral("medium"), QStringLiteral("high"),
                      QStringLiteral("xhigh")});
    effort->setCurrentIndex(2);
    column->addWidget(effort);
    auto* row = new QHBoxLayout;
    row->setSpacing(24);
    for (const auto& [value, enabled] : {std::pair{30, true}, std::pair{70, false}}) {
        auto* slider = new QSlider(Qt::Horizontal, host);
        slider->setRange(0, 100);
        slider->setValue(value);
        slider->setEnabled(enabled);
        row->addWidget(slider);
    }
    column->addLayout(row);
    host->setFixedWidth(420);
    return host;
}

QWidget* avatars(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(12);
    for (const char* name : {"Kieu Duy", "Jay", "Linh Tran"})
        row->addWidget(new Avatar(QString::fromLatin1(name), host));
    auto* large = new Avatar(QStringLiteral("Kieu Duy"), host);
    large->setDiameter(40);
    row->addWidget(large);
    auto* photo = new Avatar(QStringLiteral("Sidebar"), host);
    photo->setImage(QPixmap(QStringLiteral(":/kitchensink/images/screenshot-sidebar.png")));
    row->addWidget(photo);
    row->addStretch(1);
    return host;
}

QWidget* sidebar(QWidget* parent) {
    auto* pane = new Surface(u"sidebar-bg", parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(10, 10, 10, 10);
    column->setSpacing(0);
    auto* header = new GroupHeader(QStringLiteral("ketplus"), pane);
    header->setSelected(true);
    column->addWidget(header);
    auto* group = new QButtonGroup(pane);
    const auto item = [&](const char* title, const char* icon) {
        auto* tree = new TreeItem(QString::fromLatin1(title), QString::fromLatin1(icon), pane);
        group->addButton(tree);
        column->addSpacing(3);
        column->addWidget(tree);
        return tree;
    };
    auto* develop = item("develop", "branch");
    develop->setDetail(QStringLiteral("~/dev/ketplus"));
    develop->setCount(QStringLiteral("3"));
    develop->setChecked(true);
    item("feat/new-organization", "branch")
        ->setActivity(QStringLiteral("Working · 2 conversations"), TreeActivity::Working);
    item("fix/sync-resume", "branch")
        ->setActivity(QStringLiteral("Waiting for input"), TreeActivity::Waiting);
    item("release/1.4", "branch")->setPreviewState(InteractionState{true, false, true, false});
    column->addSpacing(10);
    auto* remote = new GroupHeader(QStringLiteral("ket-viewer · studio-mac"), pane);
    remote->setIconCode(QStringLiteral("display"));
    remote->setExpanded(false);
    column->addWidget(remote);
    column->addStretch(1);
    pane->setFixedSize(300, 330);
    return pane;
}

QWidget* tabs(QWidget* parent) {
    auto* route = new RouteTabs(parent);
    for (const char* label : {"General", "Providers", "Appearance", "Updates"})
        route->addTab(QString::fromLatin1(label));
    route->setCurrentIndex(1);
    route->setFixedWidth(520);
    return route;
}

QWidget* chrome(QWidget* parent) {
    auto* pane = new Surface(u"panel-bg", parent);
    auto* column = new QVBoxLayout(pane);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(0);
    auto* header =
        new PaneHeader(QStringLiteral("ConversationRow.qml"), QStringLiteral("file-code"), pane);
    auto* close = new IconButton(Icons::icon(u"close"), QStringLiteral("Close pane"), header);
    close->setControlSize(ControlSize::Small);
    header->actions()->addWidget(close);
    column->addWidget(header);
    auto* crumb = new Breadcrumb(pane);
    crumb->setPath({QStringLiteral("modules"), QStringLiteral("ai"), QStringLiteral("src"),
                    QStringLiteral("agent"), QStringLiteral("qml"),
                    QStringLiteral("ConversationRow.qml")});
    auto* diff = new Button(QStringLiteral("View diff"), ButtonVariant::Ghost, crumb);
    diff->setIcon(Icons::icon(u"compare"));
    diff->setControlSize(ControlSize::Small);
    crumb->actions()->addWidget(diff);
    column->addWidget(crumb);
    column->addStretch(1);
    auto* status = new StatusBar(pane);
    status->addItem(QStringLiteral("Local workspace"), QStringLiteral("success"));
    status->addItem(QStringLiteral("UTF-8"), {}, StatusBar::Side::Right);
    status->addItem(QStringLiteral("LF"), {}, StatusBar::Side::Right);
    status->addItem(QStringLiteral("KétPlus Pro"), {}, StatusBar::Side::Right);
    status->addItem({}, QStringLiteral("keyboard"), StatusBar::Side::Right);
    column->addWidget(status);
    pane->setFixedSize(620, 180);
    return pane;
}

} // namespace

QWidget* controlPage() {
    PageFrame frame = makePage(
        QStringLiteral("Controls and chrome"),
        QStringLiteral("Sliders, avatars, sidebar tree items and group headers, route tabs, the "
                       "pane header, breadcrumb and status bar, from ket::sliderRecipe, "
                       "avatarRecipe, treeItemRecipe, groupHeaderRecipe, routeTabRecipe, "
                       "paneHeaderRecipe, breadcrumbRecipe and statusBarRecipe."));
    const auto section = [&](const char* title, QWidget* widgets, const char* mode) {
        frame.body->addWidget(sectionTitle(QString::fromUtf8(title), frame.page));
        frame.body->addWidget(
            toolkits(widgets, quick(QString::fromLatin1(mode), frame.page), frame.page));
    };
    section("Sliders · live", sliders(frame.page), "sliders");
    section("Avatars", avatars(frame.page), "avatars");
    section("Sidebar tree · live", sidebar(frame.page), "sidebar");
    section("Route tabs · live", tabs(frame.page), "tabs");
    section("Pane header, breadcrumb and status bar", chrome(frame.page), "chrome");
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
