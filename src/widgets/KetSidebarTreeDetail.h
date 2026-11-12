// SPDX-License-Identifier: MPL-2.0
// Internal: painting helpers shared by the sidebar tree's widgets. Not installed.
#pragma once

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetRecipes.h"

#include <QKeyEvent>
#include <QPainter>
#include <QWidget>

namespace ket::sidebar_tree {

inline qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

inline Theme theme() { return Appearance::instance().theme(); }

inline bool activates(const QKeyEvent* event) {
    return event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter;
}

inline void paintFocus(QPainter& painter, const QRectF& rect, const QColor& ring, qreal radius) {
    if (!ring.alpha())
        return;
    painter.setPen(QPen(ring, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), radius - 1, radius - 1);
}

inline void drawIcon(QPainter& painter, const QWidget* widget, const QString& code,
                     const QRect& box, const QColor& color) {
    painter.drawPixmap(box.topLeft(),
                       Icons::pixmap(code, box.width(), widget->devicePixelRatioF(), color));
}

inline QRect squareAt(qreal x, const QRect& row, qreal side) {
    const int size = qRound(side);
    return {qRound(x), row.top() + (row.height() - size) / 2, size, size};
}

inline void retainWhenHidden(QWidget* widget) {
    QSizePolicy policy = widget->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    widget->setSizePolicy(policy);
}

} // namespace ket::sidebar_tree
