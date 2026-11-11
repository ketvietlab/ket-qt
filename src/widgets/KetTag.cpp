// SPDX-License-Identifier: MPL-2.0
#include "KetTag.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QtMath>

namespace ket {

Tag::Tag(const QString& text, const QColor& color, QWidget* parent)
    : QAbstractButton(parent), color_(color) {
    setText(text);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    setAttribute(Qt::WA_Hover);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Tag::setColor(const QColor& color) {
    color_ = color;
    update();
}

void Tag::setOverflow(bool overflow) {
    if (overflow_ == overflow)
        return;
    overflow_ = overflow;
    updateGeometry();
    update();
}

void Tag::setFilterable(bool filterable) {
    filterable_ = filterable;
    setCursor(filterable ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
}

void Tag::setLocked(bool locked) {
    if (locked_ == locked)
        return;
    locked_ = locked;
    if (locked)
        setFilterable(false);
    setAccessibleDescription(locked ? tr("Locked") : QString{});
    updateGeometry();
    update();
}

bool Tag::hitButton(const QPoint& position) const {
    return !locked_ && QAbstractButton::hitButton(position);
}

void Tag::setPreviewHovered(std::optional<bool> hovered) {
    previewHovered_ = hovered;
    update();
}

TagRecipe Tag::recipe() const {
    if (locked_ && !overflow_)
        return lockedTagRecipe(color_, Appearance::instance().theme());
    const bool hovered = previewHovered_.value_or(filterable_ && underMouse());
    return tagRecipe(color_, overflow_, hovered, Appearance::instance().theme());
}

QFont Tag::tagFont() const {
    const TagRecipe r = recipe();
    QFont result = font();
    result.setPixelSize(qRound(Appearance::instance().scaled(r.fontPixels)));
    result.setWeight(QFont::Weight(r.fontWeight));
    return result;
}

QSize Tag::sizeHint() const {
    const TagRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    // Round the advance up so the label is never elided at its own size hint.
    const int icon = r.iconCode.isEmpty()
                         ? 0
                         : qCeil(appearance.scaled(r.iconSize) + appearance.scaled(r.iconGap));
    const int width = qCeil(QFontMetricsF(tagFont()).horizontalAdvance(text())) +
                      2 * qRound(appearance.scaled(r.paddingInline)) + icon;
    return {qMin(width, qRound(appearance.scaled(r.maxWidth))),
            qRound(appearance.scaled(r.height))};
}

QSize Tag::minimumSizeHint() const {
    const TagRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    return {qRound(appearance.scaled(r.height)), qRound(appearance.scaled(r.height))};
}

void Tag::paintEvent(QPaintEvent*) {
    const TagRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const qreal radius = appearance.scaled(r.radius);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(rect(), radius, radius);
    const QFont textFont = tagFont();
    const int padding = qRound(appearance.scaled(r.paddingInline));
    QRect textRect = rect().adjusted(padding, 0, -padding, 0);
    if (!r.iconCode.isEmpty()) {
        const qreal icon = appearance.scaled(r.iconSize);
        painter.drawPixmap(QPointF(padding, (height() - icon) / 2),
                           Icons::pixmap(r.iconCode, icon, devicePixelRatioF(), r.text));
        textRect.setLeft(textRect.left() + qCeil(icon + appearance.scaled(r.iconGap)));
    }
    painter.setFont(textFont);
    painter.setPen(r.text);
    painter.drawText(textRect, Qt::AlignCenter,
                     QFontMetrics(textFont).elidedText(text(), Qt::ElideRight, textRect.width()));
}

} // namespace ket
