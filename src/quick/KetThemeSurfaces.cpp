// SPDX-License-Identifier: MPL-2.0
// Surfaces (KetSurfaceRecipes).
#include "KetSurfaceRecipes.h"
#include "KetTheme.h"
#include "KetThemeDetail.h"

namespace ket {

using detail::kShadowLengths;
using detail::scaleLengths;
using detail::themeOf;

static_assert(int(QuickTheme::TaskStatus::Stopped) == int(ket::TaskStatus::Stopped));

QVariantMap QuickTheme::commandPaletteRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::commandPaletteRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"width",
                  u"top",
                  u"radius",
                  u"maxListHeight",
                  u"inputPixels",
                  u"searchIconSize",
                  u"searchPaddingBlock",
                  u"searchPaddingInline",
                  u"searchGap",
                  u"inputPaddingBlock",
                  u"groupPixels",
                  u"groupPaddingTop",
                  u"groupPaddingInline",
                  u"groupPaddingBottom",
                  u"listPaddingInline",
                  u"listPaddingBottom",
                  u"footerPaddingBlock",
                  u"footerPaddingInline",
                  u"footerGap",
                  u"footerPixels",
                  u"emptyIconSize",
                  u"emptyTitlePixels",
                  u"emptyTextPixels",
                  u"emptyHeight"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::commandResultRecipe(bool dark, qreal zoom, bool highlighted) const {
    QVariantMap recipe = toVariantMap(ket::commandResultRecipe(highlighted, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"paddingBlock", u"paddingInline", u"spacing", u"radius", u"iconSize",
                  u"trailingSize", u"titlePixels", u"detailPixels", u"detailGap"});
    return recipe;
}

QVariantMap QuickTheme::fileRowRecipe(bool dark, qreal zoom, bool active, bool hovered,
                                      bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::fileRowRecipe(active, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"folderHeight", u"paddingInline", u"spacing", u"folderSpacing",
                  u"iconSize", u"chevronSize", u"fontPixels", u"statusPixels", u"indentStart",
                  u"indentStep"});
    return recipe;
}

QVariantMap QuickTheme::changedFilesRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::changedFilesRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingBlock", u"paddingInline", u"headingGap", u"rowPaddingBlock",
                  u"rowGap", u"footerPaddingBlock", u"iconSize", u"arrowSize", u"textPixels",
                  u"titlePixels", u"pathPixels"});
    return recipe;
}

QVariantMap QuickTheme::diffRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::diffRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"lineHeight", u"fontPixels", u"numberWidth", u"numberGap", u"signWidth",
                  u"codeGap", u"hunkPaddingBlock", u"hunkPaddingInline", u"paddingBlock",
                  u"paddingEnd"});
    return recipe;
}

QVariantMap QuickTheme::taskRowRecipe(bool dark, qreal zoom, int status, bool current) const {
    QVariantMap recipe =
        toVariantMap(ket::taskRowRecipe(ket::TaskStatus(status), current, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"minHeight", u"paddingStart", u"paddingEnd", u"spacing", u"radius", u"iconSize",
                  u"titlePixels", u"metaPixels", u"actionSize"});
    return recipe;
}

QVariantMap QuickTheme::taskBarRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                      bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::taskBarRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"radius", u"fontPixels", u"iconSize",
                  u"chevronSize", u"spacing", u"rowGap"});
    return recipe;
}

QVariantMap QuickTheme::updateCardRecipe(bool dark, qreal zoom, bool ready) const {
    QVariantMap recipe = toVariantMap(ket::updateCardRecipe(ready, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingBlock", u"paddingInline", u"titlePixels", u"detailPixels",
                  u"headerGap", u"headerColumnGap", u"sectionGap", u"notesPixels", u"notesIndent",
                  u"actionsGap", u"actionsTop"});
    return recipe;
}

QVariantMap QuickTheme::updatePillRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                         bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::updatePillRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"paddingBlock", u"paddingInline", u"radius", u"spacing", u"iconSize",
                  u"titlePixels", u"detailPixels"});
    return recipe;
}

QVariantMap QuickTheme::themeCardRecipe(bool dark, qreal zoom, bool selected, bool previewDark,
                                        bool hovered, bool pressed, bool enabled,
                                        bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::themeCardRecipe(selected, themeOf(previewDark), state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"miniHeight", u"miniMargin", u"miniSideWidth", u"labelPaddingBlock",
                  u"labelPaddingInline", u"labelPixels", u"checkSize"});
    return recipe;
}

QVariantMap QuickTheme::accentSwatchRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::accentSwatchRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"size", u"radius", u"borderWidth"});
    return recipe;
}

} // namespace ket
