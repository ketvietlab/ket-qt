// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

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
    explicit Toast(const QString& text, QWidget* anchor);

    // Shows a toast that deletes itself after the recipe duration, replacing any toast
    // this function showed earlier in the same window.
    static Toast* showMessage(QWidget* anchor, const QString& text);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);

    // Shows the toast and restarts its timer; `durationMs` <= 0 keeps it until hidden.
    void popup(int durationMs);

    [[nodiscard]] ToastRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    [[nodiscard]] QFont toastFont() const;
    [[nodiscard]] QMargins margins() const;
    void place();

    QString text_;
    QTimer timer_;
};

} // namespace ket
