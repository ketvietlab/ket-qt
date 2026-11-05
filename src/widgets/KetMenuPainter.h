// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QSize>

class QPainter;
class QStyleOption;
class QStyleOptionMenuItem;
class QWidget;

namespace ket::detail {

// Menu drawing for ket::Style (`.desktop-context-menu`); QMenu popups and QComboBox
// popups share it. Lengths follow the interface zoom.
void drawMenuPanel(const QStyleOption* option, QPainter* painter, const QWidget* widget);
void drawMenuItem(const QStyleOptionMenuItem* option, QPainter* painter, const QWidget* widget);
[[nodiscard]] QSize menuItemSize(const QStyleOptionMenuItem* option, const QWidget* widget);
[[nodiscard]] int menuPanelPadding();
void drawTooltipPanel(const QStyleOption* option, QPainter* painter, const QWidget* widget);

} // namespace ket::detail
