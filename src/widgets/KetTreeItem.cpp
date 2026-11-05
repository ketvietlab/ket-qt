// SPDX-License-Identifier: MPL-2.0
#include "KetTreeItem.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

void paintFocus(QPainter& painter, const QRectF& rect, const QColor& ring, qreal radius) {
    if (!ring.alpha())
        return;
    painter.setPen(QPen(ring, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), radius, radius);
}

// `.repository-group-name`: the folder icon and the name.
class GroupToggle final : public QAbstractButton {
  public:
    explicit GroupToggle(QWidget* parent) : QAbstractButton(parent) {
        setCheckable(true);
        setChecked(true);
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    QString iconCode;
    bool selected = false;

    [[nodiscard]] GroupHeaderRecipe recipe() const {
        return groupHeaderRecipe(selected,
                                 InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()},
                                 Appearance::instance().theme());
    }

    [[nodiscard]] QFont textFont() const {
        const GroupHeaderRecipe r = recipe();
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(r.fontPixels)));
        font.setWeight(QFont::Weight(r.fontWeight));
        return font;
    }

    [[nodiscard]] QSize sizeHint() const override {
        const GroupHeaderRecipe r = recipe();
        return {qCeil(scaled(r.iconSize) + scaled(r.nameGap) +
                      QFontMetricsF(textFont()).horizontalAdvance(text())) +
                    2,
                qRound(scaled(r.height))};
    }

    [[nodiscard]] QSize minimumSizeHint() const override {
        return {qRound(scaled(recipe().iconSize) * 3), sizeHint().height()};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const GroupHeaderRecipe r = recipe();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        paintFocus(painter, QRectF(rect()), r.focusRing, scaled(Tokens::px(u"radius-sm")));
        const qreal icon = scaled(r.iconSize);
        const QString code = !iconCode.isEmpty() ? iconCode
                             : isChecked()       ? QStringLiteral("folder-open")
                                                 : QStringLiteral("folder");
        painter.drawPixmap(QPointF(1, qRound((height() - icon) / 2)),
                           Icons::pixmap(code, icon, devicePixelRatioF(), r.text));
        const qreal x = 1 + icon + scaled(r.nameGap);
        const QFont font = textFont();
        painter.setFont(font);
        painter.setPen(r.text);
        painter.drawText(QRectF(x, 0, width() - x - 1, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetricsF(font).elidedText(text(), Qt::ElideRight, width() - x - 1));
    }
    void enterEvent(QEnterEvent* event) override {
        update();
        QAbstractButton::enterEvent(event);
    }
    void leaveEvent(QEvent* event) override {
        update();
        QAbstractButton::leaveEvent(event);
    }
};

} // namespace

TreeItem::TreeItem(const QString& title, const QString& iconCode, QWidget* parent)
    : QAbstractButton(parent), iconCode_(iconCode) {
    setText(title);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void TreeItem::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

void TreeItem::setDetail(const QString& detail) {
    detail_ = detail;
    setAccessibleDescription(secondLine());
    updateGeometry();
    update();
}

void TreeItem::setActivity(const QString& text, TreeActivity activity) {
    activityText_ = text;
    activity_ = activity;
    setAccessibleDescription(secondLine());
    updateGeometry();
    update();
}

void TreeItem::setCount(const QString& count) {
    count_ = count;
    update();
}

void TreeItem::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

QString TreeItem::secondLine() const {
    return activity_ != TreeActivity::None ? activityText_ : detail_;
}

TreeItemRecipe TreeItem::recipe() const {
    const InteractionState state = preview_.value_or(InteractionState{
        hovered_, isDown(), isEnabled(), hasFocus() && focusPolicy() != Qt::NoFocus});
    return treeItemRecipe(isChecked(), state, Appearance::instance().theme());
}

QFont TreeItem::sizedFont(qreal pixels, int weight) const {
    QFont font = QAbstractButton::font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

QSize TreeItem::sizeHint() const {
    const TreeItemRecipe r = recipe();
    const QFontMetricsF title(sizedFont(r.titlePixels, r.titleWeight));
    qreal height = title.height();
    if (!secondLine().isEmpty()) {
        const bool activity = activity_ != TreeActivity::None;
        height += activity ? scaled(r.activityOffset) +
                                 QFontMetricsF(sizedFont(r.activityPixels)).height()
                           : QFontMetricsF(sizedFont(r.detailPixels)).height();
    }
    const qreal width = 2 * scaled(r.paddingInline) + scaled(r.iconSize) + scaled(r.spacing) +
                        title.horizontalAdvance(text());
    return {qCeil(width), qCeil(height + 2 * scaled(r.paddingBlock))};
}

QSize TreeItem::minimumSizeHint() const {
    return {qRound(scaled(Tokens::px(u"space-16")) * 2), sizeHint().height()};
}

void TreeItem::paintEvent(QPaintEvent*) {
    const TreeItemRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()), radius, radius);
    paintFocus(painter, QRectF(rect()), r.focusRing, radius);

    const qreal top = scaled(r.paddingBlock);
    qreal x = scaled(r.paddingInline);
    qreal right = width() - scaled(r.paddingInline);
    const qreal icon = scaled(r.iconSize);
    if (!iconCode_.isEmpty())
        painter.drawPixmap(QPointF(qRound(x), qRound(top + scaled(r.iconOffset))),
                           Icons::pixmap(iconCode_, icon, dpr, r.icon));
    x += icon + scaled(r.spacing);
    if (!count_.isEmpty()) {
        const QFont countFont = sizedFont(r.countPixels);
        const qreal countWidth = QFontMetricsF(countFont).horizontalAdvance(count_);
        painter.setFont(countFont);
        painter.setPen(r.count);
        painter.drawText(
            QRectF(right - countWidth, top, countWidth, QFontMetricsF(countFont).height()),
            Qt::AlignRight, count_);
        right -= countWidth + scaled(r.spacing);
    }
    const QFont title = sizedFont(r.titlePixels, r.titleWeight);
    const QFontMetricsF titleMetrics(title);
    painter.setFont(title);
    painter.setPen(r.title);
    painter.drawText(QRectF(x, top, right - x, titleMetrics.height()), Qt::AlignLeft,
                     titleMetrics.elidedText(text(), Qt::ElideRight, right - x));
    qreal y = top + titleMetrics.height();
    if (activity_ != TreeActivity::None) {
        const QColor color = activity_ == TreeActivity::Working ? r.working : r.waiting;
        const QFont small = sizedFont(r.activityPixels);
        const QFontMetricsF metrics(small);
        y += scaled(r.activityOffset);
        const qreal box = scaled(r.activityIconSize);
        painter.drawPixmap(
            QPointF(qRound(x), qRound(y + (metrics.height() - box) / 2)),
            Icons::pixmap(activity_ == TreeActivity::Working ? u"loading" : u"pause-circle", box,
                          dpr, color));
        const qreal left = x + box + scaled(r.activityGap);
        painter.setFont(small);
        painter.setPen(color);
        painter.drawText(QRectF(left, y, right - left, metrics.height()), Qt::AlignLeft,
                         metrics.elidedText(activityText_, Qt::ElideRight, right - left));
    } else if (!detail_.isEmpty()) {
        const QFont small = sizedFont(r.detailPixels);
        const QFontMetricsF metrics(small);
        painter.setFont(small);
        painter.setPen(r.detail);
        painter.drawText(QRectF(x, y, right - x, metrics.height()), Qt::AlignLeft,
                         metrics.elidedText(detail_, Qt::ElideRight, right - x));
    }
}

void TreeItem::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void TreeItem::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

GroupHeader::GroupHeader(const QString& name, QWidget* parent)
    : QWidget(parent), toggle_(new GroupToggle(this)),
      action_(new IconButton(Icons::icon(u"plus"), tr("New conversation"), this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toggle_->setText(name);
    toggle_->setAccessibleName(name);
    action_->setControlSize(ControlSize::Small);
    action_->installEventFilter(this);
    toggle_->installEventFilter(this);
    auto* row = new QHBoxLayout(this);
    row->addWidget(toggle_, 0);
    row->addStretch(1);
    row->addWidget(action_);
    connect(toggle_, &QAbstractButton::toggled, this, &GroupHeader::toggled);
    connect(action_, &QAbstractButton::clicked, this, &GroupHeader::actionTriggered);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &GroupHeader::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &GroupHeader::applyRecipe);
    applyRecipe();
    updateAction();
}

QString GroupHeader::name() const { return toggle_->text(); }

void GroupHeader::setName(const QString& name) {
    toggle_->setText(name);
    toggle_->setAccessibleName(name);
    toggle_->updateGeometry();
}

bool GroupHeader::isExpanded() const { return toggle_->isChecked(); }

void GroupHeader::setExpanded(bool expanded) { toggle_->setChecked(expanded); }

void GroupHeader::setIconCode(const QString& code) {
    static_cast<GroupToggle*>(toggle_)->iconCode = code;
    toggle_->update();
}

void GroupHeader::setSelected(bool selected) {
    static_cast<GroupToggle*>(toggle_)->selected = selected;
    toggle_->update();
}

void GroupHeader::applyRecipe() {
    const GroupHeaderRecipe r = static_cast<GroupToggle*>(toggle_)->recipe();
    auto* row = static_cast<QHBoxLayout*>(layout());
    row->setContentsMargins(qRound(scaled(r.paddingStart)), 0, qRound(scaled(r.paddingEnd)), 0);
    row->setSpacing(qRound(scaled(r.spacing)));
    const int action = qRound(scaled(r.actionSize));
    action_->setFixedSize(action, action);
    setFixedHeight(qRound(scaled(r.height)));
    toggle_->updateGeometry();
    toggle_->update();
}

void GroupHeader::updateAction() {
    // `.repository-group-header .icon-btn` is transparent until the header is hovered or
    // focused; it keeps its place so the name does not move.
    const bool shown = hovered_ || action_->hasFocus() || toggle_->hasFocus();
    QSizePolicy policy = action_->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    action_->setSizePolicy(policy);
    action_->setVisible(shown);
}

void GroupHeader::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    updateAction();
    QWidget::enterEvent(event);
}

void GroupHeader::leaveEvent(QEvent* event) {
    hovered_ = false;
    updateAction();
    QWidget::leaveEvent(event);
}

bool GroupHeader::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == action_ || watched == toggle_) &&
        (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut))
        // Queued: Tab from the name moves focus to the action after the name's FocusOut.
        QMetaObject::invokeMethod(this, &GroupHeader::updateAction, Qt::QueuedConnection);
    return QWidget::eventFilter(watched, event);
}

} // namespace ket
