// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QComboBox>

#include <optional>

namespace ket {

// Drop-down select (`select`); the closed control draws ket::fieldRecipe with a chevron.
class Select final : public QComboBox {
    Q_OBJECT
  public:
    explicit Select(QWidget* parent = nullptr);

    void setPreviewState(std::optional<InteractionState> state);
    [[nodiscard]] FieldRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont fieldFont() const;

    std::optional<InteractionState> preview_;
};

} // namespace ket
