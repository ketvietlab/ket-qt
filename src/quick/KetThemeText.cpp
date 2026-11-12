// SPDX-License-Identifier: MPL-2.0
// KetTheme: the multi-line text field, drawn mentions and the expandable text.
#include "KetTheme.h"
#include "KetThemeDetail.h"

#include "KetMarkdown.h"
#include "KetMentionTokens.h"
#include "KetTextRecipes.h"

namespace ket {

using detail::scaleLengths;
using detail::themeOf;

QVariantMap QuickTheme::textAreaRecipe(bool dark, qreal zoom, bool hovered, bool focused,
                                       bool enabled, bool invalid, bool overLimit) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::textAreaRecipe(state, invalid, overLimit, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"iconSize", u"spacing", u"fontPixels",
                  u"paddingBlock", u"countPixels", u"countGap"});
    return recipe;
}

QVariantMap QuickTheme::expandableTextRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::expandableTextRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"fontPixels", u"toggleGap"});
    return recipe;
}

QString QuickTheme::mentionLinkedHtml(const QString& text, bool dark, const QStringList& unknown,
                                      int focusedMention) const {
    return ket::mentionLinkedHtml(text, ket::expandableTextRecipe(themeOf(dark)), unknown,
                                  focusedMention);
}

QString QuickTheme::mentionLinkNumber(const QString& link) const {
    return ket::mentionLinkNumber(link);
}

QVariantList QuickTheme::agentMentions(const QString& text) const {
    QVariantList spans;
    for (const MentionSpan& span : findAgentMentions(text))
        spans.append(QVariantMap{{QStringLiteral("start"), int(span.start)},
                                 {QStringLiteral("end"), int(span.end)},
                                 {QStringLiteral("number"), span.number}});
    return spans;
}

QString QuickTheme::spokenMention(const QString& number, const QString& title, bool unknown) const {
    return ket::spokenMention(number, title, unknown);
}

QString QuickTheme::characterCountText(int count, int max) const {
    return ket::characterCountText(count, max);
}

QString QuickTheme::characterCountDescription(int count, int max) const {
    return ket::characterCountDescription(count, max);
}

} // namespace ket
