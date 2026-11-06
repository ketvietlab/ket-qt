// SPDX-License-Identifier: MPL-2.0
// KetTheme's image viewer recipes (KetLightboxRecipes).
#include "KetTheme.h"
#include "KetThemeDetail.h"

#include "KetLightboxRecipes.h"

namespace ket {
namespace {

using detail::scaleLengths;
using detail::themeOf;

static_assert(int(QuickTheme::LightboxTool::Fullscreen) == int(ket::LightboxTool::Fullscreen));
static_assert(int(QuickTheme::LightboxTool::ZoomLevel) == int(ket::LightboxTool::ZoomLevel));
static_assert(int(QuickTheme::LightboxThumbSize::Large) == int(ket::LightboxThumbSize::Large));

} // namespace

QVariantMap QuickTheme::lightboxRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::lightboxRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"toolbarPaddingBlock", u"toolbarPaddingInline", u"toolbarGap", u"counterPixels",
                  u"counterLineHeight", u"zoomLevelWidth", u"stagePaddingBlock",
                  u"stagePaddingInline", u"narrowStagePaddingInline", u"narrowWidth", u"navInset",
                  u"captionPixels", u"captionLineHeight", u"captionPaddingTop",
                  u"captionPaddingInline", u"captionPaddingBottom", u"placeholderIconSize",
                  u"swipeDistance"});
    return recipe;
}

QVariantMap QuickTheme::lightboxToolRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                           bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::lightboxToolRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom, {u"size", u"iconSize", u"focusWidth", u"focusOffset"});
    return recipe;
}

QVariantMap QuickTheme::lightboxThumbRecipe(bool dark, qreal zoom, int size, bool focused) const {
    QVariantMap recipe = toVariantMap(ket::lightboxThumbRecipe(
        ket::LightboxThumbSize(qBound(0, size, 2)), focused, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"size", u"radius", u"textPixels", u"lineHeight", u"focusWidth", u"focusOffset"});
    return recipe;
}

} // namespace ket
