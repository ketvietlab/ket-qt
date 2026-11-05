// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

namespace ket {

// Non-interactive status label; colours and metrics come from ket::badgeRecipe.
class Badge final : public QWidget {
    Q_OBJECT
  public:
    explicit Badge(const QString& text = {}, BadgeTone tone = BadgeTone::Neutral,
                   QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    [[nodiscard]] BadgeTone tone() const { return tone_; }
    void setTone(BadgeTone tone);

    [[nodiscard]] BadgeRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont badgeFont() const;

    QString text_;
    BadgeTone tone_;
};

} // namespace ket
