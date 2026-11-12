// SPDX-License-Identifier: MPL-2.0
#include "KetMentionTokens.h"

#include <QCoreApplication>
#include <QTextDocument>

#include <algorithm>

namespace ket {

MentionTokens::MentionTokens(QTextDocument* document)
    : QSyntaxHighlighter(static_cast<QObject*>(document)) {
    // Connected before setDocument(), so the spans move before the highlighter redraws the
    // changed blocks.
    connect(document, &QTextDocument::contentsChange, this, &MentionTokens::contentsChanged);
    setDocument(document);
    text_ = document->toPlainText();
    reset();
}

QStringList MentionTokens::numbers() const {
    QStringList numbers;
    for (const MentionSpan& span : spans_)
        numbers.append(span.number);
    return numbers;
}

int MentionTokens::insert(int start, int end, const QString& number, const QString& title) {
    QTextCursor cursor(document());
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    const QString text = agentMentionText(number);
    inserting_ = true;
    cursor.insertText(text);
    inserting_ = false;
    // agentMentionText() ends with a space, which is not part of the token.
    const MentionSpan span{start, start + text.size() - 1, number};
    spans_.append(span);
    std::sort(spans_.begin(), spans_.end(),
              [](const MentionSpan& a, const MentionSpan& b) { return a.start < b.start; });
    if (!title.isEmpty())
        titles_.insert(number, title);
    rehighlightBlock(document()->findBlock(start));
    emit spansChanged();
    return int(start + text.size());
}

void MentionTokens::reset() {
    text_ = document()->toPlainText();
    spans_ = findAgentMentions(text_);
    rehighlight();
    emit spansChanged();
}

std::optional<std::pair<int, int>> MentionTokens::deletion(int position, bool backward) const {
    for (const MentionSpan& span : spans_) {
        const bool hit = backward ? span.start < position && position <= span.end
                                  : span.start <= position && position < span.end;
        if (hit)
            return std::pair{int(span.start), int(span.end)};
    }
    return std::nullopt;
}

int MentionTokens::snap(int position, int previous) const {
    for (const MentionSpan& span : spans_) {
        if (position <= span.start || position >= span.end)
            continue;
        if (previous <= span.start)
            return int(span.end);
        if (previous >= span.end)
            return int(span.start);
        return position - span.start <= span.end - position ? int(span.start) : int(span.end);
    }
    return position;
}

void MentionTokens::setTheme(Theme theme) {
    if (theme_ == theme)
        return;
    theme_ = theme;
    rehighlight();
}

void MentionTokens::setUnknown(const QStringList& numbers) {
    if (unknown_ == numbers)
        return;
    unknown_ = numbers;
    rehighlight();
}

void MentionTokens::setTitle(const QString& number, const QString& title) {
    titles_.insert(number, title);
}

QString spokenMention(const QString& number, const QString& title, bool unknown) {
    const QString agent = QCoreApplication::translate("ket::MentionTokens", "agent %1").arg(number);
    const QString name = title.isEmpty() ? agent : QStringLiteral("%1 (%2)").arg(title, agent);
    return unknown ? QCoreApplication::translate("ket::MentionTokens", "%1, unknown").arg(name)
                   : name;
}

QString spokenMentions(const QList<MentionSpan>& spans, const QHash<QString, QString>& titles,
                       const QStringList& unknown) {
    QStringList names;
    for (const MentionSpan& span : spans)
        names.append(
            spokenMention(span.number, titles.value(span.number), unknown.contains(span.number)));
    if (names.isEmpty())
        return {};
    return QCoreApplication::translate("ket::MentionTokens", "Mentions: %1")
        .arg(names.join(QStringLiteral(", ")));
}

QString MentionTokens::spokenMentions() const {
    return ket::spokenMentions(spans_, titles_, unknown_);
}

void MentionTokens::highlightBlock(const QString& text) {
    const qsizetype block = currentBlock().position();
    for (const MentionSpan& span : spans_) {
        const qsizetype start = qMax(span.start, block);
        const qsizetype end = qMin(span.end, block + text.size());
        if (start >= end)
            continue;
        const MentionTokenRecipe r = mentionTokenRecipe(unknown_.contains(span.number), theme_);
        QTextCharFormat format;
        format.setForeground(r.text);
        format.setBackground(r.background);
        format.setFontWeight(r.weight);
        setFormat(int(start - block), int(end - start), format);
    }
}

void MentionTokens::contentsChanged(int, int, int) {
    const QString text = document()->toPlainText();
    if (text == text_)
        return; // a format change, such as the highlighter's own
    // The edit as the common prefix and suffix of the old and new text; the signal's own
    // numbers count the document's last block separator.
    qsizetype prefix = 0;
    const qsizetype shortest = qMin(text.size(), text_.size());
    while (prefix < shortest && text.at(prefix) == text_.at(prefix))
        ++prefix;
    qsizetype suffix = 0;
    while (suffix < shortest - prefix &&
           text.at(text.size() - 1 - suffix) == text_.at(text_.size() - 1 - suffix))
        ++suffix;
    const qsizetype removed = text_.size() - prefix - suffix;
    const qsizetype added = text.size() - prefix - suffix;
    text_ = text;

    QList<MentionSpan> kept;
    for (MentionSpan span : spans_) {
        if (span.end <= prefix) {
            kept.append(span);
        } else if (span.start >= prefix + removed) {
            span.start += added - removed;
            span.end += added - removed;
            kept.append(span);
        }
        // An edit inside a token turns it back into plain text.
    }
    const QList<MentionSpan> complete = findAgentMentions(text);
    // A character typed at a token's edge changes what the text says there: `@agent/12` and a
    // `3` after it is `@agent/123`, and an `x` before the `@` is no mention at all. A token is
    // kept only while the text still holds exactly that mention.
    const auto holds = [&complete](const MentionSpan& span) {
        return std::any_of(complete.cbegin(), complete.cend(), [&span](const MentionSpan& one) {
            return one.start == span.start && one.end == span.end && one.number == span.number;
        });
    };
    kept.erase(std::remove_if(kept.begin(), kept.end(),
                              [&holds](const MentionSpan& span) { return !holds(span); }),
               kept.end());
    if (!inserting_ && added > 1) {
        // Pasted or set text: its complete mentions become tokens.
        for (const MentionSpan& span : complete) {
            if (span.start < prefix || span.end > prefix + added)
                continue;
            kept.append(span);
        }
        std::sort(kept.begin(), kept.end(),
                  [](const MentionSpan& a, const MentionSpan& b) { return a.start < b.start; });
    }
    const bool changed = kept.size() != spans_.size() || added != removed;
    spans_ = kept;
    if (changed)
        emit spansChanged();
}

} // namespace ket
