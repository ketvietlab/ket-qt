// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// Label-only chip tinted with an app-owned colour; colours and metrics come from
// ket::tagRecipe. A filterable tag is clickable and tints more strongly on hover; an
// overflow tag is the muted "+N" chip. Long labels elide at the recipe's maximum width.
class Tag final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit Tag(const QString& text = {}, const QColor& color = {}, QWidget* parent = nullptr);

    [[nodiscard]] QColor color() const { return color_; }
    void setColor(const QColor& color);
    [[nodiscard]] bool isOverflow() const { return overflow_; }
    void setOverflow(bool overflow);
    [[nodiscard]] bool isFilterable() const { return filterable_; }
    void setFilterable(bool filterable);
    // Pins the hover state for documentation pages; std::nullopt follows the pointer.
    void setPreviewHovered(std::optional<bool> hovered);

    [[nodiscard]] TagRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont tagFont() const;

    QColor color_;
    bool overflow_ = false;
    bool filterable_ = false;
    std::optional<bool> previewHovered_;
};

} // namespace ket
