// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetAppearance.h"
#include "KetRecipes.h"

#include <QPainter>
#include <QRectF>

namespace ket::detail {

// Approximates a CSS box shadow under a rounded rectangle with concentric translucent
// layers, so in-window overlays need no QGraphicsEffect. Lengths are already scaled.
inline void paintShadow(QPainter& painter, const QRectF& rect, qreal radius, const Shadow& shadow) {
    if (!shadow.color.isValid() || shadow.color.alpha() == 0 || shadow.blur <= 0)
        return;
    const int layers = qBound(4, qRound(shadow.blur / 2), 16);
    const qreal spread = shadow.blur / 2;
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    // Equal layers from `spread` inside the edge to `spread` outside it: the coverage
    // ramps linearly across the blur and is half the colour at the edge, as CSS's is.
    QColor color = shadow.color;
    color.setAlphaF(float(shadow.color.alphaF() / layers));
    painter.setBrush(color);
    for (int i = 0; i < layers; ++i) {
        const qreal grow = spread * (2.0 * (i + 0.5) / layers - 1);
        const QRectF layer = rect.translated(shadow.x, shadow.y).adjusted(-grow, -grow, grow, grow);
        if (layer.isEmpty())
            continue;
        const qreal r = qMax<qreal>(0, radius + grow);
        painter.drawRoundedRect(layer, r, r);
    }
    painter.restore();
}

// The room a shadow needs around its rectangle: left, top, right, bottom.
inline QMarginsF shadowMargins(const Shadow& shadow) {
    const qreal spread = shadow.blur / 2;
    return {qMax<qreal>(0, spread - shadow.x), qMax<qreal>(0, spread - shadow.y),
            qMax<qreal>(0, spread + shadow.x), qMax<qreal>(0, spread + shadow.y)};
}

[[nodiscard]] inline Shadow scaled(Shadow shadow) {
    const Appearance& appearance = Appearance::instance();
    shadow.x = appearance.scaled(shadow.x);
    shadow.y = appearance.scaled(shadow.y);
    shadow.blur = appearance.scaled(shadow.blur);
    return shadow;
}

} // namespace ket::detail
