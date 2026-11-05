// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QPixmap>
#include <QWidget>

namespace ket {

// A round avatar (`.avatar`): an image cropped to the circle, or up to two initials taken
// from the name. Colours and metrics come from ket::avatarRecipe.
class Avatar final : public QWidget {
    Q_OBJECT
  public:
    explicit Avatar(const QString& name = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString name() const { return name_; }
    void setName(const QString& name);
    [[nodiscard]] QString initials() const;
    void setImage(const QPixmap& image);
    // Overrides the diameter in logical pixels at 100 % (0 uses the recipe's).
    void setDiameter(qreal diameter);

    [[nodiscard]] static QString initialsOf(const QString& name);
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString name_;
    QPixmap image_;
    qreal diameter_ = 0;
};

} // namespace ket
