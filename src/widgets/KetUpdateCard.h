// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QAbstractButton>
#include <QStringList>
#include <QWidget>

#include <optional>

class QHBoxLayout;
class QLabel;
class QProgressBar;

namespace ket {

// An update in Settings (`.update-card`): the title with badges, a muted detail, an
// optional progress bar, the release notes as a list and the actions. A ready update takes
// the success border. Colours and metrics come from ket::updateCardRecipe.
class UpdateCard final : public QWidget {
    Q_OBJECT
  public:
    explicit UpdateCard(const QString& title = {}, QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setDetail(const QString& detail);
    void setNotes(const QStringList& notes);
    void setProgress(int percent); // negative hides the bar
    [[nodiscard]] bool isReady() const { return ready_; }
    void setReady(bool ready);
    // Adds a badge after the title, or an action button to the row under the notes; the
    // card takes ownership. The actions row shows once it has an action.
    void addBadge(QWidget* badge);
    void addAction(QWidget* action);
    [[nodiscard]] QProgressBar* progressBar() const { return progress_; }

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QLabel* title_;
    QHBoxLayout* header_;
    QLabel* detail_;
    QWidget* progressHost_;
    QProgressBar* progress_;
    QWidget* notesHost_;
    QWidget* actionsHost_;
    QHBoxLayout* actions_;
    bool ready_ = false;
};

// The sidebar's update notice (`.update-pill`): an icon, the title in the accent and a
// detail. Colours and metrics come from ket::updatePillRecipe.
class UpdatePill final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit UpdatePill(const QString& title = {}, const QString& detail = {},
                        QWidget* parent = nullptr);

    void setDetail(const QString& detail);
    void setPreviewState(std::optional<InteractionState> state);
    [[nodiscard]] UpdatePillRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString detail_;
    std::optional<InteractionState> preview_;
};

} // namespace ket
