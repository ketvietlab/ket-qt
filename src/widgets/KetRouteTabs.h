// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QHBoxLayout;

namespace ket {

// Page-level tabs (`.route-tabs`, also `.provider-tabs`): text tabs over a hairline, the
// active one in the accent with a 2 px underline. Arrow keys move between tabs. Colours and
// metrics come from ket::routeTabRecipe.
class RouteTabs final : public QWidget {
    Q_OBJECT
  public:
    explicit RouteTabs(QWidget* parent = nullptr);

    int addTab(const QString& text);
    [[nodiscard]] int count() const;
    [[nodiscard]] int currentIndex() const;
    void setCurrentIndex(int index);
    [[nodiscard]] QAbstractButton* tab(int index) const;

  signals:
    void currentChanged(int index);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QButtonGroup* group_;
    QHBoxLayout* row_;
};

} // namespace ket
