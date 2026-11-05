// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetConversationRecipes.h"

#include <QWidget>

class QAbstractButton;
class QHBoxLayout;
class QTextEdit;

namespace ket {

class Button;

// Message composer (`.composer`): a growing plain-text input with a send button, and a
// toolbar row below it for the app's controls (attach, mode, model…). Enter sends and
// Shift+Enter breaks the line. While busy a stop button joins the send button. Colours and
// metrics come from ket::composerRecipe.
class Composer final : public QWidget {
    Q_OBJECT
  public:
    explicit Composer(QWidget* parent = nullptr);

    [[nodiscard]] QString text() const;
    void setText(const QString& text);
    void setPlaceholderText(const QString& text);
    [[nodiscard]] bool isBusy() const { return busy_; }
    void setBusy(bool busy);

    // The toolbar's leading and trailing groups; add the app's buttons to them.
    [[nodiscard]] QHBoxLayout* leadingTools() const { return leading_; }
    [[nodiscard]] QHBoxLayout* trailingTools() const { return trailing_; }
    [[nodiscard]] QTextEdit* editor() const { return editor_; }
    [[nodiscard]] Button* sendButton() const { return send_; }

    [[nodiscard]] ComposerRecipe recipe() const;

  signals:
    void submitted(const QString& text);
    void stopRequested();

  public slots:
    void submit();

  protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void fitEditor();

    QTextEdit* editor_;
    Button* send_;
    QAbstractButton* stop_;
    QHBoxLayout* leading_;
    QHBoxLayout* trailing_;
    QWidget* toolbar_;
    bool busy_ = false;
    bool focused_ = false;
};

} // namespace ket
