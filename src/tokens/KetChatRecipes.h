// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

namespace ket {

// An anchored panel (`.usage-popover`): a titled surface with a close button, a body and an
// optional footer. It opens above its anchor, right-aligned.
struct PopoverRecipe {
    QColor background;
    QColor border;
    QColor title;
    QColor text;
    QColor muted;
    QColor divider;
    Shadow shadow;
    qreal width = 0;
    qreal padding = 0;
    qreal radius = 0;
    qreal offset = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal headerGap = 0;
    qreal bodyPixels = 0;
    qreal notePixels = 0;
    qreal noteLineHeight = 0;
    qreal dividerSpacing = 0;
    qreal footerGap = 0;
};
[[nodiscard]] PopoverRecipe popoverRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const PopoverRecipe& recipe);

// A labelled usage meter: `.usage-stat` with its progress track in the usage popover, or the
// compact `.quota` in the sidebar. The fill turns warning and danger at the usage thresholds.
struct MeterRecipe {
    QColor label;
    QColor value;
    QColor caption;
    QColor track;
    QColor fill;
    qreal labelPixels = 0;
    int valueWeight = 0;
    qreal spacing = 0;
    qreal trackHeight = 0;
    qreal trackSpacingTop = 0;
    qreal trackSpacingBottom = 0;
    qreal captionPixels = 0;
    qreal captionLineHeight = 0;
};
[[nodiscard]] MeterRecipe meterRecipe(bool compact, qreal fraction, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const MeterRecipe& recipe);

// A file attached to the composer (`.attachment-chip`): icon, name and a remove button.
struct AttachmentChipRecipe {
    QColor background;
    QColor text;
    QColor remove;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal fontPixels = 0;
    qreal maxWidth = 0;
    qreal rowGap = 0;
    qreal rowPaddingInline = 0;
    qreal rowPaddingTop = 0;
};
[[nodiscard]] AttachmentChipRecipe attachmentChipRecipe(bool removeHovered, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const AttachmentChipRecipe& recipe);

// A starter prompt on the welcome screen (`.suggestion`): icon, title, caption and a trailing
// arrow.
struct SuggestionRecipe {
    QColor background;
    QColor border;
    QColor icon;
    QColor title;
    QColor caption;
    QColor arrow;
    QColor focusRing;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal arrowSize = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal captionPixels = 0;
    qreal captionGap = 0;
    qreal opacity = 0;
};
[[nodiscard]] SuggestionRecipe suggestionRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const SuggestionRecipe& recipe);

// The agent asking for input (`.question-card`): a label row with the step, the question and
// the app's answer controls (ket::Choice, a text field and buttons).
struct QuestionCardRecipe {
    QColor background;
    QColor border;
    QColor label;
    QColor step;
    QColor heading;
    qreal padding = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal labelPixels = 0;
    int labelWeight = 0;
    qreal headingPixels = 0;
    int headingWeight = 0;
    qreal headingSpacing = 0;
    qreal controlGap = 0;
};
[[nodiscard]] QuestionCardRecipe questionCardRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const QuestionCardRecipe& recipe);

// A collapsible run of activity rows (`.activity-group`): the summary with a chevron, then the
// children behind a hairline.
struct ActivityGroupRecipe {
    QColor background;
    QColor text;
    QColor rule;
    QColor focusRing;
    qreal height = 0;
    qreal radius = 0;
    qreal fontPixels = 0;
    qreal spacing = 0;
    qreal chevronSize = 0;
    qreal childrenIndent = 0;
    qreal childrenPadding = 0;
    qreal childrenGap = 0;
    qreal childrenSpacingTop = 0;
    qreal childrenSpacingBottom = 0;
    qreal opacity = 0;
};
[[nodiscard]] ActivityGroupRecipe activityGroupRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const ActivityGroupRecipe& recipe);

// Screenshot thumbnails in an activity (`.activity-gallery`) and the `+N` tile. `hovered`
// styles one thumbnail.
struct GalleryRecipe {
    QColor background;
    QColor border;
    QColor ring;
    QColor caption;
    QColor more;
    qreal tileSize = 0;
    qreal spacing = 0;
    qreal paddingInline = 0;
    qreal paddingTop = 0;
    qreal paddingBottom = 0;
    qreal radius = 0;
    qreal moreRadius = 0;
    qreal ringWidth = 0;
    qreal captionPixels = 0;
    qreal captionGap = 0;
};
[[nodiscard]] GalleryRecipe galleryRecipe(bool hovered, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const GalleryRecipe& recipe);

} // namespace ket
