// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

class QLabel;

namespace ket {

// A labelled settings row (`.field-row`): a fixed label column, then the control with
// optional help text or an error below it. Colours and metrics come from ket::fieldRowRecipe.
class FieldRow final : public QWidget {
    Q_OBJECT
  public:
    FieldRow(const QString& label, QWidget* control, const QString& help = {},
             QWidget* parent = nullptr);

    [[nodiscard]] QWidget* control() const { return control_; }
    // The label above the control instead of beside it, for a form narrower than the label
    // column and a control need side by side. The row then needs only the control's width.
    void setStacked(bool stacked);
    [[nodiscard]] bool isStacked() const { return stacked_; }
    void setHelp(const QString& help);
    // A non-empty error replaces the help text and turns a ket::TextField invalid.
    void setError(const QString& error);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void applyRecipe();
    void fitNote();

    QLabel* label_;
    QWidget* control_;
    QLabel* note_;
    QString help_;
    QString error_;
    bool stacked_{false};
};

} // namespace ket
