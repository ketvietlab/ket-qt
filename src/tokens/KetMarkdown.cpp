// SPDX-License-Identifier: MPL-2.0
#include "KetMarkdown.h"

#include "KetAppearance.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextList>
#include <QTextTable>
#include <QtMath>

namespace ket {
namespace {

// A fence opens with three or more backticks or tildes, indented at most three spaces.
const QRegularExpression& fence() {
    static const QRegularExpression pattern(QStringLiteral(R"(^ {0,3}(`{3,}|~{3,})\s*([^`\s]*))"));
    return pattern;
}

// A quote line starts with `>`, indented at most three spaces; one space after it is markup.
const QRegularExpression& quoteLine() {
    static const QRegularExpression pattern(QStringLiteral(R"(^ {0,3}> ?(.*)$)"));
    return pattern;
}

bool isMentionChar(QChar c) {
    return !c.isSpace() && c != QLatin1Char('@') && c != QLatin1Char('"') && c != QLatin1Char('$');
}

} // namespace

QList<MarkdownSegment> splitMarkdown(QStringView markdown) {
    QList<MarkdownSegment> segments;
    QStringList prose;
    QStringList code;
    QStringList quote;
    QString marker;
    QString language;
    const auto flushProse = [&] {
        const QString text = prose.join(QLatin1Char('\n')).trimmed();
        if (!text.isEmpty())
            segments.append({MarkdownSegment::Kind::Text, text, {}});
        prose.clear();
    };
    const auto flushQuote = [&] {
        const QString text = quote.join(QLatin1Char('\n')).trimmed();
        if (!text.isEmpty())
            segments.append({MarkdownSegment::Kind::Quote, text, {}});
        quote.clear();
    };
    for (const QString& line : markdown.toString().split(QLatin1Char('\n'))) {
        if (marker.isEmpty()) {
            const QRegularExpressionMatch match = fence().match(line);
            if (match.hasMatch()) {
                flushProse();
                flushQuote();
                marker = match.captured(1);
                language = match.captured(2);
                continue;
            }
            // Top-level quotes become callouts; a line without `>` ends the quote.
            const QRegularExpressionMatch quoted = quoteLine().match(line);
            if (quoted.hasMatch()) {
                flushProse();
                quote.append(quoted.captured(1));
                continue;
            }
            flushQuote();
            prose.append(line);
            continue;
        }
        // A closing fence uses the same character and is at least as long.
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(marker.at(0)) && trimmed.size() >= marker.size() &&
            trimmed.count(marker.at(0)) == trimmed.size()) {
            segments.append({MarkdownSegment::Kind::Code, code.join(QLatin1Char('\n')), language});
            code.clear();
            marker.clear();
            language.clear();
            continue;
        }
        code.append(line);
    }
    if (!marker.isEmpty()) // an unclosed fence runs to the end, as CommonMark has it
        segments.append({MarkdownSegment::Kind::Code, code.join(QLatin1Char('\n')), language});
    flushProse();
    flushQuote();
    return segments;
}

void styleMarkdown(QTextDocument* document, const MarkdownRecipe& recipe, qreal zoom,
                   const QFont& base, const QString& monoFamily) {
    if (!document)
        return;
    const auto px = [zoom](qreal length) { return Appearance::scale(length, zoom); };
    const qreal body = px(recipe.bodyPixels);
    document->setIndentWidth(px(recipe.listIndent));
    document->setDocumentMargin(0);
    QFont font = base;
    font.setPixelSize(qRound(body));
    document->setDefaultFont(font);

    QTextCursor cursor(document);
    cursor.beginEditBlock();
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        const int level = format.headingLevel();
        const bool quote = format.intProperty(QTextFormat::BlockQuoteLevel) > 0;
        const bool listItem = block.textList() != nullptr;
        const bool last = !block.next().isValid();
        const bool nextInList =
            listItem && block.next().isValid() && block.next().textList() == block.textList();
        const bool code =
            format.hasProperty(QTextFormat::BlockCodeFence) || format.nonBreakableLines();

        const qreal size = level == 1   ? px(recipe.heading1Pixels)
                           : level == 2 ? px(recipe.heading2Pixels)
                           : level == 3 ? px(recipe.heading3Pixels)
                           : code       ? px(recipe.codePixels)
                                        : body;
        const qreal lineHeight = level > 0 ? recipe.headingLineHeight : recipe.lineHeight;
        format.setLineHeight(qFloor(size * lineHeight), QTextBlockFormat::FixedHeight);
        format.setTopMargin(0);
        format.setBottomMargin(last         ? 0
                               : level > 0  ? px(recipe.headingSpacingAfter)
                               : nextInList ? px(recipe.listItemGap)
                                            : px(recipe.paragraphSpacing));
        if (quote) {
            format.setBackground(recipe.quoteBackground);
            format.setLeftMargin(px(recipe.quotePadding));
        }
        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);

        // Character formats, fragment by fragment, keep Markdown's bold, code and links.
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;
            QTextCharFormat chars = fragment.charFormat();
            const bool mono = chars.fontFixedPitch() || code;
            if (mono && !monoFamily.isEmpty())
                chars.setFontFamilies({monoFamily});
            else
                chars.setFontFamilies(base.families());
            chars.clearProperty(QTextFormat::FontPointSize);
            chars.setProperty(QTextFormat::FontPixelSize,
                              qRound(mono && level == 0 ? px(recipe.codePixels) : size));
            if (level > 0)
                chars.setFontWeight(recipe.headingWeight);
            if (level == 1 || level == 2) {
                chars.setFontLetterSpacingType(QFont::AbsoluteSpacing);
                chars.setFontLetterSpacing(size * recipe.headingTracking);
            } else if (chars.fontWeight() >= QFont::DemiBold)
                chars.setFontWeight(recipe.strongWeight);
            if (chars.isAnchor()) {
                chars.setForeground(recipe.link);
                chars.setFontUnderline(true);
            } else {
                chars.setForeground(quote       ? recipe.quoteText
                                    : level > 0 ? recipe.heading
                                                : recipe.text);
            }
            cursor.setPosition(fragment.position());
            cursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
            cursor.setCharFormat(chars);
        }
    }

    // Tables: one collapsed hairline in the panel border colour, with padded cells.
    for (QTextFrame* frame : document->rootFrame()->childFrames()) {
        auto* table = qobject_cast<QTextTable*>(frame);
        if (!table)
            continue;
        QTextTableFormat format = table->format();
        format.setBorder(1);
        format.setBorderBrush(recipe.tableBorder);
        format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        format.setBorderCollapse(true);
        format.setCellSpacing(0);
        format.setCellPadding(px(recipe.tableCellPadding));
        table->setFormat(format);
    }
    cursor.endEditBlock();
}

std::optional<AgentMention> agentMention(QStringView text, qsizetype cursor) {
    cursor = qBound<qsizetype>(0, cursor, text.size());
    qsizetype start = cursor - 1;
    while (start >= 0 && isMentionChar(text.at(start)))
        --start;
    if (start < 0 || text.at(start) != QLatin1Char('@'))
        return std::nullopt;
    if (start > 0) {
        const QChar before = text.at(start - 1);
        if (!before.isSpace() && !QStringLiteral("([{'\"").contains(before))
            return std::nullopt;
    }
    const QStringView typed = text.mid(start + 1, cursor - start - 1);
    if (!typed.startsWith(u"agent/"))
        return std::nullopt; // a plain `@` mentions a file
    qsizetype end = cursor;
    while (end < text.size() && isMentionChar(text.at(end)))
        ++end;
    return AgentMention{start, end, typed.mid(6).toString().toLower()};
}

QString agentMentionText(QStringView number) { return QStringLiteral("@agent/%1 ").arg(number); }

} // namespace ket
