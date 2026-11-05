// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

class QLabel;
class QVBoxLayout;

namespace ket {

class Button;
class Icon;

// Centred placeholder for an empty view (`.empty-state`): an accent icon, a title, a
// message and an optional action. Colours and metrics come from ket::emptyStateRecipe.
class EmptyState final : public QWidget {
    Q_OBJECT
  public:
    explicit EmptyState(const QString& iconCode = {}, const QString& title = {},
                        const QString& message = {}, QWidget* parent = nullptr);

    void setIconCode(const QString& code);
    [[nodiscard]] QString title() const;
    void setTitle(const QString& title);
    [[nodiscard]] QString message() const { return messageText_; }
    void setMessage(const QString& message);
    // Adds (or relabels) the action button under the message.
    Button* setAction(const QString& text, const QString& iconCode = {});
    [[nodiscard]] Button* action() const { return action_; }

  private:
    void applyRecipe();
    void renderMessage();

    Icon* icon_;
    QLabel* title_;
    QLabel* message_;
    QString messageText_;
    Button* action_ = nullptr;
    QVBoxLayout* layout_;
};

} // namespace ket
