// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QFont>

namespace ket {

// Markdown in the transcript (`.answer`): headings, paragraphs, lists, quotes, inline code
// and links. Code fences are separate blocks (CodeBlockRecipe).
struct MarkdownRecipe {
    QColor text;
    QColor heading;
    QColor link;
    QColor quoteBackground;
    QColor quoteText;
    QColor quoteBorder;
    QColor rule;
    QColor tableBorder; // table cell edges: the quiet panel hairline, not Qt's default grey
    qreal bodyPixels = 0;
    qreal lineHeight = 1.0;
    qreal heading1Pixels = 0;
    qreal heading2Pixels = 0;
    qreal heading3Pixels = 0;
    qreal headingLineHeight = 1.0;
    // Letter spacing of h1 and h2, in em; negative tightens the larger headings.
    qreal headingTracking = 0;
    int headingWeight = 600;
    qreal headingSpacingAfter = 0;
    qreal paragraphSpacing = 0;
    qreal listIndent = 0;
    qreal listItemGap = 0;
    int strongWeight = 700;
    qreal quotePadding = 0;
    qreal quoteBorderWidth = 0;
    qreal codePixels = 0;
    qreal tableCellPadding = 0;
};

struct CodeBlockRecipe {
    QColor background;
    QColor border;
    QColor text;
    qreal padding = 0;
    qreal radius = 0;
    qreal margin = 0;
    qreal fontPixels = 0;
    qreal lineHeight = 1.0;
};

// The `@agent/` completion menu over the composer and one option in it.
struct MentionPickerRecipe {
    QColor background;
    QColor border;
    QColor title;
    QColor empty;
    Shadow shadow;
    qreal maxWidth = 0;
    qreal padding = 0;
    qreal radius = 0;
    qreal overlap = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal titlePaddingInline = 0;
    qreal titlePaddingTop = 0;
    qreal titlePaddingBottom = 0;
    qreal emptyPixels = 0;
    int maxItems = 0;
};

struct MentionOptionRecipe {
    QColor background;
    QColor title;
    QColor number;
    QColor place;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal fontPixels = 0;
    qreal numberPixels = 0;
    qreal placePixels = 0;
};

// Context usage as a ring (`.usage-ring`): the track and the used share.
struct UsageRingRecipe {
    QColor track;
    QColor arc;
    qreal size = 0;
    qreal stroke = 0;
};

[[nodiscard]] MarkdownRecipe markdownRecipe(Theme theme);
[[nodiscard]] CodeBlockRecipe codeBlockRecipe(Theme theme);
[[nodiscard]] MentionPickerRecipe mentionPickerRecipe(Theme theme);
[[nodiscard]] MentionOptionRecipe mentionOptionRecipe(bool highlighted, Theme theme);
// `used` is 0..1; the arc turns warning and danger near the limit.
[[nodiscard]] UsageRingRecipe usageRingRecipe(qreal used, bool hovered, Theme theme);

[[nodiscard]] QVariantMap toVariantMap(const MarkdownRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const CodeBlockRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const MentionPickerRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const MentionOptionRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const UsageRingRecipe& recipe);

} // namespace ket
