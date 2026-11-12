// SPDX-License-Identifier: MPL-2.0
// Recipes of the multi-line text field, drawn mentions and the expandable text.
#include "KetTextRecipes.h"

#include "KetMarkdown.h"
#include "KetProfile.h"

#include <QCoreApplication>

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

const Profile& profile() { return Profile::current(); }

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

QString cssColor(const QColor& color) {
    return QStringLiteral("rgba(%1,%2,%3,%4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alphaF(), 0, 'f', 3);
}

QString escapedLines(QStringView text) {
    QString html = text.toString().toHtmlEscaped();
    html.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    html.replace(QLatin1Char('\n'), QLatin1String("<br>"));
    return html;
}

} // namespace

TextAreaRecipe textAreaRecipe(const InteractionState& state, bool invalid, bool overLimit,
                              Theme theme) {
    TextAreaRecipe recipe;
    recipe.field = fieldRecipe(state, invalid || overLimit, theme);
    recipe.count = token(overLimit ? u"danger" : u"text-muted", theme);
    recipe.paddingBlock = profile().textAreaPaddingBlock;
    recipe.lineHeight = profile().textAreaLineHeight;
    recipe.minLines = profile().textAreaMinLines;
    recipe.maxLines = qMax(profile().textAreaMinLines, profile().textAreaMaxLines);
    recipe.countPixels = profile().textAreaCountPixels;
    recipe.countGap = profile().textAreaCountGap;
    return recipe;
}

MentionTokenRecipe mentionTokenRecipe(bool unknown, Theme theme) {
    MentionTokenRecipe recipe;
    recipe.text = token(unknown ? u"danger" : u"accent-text", theme);
    recipe.background = token(unknown ? u"danger-bg" : u"accent-subtle", theme);
    recipe.weight = profile().mentionTokenWeight;
    return recipe;
}

ExpandableTextRecipe expandableTextRecipe(Theme theme) {
    ExpandableTextRecipe recipe;
    recipe.text = token(u"text-main", theme);
    recipe.focus = token(u"focus-border", theme);
    recipe.mention = mentionTokenRecipe(false, theme);
    recipe.unknownMention = mentionTokenRecipe(true, theme);
    recipe.fontPixels = profile().expandablePixels;
    recipe.lineHeight = profile().expandableLineHeight;
    recipe.foldedLines = qMax(1, profile().expandableFoldedLines);
    recipe.toggleGap = profile().expandableToggleGap;
    return recipe;
}

QString characterCountText(qsizetype count, qsizetype max) {
    return QStringLiteral("%1 / %2").arg(count).arg(max);
}

QString characterCountDescription(qsizetype count, qsizetype max) {
    if (count > max)
        return QCoreApplication::translate("ket::TextArea", "%1 characters over the limit of %2")
            .arg(count - max)
            .arg(max);
    return QCoreApplication::translate("ket::TextArea", "%1 of %2 characters").arg(count).arg(max);
}

QString mentionLinkedHtml(QStringView text, const ExpandableTextRecipe& recipe,
                          const QStringList& unknown, int focusedMention) {
    QString html;
    qsizetype from = 0;
    int index = 0;
    for (const MentionSpan& span : findAgentMentions(text)) {
        html += escapedLines(text.mid(from, span.start - from));
        const MentionTokenRecipe& look =
            unknown.contains(span.number) ? recipe.unknownMention : recipe.mention;
        QString style = QStringLiteral("color:%1;background-color:%2;font-weight:%3;")
                            .arg(cssColor(look.text), cssColor(look.background))
                            .arg(look.weight);
        style +=
            index == focusedMention
                ? QStringLiteral("text-decoration:underline;color:%1;").arg(cssColor(recipe.focus))
                : QStringLiteral("text-decoration:none;");
        html += QStringLiteral("<a href=\"agent:%1\" style=\"%2\">%3</a>")
                    .arg(span.number.toHtmlEscaped(), style,
                         text.mid(span.start, span.end - span.start).toString().toHtmlEscaped());
        from = span.end;
        ++index;
    }
    html += escapedLines(text.mid(from));
    return html;
}

QString mentionLinkNumber(QStringView link) {
    return link.startsWith(u"agent:") ? link.mid(6).toString() : QString();
}

QVariantMap toVariantMap(const TextAreaRecipe& r) {
    QVariantMap map = toVariantMap(r.field);
    map.insert(QStringLiteral("count"), r.count);
    map.insert(QStringLiteral("paddingBlock"), r.paddingBlock);
    map.insert(QStringLiteral("lineHeight"), r.lineHeight);
    map.insert(QStringLiteral("minLines"), r.minLines);
    map.insert(QStringLiteral("maxLines"), r.maxLines);
    map.insert(QStringLiteral("countPixels"), r.countPixels);
    map.insert(QStringLiteral("countGap"), r.countGap);
    return map;
}

QVariantMap toVariantMap(const MentionTokenRecipe& r) {
    return mapOf({{"text", r.text}, {"background", r.background}, {"weight", r.weight}});
}

QVariantMap toVariantMap(const ExpandableTextRecipe& r) {
    return mapOf({{"text", r.text},
                  {"focus", r.focus},
                  {"mention", toVariantMap(r.mention)},
                  {"unknownMention", toVariantMap(r.unknownMention)},
                  {"fontPixels", r.fontPixels},
                  {"lineHeight", r.lineHeight},
                  {"foldedLines", r.foldedLines},
                  {"toggleGap", r.toggleGap}});
}

} // namespace ket
