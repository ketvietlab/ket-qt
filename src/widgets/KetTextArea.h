// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetMentionPicker.h"
#include "KetTextRecipes.h"

#include <QPointer>
#include <QWidget>

#include <optional>

class QTextEdit;

namespace ket {

class MentionTokens;

// Multi-line plain-text form field (`textarea`) in the text field's look; it draws
// ket::textAreaRecipe. It shows minimumLines() line boxes, grows with the text up to
// maximumLines() and then scrolls. Pasting inserts plain text and keeps line breaks; Return
// inserts a line break, Tab moves the focus, and Cmd/Ctrl+Return is left to the window
// (a QShortcut or the parent's keyPressEvent()).
//
// With maxLength() above zero, an `n / max` count sits below the field; above the limit the
// field is invalid, and the text is never cut.
//
// `@agent/` completion works as in ket::Composer: answer mentionQueryChanged() with
// setMentionItems(). A chosen mention is drawn as a token (ket::MentionTokens): Backspace or
// Delete removes it whole, the caret never stops inside it, and text() returns the plain
// `@agent/<number>`.
class TextArea final : public QWidget {
    Q_OBJECT
  public:
    explicit TextArea(QWidget* parent = nullptr);

    [[nodiscard]] QString text() const;
    void setText(const QString& text); // draws the text's mentions
    [[nodiscard]] QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    [[nodiscard]] int minimumLines() const;
    void setMinimumLines(int lines); // 0 restores the recipe's default
    [[nodiscard]] int maximumLines() const;
    void setMaximumLines(int lines); // 0 restores the recipe's default
    [[nodiscard]] int maxLength() const { return maxLength_; }
    void setMaxLength(int length); // 0: no limit and no count
    [[nodiscard]] bool isOverLimit() const;
    [[nodiscard]] bool isInvalid() const { return invalid_; }
    void setInvalid(bool invalid);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] QTextEdit* editor() const { return editor_; }
    [[nodiscard]] MentionTokens* mentions() const { return tokens_; }
    [[nodiscard]] TextAreaRecipe recipe() const;
    // The visible line boxes, between minimumLines() and maximumLines().
    [[nodiscard]] int visibleLines() const;

    void setMentionItems(const QList<MentionItem>& items);
    void closeMentions();
    [[nodiscard]] MentionPicker* mentionPicker() const { return picker_; }
    [[nodiscard]] bool isMentionOpen() const;
    // Numbers the host does not know; their tokens are drawn in the danger colours.
    void setUnknownMentions(const QStringList& numbers);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  signals:
    void textChanged();
    void mentionQueryChanged(const QString& query);
    void mentionClosed();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void changeEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void fitEditor();
    void syncAccessibility();
    void updateMention();
    void applyMention(const MentionItem& item);
    void placePicker();
    bool handleKey(QKeyEvent* event);
    [[nodiscard]] qreal lineBox() const;
    [[nodiscard]] int countHeight() const;

    QTextEdit* editor_;
    MentionTokens* tokens_;
    int minLines_ = 0;
    int maxLines_ = 0;
    int maxLength_ = 0;
    int lastPosition_ = 0;
    bool invalid_ = false;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
    QPointer<MentionPicker> picker_;
    std::optional<QString> mentionQuery_;
};

} // namespace ket
