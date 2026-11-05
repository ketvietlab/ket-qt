// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetConversationRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// One tool or command step in the transcript (`.activity-row`): a Két icon, the summary and
// an optional detail. An expandable row is checkable and ends in a chevron that points up
// while expanded; connect toggled() to show its children. Colours and metrics come from
// ket::activityRowRecipe.
class ActivityRow final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit ActivityRow(const QString& text = {}, const QString& iconCode = {},
                         QWidget* parent = nullptr);

    void setIconCode(const QString& code);
    [[nodiscard]] QString detail() const { return detail_; }
    void setDetail(const QString& detail);
    [[nodiscard]] bool isExpandable() const { return isCheckable(); }
    void setExpandable(bool expandable);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] ActivityRowRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    QString iconCode_;
    QString detail_;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

} // namespace ket
