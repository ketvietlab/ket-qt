// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetAttachmentChip.h"
#include "KetConversationRecipes.h"
#include "KetMentionPicker.h"

#include <QPointer>
#include <QWidget>

#include <optional>

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

    // `@agent/` completion: answer mentionQueryChanged() with setMentionItems(), which shows
    // the picker over the composer (an empty list shows its empty message).
    void setMentionItems(const QList<MentionItem>& items);
    void closeMentions();
    [[nodiscard]] MentionPicker* mentionPicker() const { return picker_; }
    [[nodiscard]] bool isMentionOpen() const;

    // Attachments (`.attachment-chips`) above the input; removing a chip emits
    // attachmentRemoved() after it is gone.
    AttachmentChip* addAttachment(const QString& name,
                                  const QString& iconCode = QStringLiteral("file"));
    [[nodiscard]] QStringList attachments() const;
    void removeAttachment(int index);
    void clearAttachments();

  signals:
    void submitted(const QString& text);
    void stopRequested();
    void mentionQueryChanged(const QString& query);
    void mentionClosed();
    void attachmentRemoved(int index, const QString& name);

  public slots:
    void submit();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void applyRecipe();
    void fitEditor();
    void updateMention();
    void applyMention(const MentionItem& item);
    void placePicker();
    bool handleMentionKey(QKeyEvent* event);

    QTextEdit* editor_;
    Button* send_;
    QAbstractButton* stop_;
    QHBoxLayout* leading_;
    QHBoxLayout* trailing_;
    QHBoxLayout* input_;
    QWidget* chips_;
    QHBoxLayout* chipRow_;
    QList<AttachmentChip*> attachments_;
    QWidget* toolbar_;
    bool busy_ = false;
    bool focused_ = false;
    QPointer<MentionPicker> picker_;
    std::optional<QString> mentionQuery_;
};

} // namespace ket
