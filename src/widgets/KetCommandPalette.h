// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QPointer>
#include <QWidget>

class QLineEdit;
class QScrollArea;

namespace ket {

struct CommandItem {
    QString id;
    QString title;
    QString detail;
    QString iconCode;
};

struct CommandGroup {
    QString title;
    QList<CommandItem> items;
};

class CommandList;

// The command palette (`.spotlight`): a search field over grouped results, opened near the
// top of the window over a backdrop. It is an in-window overlay like ket::Dialog. Typing
// emits queryChanged(); answer with setGroups(). Up and Down move the highlight, Enter or a
// click emits activated() and closes, Esc or a click on the backdrop closes. Colours and
// metrics come from ket::commandPaletteRecipe and ket::commandResultRecipe.
class CommandPalette final : public QWidget {
    Q_OBJECT
  public:
    // `anchor` is any widget in the window to cover.
    explicit CommandPalette(QWidget* anchor);

    void setGroups(const QList<CommandGroup>& groups);
    [[nodiscard]] QList<CommandGroup> groups() const { return groups_; }
    void setPlaceholderText(const QString& text);
    void setEmptyText(const QString& title, const QString& message);
    [[nodiscard]] QString query() const;
    [[nodiscard]] int highlightedIndex() const { return highlighted_; }
    void setHighlightedIndex(int index); // over all items, in group order
    [[nodiscard]] QLineEdit* searchField() const { return search_; }
    [[nodiscard]] QWidget* panel() const { return panel_; }
    [[nodiscard]] bool isOpen() const { return isVisible(); }

  public slots:
    void open();
    void dismiss();

  signals:
    void queryChanged(const QString& query);
    void activated(const ket::CommandItem& item);
    void closed();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    friend class CommandList;
    void applyRecipe();
    void place();
    void activate(int index);
    [[nodiscard]] int itemCount() const;
    [[nodiscard]] const CommandItem* itemAt(int index) const;

    QWidget* panel_;
    QLineEdit* search_;
    QScrollArea* scroll_;
    CommandList* list_;
    QWidget* footer_;
    QList<CommandGroup> groups_;
    QString emptyTitle_;
    QString emptyMessage_;
    int highlighted_ = 0;
    QPointer<QWidget> restoreFocus_;
};

} // namespace ket
