// SPDX-License-Identifier: MPL-2.0
// Chat extras (KetChatRecipes).
#include "KetChatRecipes.h"
#include "KetTheme.h"
#include "KetThemeDetail.h"

namespace ket {

using detail::kShadowLengths;
using detail::scaleLengths;
using detail::themeOf;

QVariantMap QuickTheme::popoverRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::popoverRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"width", u"padding", u"radius", u"offset", u"titlePixels", u"headerGap",
                  u"bodyPixels", u"notePixels", u"dividerSpacing", u"footerGap"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::meterRecipe(bool dark, qreal zoom, bool compact, qreal fraction) const {
    QVariantMap recipe = toVariantMap(ket::meterRecipe(compact, fraction, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"labelPixels", u"spacing", u"trackHeight", u"trackSpacingTop",
                  u"trackSpacingBottom", u"captionPixels"});
    return recipe;
}

QVariantMap QuickTheme::attachmentChipRecipe(bool dark, qreal zoom, bool removeHovered) const {
    QVariantMap recipe = toVariantMap(ket::attachmentChipRecipe(removeHovered, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"radius", u"spacing", u"iconSize", u"fontPixels",
                  u"maxWidth", u"rowGap", u"rowPaddingInline", u"rowPaddingTop"});
    return recipe;
}

QVariantMap QuickTheme::suggestionRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                         bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::suggestionRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"paddingBlock", u"paddingInline", u"radius", u"spacing", u"iconSize",
                  u"arrowSize", u"titlePixels", u"captionPixels", u"captionGap"});
    return recipe;
}

QVariantMap QuickTheme::questionCardRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::questionCardRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"padding", u"radius", u"spacing", u"iconSize", u"labelPixels", u"headingPixels",
                  u"headingSpacing", u"controlGap"});
    return recipe;
}

QVariantMap QuickTheme::activityGroupRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                            bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::activityGroupRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"fontPixels", u"spacing", u"chevronSize",
                  u"childrenIndent", u"childrenPadding", u"childrenGap", u"childrenSpacingTop",
                  u"childrenSpacingBottom"});
    return recipe;
}

QVariantMap QuickTheme::galleryRecipe(bool dark, qreal zoom, bool hovered) const {
    QVariantMap recipe = toVariantMap(ket::galleryRecipe(hovered, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"tileSize", u"spacing", u"paddingInline", u"paddingTop", u"paddingBottom",
                  u"radius", u"moreRadius", u"ringWidth", u"captionPixels", u"captionGap"});
    return recipe;
}

} // namespace ket
