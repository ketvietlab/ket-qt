// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QAbstractButton>
#include <QWidget>

#include <optional>

namespace ket {

class IconButton;

enum class TreeActivity { None, Working, Waiting };

// A two-line sidebar item (`.tree-item`), such as a worktree: a Két icon, the title, then
// an activity line (`.tree-activity`) or a muted detail, and an optional count on the right
// (`.tree-count`). It is checkable; checked is the mock's `.selected`. Colours and metrics
// come from ket::treeItemRecipe.
class TreeItem final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit TreeItem(const QString& title = {}, const QString& iconCode = {},
                      QWidget* parent = nullptr);

    void setIconCode(const QString& code);
    [[nodiscard]] QString detail() const { return detail_; }
    void setDetail(const QString& detail);
    void setActivity(const QString& text, TreeActivity activity);
    [[nodiscard]] TreeActivity activity() const { return activity_; }
    void setCount(const QString& count);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] TreeItemRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    [[nodiscard]] QFont sizedFont(qreal pixels, int weight = QFont::Normal) const;
    [[nodiscard]] QString secondLine() const;

    QString iconCode_;
    QString detail_;
    QString activityText_;
    QString count_;
    TreeActivity activity_ = TreeActivity::None;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

// A sidebar group header (`.repository-group-header`): a folder icon and the name, which
// toggle the group, and an action (usually "New conversation") that shows while the header
// is hovered or focused. An activatable header splits them: the icon folds the group and the
// name is a control of its own that emits activated() (it opens something, such as a page).
// They are separate keyboard stops and say which is which. Colours and metrics come from
// ket::groupHeaderRecipe.
class GroupHeader final : public QWidget {
    Q_OBJECT
  public:
    explicit GroupHeader(const QString& name = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString name() const;
    void setName(const QString& name);
    [[nodiscard]] bool isExpanded() const;
    void setExpanded(bool expanded);
    // A fixed icon (such as `display` for a remote group); empty uses the folder pair.
    void setIconCode(const QString& code);
    void setSelected(bool selected);
    [[nodiscard]] bool isActivatable() const { return activatable_; }
    void setActivatable(bool activatable);
    [[nodiscard]] QAbstractButton* toggle() const { return toggle_; }
    // The name of an activatable header; hidden otherwise.
    [[nodiscard]] QAbstractButton* nameButton() const { return name_; }
    [[nodiscard]] IconButton* action() const { return action_; }

  signals:
    void toggled(bool expanded);
    void actionTriggered();
    // The name of an activatable header was clicked or pressed with Space or Enter.
    void activated();

  protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void updateAction();
    void updateAccessibleNames();

    QAbstractButton* toggle_;
    QAbstractButton* name_;
    IconButton* action_;
    bool hovered_ = false;
    bool activatable_ = false;
};

} // namespace ket
