// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QRect>
#include <QStyle>

class QPainter;
class QStyleOptionComplex;
class QStyleOptionSlider;
class QWidget;

namespace ket::detail {

// Horizontal sliders for ket::Style (ket::sliderRecipe): a rounded track filled up to a round
// handle. Vertical sliders stay Fusion.
[[nodiscard]] bool isHorizontalSlider(const QStyleOptionComplex* option);
void drawSlider(const QStyleOptionSlider* option, QPainter* painter, const QWidget* widget);
[[nodiscard]] QRect sliderSubControlRect(const QStyleOptionSlider* option,
                                         QStyle::SubControl control);
[[nodiscard]] int sliderThickness();
[[nodiscard]] int sliderHandleLength();

} // namespace ket::detail
