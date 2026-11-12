// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRichRecipes.h"

#include <QList>
#include <QString>

#include <optional>

class QTextDocument;

namespace ket {

// A transcript answer splits into prose and fenced code: prose renders as Markdown text,
// each fence as a code block with its own surface and copy button.
struct MarkdownSegment {
    enum class Kind { Text, Code, Quote };
    Kind kind = Kind::Text;
    QString text;     // Markdown for Text; the code, without the fences, for Code
    QString language; // the fence's info string, for Code
};

[[nodiscard]] QList<MarkdownSegment> splitMarkdown(QStringView markdown);

// Applies `recipe` (lengths at 100% zoom) to a document filled by setMarkdown(): body and
// heading sizes, CSS line boxes, paragraph and list spacing, quotes, inline code and links.
// `base` supplies the family; `monoFamily` is used for code (empty keeps the base family).
void styleMarkdown(QTextDocument* document, const MarkdownRecipe& recipe, qreal zoom,
                   const QFont& base, const QString& monoFamily);

// An `@agent/` mention being typed at `cursor` (K.conversationMention in the mock): the
// range to replace and the query after `@agent/`, lower-cased. A plain `@` is not one.
struct AgentMention {
    qsizetype start = 0;
    qsizetype end = 0;
    QString query;
};

[[nodiscard]] std::optional<AgentMention> agentMention(QStringView text, qsizetype cursor);
// The text that replaces a mention: `@agent/<number> `.
[[nodiscard]] QString agentMentionText(QStringView number);

// A complete `@agent/<number>` in a text: [start, end) and the number.
struct MentionSpan {
    qsizetype start = 0;
    qsizetype end = 0;
    QString number;
};

// Every complete mention in `text`. A number is letters, digits, `-` and `_`, so trailing
// punctuation ends it; the `@` follows the start, a space or an opening bracket or quote, as
// in agentMention().
[[nodiscard]] QList<MentionSpan> findAgentMentions(QStringView text);

} // namespace ket
