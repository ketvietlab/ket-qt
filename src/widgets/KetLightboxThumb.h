// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetLightboxRecipes.h"

#include <QAbstractButton>
#include <QPixmap>
#include <optional>

namespace ket {

// The thumbnail that opens the image viewer (`[data-ui="lightbox-thumb"]`): the picture
// cropped to a square tile. Without a picture it is the empty tile
// (`[data-ui="lightbox-empty"]`) and shows its text. Colours and metrics come from
// ket::lightboxThumbRecipe.
class LightboxThumb final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit LightboxThumb(const QPixmap& image = {}, const QString& description = {},
                           QWidget* parent = nullptr);

    [[nodiscard]] QPixmap image() const { return image_; }
    void setImage(const QPixmap& image);
    [[nodiscard]] LightboxThumbSize thumbSize() const { return size_; }
    void setThumbSize(LightboxThumbSize size);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] LightboxThumbRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QPixmap image_;
    LightboxThumbSize size_ = LightboxThumbSize::Medium;
    std::optional<InteractionState> preview_;
};

} // namespace ket
