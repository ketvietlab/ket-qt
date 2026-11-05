// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// A bordered option with a checkbox or radio, a title and an optional description
// (`.choice`); it draws ket::choiceRecipe around ket::toggleRecipe.
class Choice final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit Choice(const QString& title, const QString& description = {},
                    ToggleKind kind = ToggleKind::Checkbox, QWidget* parent = nullptr);

    [[nodiscard]] QString description() const { return description_; }
    void setDescription(const QString& description);
    [[nodiscard]] ToggleKind kind() const { return kind_; }
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] ChoiceRecipe recipe() const;
    [[nodiscard]] ToggleRecipe toggle() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] InteractionState state() const;
    [[nodiscard]] QFont titleFont() const;
    [[nodiscard]] QFont descriptionFont() const;
    [[nodiscard]] qreal textLeft() const;

    QString description_;
    ToggleKind kind_;
    std::optional<InteractionState> preview_;
};

} // namespace ket
