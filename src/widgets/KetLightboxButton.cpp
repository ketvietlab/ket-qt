// SPDX-License-Identifier: MPL-2.0
#include "KetLightboxButton.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>

#include <cmath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

LightboxButton::LightboxButton(const QString& code, const QString& label, QWidget* parent)
    : QAbstractButton(parent), code_(code) {
    setAccessibleName(label);
    setToolTip(label);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void LightboxButton::setFlipped(bool flipped) {
    flipped_ = flipped;
    update();
}

void LightboxButton::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

LightboxToolRecipe LightboxButton::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
    return lightboxToolRecipe(state, Appearance::instance().theme());
}

QSize LightboxButton::sizeHint() const {
    const int side = qRound(scaled(recipe().size));
    if (code_.isEmpty())
        return {qRound(scaled(lightboxRecipe(Appearance::instance().theme()).zoomLevelWidth)),
                side};
    return {side, side};
}

void LightboxButton::paintEvent(QPaintEvent*) {
    const LightboxToolRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const QRectF bounds = rect();
    const qreal radius = bounds.height() / 2;
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(bounds, radius, radius);
    if (r.focusRing.alpha()) {
        // Inside the bounds: Qt Widgets cannot paint the design system's outer outline.
        const qreal width = scaled(r.focusWidth);
        const qreal inset = width / 2;
        painter.setPen(QPen(r.focusRing, width));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(bounds.adjusted(inset, inset, -inset, -inset), radius - inset,
                                radius - inset);
    }
    if (code_.isEmpty()) {
        QFont label = font();
        label.setPixelSize(
            qRound(scaled(lightboxRecipe(Appearance::instance().theme()).counterPixels)));
        label.setFeature(QFont::Tag("tnum"), 1);
        painter.setFont(label);
        painter.setPen(r.glyph);
        painter.drawText(bounds, Qt::AlignCenter, text());
        return;
    }
    const qreal box = scaled(r.iconSize);
    const qreal ratio = devicePixelRatioF();
    const QPixmap glyph = Icons::pixmap(code_, box, ratio, r.glyph);
    // On whole device pixels, as ket::Icon places a glyph.
    const QPointF origin(std::round((bounds.width() - box) / 2 * ratio) / ratio,
                         std::round((bounds.height() - box) / 2 * ratio) / ratio);
    if (flipped_) {
        painter.translate(origin.x() + box, origin.y());
        painter.scale(-1, 1);
        painter.drawPixmap(QPointF(), glyph);
    } else {
        painter.drawPixmap(origin, glyph);
    }
}

} // namespace ket
