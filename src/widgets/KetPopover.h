// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QPointer>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QVBoxLayout;

namespace ket {

class IconButton;

// An anchored in-window panel (`.usage-popover`): a title with a close button, a body the
// app fills, and an optional right-aligned footer. openAbove() places it above an anchor,
// right-aligned, as a child of the anchor's window; Esc, the close button and a click
// outside close it. Colours and metrics come from ket::popoverRecipe.
class Popover final : public QWidget {
    Q_OBJECT
  public:
    explicit Popover(const QString& title = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString title() const;
    void setTitle(const QString& title);
    // The body column; add the app's content to it.
    [[nodiscard]] QVBoxLayout* body() const { return body_; }
    // Adds a button to the right-aligned footer, which shows once it has one.
    void addFooterWidget(QWidget* widget);
    // Body helpers in the popover's type: a section heading, a muted note and a divider.
    QLabel* addHeading(const QString& text);
    QLabel* addNote(const QString& text);
    void addDivider();

    void openAbove(QWidget* anchor);
    [[nodiscard]] QMargins shadowMargins() const;
    [[nodiscard]] PopoverRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  signals:
    void closed();

  public slots:
    void dismiss();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void place();

    QLabel* title_;
    IconButton* close_;
    QVBoxLayout* body_;
    QHBoxLayout* footer_;
    QWidget* footerHost_;
    QPointer<QWidget> anchor_;
    QPointer<QWidget> window_;
};

} // namespace ket
