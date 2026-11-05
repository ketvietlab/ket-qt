// SPDX-License-Identifier: MPL-2.0
#include "KetRecipes.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

struct Palette {
    QStringView rest;
    QStringView hover;
    QStringView pressed;
    QStringView border; // empty: no border
    QStringView text;
};

Palette paletteFor(ButtonVariant variant) {
    switch (variant) {
    case ButtonVariant::Primary:
        return {u"action-primary-bg",
                u"action-primary-hover",
                u"action-primary-active",
                {},
                u"action-primary-ink"};
    case ButtonVariant::Secondary:
        return {u"interactive-bg", u"interactive-hover", u"interactive-active", u"border-default",
                u"text-main"};
    case ButtonVariant::Ghost:
        return {{}, u"interactive-hover", u"interactive-active", {}, u"text-main"};
    case ButtonVariant::Danger:
        return {u"danger-bg", u"danger-bg", u"danger-bg", u"danger-border", u"danger"};
    }
    return {};
}

} // namespace

ButtonRecipe buttonRecipe(ButtonVariant variant, ControlSize size, const InteractionState& state,
                          Theme theme) {
    const Palette palette = paletteFor(variant);
    ButtonRecipe recipe;
    QStringView fill = palette.rest;
    if (state.enabled && state.pressed)
        fill = palette.pressed;
    else if (state.enabled && state.hovered)
        fill = palette.hover;
    recipe.background = fill.isEmpty() ? transparent() : token(fill, theme);
    recipe.border = palette.border.isEmpty() ? transparent() : token(palette.border, theme);
    recipe.text = token(palette.text, theme);
    if (variant == ButtonVariant::Danger && state.enabled && (state.hovered || state.pressed))
        recipe.text = token(u"danger-hover", theme);
    if (!state.enabled) {
        // Disabled controls keep their shape and fade, as the design system's pending state does.
        recipe.opacity = Tokens::number(u"opacity-pending");
    }
    recipe.focusRing =
        state.focused && state.enabled ? token(u"focus-border", theme) : transparent();
    recipe.height =
        Tokens::px(size == ControlSize::Small ? u"control-height-sm" : u"control-height-md");
    recipe.radius = Tokens::px(u"control-radius");
    recipe.paddingInline =
        Tokens::px(size == ControlSize::Small ? u"space-2-5" : u"control-padding-inline");
    recipe.iconSize = 16;
    recipe.spacing = Tokens::px(u"space-1-5");
    recipe.fontPixels = Tokens::px(u"action-font-size");
    recipe.fontWeight = qRound(Tokens::number(u"weight-medium"));
    return recipe;
}

BadgeRecipe badgeRecipe(BadgeTone tone, Theme theme) {
    BadgeRecipe recipe;
    switch (tone) {
    case BadgeTone::Neutral:
        recipe.background = token(u"surface-hover", theme);
        recipe.border = token(u"border-default", theme);
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
    recipe.height = Tokens::px(u"line-sm");
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.paddingInline = Tokens::px(u"space-1-5");
    recipe.fontPixels = Tokens::px(u"text-xs");
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

} // namespace ket
