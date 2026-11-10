// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QPixmap>
#include <QPointer>
#include <QTimer>
#include <QWidget>

namespace ket {

// Transient confirmation (`.toast`) centred near the bottom of a window. It is a child
// of the window, not a separate top-level, so it moves and hides with it. Colours and
// metrics come from ket::toastRecipe.
class Toast final : public QWidget {
    Q_OBJECT
  public:
    // `anchor` is any widget in the target window; the toast attaches to its window.
    explicit Toast(const QString& text, QWidget* anchor, ToastTone tone = ToastTone::Success);

    // Shows a toast that deletes itself after the recipe duration, replacing any toast
    // this function showed earlier in the same window.
    static Toast* showMessage(QWidget* anchor, const QString& text,
                              ToastTone tone = ToastTone::Success);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);

    // Shows the toast and restarts its timer; `durationMs` <= 0 keeps it until hidden.
    void popup(int durationMs);

    [[nodiscard]] ToastTone tone() const { return tone_; }
    [[nodiscard]] ToastRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    [[nodiscard]] QFont toastFont() const;
    [[nodiscard]] QMargins margins() const;
    void place();
    // The text's box: as wide as its longest line, wrapped to fit the card's widest width.
    [[nodiscard]] QRectF textBox() const;

    QString text_;
    ToastTone tone_;
    QTimer timer_;
    QPixmap underlay_; // the window beneath, when the toast is above native children
};

} // namespace ket
