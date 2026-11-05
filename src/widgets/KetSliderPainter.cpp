// SPDX-License-Identifier: MPL-2.0
#include "KetSliderPainter.h"

#include "KetAppearance.h"
#include "KetControlRecipes.h"

#include <QPainter>
#include <QStyleOptionSlider>
#include <QtMath>

namespace ket::detail {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

SliderRecipe recipeFor(const QStyleOptionComplex* option) {
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool active = option->activeSubControls & QStyle::SC_SliderHandle;
    const InteractionState state{enabled && ((option->state & QStyle::State_MouseOver) != 0),
                                 enabled && active && ((option->state & QStyle::State_Sunken) != 0),
                                 enabled,
                                 enabled && ((option->state & QStyle::State_HasFocus) != 0)};
    return sliderRecipe(state, Appearance::instance().theme());
}

} // namespace

bool isHorizontalSlider(const QStyleOptionComplex* option) {
    const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
    return slider && slider->orientation == Qt::Horizontal;
}

int sliderThickness() {
    return qRound(scaled(sliderRecipe({}, Appearance::instance().theme()).height));
}

int sliderHandleLength() {
    return qRound(scaled(sliderRecipe({}, Appearance::instance().theme()).handleSize));
}

QRect sliderSubControlRect(const QStyleOptionSlider* option, QStyle::SubControl control) {
    const QRect box = option->rect;
    const int handle = sliderHandleLength();
    const int span = qMax(0, box.width() - handle);
    const int position = QStyle::sliderPositionFromValue(
        option->minimum, option->maximum, option->sliderPosition, span, option->upsideDown);
    switch (control) {
    case QStyle::SC_SliderHandle:
        return {box.left() + position, box.top() + (box.height() - handle) / 2, handle, handle};
    case QStyle::SC_SliderGroove:
        return {box.left() + handle / 2, box.top(), span, box.height()};
    default:
        return box;
    }
}

void drawSlider(const QStyleOptionSlider* option, QPainter* painter, const QWidget*) {
    const SliderRecipe r = recipeFor(option);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(r.opacity);
    const QRect groove = sliderSubControlRect(option, QStyle::SC_SliderGroove);
    const QRectF handle = sliderSubControlRect(option, QStyle::SC_SliderHandle);
    const qreal height = scaled(r.trackHeight);
    const QRectF track(groove.left(), groove.center().y() + 0.5 - height / 2, groove.width(),
                       height);
    painter->setPen(Qt::NoPen);
    painter->setBrush(r.track);
    painter->drawRoundedRect(track, height / 2, height / 2);
    const qreal centre = handle.center().x();
    const QRectF filled = option->upsideDown
                              ? QRectF(QPointF(centre, track.top()), track.bottomRight())
                              : QRectF(track.topLeft(), QPointF(centre, track.bottom()));
    painter->setBrush(r.fill);
    painter->drawRoundedRect(filled, height / 2, height / 2);
    if (r.focusRing.alpha()) {
        painter->setPen(QPen(r.focusRing, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(handle.adjusted(-2, -2, 2, 2));
    }
    const qreal border = scaled(r.handleBorderWidth);
    painter->setPen(QPen(r.handleBorder, border));
    painter->setBrush(r.handle);
    painter->drawEllipse(handle.adjusted(border / 2, border / 2, -border / 2, -border / 2));
    painter->restore();
}

} // namespace ket::detail
