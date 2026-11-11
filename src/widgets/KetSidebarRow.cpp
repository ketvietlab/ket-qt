// SPDX-License-Identifier: MPL-2.0
// The sidebar tree's one-line conversation row.
#include "KetSidebarTree.h"

#include "KetProfile.h"
#include "KetSidebarTreeDetail.h"

#include <QtMath>

namespace ket {

using namespace sidebar_tree;

SidebarRow::SidebarRow(const QString& title, QWidget* parent) : QAbstractButton(parent) {
    setText(title);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    frame_.setInterval(16);
    connect(&frame_, &QTimer::timeout, this, qOverload<>(&QWidget::update));
    clock_.start();
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, [this] {
        setFixedHeight(qRound(scaled(recipe().height)));
        updateGeometry();
        update();
    });
    setFixedHeight(qRound(scaled(recipe().height)));
    updateAccessibility();
}

void SidebarRow::setLevel(int level) {
    level_ = qMax(0, level);
    update();
}

void SidebarRow::setNumber(const QString& number) {
    number_ = number;
    updateAccessibility();
    update();
}

void SidebarRow::setStatus(ConversationStatus status) {
    status_ = status;
    updateAccessibility();
    updateSpinner();
    update();
}

void SidebarRow::setStatusLabel(const QString& label) {
    statusLabel_ = label;
    updateAccessibility();
}

void SidebarRow::setStatusDetail(const QString& detail) {
    statusDetail_ = detail;
    updateAccessibility();
}

void SidebarRow::setUnread(bool unread) {
    unread_ = unread;
    updateAccessibility();
    update();
}

bool SidebarRow::isUnreadDotShown() const {
    return unread_ && status_ == ConversationStatus::Completed;
}

void SidebarRow::setRole(const QString& role) {
    role_ = role;
    updateAccessibility();
}

void SidebarRow::setWorktree(const QString& worktree) {
    worktree_ = worktree;
    updateAccessibility();
}

QString SidebarRow::stateText() const {
    return sidebarRowStateText(status_, unread_, statusLabel_, statusDetail_);
}

void SidebarRow::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

SidebarRowRecipe SidebarRow::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{hovered_, isDown(), isEnabled(), hasFocus()});
    // A finished row not read yet draws a heavier title; other states draw the plain one.
    return sidebarRowRecipe(isChecked(), isUnreadDotShown(), state, theme());
}

SidebarTreeColumns SidebarRow::columns() const {
    return sidebarTreeColumns(level_, Appearance::instance().zoom());
}

QFont SidebarRow::textFont(qreal pixels, int weight, bool mono) const {
    QFont font = this->font();
    if (mono)
        if (const QString family = Profile::current().monoFamily(); !family.isEmpty())
            font.setFamilies({family});
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

QString SidebarRow::numberText() const {
    return number_.isEmpty() ? QString() : QStringLiteral("#") + number_;
}

QRect SidebarRow::highlightRect() const {
    const int left = qRound(columns().highlight);
    return {left, 0, width() - left, height()};
}

QRect SidebarRow::markRect() const {
    const int size = qRound(scaled(recipe().markSize));
    return {qRound(columns().mark), (height() - size) / 2, size, size};
}

QRect SidebarRow::titleRect() const {
    const SidebarRowRecipe r = recipe();
    int right = width() - qRound(scaled(r.numberEnd));
    if (!number_.isEmpty()) {
        const QFont font = textFont(r.numberPixels, r.numberWeight, true);
        right -=
            qCeil(QFontMetricsF(font).horizontalAdvance(numberText())) + qRound(scaled(r.titleGap));
    }
    const int left = qRound(columns().text);
    return {left, 0, qMax(0, right - left), height()};
}

QSize SidebarRow::sizeHint() const {
    const SidebarRowRecipe r = recipe();
    const QFont title = textFont(r.titlePixels, r.titleWeight);
    return {qCeil(columns().text + QFontMetricsF(title).horizontalAdvance(text())) +
                qRound(scaled(r.titleGap + r.numberEnd)) +
                qCeil(QFontMetricsF(textFont(r.numberPixels, r.numberWeight, true))
                          .horizontalAdvance(numberText())),
            qRound(scaled(r.height))};
}

QSize SidebarRow::minimumSizeHint() const {
    return {qRound(columns().text) * 2, qRound(scaled(recipe().height))};
}

void SidebarRow::updateAccessibility() {
    setAccessibleName(number_.isEmpty() ? text() : QStringLiteral("%1, #%2").arg(text(), number_));
    const QString description = sidebarRowDescription(role_, stateText(), worktree_);
    setAccessibleDescription(description);
    setToolTip(description);
}

void SidebarRow::paintEvent(QPaintEvent*) {
    const SidebarRowRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    const QRectF highlight(highlightRect());
    if (r.background.alpha()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(highlight, radius, radius);
    }
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(highlight.adjusted(1, 1, -1, -1), radius - 1, radius - 1);
    }

    // The mark: the unread dot once finished and unread, else the state's icon.
    const QRect mark = markRect();
    if (isUnreadDotShown()) {
        const qreal dot = scaled(r.unreadDotSize);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.unreadDot);
        painter.drawEllipse(
            QRectF(QPointF(mark.center()) + QPointF(0.5, 0.5) - QPointF(dot / 2, dot / 2),
                   QSizeF(dot, dot)));
    } else {
        const ConversationStatusRecipe s = conversationStatusRecipe(status_, theme());
        const QPixmap icon = Icons::pixmap(s.icon, mark.width(), devicePixelRatioF(), s.iconColor);
        if (s.spinning && s.spinMs > 0) {
            painter.save();
            painter.translate(QRectF(mark).center());
            painter.rotate(360.0 * qreal(clock_.elapsed() % s.spinMs) / s.spinMs);
            painter.drawPixmap(QPointF(-mark.width() / 2.0, -mark.height() / 2.0), icon);
            painter.restore();
        } else {
            painter.drawPixmap(mark.topLeft(), icon);
        }
    }

    const QRect title = titleRect();
    const QFont titleFont = textFont(r.titlePixels, r.titleWeight);
    painter.setFont(titleFont);
    painter.setPen(r.title);
    painter.drawText(title, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(titleFont).elidedText(text(), Qt::ElideRight, title.width()));
    if (!number_.isEmpty()) {
        const QFont numberFont = textFont(r.numberPixels, r.numberWeight, true);
        painter.setFont(numberFont);
        painter.setPen(r.number);
        const int right = width() - qRound(scaled(r.numberEnd));
        painter.drawText(QRect(title.right() + 1, 0, right - title.right() - 1, height()),
                         Qt::AlignRight | Qt::AlignVCenter, numberText());
    }
}

void SidebarRow::keyPressEvent(QKeyEvent* event) {
    if (activates(event) && !event->isAutoRepeat()) {
        click();
        return;
    }
    QAbstractButton::keyPressEvent(event);
}

void SidebarRow::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void SidebarRow::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void SidebarRow::showEvent(QShowEvent* event) {
    updateSpinner();
    QAbstractButton::showEvent(event);
}

void SidebarRow::hideEvent(QHideEvent* event) {
    frame_.stop();
    QAbstractButton::hideEvent(event);
}

void SidebarRow::updateSpinner() {
    if (isVisible() && conversationStatusRecipe(status_, theme()).spinning)
        frame_.start();
    else
        frame_.stop();
}

} // namespace ket
