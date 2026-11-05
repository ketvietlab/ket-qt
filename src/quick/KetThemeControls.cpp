// SPDX-License-Identifier: MPL-2.0
// Controls and chrome (KetControlRecipes).
#include "KetControlRecipes.h"
#include "KetTheme.h"
#include "KetThemeDetail.h"

namespace ket {

using detail::kShadowLengths;
using detail::scaleLengths;
using detail::themeOf;

QVariantMap QuickTheme::sliderRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                     bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::sliderRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"trackHeight", u"handleSize", u"handleBorderWidth", u"labelPixels",
                  u"rangePixels", u"headerGap", u"labelsGap"});
    return recipe;
}

QVariantMap QuickTheme::avatarRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::avatarRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"size", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::treeItemRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                       bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::treeItemRecipe(selected, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"paddingBlock", u"paddingInline", u"spacing", u"radius", u"iconSize",
                  u"iconOffset", u"titlePixels", u"detailPixels", u"activityPixels",
                  u"activityIconSize", u"activityGap", u"activityOffset", u"countPixels"});
    return recipe;
}

QVariantMap QuickTheme::groupHeaderRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                          bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::groupHeaderRecipe(selected, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingStart", u"paddingEnd", u"spacing", u"nameGap", u"iconSize",
                  u"fontPixels", u"actionSize"});
    return recipe;
}

QVariantMap QuickTheme::routeTabRecipe(bool dark, qreal zoom, bool active, bool hovered,
                                       bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::routeTabRecipe(active, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"fontPixels", u"paddingTop", u"paddingBottom", u"indicatorHeight", u"spacing"});
    return recipe;
}

QVariantMap QuickTheme::statusBarRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::statusBarRecipe(themeOf(dark)));
    scaleLengths(
        recipe, zoom,
        {u"height", u"paddingInline", u"fontPixels", u"iconSize", u"itemGap", u"groupGap"});
    return recipe;
}

QVariantMap QuickTheme::breadcrumbRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::breadcrumbRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"spacing", u"fontPixels", u"iconSize"});
    return recipe;
}

QVariantMap QuickTheme::paneHeaderRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::paneHeaderRecipe(themeOf(dark)));
    scaleLengths(
        recipe, zoom,
        {u"height", u"paddingStart", u"paddingEnd", u"spacing", u"fontPixels", u"iconSize"});
    return recipe;
}

} // namespace ket
