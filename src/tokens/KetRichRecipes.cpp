// SPDX-License-Identifier: MPL-2.0
// Rich transcript recipes: Markdown, code blocks, the mention picker and the usage ring.
#include "KetRichRecipes.h"

#include "KetProfile.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

const Profile& profile() { return Profile::current(); }

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

} // namespace

MarkdownRecipe markdownRecipe(Theme theme) {
    MarkdownRecipe recipe;
    recipe.text = token(u"text-main", theme);
    recipe.heading = token(u"text-main", theme);
    recipe.link = token(u"accent-text", theme);
    recipe.quoteBackground = token(u"accent-subtle", theme);
    recipe.quoteText = token(u"text-main", theme);
    recipe.quoteBorder = token(u"accent", theme);
    recipe.rule = token(u"panel-border", theme);
    recipe.bodyPixels = profile().readingPixels;
    recipe.lineHeight = profile().readingLineHeight;
    recipe.heading1Pixels = profile().markdownHeading1Pixels;
    recipe.heading2Pixels = profile().markdownHeading2Pixels;
    recipe.heading3Pixels = profile().markdownHeading3Pixels;
    recipe.headingLineHeight = profile().markdownHeadingLineHeight;
    recipe.headingWeight = qRound(Tokens::number(u"weight-semibold"));
    recipe.headingSpacingAfter = profile().markdownHeadingSpacingAfter;
    recipe.paragraphSpacing = profile().markdownParagraphSpacing;
    recipe.listIndent = profile().markdownListIndent;
    recipe.listItemGap = profile().markdownListItemGap;
    recipe.strongWeight = profile().markdownStrongWeight;
    recipe.quotePadding = profile().markdownQuotePadding;
    recipe.quoteBorderWidth = profile().markdownQuoteBorder;
    recipe.codePixels = profile().codePixels;
    return recipe;
}

CodeBlockRecipe codeBlockRecipe(Theme theme) {
    CodeBlockRecipe recipe;
    recipe.background = token(u"panel-bg-subtle", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.text = token(u"text-main", theme);
    recipe.padding = profile().codePadding;
    recipe.radius = profile().codeRadius;
    recipe.margin = profile().codeMargin;
    recipe.fontPixels = profile().codePixels;
    recipe.lineHeight = profile().codeLineHeight;
    return recipe;
}

MentionPickerRecipe mentionPickerRecipe(Theme theme) {
    MentionPickerRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"border-strong", theme);
    recipe.title = token(u"text-muted", theme);
    recipe.empty = token(u"text-muted", theme);
    recipe.shadow = shadowToken(u"shadow-popover", theme);
    recipe.maxWidth = profile().mentionMaxWidth;
    recipe.padding = profile().mentionPadding;
    recipe.radius = profile().mentionRadius;
    recipe.overlap = profile().mentionOverlap;
    recipe.titlePixels = profile().mentionTitlePixels;
    recipe.titleWeight = profile().mentionTitleWeight;
    recipe.titlePaddingInline = profile().mentionTitlePaddingInline;
    recipe.titlePaddingTop = profile().mentionTitlePaddingTop;
    recipe.titlePaddingBottom = profile().mentionTitlePaddingBottom;
    recipe.emptyPixels = profile().mentionOptionPixels;
    recipe.maxItems = profile().mentionMaxItems;
    return recipe;
}

MentionOptionRecipe mentionOptionRecipe(bool highlighted, Theme theme) {
    MentionOptionRecipe recipe;
    recipe.background = highlighted ? token(u"accent-subtle", theme) : transparent();
    recipe.title = token(highlighted ? u"accent-text" : u"text-main", theme);
    recipe.number = token(u"text-muted", theme);
    recipe.place = token(u"text-muted", theme);
    recipe.height = profile().mentionOptionHeight;
    recipe.paddingInline = profile().mentionOptionPadding;
    recipe.radius = profile().mentionOptionRadius;
    recipe.spacing = profile().mentionOptionGap;
    recipe.fontPixels = profile().mentionOptionPixels;
    recipe.numberPixels = profile().conversationNumberPixels;
    recipe.placePixels = profile().mentionPlacePixels;
    return recipe;
}

UsageRingRecipe usageRingRecipe(qreal used, bool hovered, Theme theme) {
    UsageRingRecipe recipe;
    recipe.track = token(u"border-default", theme);
    if (used >= profile().usageDangerAt)
        recipe.arc = token(u"danger", theme);
    else if (used >= profile().usageWarningAt)
        recipe.arc = token(u"warning", theme);
    else
        recipe.arc = token(hovered ? u"accent" : u"text-muted", theme); // `.usage-ring:hover`
    recipe.size = profile().usageRingSize;
    recipe.stroke = profile().usageRingStroke;
    return recipe;
}

QVariantMap toVariantMap(const MarkdownRecipe& r) {
    return mapOf({{"text", r.text},
                  {"heading", r.heading},
                  {"link", r.link},
                  {"quoteBackground", r.quoteBackground},
                  {"quoteText", r.quoteText},
                  {"rule", r.rule},
                  {"bodyPixels", r.bodyPixels},
                  {"lineHeight", r.lineHeight},
                  {"heading1Pixels", r.heading1Pixels},
                  {"heading2Pixels", r.heading2Pixels},
                  {"heading3Pixels", r.heading3Pixels},
                  {"headingLineHeight", r.headingLineHeight},
                  {"headingWeight", r.headingWeight},
                  {"headingSpacingAfter", r.headingSpacingAfter},
                  {"paragraphSpacing", r.paragraphSpacing},
                  {"listIndent", r.listIndent},
                  {"listItemGap", r.listItemGap},
                  {"strongWeight", r.strongWeight},
                  {"quotePadding", r.quotePadding},
                  {"quoteBorder", r.quoteBorder},
                  {"quoteBorderWidth", r.quoteBorderWidth},
                  {"codePixels", r.codePixels}});
}

QVariantMap toVariantMap(const CodeBlockRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"margin", r.margin},
                  {"fontPixels", r.fontPixels},
                  {"lineHeight", r.lineHeight}});
}

QVariantMap toVariantMap(const MentionPickerRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"title", r.title},
                  {"empty", r.empty},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"maxWidth", r.maxWidth},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"overlap", r.overlap},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"titlePaddingInline", r.titlePaddingInline},
                  {"titlePaddingTop", r.titlePaddingTop},
                  {"titlePaddingBottom", r.titlePaddingBottom},
                  {"emptyPixels", r.emptyPixels},
                  {"maxItems", r.maxItems}});
}

QVariantMap toVariantMap(const MentionOptionRecipe& r) {
    return mapOf({{"background", r.background},
                  {"title", r.title},
                  {"number", r.number},
                  {"place", r.place},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"fontPixels", r.fontPixels},
                  {"numberPixels", r.numberPixels},
                  {"placePixels", r.placePixels}});
}

QVariantMap toVariantMap(const UsageRingRecipe& r) {
    return mapOf({{"track", r.track}, {"arc", r.arc}, {"size", r.size}, {"stroke", r.stroke}});
}

} // namespace ket
