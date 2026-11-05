// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QAbstractButton>
#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;

namespace ket {

// One file in a ket::ChangedFiles card (`.changed-file`): the name over its folder, the
// added and removed line counts, and an arrow to open it.
class ChangedFileRow final : public QAbstractButton {
    Q_OBJECT
  public:
    ChangedFileRow(const QString& name, const QString& path, int added, int removed,
                   QWidget* parent = nullptr);

    [[nodiscard]] QString path() const { return path_; }
    [[nodiscard]] int added() const { return added_; }
    [[nodiscard]] int removed() const { return removed_; }
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    QString path_;
    int added_;
    int removed_;
};

// The changed files card in the transcript (`.changed-files`): a heading with the totals,
// a row per file and a footer for the app's link and note. Colours and metrics come from
// ket::changedFilesRecipe.
class ChangedFiles final : public QWidget {
    Q_OBJECT
  public:
    explicit ChangedFiles(QWidget* parent = nullptr);

    ChangedFileRow* addFile(const QString& name, const QString& path, int added, int removed);
    [[nodiscard]] QList<ChangedFileRow*> files() const { return rows_; }
    [[nodiscard]] QString title() const; // "3 files changed" unless set
    void setTitle(const QString& title);
    void setNote(const QString& note); // the footer's right-hand text
    [[nodiscard]] QHBoxLayout* footer() const { return footer_; }

  signals:
    void fileActivated(int index);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QWidget* heading_;
    QVBoxLayout* rowsLayout_;
    QWidget* footerHost_;
    QHBoxLayout* footer_;
    QList<ChangedFileRow*> rows_;
    QString title_;
    QString note_;
    friend class ChangedHeading;
};

} // namespace ket
