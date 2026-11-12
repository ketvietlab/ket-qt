// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSidebarTreeRecipes.h"

#include <QAbstractButton>
#include <QWidget>

#include <optional>

class QVBoxLayout;

namespace ket {

class Badge;
class IconButton;

// The sidebar tree: a repository header, a team branch, a section label and one-line
// conversation rows, with guide lines under each chevron. Every part spans the tree's width
// and places its chevron, mark and text in the columns of its level
// (ket::sidebarTreeColumns), so the text of a header and of the rows under it share one
// edge at every zoom. Level 0 is what sits directly under a repository; a team's members
// are one level deeper. Colours and metrics come from the ket::sidebar*Recipe functions.

// A repository header: the chevron, a folder and the name in capitals; then a dot while it
// is folded with something waiting, the conversation count, and an add action shown while
// the header is hovered or focused. It is checkable: checked is unfolded. Return, Enter and
// Space fold or unfold it.
class SidebarRepositoryHeader final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit SidebarRepositoryHeader(const QString& name = {}, QWidget* parent = nullptr);

    [[nodiscard]] bool isExpanded() const { return isChecked(); }
    void setExpanded(bool expanded) { setChecked(expanded); }
    void setConversationCount(int count);
    [[nodiscard]] int conversationCount() const { return conversations_; }
    // Conversations or decisions waiting for the user; a folded header shows a dot.
    void setWaitingCount(int count);
    [[nodiscard]] int waitingCount() const { return waiting_; }
    [[nodiscard]] bool isWaitingDotShown() const;
    void setSelected(bool selected);
    // A header that follows another repository keeps the divider's space above it.
    void setDivided(bool divided);
    [[nodiscard]] IconButton* action() const { return action_; }
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] SidebarRepositoryRecipe recipe() const;
    [[nodiscard]] SidebarTreeColumns columns() const;
    // Where the name is drawn, in this widget's coordinates.
    [[nodiscard]] QRect nameRect() const;
    [[nodiscard]] QRect markRect() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  signals:
    void actionTriggered();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void layoutTrailing();
    void updateDescription();
    [[nodiscard]] bool hot() const;
    [[nodiscard]] QRect rowRect() const;
    [[nodiscard]] QFont nameFont() const;

    IconButton* action_;
    Badge* count_;
    int conversations_ = 0;
    int waiting_ = 0;
    bool selected_ = false;
    bool divided_ = false;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

// A team branch: the chevron folds its members, the name opens the team. They are two
// keyboard stops. While a decision waits, the needs-input mark shows and the name is heavier.
// The member count trails; add and menu actions show while the branch is hovered or focused.
class SidebarTeamBranch final : public QWidget {
    Q_OBJECT
  public:
    explicit SidebarTeamBranch(const QString& name = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString name() const;
    void setName(const QString& name);
    // The level of the members; the branch draws its chevron on that level's guide.
    void setLevel(int level);
    [[nodiscard]] int level() const { return level_; }
    [[nodiscard]] bool isExpanded() const;
    void setExpanded(bool expanded);
    void setMemberCount(int count);
    [[nodiscard]] int memberCount() const { return members_; }
    void setDecisionCount(int count);
    [[nodiscard]] int decisionCount() const { return decisions_; }
    void setSelected(bool selected);
    [[nodiscard]] bool isSelected() const { return selected_; }
    [[nodiscard]] QAbstractButton* foldButton() const { return fold_; }
    [[nodiscard]] QAbstractButton* nameButton() const { return name_; }
    [[nodiscard]] IconButton* addAction() const { return add_; }
    [[nodiscard]] IconButton* menuAction() const { return menu_; }
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] SidebarTeamRecipe recipe() const;
    [[nodiscard]] SidebarTreeColumns columns() const;
    [[nodiscard]] QRect nameRect() const;
    [[nodiscard]] QRect markRect() const;
    // The highlight's rectangle: it starts right of the chevron.
    [[nodiscard]] QRect highlightRect() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  signals:
    void toggled(bool expanded);
    void activated();
    void addTriggered();
    void menuTriggered();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void layoutParts();
    void updateAccessibility();
    [[nodiscard]] bool hot() const;
    [[nodiscard]] InteractionState nameState() const;
    [[nodiscard]] QFont nameFont() const;

    QAbstractButton* fold_;
    QAbstractButton* name_;
    IconButton* add_;
    IconButton* menu_;
    Badge* count_;
    int level_ = 1;
    int members_ = 0;
    int decisions_ = 0;
    bool selected_ = false;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

// A section label, "Conversations (n)", with an add action. The label is a heading, not a
// control; its action is a keyboard stop.
class SidebarSectionLabel final : public QWidget {
    Q_OBJECT
  public:
    explicit SidebarSectionLabel(const QString& text = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    void setCount(int count);
    [[nodiscard]] int count() const { return count_; }
    // "Conversations (3)".
    [[nodiscard]] QString displayText() const;
    void setLevel(int level);
    [[nodiscard]] int level() const { return level_; }
    [[nodiscard]] IconButton* action() const { return action_; }

    [[nodiscard]] SidebarSectionRecipe recipe() const;
    [[nodiscard]] SidebarTreeColumns columns() const;
    [[nodiscard]] QRect textRect() const;
    [[nodiscard]] QRect markRect() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  signals:
    void actionTriggered();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    void applyRecipe();
    void updateAccessibility();
    [[nodiscard]] QFont textFont() const;

    IconButton* action_;
    QString text_;
    int count_ = -1;
    int level_ = 0;
};

// A one-line conversation row: its status mark in the state's colour (a dot instead once it
// finished and is unread, with a heavier title), the title and "#number". It is checkable;
// a QButtonGroup keeps one selected. Return, Enter and Space activate it. The accessible
// description says the role, the state in words and the worktree.
class SidebarRow final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit SidebarRow(const QString& title = {}, QWidget* parent = nullptr);
    ~SidebarRow() override;

    void setLevel(int level);
    [[nodiscard]] int level() const { return level_; }
    [[nodiscard]] QString number() const { return number_; }
    void setNumber(const QString& number);
    [[nodiscard]] ConversationStatus status() const { return status_; }
    void setStatus(ConversationStatus status);
    // Overrides the English status label; an empty string restores it.
    void setStatusLabel(const QString& label);
    // A short detail said after the state, such as the question that waits.
    void setStatusDetail(const QString& detail);
    [[nodiscard]] bool isUnread() const { return unread_; }
    void setUnread(bool unread);
    [[nodiscard]] bool isUnreadDotShown() const;
    // The member's role in its team ("Leader", "Developer"); empty outside a team.
    void setRole(const QString& role);
    void setWorktree(const QString& worktree);
    [[nodiscard]] QString stateText() const;
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] SidebarRowRecipe recipe() const;
    [[nodiscard]] SidebarTreeColumns columns() const;
    [[nodiscard]] QRect titleRect() const;
    [[nodiscard]] QRect markRect() const;
    [[nodiscard]] QRect highlightRect() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

  private:
    void updateAccessibility();
    void updateSpinner();
    [[nodiscard]] QFont textFont(qreal pixels, int weight, bool mono = false) const;
    [[nodiscard]] QString numberText() const;

    QString number_;
    QString statusLabel_;
    QString statusDetail_;
    QString role_;
    QString worktree_;
    ConversationStatus status_ = ConversationStatus::NotStarted;
    int level_ = 0;
    bool unread_ = false;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

// A column of tree parts with the guide line of its level: a 1 px line under the chevron's
// centre, as long as what it holds. Hide it to fold. Below a team's members (level 1 and
// deeper) it keeps the branch gap.
class SidebarTreeGuide final : public QWidget {
    Q_OBJECT
  public:
    explicit SidebarTreeGuide(int level = 0, QWidget* parent = nullptr);

    void setLevel(int level);
    [[nodiscard]] int level() const { return level_; }
    void addPart(QWidget* part);
    [[nodiscard]] QVBoxLayout* partsLayout() const;
    // The line's rectangle, in this widget's coordinates.
    [[nodiscard]] QRect guideRect() const;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    int level_ = 0;
};

} // namespace ket
