// SPDX-License-Identifier: MPL-2.0
#include "KetActivityRow.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

ActivityRow::ActivityRow(const QString& text, const QString& iconCode, QWidget* parent)
    : QAbstractButton(parent), iconCode_(iconCode) {
    setText(text);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void ActivityRow::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

void ActivityRow::setDetail(const QString& detail) {
    detail_ = detail;
    updateGeometry();
    update();
}

void ActivityRow::setExpandable(bool expandable) {
    setCheckable(expandable);
    setAccessibleDescription(expandable ? tr("Expandable") : QString());
    update();
}

void ActivityRow::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

ActivityRowRecipe ActivityRow::recipe() const {
    const InteractionState state = preview_.value_or(InteractionState{
        hovered_, isDown(), isEnabled(), hasFocus() && focusPolicy() != Qt::NoFocus});
    return activityRowRecipe(state, Appearance::instance().theme());
}

QSize ActivityRow::sizeHint() const {
    const ActivityRowRecipe r = recipe();
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.fontPixels)));
    qreal width = 2 * scaled(r.paddingInline) + QFontMetricsF(font).horizontalAdvance(text());
    if (!iconCode_.isEmpty())
        width += scaled(r.iconSize) + scaled(r.spacing);
    if (isCheckable())
        width += scaled(r.chevronSize) + scaled(r.spacing);
    return {qCeil(width), qRound(scaled(r.height))};
}

QSize ActivityRow::minimumSizeHint() const {
    return {qRound(scaled(recipe().height)), sizeHint().height()};
}

void ActivityRow::paintEvent(QPaintEvent*) {
    const ActivityRowRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()), radius, radius);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
    }
    qreal x = scaled(r.paddingInline);
    qreal right = width() - scaled(r.paddingInline);
    if (!iconCode_.isEmpty()) {
        const qreal box = scaled(r.iconSize);
        painter.drawPixmap(QPointF(qRound(x), qRound((height() - box) / 2)),
                           Icons::pixmap(iconCode_, box, dpr, r.text));
        x += box + scaled(r.spacing);
    }
    if (isCheckable()) {
        const qreal box = scaled(r.chevronSize);
        painter.drawPixmap(
            QPointF(qRound(right - box), qRound((height() - box) / 2)),
            Icons::pixmap(isChecked() ? u"chevron-up" : u"chevron-down", box, dpr, r.text));
        right -= box + scaled(r.spacing);
    }
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.fontPixels)));
    const QFontMetricsF metrics(font);
    const qreal textWidth = qMin(metrics.horizontalAdvance(text()), right - x);
    painter.setFont(font);
    painter.setPen(r.text);
    painter.drawText(QRectF(x, 0, textWidth, height()), Qt::AlignLeft | Qt::AlignVCenter,
                     metrics.elidedText(text(), Qt::ElideRight, textWidth));
    if (!detail_.isEmpty() && textWidth < right - x) {
        QFont small = this->font();
        small.setPixelSize(qRound(scaled(r.detailPixels)));
        const qreal left = x + textWidth + scaled(r.spacing);
        painter.setFont(small);
        painter.setPen(r.detail);
        painter.drawText(QRectF(left, 0, right - left, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetricsF(small).elidedText(detail_, Qt::ElideRight, right - left));
    }
}

void ActivityRow::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ActivityRow::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

} // namespace ket
