// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QLineEdit>

#include <optional>

namespace ket {

// Single-line text field (`input`); colours and metrics come from ket::fieldRecipe. A
// leading Két Symbol icon makes the search field (`.search-field`, icon "search").
class TextField final : public QLineEdit {
    Q_OBJECT
  public:
    explicit TextField(QWidget* parent = nullptr);

    [[nodiscard]] QString leadingIcon() const { return leadingIcon_; }
    void setLeadingIcon(const QString& code);
    [[nodiscard]] bool isInvalid() const { return invalid_; }
    void setInvalid(bool invalid);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] FieldRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

  private:
    void applyRecipe();

    QString leadingIcon_;
    bool invalid_ = false;
    std::optional<InteractionState> preview_;
};

} // namespace ket
