// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;

namespace ket {

class Icon;

// The agent asking for input (`.question-card`): an accent label row with an optional step
// ("1 / 2"), the question, then the app's answer controls in body() (ket::Choice, a
// ket::TextField) and its buttons in footer(). Colours and metrics come from
// ket::questionCardRecipe.
class QuestionCard final : public QWidget {
    Q_OBJECT
  public:
    explicit QuestionCard(const QString& question = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString question() const;
    void setQuestion(const QString& question);
    [[nodiscard]] QString label() const;
    void setLabel(const QString& label);
    [[nodiscard]] QString step() const;
    void setStep(const QString& step);
    [[nodiscard]] QVBoxLayout* body() const { return body_; }
    [[nodiscard]] QHBoxLayout* footer() const { return footer_; }

    [[nodiscard]] QuestionCardRecipe recipe() const;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    Icon* icon_;
    QLabel* label_;
    QLabel* step_;
    QLabel* question_;
    QVBoxLayout* body_;
    QHBoxLayout* footer_;
};

} // namespace ket
