// SPDX-License-Identifier: MPL-2.0
// Conversation recipes: sidebar conversation rows, the composer and transcript blocks.
#include "KetConversationRecipes.h"

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

QColor focusRing(const InteractionState& state, Theme theme) {
    return state.enabled && state.focused ? token(u"focus-border", theme) : transparent();
}

} // namespace

ConversationRowRecipe conversationRowRecipe(bool selected, const InteractionState& state,
                                            Theme theme) {
    ConversationRowRecipe recipe;
    const bool live = state.enabled;
    recipe.background =
        selected || (live && state.hovered) ? token(u"surface-hover", theme) : transparent();
    recipe.title = token(u"text-main", theme);
    recipe.number = token(u"text-muted", theme);
    recipe.meta = token(u"text-muted", theme);
    recipe.focusRing = focusRing(state, theme);
    recipe.height = profile().conversationRowHeight;
    recipe.paddingInline = profile().conversationRowPaddingInline;
    recipe.paddingBlock = profile().conversationRowPaddingBlock;
    recipe.radius = profile().conversationRowRadius;
    recipe.spacing = profile().conversationRowGap;
    recipe.lineGap = profile().conversationLineGap;
    recipe.titlePixels = profile().conversationTitlePixels;
    recipe.titleLineHeight = profile().conversationTitleLineHeight;
    recipe.titleWeight =
        selected ? profile().conversationSelectedTitleWeight : profile().conversationTitleWeight;
    recipe.numberPixels = profile().conversationNumberPixels;
    recipe.metaPixels = profile().conversationMetaPixels;
    recipe.metaHeight = profile().conversationMetaHeight;
    recipe.metaGap = profile().conversationMetaGap;
    recipe.tagGap = profile().conversationTagGap;
    recipe.tagMaxWidth = profile().conversationTagMaxWidth;
    recipe.indicatorSize = profile().conversationIndicatorSize;
    recipe.indicatorTop = profile().conversationIndicatorTop;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

ConversationStatusRecipe conversationStatusRecipe(ConversationStatus status, Theme theme) {
    ConversationStatusRecipe recipe;
    recipe.labelColor = token(u"text-muted", theme);
    recipe.spinMs = profile().conversationSpinMs;
    switch (status) {
    case ConversationStatus::NotStarted:
        recipe.icon = QStringLiteral("chat");
        recipe.iconColor = token(u"text-muted", theme);
        break;
    case ConversationStatus::Working:
        // The row indicator is text-main (desktop-alignment), not the accent.
        recipe.icon = QStringLiteral("loading");
        recipe.iconColor = token(u"text-main", theme);
        recipe.spinning = true;
        break;
    case ConversationStatus::NeedsInput:
        recipe.icon = QStringLiteral("pause-circle");
        recipe.iconColor = token(u"warning", theme);
        recipe.labelColor = recipe.iconColor;
        break;
    case ConversationStatus::Error:
        recipe.icon = QStringLiteral("warning");
        recipe.iconColor = token(u"danger", theme);
        recipe.labelColor = recipe.iconColor;
        break;
    case ConversationStatus::Completed:
        recipe.icon = QStringLiteral("success");
        recipe.iconColor = token(u"success", theme);
        break;
    case ConversationStatus::Stopped:
        recipe.icon = QStringLiteral("stop");
        recipe.iconColor = token(u"text-muted", theme);
        break;
    case ConversationStatus::AwaitingReply:
        // A finished turn that asks for the user: not the success tick, and not the warning
        // pause of a blocked turn.
        recipe.icon = QStringLiteral("help");
        recipe.iconColor = token(u"accent-text", theme);
        recipe.labelColor = recipe.iconColor;
        break;
    }
    return recipe;
}

QString conversationStatusLabel(ConversationStatus status) {
    switch (status) {
    case ConversationStatus::NotStarted:
        return QStringLiteral("Not started");
    case ConversationStatus::Working:
        return QStringLiteral("Working");
    case ConversationStatus::NeedsInput:
        return QStringLiteral("Needs input");
    case ConversationStatus::Error:
        return QStringLiteral("Reply failed");
    case ConversationStatus::Completed:
        return QStringLiteral("Completed");
    case ConversationStatus::Stopped:
        return QStringLiteral("Stopped");
    case ConversationStatus::AwaitingReply:
        return QStringLiteral("Waiting for you");
    }
    return {};
}

ComposerRecipe composerRecipe(bool focused, Theme theme) {
    ComposerRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(focused ? u"accent-border" : u"border-default", theme);
    recipe.ring = focused ? token(u"accent-subtle", theme) : transparent();
    recipe.text = token(u"text-main", theme);
    recipe.placeholder = token(u"input-placeholder", theme);
    recipe.shadow = shadowToken(u"shadow-xs", theme);
    recipe.radius = profile().composerRadius;
    recipe.padding = profile().composerPadding;
    recipe.ringWidth = profile().composerRingWidth;
    recipe.textPixels = profile().composerTextPixels;
    recipe.lineHeight = profile().composerLineHeight;
    recipe.textPaddingInline = profile().composerTextPaddingInline;
    recipe.textPaddingBlock = profile().composerTextPaddingBlock;
    recipe.minInputHeight = profile().composerMinInputHeight;
    recipe.maxLines = profile().composerMaxLines;
    recipe.sendSize = profile().composerSendSize;
    recipe.attachIconSize = profile().composerAttachIconSize;
    recipe.toolbarHeight = profile().composerToolbarHeight;
    recipe.toolbarGap = profile().composerToolbarGap;
    return recipe;
}

ComposerAction composerAction(bool busy, bool hasText) {
    return busy && !hasText ? ComposerAction::Stop : ComposerAction::Send;
}

ComposerActionRecipe composerActionRecipe(ComposerAction action, bool enabled,
                                          const InteractionState& state, Theme theme) {
    ComposerActionRecipe recipe;
    const bool hot = enabled && (state.hovered || state.pressed);
    if (action == ComposerAction::Stop) {
        recipe.background = token(hot ? u"text-secondary" : u"text-main", theme);
        recipe.icon = token(u"page-bg", theme);
    } else if (enabled) {
        recipe.background = token(state.pressed   ? u"accent-active"
                                  : state.hovered ? u"accent-hover"
                                                  : u"accent",
                                  theme);
        recipe.icon = token(u"on-accent", theme);
    } else {
        recipe.background = token(u"surface-hover", theme);
        recipe.icon = token(u"text-disabled", theme);
    }
    recipe.focusRing = enabled && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.size = profile().composerSendSize;
    recipe.iconSize = profile().composerSendIconSize;
    recipe.stopGlyph = profile().composerStopGlyph;
    recipe.stopRadius = profile().composerStopGlyphRadius;
    return recipe;
}

QVariantMap toVariantMap(const ComposerActionRecipe& r) {
    return mapOf({{"background", r.background},
                  {"icon", r.icon},
                  {"focusRing", r.focusRing},
                  {"size", r.size},
                  {"iconSize", r.iconSize},
                  {"stopGlyph", r.stopGlyph},
                  {"stopRadius", r.stopRadius},
                  {"opacity", r.opacity}});
}

UserMessageRecipe userMessageRecipe(Theme theme) {
    UserMessageRecipe recipe;
    // A tinted bubble: the accent at full strength is too loud for a whole message.
    recipe.background = token(u"accent-subtle", theme);
    recipe.text = token(u"text-main", theme);
    recipe.radius = profile().userBubbleRadius;
    recipe.paddingInline = profile().userBubblePaddingInline;
    recipe.paddingBlock = profile().userBubblePaddingBlock;
    recipe.fontPixels = profile().readingPixels;
    recipe.lineHeight = profile().readingLineHeight;
    recipe.maxWidth = profile().userBubbleMaxWidth;
    recipe.spacingAfter = profile().messageSpacing;
    return recipe;
}

AssistantMessageRecipe assistantMessageRecipe(Theme theme) {
    AssistantMessageRecipe recipe;
    recipe.avatarBackground = token(u"accent-subtle", theme);
    recipe.avatarIcon = token(u"accent-text", theme);
    recipe.name = token(u"text-main", theme);
    recipe.time = token(u"text-muted", theme);
    recipe.body = token(u"text-main", theme);
    recipe.avatarSize = profile().avatarSize;
    recipe.avatarRadius = profile().avatarRadius;
    recipe.avatarIconSize = profile().smallIconSize;
    recipe.spacing = profile().assistantGap;
    recipe.namePixels = profile().messageNamePixels;
    recipe.nameWeight = qRound(Tokens::number(u"weight-semibold"));
    recipe.timePixels = profile().messageTimePixels;
    recipe.metaGap = profile().messageMetaGap;
    recipe.metaBottom = profile().messageMetaBottom;
    recipe.bodyPixels = profile().readingPixels;
    recipe.lineHeight = profile().readingLineHeight;
    recipe.spacingAfter = profile().messageSpacing;
    return recipe;
}

DayDividerRecipe dayDividerRecipe(Theme theme) {
    DayDividerRecipe recipe;
    recipe.line = token(u"panel-border", theme);
    recipe.text = token(u"text-muted", theme);
    recipe.fontPixels = profile().dayDividerPixels;
    recipe.spacing = profile().dayDividerGap;
    recipe.marginTop = profile().dayDividerMarginTop;
    recipe.marginBottom = profile().dayDividerMarginBottom;
    return recipe;
}

ActivityRowRecipe activityRowRecipe(const InteractionState& state, Theme theme) {
    ActivityRowRecipe recipe;
    const bool live = state.enabled;
    recipe.background =
        live && (state.hovered || state.pressed) ? token(u"surface-hover", theme) : transparent();
    recipe.text = token(u"text-muted", theme);
    recipe.detail = token(u"text-muted", theme);
    recipe.focusRing = focusRing(state, theme);
    recipe.height = profile().activityRowHeight;
    recipe.paddingInline = profile().activityRowPaddingInline;
    recipe.spacing = profile().activityRowGap;
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.fontPixels = profile().activityRowPixels;
    recipe.detailPixels = profile().menuItemFontPixels;
    recipe.iconSize = profile().activityRowIconSize;
    recipe.chevronSize = profile().activityRowChevronSize;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const ConversationRowRecipe& r) {
    return mapOf({{"background", r.background},
                  {"title", r.title},
                  {"number", r.number},
                  {"meta", r.meta},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"lineGap", r.lineGap},
                  {"titlePixels", r.titlePixels},
                  {"titleLineHeight", r.titleLineHeight},
                  {"titleWeight", r.titleWeight},
                  {"numberPixels", r.numberPixels},
                  {"metaPixels", r.metaPixels},
                  {"metaHeight", r.metaHeight},
                  {"metaGap", r.metaGap},
                  {"tagGap", r.tagGap},
                  {"tagMaxWidth", r.tagMaxWidth},
                  {"indicatorSize", r.indicatorSize},
                  {"indicatorTop", r.indicatorTop},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const ConversationStatusRecipe& r) {
    return mapOf({{"icon", r.icon},
                  {"iconColor", r.iconColor},
                  {"labelColor", r.labelColor},
                  {"spinning", r.spinning},
                  {"spinMs", r.spinMs}});
}

QVariantMap toVariantMap(const ComposerRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"ring", r.ring},
                  {"text", r.text},
                  {"placeholder", r.placeholder},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"radius", r.radius},
                  {"padding", r.padding},
                  {"ringWidth", r.ringWidth},
                  {"textPixels", r.textPixels},
                  {"lineHeight", r.lineHeight},
                  {"textPaddingInline", r.textPaddingInline},
                  {"textPaddingBlock", r.textPaddingBlock},
                  {"minInputHeight", r.minInputHeight},
                  {"maxLines", r.maxLines},
                  {"sendSize", r.sendSize},
                  {"attachIconSize", r.attachIconSize},
                  {"toolbarHeight", r.toolbarHeight},
                  {"toolbarGap", r.toolbarGap}});
}

QVariantMap toVariantMap(const UserMessageRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"fontPixels", r.fontPixels},
                  {"lineHeight", r.lineHeight},
                  {"maxWidth", r.maxWidth},
                  {"spacingAfter", r.spacingAfter}});
}

QVariantMap toVariantMap(const AssistantMessageRecipe& r) {
    return mapOf({{"avatarBackground", r.avatarBackground},
                  {"avatarIcon", r.avatarIcon},
                  {"name", r.name},
                  {"time", r.time},
                  {"body", r.body},
                  {"avatarSize", r.avatarSize},
                  {"avatarRadius", r.avatarRadius},
                  {"avatarIconSize", r.avatarIconSize},
                  {"spacing", r.spacing},
                  {"namePixels", r.namePixels},
                  {"nameWeight", r.nameWeight},
                  {"timePixels", r.timePixels},
                  {"metaGap", r.metaGap},
                  {"metaBottom", r.metaBottom},
                  {"bodyPixels", r.bodyPixels},
                  {"lineHeight", r.lineHeight},
                  {"spacingAfter", r.spacingAfter}});
}

QVariantMap toVariantMap(const DayDividerRecipe& r) {
    return mapOf({{"line", r.line},
                  {"text", r.text},
                  {"fontPixels", r.fontPixels},
                  {"spacing", r.spacing},
                  {"marginTop", r.marginTop},
                  {"marginBottom", r.marginBottom}});
}

QVariantMap toVariantMap(const ActivityRowRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"detail", r.detail},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"radius", r.radius},
                  {"fontPixels", r.fontPixels},
                  {"detailPixels", r.detailPixels},
                  {"iconSize", r.iconSize},
                  {"chevronSize", r.chevronSize},
                  {"opacity", r.opacity}});
}

} // namespace ket
