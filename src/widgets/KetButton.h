// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// Design-system button for Qt Widgets; colours and metrics come from ket::buttonRecipe.
// Icons are drawn as masks in the text colour, like the mock's `currentColor` icons.
class Button : public QAbstractButton {
    Q_OBJECT
  public:
    explicit Button(const QString& text = {}, ButtonVariant variant = ButtonVariant::Secondary,
                    QWidget* parent = nullptr);

    [[nodiscard]] ButtonVariant variant() const { return variant_; }
    void setVariant(ButtonVariant variant);
    [[nodiscard]] ControlSize controlSize() const { return size_; }
    void setControlSize(ControlSize size);

    // Draws a fixed interaction state instead of the live one, for documentation pages and
    // screenshot tests (the kitchensink state matrix). std::nullopt returns to live state.
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] virtual ButtonRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override { return sizeHint(); }

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    [[nodiscard]] InteractionState interactionState() const;

  private:
    ButtonVariant variant_;
    ControlSize size_ = ControlSize::Medium;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

// Square icon-only button (`.icon-btn`). The label is the accessible name and tooltip.
class IconButton final : public Button {
    Q_OBJECT
  public:
    explicit IconButton(const QIcon& icon, const QString& label, QWidget* parent = nullptr);

    [[nodiscard]] QString label() const { return accessibleName(); }
    void setLabel(const QString& label);

    [[nodiscard]] ButtonRecipe recipe() const override;
    [[nodiscard]] QSize sizeHint() const override;
};

} // namespace ket
