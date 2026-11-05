// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QWidget>

class QAbstractButton;
class QVBoxLayout;

namespace ket {

class IconButton;

// A background task (`.bg-row`): a status icon, the kind (`terminal` for a shell, `bot` for
// an agent), the title and a muted meta line, then Stop while it runs or Dismiss once it
// has ended. Colours and metrics come from ket::taskRowRecipe.
class TaskRow final : public QWidget {
    Q_OBJECT
  public:
    TaskRow(const QString& title, const QString& kindIcon = QStringLiteral("terminal"),
            QWidget* parent = nullptr);

    [[nodiscard]] QString title() const { return title_; }
    void setTitle(const QString& title);
    void setMeta(const QString& meta);
    [[nodiscard]] TaskStatus status() const { return status_; }
    void setStatus(TaskStatus status);
    void setCurrent(bool current);
    [[nodiscard]] IconButton* stopButton() const { return stop_; }
    [[nodiscard]] IconButton* dismissButton() const { return dismiss_; }

    [[nodiscard]] TaskRowRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  signals:
    void stopRequested();
    void dismissRequested();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    void applyRecipe();
    void place();
    [[nodiscard]] qreal textLeft() const;

    QString title_;
    QString kindIcon_;
    QString meta_;
    TaskStatus status_ = TaskStatus::Running;
    bool current_ = false;
    IconButton* stop_;
    IconButton* dismiss_;
};

// The background task strip above the composer (`.bg-bar`): a toggle with the count, then
// the rows while expanded. Colours and metrics come from ket::taskBarRecipe.
class TaskBar final : public QWidget {
    Q_OBJECT
  public:
    explicit TaskBar(QWidget* parent = nullptr);

    TaskRow* addTask(const QString& title, const QString& kindIcon = QStringLiteral("terminal"));
    void removeTask(TaskRow* row);
    [[nodiscard]] QList<TaskRow*> tasks() const;
    [[nodiscard]] bool isExpanded() const;
    void setExpanded(bool expanded);
    [[nodiscard]] QAbstractButton* toggle() const { return toggle_; }

  signals:
    void toggled(bool expanded);

  private:
    void applyRecipe();
    void updateToggle();

    QAbstractButton* toggle_;
    QWidget* rowsHost_;
    QVBoxLayout* rows_;
};

} // namespace ket
