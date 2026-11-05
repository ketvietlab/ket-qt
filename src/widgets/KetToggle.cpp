// SPDX-License-Identifier: MPL-2.0
#include "KetToggle.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QtMath>

namespace ket {

Toggle::Toggle(ToggleKind kind, const QString& text, QWidget* parent)
    : QAbstractButton(parent), kind_(kind) {
    setText(text);
    setCheckable(true);
    setAutoExclusive(kind == ToggleKind::Radio);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Toggle::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

ToggleRecipe Toggle::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
    return toggleRecipe(kind_, isChecked(), state, Appearance::instance().theme());
}

QFont Toggle::labelFont() const {
    QFont f = font();
    f.setPixelSize(qRound(Appearance::instance().scaled(recipe().fontPixels)));
    return f;
}

QSize Toggle::sizeHint() const {
    const ToggleRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QFontMetrics metrics(labelFont());
    const qreal box = appearance.scaled(r.size);
    const qreal label =
        text().isEmpty() ? 0 : appearance.scaled(r.spacing) + metrics.horizontalAdvance(text());
    // 2 px on each side of the indicator hold its focus ring.
    return {qCeil(box + label) + 4, qMax(qCeil(box) + 4, metrics.height())};
}

void Toggle::paintIndicator(QPainter& painter, const QRectF& box, ToggleKind kind,
                            const ToggleRecipe& r, qreal devicePixelRatio) {
    const qreal scale = box.width() / r.size;
    const qreal radius = r.radius * scale;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    if (r.mark.alpha()) {
        if (kind == ToggleKind::Radio) {
            const qreal dot = box.width() * 0.375;
            painter.setPen(Qt::NoPen);
            painter.setBrush(r.mark);
            painter.drawEllipse(box.center(), dot / 2, dot / 2);
        } else {
            painter.drawPixmap(box.topLeft(), Icons::pixmap(u"check", box.width(), devicePixelRatio,
                                                            r.mark, IconWeight::Regular));
        }
    }
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(box.adjusted(-2, -2, 2, 2), radius + 2, radius + 2);
    }
}

void Toggle::paintEvent(QPaintEvent*) {
    const ToggleRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    QPainter painter(this);
    painter.setOpacity(r.opacity);
    const qreal box = appearance.scaled(r.size);
    // Leave room for the focus ring around the indicator.
    const QRectF indicator(2, (height() - box) / 2.0, box, box);
    paintIndicator(painter, indicator, kind_, r, devicePixelRatioF());
    if (!text().isEmpty()) {
        painter.setFont(labelFont());
        painter.setPen(r.label);
        const qreal x = indicator.right() + appearance.scaled(r.spacing);
        painter.drawText(QRectF(x, 0, width() - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         text());
    }
}

Checkbox::Checkbox(const QString& text, QWidget* parent)
    : Toggle(ToggleKind::Checkbox, text, parent) {}

Radio::Radio(const QString& text, QWidget* parent) : Toggle(ToggleKind::Radio, text, parent) {}

} // namespace ket
