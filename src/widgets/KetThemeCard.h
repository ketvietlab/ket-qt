// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// A theme choice in Settings (`.theme-card`): a miniature of the window drawn in the
// colours of `preview` and the label with a check. The card is checkable; group cards with
// a QButtonGroup. Colours and metrics come from ket::themeCardRecipe.
class ThemeCard final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit ThemeCard(const QString& title, Theme preview, QWidget* parent = nullptr);

    [[nodiscard]] Theme preview() const { return preview_; }
    void setPreview(Theme preview);
    void setPreviewState(std::optional<InteractionState> state);
    [[nodiscard]] ThemeCardRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    Theme preview_;
    std::optional<InteractionState> previewState_;
};

// The accent sample in Settings (`.accent-swatch`): the accent in a subtle frame.
class AccentSwatch final : public QWidget {
    Q_OBJECT
  public:
    explicit AccentSwatch(QWidget* parent = nullptr);

    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
};

} // namespace ket
