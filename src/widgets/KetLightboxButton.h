// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetLightboxRecipes.h"

#include <QAbstractButton>
#include <optional>

namespace ket {

// A round button over the image viewer's backdrop (`[data-ui="lightbox-tool"]` and
// `[data-ui="lightbox-nav"]`): a Két Symbol glyph, or with no code a pill with its text
// (the zoom percentage). `label` is the accessible name and the tooltip.
class LightboxButton final : public QAbstractButton {
    Q_OBJECT
  public:
    LightboxButton(const QString& code, const QString& label, QWidget* parent = nullptr);

    [[nodiscard]] QString code() const { return code_; }
    // Draws the glyph mirrored, for a clockwise Rotate from the anticlockwise `restore`.
    void setFlipped(bool flipped);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] LightboxToolRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString code_;
    bool flipped_ = false;
    std::optional<InteractionState> preview_;
};

} // namespace ket
