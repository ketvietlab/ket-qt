// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QPixmap>
#include <QPointer>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QVBoxLayout;

namespace ket {

class Button;
class IconButton;

// Modal dialog drawn inside its window (`.modal` over `.modal-backdrop`), as the mock does,
// rather than a separate top-level. While open it covers the window: the backdrop takes
// the mouse, Tab stays inside the card, Escape, the close button and a backdrop click
// reject. Colours and metrics come from ket::dialogRecipe.
class Dialog final : public QWidget {
    Q_OBJECT
  public:
    // `anchor` is any widget in the window to cover.
    explicit Dialog(const QString& title, QWidget* anchor);

    [[nodiscard]] QString title() const;
    void setTitle(const QString& title);

    // The body content; the dialog takes ownership and deletes the previous body.
    void setBody(QWidget* body);
    [[nodiscard]] QWidget* body() const { return body_; }

    // The card's width in unscaled pixels, as the mock's `width: px(N)`; 0 uses the recipe's.
    // The card still keeps the recipe's margin inside the window.
    void setCardWidth(qreal pixels);
    [[nodiscard]] qreal cardWidth() const { return cardWidth_; }

    // The body draws its own header and footer (a page already built as the mock's modal):
    // the card shows no header and pads nothing. The title stays the card's accessible name.
    void setChromeless(bool chromeless);
    [[nodiscard]] bool isChromeless() const { return chromeless_; }

    // Appends a footer button (right-aligned, in insertion order).
    Button* addButton(const QString& text, ButtonVariant variant = ButtonVariant::Secondary);

    [[nodiscard]] bool isOpen() const { return isVisible(); }
    [[nodiscard]] QWidget* card() const { return card_; }
    [[nodiscard]] DialogRecipe recipe() const;

  public slots:
    void open();
    void accept();
    void reject();

  signals:
    void accepted();
    void rejected();
    void finished(bool accepted);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool focusNextPrevChild(bool next) override;

  private:
    void applyRecipe();
    void place();
    void finish(bool accepted);

    QWidget* card_;
    QWidget* header_;
    QLabel* title_;
    IconButton* close_;
    QVBoxLayout* bodyLayout_;
    QWidget* body_ = nullptr;
    QWidget* footer_;
    QHBoxLayout* footerLayout_;
    QPointer<QWidget> restoreFocus_;
    qreal cardWidth_ = 0;
    bool chromeless_ = false;
    QPixmap underlay_; // the window beneath, when the dialog is above native children
};

} // namespace ket
