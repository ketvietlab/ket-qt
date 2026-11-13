// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QObject>

namespace ket {

// The one theme state an application shares between its Widgets and Qt Quick layers.
// It follows the system colour scheme until setMode() pins Light or Dark.
class Appearance final : public QObject {
    Q_OBJECT
  public:
    enum class Mode { System, Light, Dark };
    Q_ENUM(Mode)

    // Created on first use; requires a QGuiApplication.
    [[nodiscard]] static Appearance& instance();

    [[nodiscard]] Mode mode() const { return mode_; }
    [[nodiscard]] Theme theme() const { return theme_; }
    [[nodiscard]] bool dark() const { return theme_ == Theme::Dark; }
    void setMode(Mode mode);

    // GUI thread only. Replaces this theme branch's complete override set. Unknown
    // names, non-color tokens and invalid colors are ignored and returned, sorted.
    // Every explicit set/clear publishes exactly one themeChanged(currentTheme),
    // including inactive branches and no-op requests, to refresh recipe bindings.
    [[nodiscard]] QStringList setColorOverrides(Theme theme, const QHash<QString, QColor>& colors);
    void clearColorOverrides(Theme theme);

    // Interface zoom: 1.0 = 100%. Components scale token lengths by it.
    [[nodiscard]] qreal zoom() const { return zoom_; }
    void setZoom(qreal zoom);
    [[nodiscard]] qreal scaled(qreal logicalPixels) const { return scale(logicalPixels, zoom_); }
    // Whole pixels at every zoom step; hairlines (<= 1 px) are not scaled.
    [[nodiscard]] static qreal scale(qreal logicalPixels, qreal zoom);

  signals:
    void themeChanged(ket::Theme theme);
    void zoomChanged(qreal zoom);

  private:
    explicit Appearance(QObject* parent = nullptr);
    void update();

    Mode mode_ = Mode::System;
    Theme theme_ = Theme::Light;
    qreal zoom_ = 1.0;
};

} // namespace ket
