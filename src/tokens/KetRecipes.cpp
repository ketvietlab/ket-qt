// SPDX-License-Identifier: MPL-2.0
#include "KetRecipes.h"

#include "KetProfile.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

const Profile& profile() { return Profile::current(); }

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
        recipe.opacity = profile().disabledOpacity;
    recipe.focusRing = state.focused && live ? token(u"focus-border", theme) : transparent();
    recipe.radius = profile().controlRadius;
    recipe.fontWeight = profile().buttonFontWeight;
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
    recipe.iconSize = small ? profile().smallIconSize : profile().iconSize;
    recipe.spacing = profile().buttonGap;
    // `--kp-text-base` at the default 15 px interface size.
    recipe.fontPixels = small ? profile().smallButtonFontPixels : profile().buttonFontPixels;
    return recipe;
}

ButtonRecipe iconButtonRecipe(ControlSize size, const InteractionState& state, Theme theme) {
    const Palette palette{{}, u"surface-hover", u"interactive-active", {}, {}, u"text-secondary"};
    ButtonRecipe recipe = paint(palette, state, theme);
    const bool small = size == ControlSize::Small;
    recipe.height = small ? Tokens::px(u"control-height-xs") : profile().iconButtonSize;
    recipe.iconSize = small ? profile().smallIconSize : profile().iconSize;
    recipe.fontPixels = profile().buttonFontPixels;
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
        recipe.background.setAlphaF(
            float(color.alphaF() * (hovered ? profile().tagHoverTint : profile().tagTint)));
        recipe.text = token(u"text-secondary", theme);
    }
    recipe.height = profile().tagHeight;
    recipe.radius = profile().tagRadius;
    recipe.paddingInline = overflow ? profile().tagOverflowPadding : profile().tagPadding;
    recipe.maxWidth = profile().tagMaxWidth;
    recipe.fontPixels = Tokens::px(u"text-2xs");
    recipe.fontWeight = qRound(Tokens::number(u"weight-medium"));
    return recipe;
}

TagRecipe lockedTagRecipe(const QColor& color, Theme theme) {
    TagRecipe recipe = tagRecipe(color, false, true, theme);
    recipe.text = token(u"text-primary", theme);
    recipe.iconCode = QStringLiteral("lock");
    recipe.iconSize = recipe.fontPixels;
    recipe.iconGap = recipe.paddingInline / 2;
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
            {QStringLiteral("fontWeight"), r.fontWeight},
            {QStringLiteral("iconCode"), r.iconCode},
            {QStringLiteral("iconSize"), r.iconSize},
            {QStringLiteral("iconGap"), r.iconGap}};
}

FieldRecipe fieldRecipe(const InteractionState& state, bool invalid, Theme theme) {
    FieldRecipe recipe;
    const bool live = state.enabled;
    const bool focused = live && state.focused;
    const bool hovered = live && state.hovered && !focused;
    recipe.background = token(focused   ? u"input-bg-focus"
                              : hovered ? u"input-bg-hover"
                                        : u"input-bg",
                              theme);
    if (invalid)
        recipe.border = token(u"danger", theme);
    else
        recipe.border = token(focused   ? u"focus-border"
                              : hovered ? u"input-border-hover"
                                        : u"input-border",
                              theme);
    recipe.borderWidth = focused ? 2 : 1;
    recipe.text = token(u"input-text", theme);
    recipe.placeholder = token(u"input-placeholder", theme);
    recipe.icon = token(u"text-muted", theme);
    recipe.height = profile().fieldHeight;
    recipe.radius = profile().controlRadius;
    recipe.paddingInline = profile().fieldPadding;
    recipe.iconSize = profile().fieldIconSize;
    recipe.spacing = profile().fieldIconGap;
    recipe.fontPixels = profile().fieldFontPixels;
    if (!live)
        recipe.opacity = profile().disabledOpacity;
    return recipe;
}

ToggleRecipe toggleRecipe(ToggleKind kind, bool checked, const InteractionState& state,
                          Theme theme) {
    ToggleRecipe recipe;
    const bool live = state.enabled;
    if (checked) {
        recipe.background = token(live && state.hovered ? u"accent-hover" : u"accent", theme);
        recipe.border = recipe.background;
        recipe.mark = token(u"action-primary-ink", theme);
        if (kind == ToggleKind::Radio) {
            // A ring with a dot: the accent border around the field colour.
            recipe.background = token(u"input-bg", theme);
            recipe.mark = recipe.border;
        }
    } else {
        recipe.background = token(live && state.hovered ? u"input-bg-hover" : u"input-bg", theme);
        recipe.border =
            token(live && state.hovered ? u"input-border-hover" : u"border-strong", theme);
        recipe.mark = transparent();
    }
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.label = token(u"text-secondary", theme);
    recipe.size = profile().toggleSize;
    recipe.radius = kind == ToggleKind::Radio ? profile().toggleSize / 2 : Tokens::px(u"radius-xs");
    recipe.spacing = Tokens::px(u"space-2");
    recipe.fontPixels = Tokens::px(u"text-sm");
    if (!live)
        recipe.opacity = profile().disabledOpacity;
    return recipe;
}

ChoiceRecipe choiceRecipe(bool checked, const InteractionState& state, Theme theme) {
    ChoiceRecipe recipe;
    const bool live = state.enabled;
    if (checked) {
        recipe.background = token(u"accent-subtle", theme);
        recipe.border = token(u"accent-border", theme);
    } else {
        recipe.background = live && state.hovered ? token(u"surface-hover", theme) : transparent();
        recipe.border = token(u"panel-border", theme);
    }
    recipe.title = token(u"text-main", theme);
    recipe.description = token(u"text-muted", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.padding = profile().choicePadding;
    recipe.radius = profile().choiceRadius;
    recipe.spacing = Tokens::px(u"space-2-5");
    recipe.titlePixels = Tokens::px(u"text-sm");
    recipe.titleWeight = profile().choiceTitleWeight;
    recipe.descriptionPixels = Tokens::px(u"text-xs");
    if (!live)
        recipe.opacity = profile().disabledOpacity;
    return recipe;
}

FieldRowRecipe fieldRowRecipe(Theme theme) {
    FieldRowRecipe recipe;
    recipe.label = token(u"text-secondary", theme);
    recipe.help = token(u"text-muted", theme);
    recipe.error = token(u"danger", theme);
    recipe.labelWidth = profile().fieldLabelWidth;
    recipe.gap = profile().fieldRowGap;
    recipe.labelTop = profile().fieldLabelTop;
    recipe.labelPixels = Tokens::px(u"text-md");
    recipe.helpPixels = Tokens::px(u"text-sm");
    recipe.helpGap = profile().fieldHelpGap;
    recipe.rowGap = Tokens::px(u"space-5");
    return recipe;
}

QVariantMap toVariantMap(const FieldRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("border"), r.border},
            {QStringLiteral("text"), r.text},
            {QStringLiteral("placeholder"), r.placeholder},
            {QStringLiteral("icon"), r.icon},
            {QStringLiteral("height"), r.height},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("borderWidth"), r.borderWidth},
            {QStringLiteral("paddingInline"), r.paddingInline},
            {QStringLiteral("iconSize"), r.iconSize},
            {QStringLiteral("spacing"), r.spacing},
            {QStringLiteral("fontPixels"), r.fontPixels},
            {QStringLiteral("opacity"), r.opacity}};
}

QVariantMap toVariantMap(const ToggleRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("border"), r.border},
            {QStringLiteral("mark"), r.mark},
            {QStringLiteral("focusRing"), r.focusRing},
            {QStringLiteral("label"), r.label},
            {QStringLiteral("size"), r.size},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("spacing"), r.spacing},
            {QStringLiteral("fontPixels"), r.fontPixels},
            {QStringLiteral("opacity"), r.opacity}};
}

QVariantMap toVariantMap(const ChoiceRecipe& r) {
    return {{QStringLiteral("background"), r.background},
            {QStringLiteral("border"), r.border},
            {QStringLiteral("title"), r.title},
            {QStringLiteral("description"), r.description},
            {QStringLiteral("focusRing"), r.focusRing},
            {QStringLiteral("padding"), r.padding},
            {QStringLiteral("radius"), r.radius},
            {QStringLiteral("spacing"), r.spacing},
            {QStringLiteral("titlePixels"), r.titlePixels},
            {QStringLiteral("titleWeight"), r.titleWeight},
            {QStringLiteral("descriptionPixels"), r.descriptionPixels},
            {QStringLiteral("opacity"), r.opacity}};
}

QVariantMap toVariantMap(const FieldRowRecipe& r) {
    return {{QStringLiteral("label"), r.label},
            {QStringLiteral("help"), r.help},
            {QStringLiteral("error"), r.error},
            {QStringLiteral("labelWidth"), r.labelWidth},
            {QStringLiteral("gap"), r.gap},
            {QStringLiteral("labelTop"), r.labelTop},
            {QStringLiteral("labelPixels"), r.labelPixels},
            {QStringLiteral("helpPixels"), r.helpPixels},
            {QStringLiteral("helpGap"), r.helpGap},
            {QStringLiteral("rowGap"), r.rowGap}};
}

} // namespace ket
