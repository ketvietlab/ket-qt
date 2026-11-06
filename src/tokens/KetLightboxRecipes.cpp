// SPDX-License-Identifier: MPL-2.0
// The image viewer (`interactions/lightbox` in the design system).
#include "KetLightboxRecipes.h"

#include "KetProfile.h"

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

QColor withAlpha(QColor color, qreal alpha) {
    color.setAlphaF(float(alpha));
    return color;
}

// The design system mixes the current theme's text-main into the backdrop and panel-bg
// into the glyphs, which turns the viewer light in the dark theme. A photo viewer stays
// dark, so both come from the light theme, as the dialog backdrop does.
QColor foreground() { return token(u"panel-bg", Theme::Light); }

} // namespace

LightboxRecipe lightboxRecipe(Theme) {
    LightboxRecipe recipe;
    recipe.backdrop =
        withAlpha(token(u"text-main", Theme::Light), profile().lightboxBackdropOpacity);
    recipe.foreground = foreground();
    recipe.toolbarPaddingBlock = Tokens::px(u"space-2");
    recipe.toolbarPaddingInline = Tokens::px(u"space-3");
    recipe.toolbarGap = Tokens::px(u"space-1");
    recipe.counterPixels = Tokens::px(u"text-sm");
    recipe.counterLineHeight = Tokens::px(u"line-sm");
    recipe.zoomLevelWidth = profile().lightboxZoomLevelWidth;
    recipe.stagePaddingBlock = Tokens::px(u"space-2");
    recipe.stagePaddingInline = Tokens::px(u"space-12");
    recipe.narrowStagePaddingInline = Tokens::px(u"space-2");
    recipe.narrowWidth = profile().lightboxNarrowWidth;
    recipe.navInset = Tokens::px(u"space-3");
    recipe.captionPixels = Tokens::px(u"text-sm");
    recipe.captionLineHeight = Tokens::px(u"line-sm");
    recipe.captionPaddingTop = Tokens::px(u"space-2");
    recipe.captionPaddingInline = Tokens::px(u"space-4");
    recipe.captionPaddingBottom = Tokens::px(u"space-4");
    recipe.placeholderIconSize = profile().lightboxPlaceholderIconSize;
    recipe.placeholderOpacity = profile().lightboxPlaceholderOpacity;
    recipe.maxZoom = profile().lightboxMaxZoom;
    recipe.zoomStep = profile().lightboxZoomStep;
    recipe.tapZoom = profile().lightboxTapZoom;
    recipe.wheelStep = profile().lightboxWheelStep;
    recipe.swipeDistance = profile().lightboxSwipeDistance;
    recipe.transitionMs = profile().lightboxTransitionMs;
    return recipe;
}

QVariantMap toVariantMap(const LightboxRecipe& r) {
    return mapOf({{"backdrop", r.backdrop},
                  {"foreground", r.foreground},
                  {"toolbarPaddingBlock", r.toolbarPaddingBlock},
                  {"toolbarPaddingInline", r.toolbarPaddingInline},
                  {"toolbarGap", r.toolbarGap},
                  {"counterPixels", r.counterPixels},
                  {"counterLineHeight", r.counterLineHeight},
                  {"zoomLevelWidth", r.zoomLevelWidth},
                  {"stagePaddingBlock", r.stagePaddingBlock},
                  {"stagePaddingInline", r.stagePaddingInline},
                  {"narrowStagePaddingInline", r.narrowStagePaddingInline},
                  {"narrowWidth", r.narrowWidth},
                  {"navInset", r.navInset},
                  {"captionPixels", r.captionPixels},
                  {"captionLineHeight", r.captionLineHeight},
                  {"captionPaddingTop", r.captionPaddingTop},
                  {"captionPaddingInline", r.captionPaddingInline},
                  {"captionPaddingBottom", r.captionPaddingBottom},
                  {"placeholderIconSize", r.placeholderIconSize},
                  {"placeholderOpacity", r.placeholderOpacity},
                  {"maxZoom", r.maxZoom},
                  {"zoomStep", r.zoomStep},
                  {"tapZoom", r.tapZoom},
                  {"wheelStep", r.wheelStep},
                  {"swipeDistance", r.swipeDistance},
                  {"transitionMs", r.transitionMs}});
}

LightboxToolRecipe lightboxToolRecipe(const InteractionState& state, Theme) {
    LightboxToolRecipe recipe;
    const bool hovered = state.enabled && (state.hovered || state.pressed);
    recipe.background = withAlpha(foreground(), hovered ? profile().lightboxToolHoverOpacity
                                                        : profile().lightboxToolOpacity);
    recipe.glyph = foreground();
    recipe.focusRing = state.focused && state.enabled ? foreground() : QColor(Qt::transparent);
    recipe.size = profile().lightboxToolSize;
    recipe.iconSize = profile().lightboxToolIconSize;
    recipe.focusWidth = 2;
    recipe.focusOffset = 2;
    recipe.opacity = state.enabled ? 1.0 : profile().lightboxDisabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const LightboxToolRecipe& r) {
    return mapOf({{"background", r.background},
                  {"glyph", r.glyph},
                  {"focusRing", r.focusRing},
                  {"size", r.size},
                  {"iconSize", r.iconSize},
                  {"focusWidth", r.focusWidth},
                  {"focusOffset", r.focusOffset},
                  {"opacity", r.opacity}});
}

LightboxThumbRecipe lightboxThumbRecipe(LightboxThumbSize size, bool focused, Theme theme) {
    LightboxThumbRecipe recipe;
    recipe.background = token(u"surface-raised", theme);
    recipe.border = token(u"border-default", theme);
    recipe.text = token(u"text-muted", theme);
    recipe.focusRing = focused ? token(u"focus-border", theme) : QColor(Qt::transparent);
    switch (size) {
    case LightboxThumbSize::Small:
        recipe.size = profile().lightboxThumbSmall;
        break;
    case LightboxThumbSize::Medium:
        recipe.size = profile().lightboxThumbMedium;
        break;
    case LightboxThumbSize::Large:
        recipe.size = profile().lightboxThumbLarge;
        break;
    }
    recipe.radius = Tokens::px(size == LightboxThumbSize::Small ? u"radius-sm" : u"radius-md");
    recipe.textPixels = Tokens::px(u"text-sm");
    recipe.lineHeight = Tokens::px(u"line-sm");
    recipe.focusWidth = 2;
    recipe.focusOffset = 2;
    return recipe;
}

QVariantMap toVariantMap(const LightboxThumbRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"focusRing", r.focusRing},
                  {"size", r.size},
                  {"radius", r.radius},
                  {"textPixels", r.textPixels},
                  {"lineHeight", r.lineHeight},
                  {"focusWidth", r.focusWidth},
                  {"focusOffset", r.focusOffset}});
}

} // namespace ket
