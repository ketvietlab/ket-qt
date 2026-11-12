// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetMarkdown.h"
#include "KetTextRecipes.h"

#include <QHash>
#include <QSyntaxHighlighter>

#include <optional>

namespace ket {

// A mention as assistive technology reads it: "Refine navigation (agent 2035)", "agent 2035"
// without a title, with ", unknown" when the host does not know the number.
[[nodiscard]] QString spokenMention(const QString& number, const QString& title, bool unknown);
// "Mentions: …" for every span, or an empty string.
[[nodiscard]] QString spokenMentions(const QList<MentionSpan>& spans,
                                     const QHash<QString, QString>& titles,
                                     const QStringList& unknown);

// The drawn `@agent/` mentions of an editable plain-text document (QTextEdit or a Qt Quick
// TextEdit). The document's text stays plain: text() of the field returns `@agent/<number>`,
// and the tokens are character formats drawn over it.
//
// - Text inserted in one edit (set, pasted, or a choice from the mention picker) has its
//   mentions drawn; characters typed one by one do not become a token until one is chosen.
// - A token is atomic: an edit inside it turns it back into plain text, Backspace at its end
//   and Delete at its start remove it whole (deletion()), and the caret never stops inside it
//   (snap()).
// - The host marks numbers it does not know (setUnknown()); they are drawn in the danger
//   colours, and the text does not change.
class MentionTokens final : public QSyntaxHighlighter {
    Q_OBJECT
  public:
    explicit MentionTokens(QTextDocument* document);

    [[nodiscard]] QList<MentionSpan> spans() const { return spans_; }
    [[nodiscard]] QStringList numbers() const;
    // Replaces [start, end) with `@agent/<number> ` and draws it; the caret position after it.
    int insert(int start, int end, const QString& number, const QString& title = {});
    // Draws every mention of the current text again (after the host sets a text).
    void reset();

    // The range Backspace (`backward`) or Delete removes at `position`, when it is a token.
    [[nodiscard]] std::optional<std::pair<int, int>> deletion(int position, bool backward) const;
    // `position` moved out of a token: past it in the direction the caret moved from
    // `previous`, or to the nearer edge when `previous` was inside it too.
    [[nodiscard]] int snap(int position, int previous) const;

    void setTheme(Theme theme);
    void setUnknown(const QStringList& numbers);
    [[nodiscard]] QStringList unknown() const { return unknown_; }
    // The conversation each number names, for assistive technology.
    void setTitle(const QString& number, const QString& title);
    [[nodiscard]] QString title(const QString& number) const { return titles_.value(number); }
    // "Mentions: Refine navigation (agent 2035), …", or an empty string.
    [[nodiscard]] QString spokenMentions() const;

  signals:
    void spansChanged();

  protected:
    void highlightBlock(const QString& text) override;

  private:
    void contentsChanged(int position, int removed, int added);

    QList<MentionSpan> spans_;
    QString text_; // the text the spans refer to
    QStringList unknown_;
    QHash<QString, QString> titles_;
    Theme theme_ = Theme::Light;
    bool inserting_ = false;
};

} // namespace ket
