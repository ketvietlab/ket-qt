// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// Checkbox and radio button with an inline label; they draw ket::toggleRecipe.
class Toggle : public QAbstractButton {
    Q_OBJECT
  public:
    [[nodiscard]] ToggleKind kind() const { return kind_; }
    void setPreviewState(std::optional<InteractionState> state);
    [[nodiscard]] ToggleRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override { return sizeHint(); }

    // Draws the indicator alone into `box` (shared with ket::Choice).
    static void paintIndicator(QPainter& painter, const QRectF& box, ToggleKind kind,
                               const ToggleRecipe& recipe, qreal devicePixelRatio);

  protected:
    Toggle(ToggleKind kind, const QString& text, QWidget* parent);
    void paintEvent(QPaintEvent* event) override;
    [[nodiscard]] bool hitButton(const QPoint& pos) const override { return rect().contains(pos); }

  private:
    [[nodiscard]] QFont labelFont() const;

    ToggleKind kind_;
    std::optional<InteractionState> preview_;
};

class Checkbox final : public Toggle {
    Q_OBJECT
  public:
    explicit Checkbox(const QString& text = {}, QWidget* parent = nullptr);
};

class Radio final : public Toggle {
    Q_OBJECT
  public:
    explicit Radio(const QString& text = {}, QWidget* parent = nullptr);
};

} // namespace ket
