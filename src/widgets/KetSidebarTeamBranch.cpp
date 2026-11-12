// SPDX-License-Identifier: MPL-2.0
// The sidebar tree's team branch.
#include "KetSidebarTree.h"

#include "KetBadge.h"
#include "KetButton.h"
#include "KetSidebarTreeDetail.h"

#include <QEvent>
#include <QtMath>

namespace ket {
namespace {

using namespace sidebar_tree;

// A control of a team branch that draws nothing itself: the branch paints it from its
// state, so the highlight, the chevron and their focus rings come from one recipe.
class BranchButton final : public QAbstractButton {
  public:
    explicit BranchButton(QWidget* parent) : QAbstractButton(parent) {
        setFocusPolicy(Qt::StrongFocus);
    }

  protected:
    void paintEvent(QPaintEvent*) override {}
    void keyPressEvent(QKeyEvent* event) override {
        if (activates(event) && !event->isAutoRepeat()) {
            click();
            return;
        }
        QAbstractButton::keyPressEvent(event);
    }
    bool event(QEvent* event) override {
        switch (event->type()) {
        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            parentWidget()->update();
            break;
        default:
            break;
        }
        return QAbstractButton::event(event);
    }
};

} // namespace

using namespace sidebar_tree;

SidebarTeamBranch::SidebarTeamBranch(const QString& name, QWidget* parent)
    : QWidget(parent), fold_(new BranchButton(this)), name_(new BranchButton(this)),
      add_(new IconButton(Icons::icon(u"plus"), tr("Add member"), this)),
      menu_(new IconButton(Icons::icon(u"more"), tr("Team actions"), this)),
      count_(new Badge({}, BadgeTone::Neutral, this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    fold_->setCheckable(true);
    fold_->setChecked(true);
    name_->setText(name);
    name_->setCursor(Qt::PointingHandCursor);
    for (IconButton* action : {add_, menu_}) {
        action->setControlSize(ControlSize::Small);
        action->installEventFilter(this);
        retainWhenHidden(action);
    }
    fold_->installEventFilter(this);
    name_->installEventFilter(this);
    count_->setAttribute(Qt::WA_TransparentForMouseEvents);
    count_->setText(QStringLiteral("0"));
    connect(fold_, &QAbstractButton::toggled, this, [this](bool expanded) {
        updateAccessibility();
        update();
        emit toggled(expanded);
    });
    connect(name_, &QAbstractButton::clicked, this, &SidebarTeamBranch::activated);
    connect(add_, &QAbstractButton::clicked, this, &SidebarTeamBranch::addTriggered);
    connect(menu_, &QAbstractButton::clicked, this, &SidebarTeamBranch::menuTriggered);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            &SidebarTeamBranch::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this,
            &SidebarTeamBranch::applyRecipe);
    applyRecipe();
    updateAccessibility();
}

QString SidebarTeamBranch::name() const { return name_->text(); }

void SidebarTeamBranch::setName(const QString& name) {
    name_->setText(name);
    updateAccessibility();
    update();
}

void SidebarTeamBranch::setLevel(int level) {
    level_ = qMax(0, level);
    layoutParts();
    update();
}

bool SidebarTeamBranch::isExpanded() const { return fold_->isChecked(); }

void SidebarTeamBranch::setExpanded(bool expanded) { fold_->setChecked(expanded); }

void SidebarTeamBranch::setMemberCount(int count) {
    members_ = qMax(0, count);
    count_->setText(QString::number(members_));
    updateAccessibility();
    layoutParts();
}

void SidebarTeamBranch::setDecisionCount(int count) {
    decisions_ = qMax(0, count);
    updateAccessibility();
    layoutParts();
    update();
}

void SidebarTeamBranch::setSelected(bool selected) {
    selected_ = selected;
    update();
}

void SidebarTeamBranch::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    layoutParts();
    update();
}

InteractionState SidebarTeamBranch::nameState() const {
    return preview_.value_or(
        InteractionState{name_->underMouse(), name_->isDown(), isEnabled(), name_->hasFocus()});
}

SidebarTeamRecipe SidebarTeamBranch::recipe() const {
    return sidebarTeamRecipe(selected_, decisions_ > 0, nameState(), theme());
}

SidebarTreeColumns SidebarTeamBranch::columns() const {
    return sidebarTreeColumns(level_, Appearance::instance().zoom());
}

bool SidebarTeamBranch::hot() const {
    if (preview_)
        return preview_->hovered || preview_->focused;
    return hovered_ || fold_->hasFocus() || name_->hasFocus() || add_->hasFocus() ||
           menu_->hasFocus();
}

QFont SidebarTeamBranch::nameFont() const {
    const SidebarTeamRecipe r = recipe();
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.namePixels)));
    font.setWeight(QFont::Weight(r.nameWeight));
    return font;
}

QRect SidebarTeamBranch::highlightRect() const {
    const int left = qRound(columns().highlight);
    return {left, 0, width() - left, height()};
}

QRect SidebarTeamBranch::markRect() const {
    return squareAt(columns().mark, rect(), scaled(recipe().iconSize));
}

QRect SidebarTeamBranch::nameRect() const {
    const SidebarTeamRecipe r = recipe();
    const int gap = qRound(scaled(r.trailingGap));
    int right = count_->x() - gap;
    if (decisions_ > 0)
        right -= qRound(scaled(r.iconSize)) + gap;
    if (hot())
        right = add_->x() - gap;
    const int left = qRound(columns().text);
    return {left, 0, qMax(0, right - left), height()};
}

QSize SidebarTeamBranch::sizeHint() const {
    const SidebarTeamRecipe r = recipe();
    return {qCeil(columns().text + QFontMetricsF(nameFont()).horizontalAdvance(name())) +
                add_->sizeHint().width() * 2 + count_->sizeHint().width() +
                qRound(scaled(r.trailingEnd)),
            qRound(scaled(r.height))};
}

QSize SidebarTeamBranch::minimumSizeHint() const {
    return {qRound(columns().text) * 2, sizeHint().height()};
}

void SidebarTeamBranch::applyRecipe() {
    setFixedHeight(qRound(scaled(recipe().height)));
    updateGeometry();
    layoutParts();
    update();
}

void SidebarTeamBranch::layoutParts() {
    const SidebarTeamRecipe r = recipe();
    const SidebarTreeColumns c = columns();
    const QRect row = rect();
    // The chevron's control spans from as far left of the chevron as the highlight is right
    // of it, so the chevron sits in its middle.
    const int highlight = qRound(c.highlight);
    const int pad = qMax(0, highlight - qRound(c.chevron) - qRound(scaled(r.chevronSize)));
    const int foldLeft = qRound(c.chevron) - pad;
    fold_->setGeometry(foldLeft, 0, highlight - foldLeft, row.height());
    name_->setGeometry(highlightRect());
    const int gap = qRound(scaled(r.trailingGap));
    int right = width() - qRound(scaled(r.trailingEnd));
    const QSize badge = count_->sizeHint();
    count_->setGeometry(right - badge.width(), (row.height() - badge.height()) / 2, badge.width(),
                        badge.height());
    right = count_->x() - gap;
    if (decisions_ > 0)
        right -= qRound(scaled(r.iconSize)) + gap;
    for (IconButton* action : {menu_, add_}) {
        const QSize size = action->sizeHint();
        action->setGeometry(right - size.width(), (row.height() - size.height()) / 2, size.width(),
                            size.height());
        right = action->x() - gap;
    }
    const bool shown = hot();
    add_->setVisible(shown);
    menu_->setVisible(shown);
    count_->raise();
}

void SidebarTeamBranch::updateAccessibility() {
    const QString team = name();
    fold_->setAccessibleName(isExpanded() ? tr("Fold %1").arg(team) : tr("Unfold %1").arg(team));
    name_->setAccessibleName(tr("Team %1").arg(team));
    name_->setAccessibleDescription(sidebarTeamDescription(members_, decisions_));
}

void SidebarTeamBranch::paintEvent(QPaintEvent*) {
    const SidebarTeamRecipe r = recipe();
    const SidebarTreeColumns c = columns();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    const QRect highlight = highlightRect();
    if (r.highlight.alpha()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.highlight);
        painter.drawRoundedRect(QRectF(highlight), radius, radius);
    }
    paintFocus(painter, QRectF(highlight), r.focusRing, radius);
    const InteractionState foldState = preview_
                                           ? InteractionState{}
                                           : InteractionState{fold_->underMouse(), fold_->isDown(),
                                                              isEnabled(), fold_->hasFocus()};
    const SidebarTeamRecipe fold = sidebarTeamRecipe(selected_, decisions_ > 0, foldState, theme());
    paintFocus(painter, QRectF(fold_->geometry()), fold.focusRing,
               scaled(Tokens::px(u"radius-sm")));
    drawIcon(painter, this, sidebarChevronIcon(isExpanded()),
             squareAt(c.chevron, rect(), scaled(r.chevronSize)), r.chevron);
    drawIcon(painter, this, QStringLiteral("users"), markRect(), r.icon);
    const QRect name = nameRect();
    const QFont font = nameFont();
    painter.setFont(font);
    painter.setPen(r.name);
    painter.drawText(name, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(font).elidedText(this->name(), Qt::ElideRight, name.width()));
    if (decisions_ > 0) {
        const qreal icon = scaled(r.iconSize);
        drawIcon(painter, this, r.waitingIcon,
                 squareAt(count_->x() - scaled(r.trailingGap) - qRound(icon), rect(), icon),
                 r.waitingColor);
    }
}

void SidebarTeamBranch::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    layoutParts();
}

void SidebarTeamBranch::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    layoutParts();
    update();
    QWidget::enterEvent(event);
}

void SidebarTeamBranch::leaveEvent(QEvent* event) {
    hovered_ = false;
    layoutParts();
    update();
    QWidget::leaveEvent(event);
}

bool SidebarTeamBranch::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == fold_ || watched == name_ || watched == add_ || watched == menu_) &&
        (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut))
        QMetaObject::invokeMethod(this, &SidebarTeamBranch::layoutParts, Qt::QueuedConnection);
    return QWidget::eventFilter(watched, event);
}

} // namespace ket
