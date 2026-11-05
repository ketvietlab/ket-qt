// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

namespace ket {

// Design-system button for Qt Widgets; colours and metrics come from ket::buttonRecipe.
class Button final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit Button(const QString& text = {}, ButtonVariant variant = ButtonVariant::Secondary,
                    QWidget* parent = nullptr);

    [[nodiscard]] ButtonVariant variant() const { return variant_; }
    void setVariant(ButtonVariant variant);
    [[nodiscard]] ControlSize controlSize() const { return size_; }
    void setControlSize(ControlSize size);

    [[nodiscard]] ButtonRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override { return sizeHint(); }

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    ButtonVariant variant_;
    ControlSize size_ = ControlSize::Medium;
    bool hovered_ = false;
};

} // namespace ket
