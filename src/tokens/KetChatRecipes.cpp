// SPDX-License-Identifier: MPL-2.0
// Chat extras: popover, meters, attachments, suggestions, questions, activity groups and galleries.
#include "KetChatRecipes.h"

#include "KetProfile.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

[[maybe_unused]] QColor transparent() { return QColor(Qt::transparent); }

const Profile& profile() { return Profile::current(); }

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

QColor meterFill(qreal fraction, Theme theme) {
    if (fraction >= profile().usageDangerAt)
        return token(u"danger", theme);
    if (fraction >= profile().usageWarningAt)
        return token(u"warning", theme);
    return token(u"accent", theme);
}

} // namespace

PopoverRecipe popoverRecipe(Theme theme) {
    PopoverRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"border-default", theme);
    recipe.title = token(u"text-main", theme);
    recipe.text = token(u"text-main", theme);
    recipe.muted = token(u"text-muted", theme);
    recipe.divider = token(u"panel-divider", theme);
    recipe.shadow = shadowToken(u"shadow-md", theme);
    recipe.width = profile().popoverWidth;
    recipe.padding = profile().popoverPadding;
    recipe.radius = profile().popoverRadius;
    recipe.offset = profile().popoverOffset;
    recipe.titlePixels = profile().popoverTitlePixels;
    recipe.titleWeight = profile().popoverTitleWeight;
    recipe.headerGap = profile().popoverHeaderGap;
    recipe.bodyPixels = profile().popoverBodyPixels;
    recipe.notePixels = profile().popoverNotePixels;
    recipe.noteLineHeight = profile().popoverNoteLineHeight;
    recipe.dividerSpacing = profile().popoverDividerSpacing;
    recipe.footerGap = profile().popoverFooterGap;
    return recipe;
}

QVariantMap toVariantMap(const PopoverRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"title", r.title},
                  {"text", r.text},
                  {"muted", r.muted},
                  {"divider", r.divider},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"width", r.width},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"offset", r.offset},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"headerGap", r.headerGap},
                  {"bodyPixels", r.bodyPixels},
                  {"notePixels", r.notePixels},
                  {"noteLineHeight", r.noteLineHeight},
                  {"dividerSpacing", r.dividerSpacing},
                  {"footerGap", r.footerGap}});
}

MeterRecipe meterRecipe(bool compact, qreal fraction, Theme theme) {
    MeterRecipe recipe;
    recipe.label = token(compact ? u"text-secondary" : u"text-main", theme);
    recipe.value = token(u"text-main", theme);
    recipe.caption = token(u"text-muted", theme);
    recipe.track = token(u"border-default", theme);
    recipe.fill = meterFill(fraction, theme);
    recipe.labelPixels = compact ? profile().meterCompactLabelPixels : profile().meterLabelPixels;
    recipe.valueWeight = compact ? profile().meterCompactValueWeight : profile().meterValueWeight;
    recipe.spacing = profile().meterGap;
    recipe.trackHeight = compact ? profile().meterCompactTrackHeight : profile().meterTrackHeight;
    recipe.trackSpacingTop =
        compact ? profile().meterCompactTrackSpacingTop : profile().meterTrackSpacingTop;
    recipe.trackSpacingBottom =
        compact ? profile().meterCompactTrackSpacingBottom : profile().meterTrackSpacingBottom;
    recipe.captionPixels =
        compact ? profile().meterCompactCaptionPixels : profile().meterCaptionPixels;
    recipe.captionLineHeight = profile().meterCaptionLineHeight;
    return recipe;
}

QVariantMap toVariantMap(const MeterRecipe& r) {
    return mapOf({{"label", r.label},
                  {"value", r.value},
                  {"caption", r.caption},
                  {"track", r.track},
                  {"fill", r.fill},
                  {"labelPixels", r.labelPixels},
                  {"valueWeight", r.valueWeight},
                  {"spacing", r.spacing},
                  {"trackHeight", r.trackHeight},
                  {"trackSpacingTop", r.trackSpacingTop},
                  {"trackSpacingBottom", r.trackSpacingBottom},
                  {"captionPixels", r.captionPixels},
                  {"captionLineHeight", r.captionLineHeight}});
}

AttachmentChipRecipe attachmentChipRecipe(bool removeHovered, Theme theme) {
    AttachmentChipRecipe recipe;
    recipe.background = token(u"accent-subtle", theme);
    recipe.text = token(u"text-main", theme);
    recipe.remove = token(removeHovered ? u"accent-text" : u"text-muted", theme);
    recipe.height = profile().chipHeight;
    recipe.paddingInline = profile().chipPaddingInline;
    recipe.radius = profile().chipRadius;
    recipe.spacing = profile().chipGap;
    recipe.iconSize = profile().chipIconSize;
    recipe.fontPixels = profile().chipPixels;
    recipe.maxWidth = profile().chipMaxWidth;
    recipe.rowGap = profile().chipRowGap;
    recipe.rowPaddingInline = profile().chipRowPaddingInline;
    recipe.rowPaddingTop = profile().chipRowPaddingTop;
    return recipe;
}

QVariantMap toVariantMap(const AttachmentChipRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"remove", r.remove},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"fontPixels", r.fontPixels},
                  {"maxWidth", r.maxWidth},
                  {"rowGap", r.rowGap},
                  {"rowPaddingInline", r.rowPaddingInline},
                  {"rowPaddingTop", r.rowPaddingTop}});
}

SuggestionRecipe suggestionRecipe(const InteractionState& state, Theme theme) {
    SuggestionRecipe recipe;
    const bool live = state.enabled;
    recipe.background = live && (state.hovered || state.pressed) ? token(u"accent-subtle", theme)
                                                                 : token(u"panel-bg", theme);
    recipe.border = live && (state.hovered || state.pressed) ? token(u"accent-border", theme)
                                                             : token(u"panel-border", theme);
    recipe.icon = token(u"accent-text", theme);
    recipe.title = token(u"text-main", theme);
    recipe.caption = token(u"text-muted", theme);
    recipe.arrow = token(u"text-muted", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.paddingBlock = profile().suggestionPaddingBlock;
    recipe.paddingInline = profile().suggestionPaddingInline;
    recipe.radius = profile().suggestionRadius;
    recipe.spacing = profile().suggestionGap;
    recipe.iconSize = profile().iconSize;
    recipe.arrowSize = profile().suggestionArrowSize;
    recipe.titlePixels = profile().suggestionTitlePixels;
    recipe.titleWeight = profile().suggestionTitleWeight;
    recipe.captionPixels = profile().suggestionCaptionPixels;
    recipe.captionGap = profile().suggestionCaptionGap;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const SuggestionRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"icon", r.icon},
                  {"title", r.title},
                  {"caption", r.caption},
                  {"arrow", r.arrow},
                  {"focusRing", r.focusRing},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"arrowSize", r.arrowSize},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"captionPixels", r.captionPixels},
                  {"captionGap", r.captionGap},
                  {"opacity", r.opacity}});
}

QuestionCardRecipe questionCardRecipe(Theme theme) {
    QuestionCardRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"accent-border", theme);
    recipe.label = token(u"accent-text", theme);
    recipe.step = token(u"text-muted", theme);
    recipe.heading = token(u"text-main", theme);
    recipe.padding = profile().questionPadding;
    recipe.radius = profile().questionRadius;
    recipe.spacing = profile().questionGap;
    recipe.iconSize = profile().smallIconSize;
    recipe.labelPixels = profile().questionLabelPixels;
    recipe.labelWeight = profile().questionLabelWeight;
    recipe.headingPixels = profile().questionHeadingPixels;
    recipe.headingWeight = profile().questionHeadingWeight;
    recipe.headingSpacing = profile().questionHeadingSpacing;
    recipe.controlGap = profile().questionControlGap;
    return recipe;
}

QVariantMap toVariantMap(const QuestionCardRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"label", r.label},
                  {"step", r.step},
                  {"heading", r.heading},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"labelPixels", r.labelPixels},
                  {"labelWeight", r.labelWeight},
                  {"headingPixels", r.headingPixels},
                  {"headingWeight", r.headingWeight},
                  {"headingSpacing", r.headingSpacing},
                  {"controlGap", r.controlGap}});
}

ActivityGroupRecipe activityGroupRecipe(const InteractionState& state, Theme theme) {
    ActivityGroupRecipe recipe;
    const bool live = state.enabled;
    recipe.background =
        live && (state.hovered || state.pressed) ? token(u"surface-hover", theme) : transparent();
    recipe.text = token(u"text-muted", theme);
    recipe.rule = token(u"border-default", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.height = profile().activityGroupHeight;
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.fontPixels = profile().activityRowPixels;
    recipe.spacing = profile().activityGroupGap;
    recipe.chevronSize = profile().activityGroupChevronSize;
    recipe.childrenIndent = profile().activityChildrenIndent;
    recipe.childrenPadding = profile().activityChildrenPadding;
    recipe.childrenGap = profile().activityChildrenGap;
    recipe.childrenSpacingTop = profile().activityChildrenSpacingTop;
    recipe.childrenSpacingBottom = profile().activityChildrenSpacingBottom;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const ActivityGroupRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"rule", r.rule},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"fontPixels", r.fontPixels},
                  {"spacing", r.spacing},
                  {"chevronSize", r.chevronSize},
                  {"childrenIndent", r.childrenIndent},
                  {"childrenPadding", r.childrenPadding},
                  {"childrenGap", r.childrenGap},
                  {"childrenSpacingTop", r.childrenSpacingTop},
                  {"childrenSpacingBottom", r.childrenSpacingBottom},
                  {"opacity", r.opacity}});
}

GalleryRecipe galleryRecipe(bool hovered, Theme theme) {
    GalleryRecipe recipe;
    recipe.background = token(u"surface-raised", theme);
    recipe.border = token(hovered ? u"accent-border" : u"border-default", theme);
    recipe.ring = hovered ? token(u"accent-subtle", theme) : transparent();
    recipe.caption = token(u"text-muted", theme);
    recipe.more = token(u"text-muted", theme);
    recipe.tileSize = profile().galleryTileSize;
    recipe.spacing = profile().galleryGap;
    recipe.paddingInline = profile().galleryPaddingInline;
    recipe.paddingTop = profile().galleryPaddingTop;
    recipe.paddingBottom = profile().galleryPaddingBottom;
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.moreRadius = profile().galleryMoreRadius;
    recipe.ringWidth = profile().galleryRingWidth;
    recipe.captionPixels = profile().galleryCaptionPixels;
    recipe.captionGap = profile().galleryCaptionGap;
    return recipe;
}

QVariantMap toVariantMap(const GalleryRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"ring", r.ring},
                  {"caption", r.caption},
                  {"more", r.more},
                  {"tileSize", r.tileSize},
                  {"spacing", r.spacing},
                  {"paddingInline", r.paddingInline},
                  {"paddingTop", r.paddingTop},
                  {"paddingBottom", r.paddingBottom},
                  {"radius", r.radius},
                  {"moreRadius", r.moreRadius},
                  {"ringWidth", r.ringWidth},
                  {"captionPixels", r.captionPixels},
                  {"captionGap", r.captionGap}});
}

} // namespace ket
