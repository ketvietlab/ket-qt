// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QPixmap>
#include <QWidget>

namespace ket {

struct GalleryImage {
    QPixmap image;
    QString caption;
};

// Screenshot thumbnails in an activity (`.activity-gallery`): square tiles with a caption
// that wrap to the width. With more images than maxVisible() a `+N` tile ends the row;
// clicking it shows the rest. Clicking a tile emits activated(index) for the app's preview.
// Colours and metrics come from ket::galleryRecipe.
class Gallery final : public QWidget {
    Q_OBJECT
  public:
    explicit Gallery(QWidget* parent = nullptr);

    [[nodiscard]] QList<GalleryImage> images() const { return images_; }
    void setImages(const QList<GalleryImage>& images);
    [[nodiscard]] int maxVisible() const { return maxVisible_; }
    void setMaxVisible(int count); // 0 shows every image
    [[nodiscard]] bool isExpanded() const { return expanded_; }
    void setExpanded(bool expanded);
    [[nodiscard]] QList<QWidget*> tiles() const { return tiles_; }

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  signals:
    void activated(int index);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void rebuild();
    void place();
    [[nodiscard]] int columns(int width) const;
    [[nodiscard]] QSize tileSize() const;

    QList<GalleryImage> images_;
    QList<QWidget*> tiles_;
    int maxVisible_ = 0;
    bool expanded_ = false;
};

} // namespace ket
