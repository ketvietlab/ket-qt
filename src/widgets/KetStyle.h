// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QProxyStyle>

class QAction;
class QApplication;

namespace ket {

// Fusion-based style that renders Qt Widgets from Két Design System tokens, so a
// window looks the same on macOS, Windows and Linux. install() also keeps the
// application palette and font in step with ket::Appearance.
class Style final : public QProxyStyle {
    Q_OBJECT
  public:
    Style();

    // QAction properties read by the menu painter: a destructive command drawn in the
    // danger colour, and a secondary line under the title (`.desktop-context-menu small`).
    static constexpr char kDangerProperty[] = "ketDanger";
    static constexpr char kDescriptionProperty[] = "ketDescription";
    static void setDanger(QAction* action, bool danger = true);
    static void setDescription(QAction* action, const QString& description);

    static void install(QApplication& application);
    [[nodiscard]] static QPalette paletteFor(Theme theme);
    // The design system's sans font at text-base, falling back to the platform font.
    [[nodiscard]] static QFont interfaceFont();

    void polish(QPalette& palette) override;
    void polish(QWidget* widget) override;
    void unpolish(QWidget* widget) override;
    using QProxyStyle::polish;
    using QProxyStyle::unpolish;
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size,
                           const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = nullptr) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                            QPainter* painter, const QWidget* widget = nullptr) const override;
    [[nodiscard]] QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                                       SubControl subControl,
                                       const QWidget* widget = nullptr) const override;
    [[nodiscard]] SubControl hitTestComplexControl(ComplexControl control,
                                                   const QStyleOptionComplex* option,
                                                   const QPoint& position,
                                                   const QWidget* widget = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
                    const QWidget* widget = nullptr) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                       const QWidget* widget = nullptr) const override;
};

} // namespace ket
