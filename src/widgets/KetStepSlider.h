// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QStringList>
#include <QWidget>
#include <optional>

class QLabel;
class QSlider;

namespace ket {

// A labelled choice among ordered steps (`.effort-picker`): the label and the current step in
// the accent on one line, a notched track and the first and last steps below
// (`.range-labels`). The track is a pill holding a rounded track with one dot per step; the
// chosen step is a ringed thumb and the track fills up to it, so every step shows before it is
// chosen. A click or drag picks the nearest step, Left and Right move one step, and the focus
// ring follows keyboard focus only. It is disabled with fewer than two steps. Colours and
// metrics come from ket::sliderRecipe; the geometry from ket::stepNotchCentre and
// ket::stepNotchNearest, as KetStepSlider.qml draws it.
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
    // The notched track: a QSlider over the step indexes (accessible as a slider).
    [[nodiscard]] QSlider* slider() const { return slider_; }
    // Pins the track's hover, press and focus state, for the kitchensink.
    void setPreviewState(std::optional<InteractionState> state);

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
