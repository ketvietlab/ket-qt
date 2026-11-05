// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QProxyStyle>

class QApplication;

namespace ket {

// Fusion-based style that renders Qt Widgets from Két Design System tokens, so a
// window looks the same on macOS, Windows and Linux. install() also keeps the
// application palette and font in step with ket::Appearance.
class Style final : public QProxyStyle {
    Q_OBJECT
  public:
    Style();

    static void install(QApplication& application);
    [[nodiscard]] static QPalette paletteFor(Theme theme);
    // The design system's sans font at text-base, falling back to the platform font.
    [[nodiscard]] static QFont interfaceFont();

    void polish(QPalette& palette) override;
    using QProxyStyle::polish;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
                    const QWidget* widget = nullptr) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                       const QWidget* widget = nullptr) const override;
};

} // namespace ket
