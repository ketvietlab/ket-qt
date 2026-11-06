// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>

namespace ket {

// Syntax colours for code blocks and code views. Product colours come only from tokens: the
// accent text for keywords and the chart series for the other roles, so every role stays
// distinct and readable in both themes.
enum class SyntaxRole { Keyword, String, Number, Type, Comment, Attribute };

struct SyntaxRecipe {
    QColor keyword;
    QColor string;
    QColor number;
    QColor type;
    QColor comment;
    QColor attribute; // QML, JSON and YAML keys; shell flags
};

[[nodiscard]] SyntaxRecipe syntaxRecipe(Theme theme);
[[nodiscard]] QColor syntaxColor(const SyntaxRecipe& recipe, SyntaxRole role);

struct SyntaxSpan {
    qsizetype start = 0;
    qsizetype length = 0;
    SyntaxRole role = SyntaxRole::Keyword;
};

// Spans of one line in a fence's language (the info string: "cpp", "qml", "sh", ...). `state`
// carries an open block comment from line to line; pass 0 for the first line. An empty,
// "text" or unknown language yields no spans.
[[nodiscard]] QList<SyntaxSpan> highlightLine(QStringView line, QStringView language, int& state);
// Spans of a whole snippet, offsets into `code`.
[[nodiscard]] QList<SyntaxSpan> highlightCode(QStringView code, QStringView language);
// Each line as escaped rich text with the role colours, for a Text item per line.
[[nodiscard]] QStringList highlightedLines(QStringView code, QStringView language,
                                           const SyntaxRecipe& recipe);

// Colours a QTextDocument in place, line by line (Qt Quick TextEdit, QTextEdit).
class SyntaxHighlighter final : public QSyntaxHighlighter {
    Q_OBJECT
  public:
    explicit SyntaxHighlighter(QTextDocument* document);

    void setLanguage(const QString& language);
    void setRecipe(const SyntaxRecipe& recipe);

  protected:
    void highlightBlock(const QString& text) override;

  private:
    QString language_;
    SyntaxRecipe recipe_;
};

} // namespace ket
