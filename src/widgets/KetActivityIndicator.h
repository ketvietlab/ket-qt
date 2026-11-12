// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

#include <optional>

namespace ket {

// Animated working indicator (`.working-bars`): bars that shrink and fade in turn. It
// animates only while visible. Colours and metrics come from ket::activityRecipe.
class ActivityIndicator final : public QWidget {
    Q_OBJECT
  public:
    explicit ActivityIndicator(QWidget* parent = nullptr);
    ~ActivityIndicator() override;

    // An app colour for the bars; std::nullopt keeps the recipe's success colour.
    void setColor(std::optional<QColor> color);
    // Freezes the animation at `elapsedMs`, for screenshots; std::nullopt resumes it.
    void setPreviewTime(std::optional<qint64> elapsedMs);

    [[nodiscard]] ActivityRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

  private:
    void updateClock();

    std::optional<QColor> color_;
    std::optional<qint64> preview_;
};

} // namespace ket
