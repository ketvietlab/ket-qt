// SPDX-License-Identifier: MPL-2.0
#include "KetRecipes.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

// Mock literals without a design-system token.
constexpr qreal kDisabledOpacity = 0.48; // `button:disabled`
constexpr qreal kButtonGap = 7;          // `.btn` gap
// Icon boxes: the mock's `.icon` (17 px) and compact header icons (14 px). ket::Icons draws
// the 16 px Két Symbol master centred in the 17 px box and scales it down only for 14 px.
constexpr qreal kIconSize = 17;
constexpr qreal kSmallIconSize = 14;
constexpr qreal kIconButtonSize = 30; // `.icon-btn`
constexpr qreal kTagHeight = 18;      // `.conversation-tag`
constexpr qreal kTagRadius = 4;
constexpr qreal kTagMaxWidth = 104;
constexpr qreal kTagPadding = 6;
constexpr qreal kTagOverflowPadding = 5; // `.tag-more`
constexpr qreal kTagTint = 0.13;         // color-mix(tag 13%, transparent)
constexpr qreal kTagHoverTint = 0.22;

struct Palette {
    QStringView rest; // empty: transparent
    QStringView hover;
    QStringView pressed;
    QStringView border; // empty: no border
    QStringView hoverBorder;
    QStringView text;
};

Palette paletteFor(ButtonVariant variant) {
    switch (variant) {
    case ButtonVariant::Primary:
        return {u"action-primary-bg", u"action-primary-hover", u"action-primary-active", {}, {},
                u"action-primary-ink"};
    case ButtonVariant::Secondary:
        return {u"interactive-bg", u"interactive-hover", u"interactive-active",
                u"border-default", u"border-strong",     u"text-main"};
    case ButtonVariant::Ghost:
        return {{}, u"surface-hover", u"interactive-active", {}, {}, u"text-main"};
    case ButtonVariant::Danger:
        return {u"interactive-bg", u"interactive-hover", u"interactive-active",
                u"danger-border",  u"danger-border",     u"danger"};
    case ButtonVariant::GhostDanger:
        return {
            {},       u"surface-hover", u"interactive-active", u"danger-border", u"danger-border",
            u"danger"};
    }
    return {};
}

QColor optionalToken(QStringView name, Theme theme) {
    return name.isEmpty() ? transparent() : token(name, theme);
}

ButtonRecipe paint(const Palette& palette, const InteractionState& state, Theme theme) {
    ButtonRecipe recipe;
    const bool live = state.enabled;
    QStringView fill = palette.rest;
    if (live && state.pressed)
        fill = palette.pressed;
    else if (live && state.hovered)
        fill = palette.hover;
    recipe.background = optionalToken(fill, theme);
    recipe.border = optionalToken(
        live && (state.hovered || state.pressed) ? palette.hoverBorder : palette.border, theme);
    recipe.text = token(palette.text, theme);
    if (!live)
        recipe.opacity = kDisabledOpacity;
    recipe.focusRing = state.focused && live ? token(u"focus-border", theme) : transparent();
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.fontWeight = qRound(Tokens::number(u"weight-normal"));
    return recipe;
}

} // namespace

ButtonRecipe buttonRecipe(ButtonVariant variant, ControlSize size, const InteractionState& state,
                          Theme theme) {
    ButtonRecipe recipe = paint(paletteFor(variant), state, theme);
    const bool small = size == ControlSize::Small;
    recipe.height = Tokens::px(small ? u"control-height-sm" : u"control-height-md");
    // The mock's 11 px padding plus its 1 px border.
    recipe.paddingInline = Tokens::px(small ? u"space-2-5" : u"control-padding-inline");
    recipe.iconSize = small ? kSmallIconSize : kIconSize;
    recipe.spacing = kButtonGap;
    // `--kp-text-base` at the default 15 px interface size.
    recipe.fontPixels = Tokens::px(small ? u"text-sm" : u"text-md");
    return recipe;
}

ButtonRecipe iconButtonRecipe(ControlSize size, const InteractionState& state, Theme theme) {
    const Palette palette{{}, u"surface-hover", u"interactive-active", {}, {}, u"text-secondary"};
    ButtonRecipe recipe = paint(palette, state, theme);
    const bool small = size == ControlSize::Small;
    recipe.height = small ? Tokens::px(u"control-height-xs") : kIconButtonSize;
    recipe.iconSize = small ? kSmallIconSize : kIconSize;
    recipe.fontPixels = Tokens::px(u"text-md");
    return recipe;
}

BadgeRecipe badgeRecipe(BadgeTone tone, Theme theme) {
    BadgeRecipe recipe;
    switch (tone) {
    case BadgeTone::Neutral:
        recipe.background = token(u"panel-bg", theme);
        recipe.border = token(u"panel-border", theme);
        recipe.text = token(u"text-secondary", theme);
        break;
    case BadgeTone::Accent:
        recipe.background = token(u"accent-subtle", theme);
        recipe.border = token(u"accent-border", theme);
        recipe.text = token(u"accent-text", theme);
        break;
    case BadgeTone::Info:
        recipe.background = token(u"info-bg", theme);
        recipe.border = token(u"info-border", theme);
        recipe.text = token(u"info", theme);
        break;
    case BadgeTone::Success:
        recipe.background = token(u"success-bg", theme);
        recipe.border = token(u"success-border", theme);
        recipe.text = token(u"success", theme);
        break;
    case BadgeTone::Warning:
        recipe.background = token(u"warning-bg", theme);
        recipe.border = token(u"warning-border", theme);
        recipe.text = token(u"warning", theme);
        break;
    case BadgeTone::Danger:
        recipe.background = token(u"danger-bg", theme);
        recipe.border = token(u"danger-border", theme);
        recipe.text = token(u"badge-danger-text", theme);
        break;
    }
    // `.badge`: a 20 px line inside a 1 px border, 7 px padding plus the border.
    recipe.height = Tokens::px(u"line-sm") + 2;
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.paddingInline = Tokens::px(u"space-2");
    recipe.fontPixels = Tokens::px(u"text-xs");
    recipe.fontWeight = qRound(Tokens::number(u"weight-normal"));
    return recipe;
}

TagRecipe tagRecipe(const QColor& color, bool overflow, bool hovered, Theme theme) {
    TagRecipe recipe;
    if (overflow) {
        recipe.background = token(u"surface-hover", theme);
        recipe.text = token(u"text-muted", theme);
    } else {
        recipe.background = color;
        recipe.background.setAlphaF(float(color.alphaF() * (hovered ? kTagHoverTint : kTagTint)));
        recipe.text = token(u"text-secondary", theme);
    }
    recipe.height = kTagHeight;
    recipe.radius = kTagRadius;
    recipe.paddingInline = overflow ? kTagOverflowPadding : kTagPadding;
    recipe.maxWidth = kTagMaxWidth;
    recipe.fontPixels = Tokens::px(u"text-2xs");
    recipe.fontWeight = qRound(Tokens::number(u"weight-medium"));
    return recipe;
}

QVariantMap toVariantMap(const ButtonRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("border"), r.border},
            {QStringLiteral("text"), r.text},
            {QStringLiteral("focusRing"), r.focusRing},
            {QStringLiteral("height"), r.height},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("paddingInline"), r.paddingInline},
            {QStringLiteral("iconSize"), r.iconSize},
            {QStringLiteral("spacing"), r.spacing},
            {QStringLiteral("fontPixels"), r.fontPixels},
            {QStringLiteral("fontWeight"), r.fontWeight},
            {QStringLiteral("opacity"), r.opacity}};
}

QVariantMap toVariantMap(const BadgeRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("border"), r.border},
            {QStringLiteral("text"), r.text},
            {QStringLiteral("height"), r.height},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("paddingInline"), r.paddingInline},
            {QStringLiteral("fontPixels"), r.fontPixels},
            {QStringLiteral("fontWeight"), r.fontWeight}};
}

QVariantMap toVariantMap(const TagRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("text"), r.text},
            {QStringLiteral("height"), r.height},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("paddingInline"), r.paddingInline},
            {QStringLiteral("maxWidth"), r.maxWidth},
            {QStringLiteral("fontPixels"), r.fontPixels},
            {QStringLiteral("fontWeight"), r.fontWeight}};
}

} // namespace ket
