// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QWidget>

#include <optional>

class QAbstractButton;
class QVBoxLayout;

namespace ket {

// A collapsible run of transcript steps (`.activity-group`): a muted summary with a chevron
// that flips while open, then the children (ket::ActivityRow, ket::Gallery…) behind a
// hairline on their leading edge. Colours and metrics come from ket::activityGroupRecipe.
class ActivityGroup final : public QWidget {
    Q_OBJECT
  public:
    explicit ActivityGroup(const QString& text = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString text() const;
    void setText(const QString& text);
    void addRow(QWidget* row);
    [[nodiscard]] QVBoxLayout* rows() const { return rows_; }
    [[nodiscard]] bool isExpanded() const;
    void setExpanded(bool expanded);
    [[nodiscard]] QAbstractButton* summary() const;
    void setPreviewState(std::optional<InteractionState> state);

  signals:
    void toggled(bool expanded);

  private:
    void applyRecipe();

    QAbstractButton* summary_;
    QWidget* children_;
    QVBoxLayout* rows_;
};

} // namespace ket
