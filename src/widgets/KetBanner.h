// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

class QHBoxLayout;
class QLabel;

namespace ket {

class Button;
class Icon;

// Inline notice (`.banner`): a tone icon, a title, an optional message and an optional
// action button. Colours and metrics come from ket::bannerRecipe.
class Banner final : public QWidget {
    Q_OBJECT
  public:
    explicit Banner(BannerTone tone = BannerTone::Info, const QString& title = {},
                    const QString& message = {}, QWidget* parent = nullptr);

    [[nodiscard]] BannerTone tone() const { return tone_; }
    void setTone(BannerTone tone);
    [[nodiscard]] QString title() const;
    void setTitle(const QString& title);
    [[nodiscard]] QString message() const;
    void setMessage(const QString& message);

    // Adds (or relabels) the trailing action button; connect to its clicked() signal.
    Button* setAction(const QString& text, const QString& iconCode = {});
    [[nodiscard]] Button* action() const { return action_; }

    [[nodiscard]] BannerRecipe recipe() const;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    BannerTone tone_;
    Icon* icon_;
    QLabel* title_;
    QLabel* message_;
    Button* action_ = nullptr;
    QHBoxLayout* layout_;
};

} // namespace ket
