// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QStringList>
#include <QWidget>

class QLabel;
class QSlider;

namespace ket {

// A labelled choice among ordered steps (`.effort-picker`): the label and the current step in
// the accent on one line, a slider (drawn by ket::Style) and the first and last steps below
// (`.range-labels`). Colours and metrics come from ket::sliderRecipe.
class StepSlider final : public QWidget {
    Q_OBJECT
  public:
    explicit StepSlider(const QString& label = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString label() const;
    void setLabel(const QString& label);
    [[nodiscard]] QStringList steps() const { return steps_; }
    void setSteps(const QStringList& steps);
    [[nodiscard]] int currentIndex() const;
    void setCurrentIndex(int index);
    [[nodiscard]] QString currentStep() const;
    [[nodiscard]] QSlider* slider() const { return slider_; }

  signals:
    void currentIndexChanged(int index);

  private:
    void applyRecipe();
    void updateLabels();

    QLabel* label_;
    QLabel* value_;
    QSlider* slider_;
    QLabel* first_;
    QLabel* last_;
    QStringList steps_;
};

} // namespace ket
