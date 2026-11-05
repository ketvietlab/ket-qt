// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QSize>

class QPainter;
class QStyleOption;
class QStyleOptionProgressBar;
class QStyleOptionTab;
class QStyleOptionTabBarBase;
class QWidget;

namespace ket::detail {

// Tab strips (`.file-tabs`) and progress bars for ket::Style. Only horizontal, top tab bars
// and horizontal progress bars are drawn here; ket::Style leaves the rest to Fusion.
[[nodiscard]] bool isTopTab(const QStyleOption* option);
void drawTabBarBase(const QStyleOption* option, QPainter* painter);
void drawTab(const QStyleOptionTab* option, QPainter* painter, const QWidget* widget);
void drawTabLabel(const QStyleOptionTab* option, QPainter* painter, const QWidget* widget);
[[nodiscard]] QSize tabSize(const QStyleOptionTab* option, const QSize& contents,
                            const QWidget* widget);
void drawTabClose(const QStyleOption* option, QPainter* painter, const QWidget* widget);

void drawProgressBar(const QStyleOptionProgressBar* option, QPainter* painter,
                     const QWidget* widget);
[[nodiscard]] int progressHeight();

} // namespace ket::detail
