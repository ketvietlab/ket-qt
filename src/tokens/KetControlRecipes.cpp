// SPDX-License-Identifier: MPL-2.0
// Controls and chrome: slider, avatar, tree items, group headers, route tabs, status bar,
// breadcrumb and pane header.
#include "KetControlRecipes.h"

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

} // namespace

SliderRecipe sliderRecipe(const InteractionState& state, Theme theme) {
    SliderRecipe recipe;
    const bool live = state.enabled;
    recipe.track = token(u"border-default", theme);
    recipe.fill =
        token(live && (state.hovered || state.pressed) ? u"accent-hover" : u"accent", theme);
    recipe.handle = token(u"panel-bg", theme);
    recipe.handleBorder =
        token(live && (state.hovered || state.pressed) ? u"accent-hover" : u"accent", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.label = token(u"text-main", theme);
    recipe.value = token(u"accent-text", theme);
    recipe.rangeLabel = token(u"text-muted", theme);
    recipe.height = profile().sliderHeight;
    recipe.trackHeight = profile().sliderTrackHeight;
    recipe.handleSize = profile().sliderHandleSize;
    recipe.handleBorderWidth = profile().sliderHandleBorder;
    recipe.labelPixels = profile().sliderLabelPixels;
    recipe.labelWeight = profile().sliderLabelWeight;
    recipe.rangePixels = profile().sliderRangePixels;
    recipe.headerGap = profile().sliderHeaderGap;
    recipe.labelsGap = profile().sliderLabelsGap;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const SliderRecipe& r) {
    return mapOf({{"track", r.track},
                  {"fill", r.fill},
                  {"handle", r.handle},
                  {"handleBorder", r.handleBorder},
                  {"focusRing", r.focusRing},
                  {"label", r.label},
                  {"value", r.value},
                  {"rangeLabel", r.rangeLabel},
                  {"height", r.height},
                  {"trackHeight", r.trackHeight},
                  {"handleSize", r.handleSize},
                  {"handleBorderWidth", r.handleBorderWidth},
                  {"labelPixels", r.labelPixels},
                  {"labelWeight", r.labelWeight},
                  {"rangePixels", r.rangePixels},
                  {"headerGap", r.headerGap},
                  {"labelsGap", r.labelsGap},
                  {"opacity", r.opacity}});
}

AvatarRecipe avatarRecipe(Theme theme) {
    AvatarRecipe recipe;
    recipe.background = token(u"surface-raised", theme);
    recipe.border = token(u"border-default", theme);
    recipe.text = token(u"text-main", theme);
    recipe.size = profile().avatarDiameter;
    recipe.fontPixels = profile().avatarPixels;
    recipe.fontWeight = profile().avatarWeight;
    return recipe;
}

QVariantMap toVariantMap(const AvatarRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"size", r.size},
                  {"fontPixels", r.fontPixels},
                  {"fontWeight", r.fontWeight}});
}

TreeItemRecipe treeItemRecipe(bool selected, const InteractionState& state, Theme theme) {
    TreeItemRecipe recipe;
    const bool live = state.enabled;
    recipe.background = selected                                   ? token(u"accent-subtle", theme)
                        : live && (state.hovered || state.pressed) ? token(u"surface-hover", theme)
                                                                   : transparent();
    recipe.title = token(selected ? u"accent-text" : u"text-main", theme);
    recipe.icon = token(selected ? u"accent-text" : u"text-muted", theme);
    recipe.detail = token(u"text-muted", theme);
    recipe.count = token(u"accent-text", theme);
    recipe.working = token(u"accent-text", theme);
    recipe.waiting = token(u"warning", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.paddingBlock = profile().treeItemPaddingBlock;
    recipe.paddingInline = profile().treeItemPaddingInline;
    recipe.spacing = profile().treeItemGap;
    recipe.radius = profile().treeItemRadius;
    recipe.iconSize = profile().smallIconSize;
    recipe.iconOffset = profile().treeItemIconOffset;
    recipe.titlePixels = profile().treeItemTitlePixels;
    recipe.titleWeight = profile().treeItemTitleWeight;
    recipe.detailPixels = profile().treeItemDetailPixels;
    recipe.activityPixels = profile().treeItemActivityPixels;
    recipe.activityIconSize = profile().treeItemActivityIconSize;
    recipe.activityGap = profile().treeItemActivityGap;
    recipe.activityOffset = profile().treeItemActivityOffset;
    recipe.countPixels = profile().treeItemActivityPixels;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const TreeItemRecipe& r) {
    return mapOf({{"background", r.background},
                  {"title", r.title},
                  {"icon", r.icon},
                  {"detail", r.detail},
                  {"count", r.count},
                  {"working", r.working},
                  {"waiting", r.waiting},
                  {"focusRing", r.focusRing},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"radius", r.radius},
                  {"iconSize", r.iconSize},
                  {"iconOffset", r.iconOffset},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"detailPixels", r.detailPixels},
                  {"activityPixels", r.activityPixels},
                  {"activityIconSize", r.activityIconSize},
                  {"activityGap", r.activityGap},
                  {"activityOffset", r.activityOffset},
                  {"countPixels", r.countPixels},
                  {"opacity", r.opacity}});
}

GroupHeaderRecipe groupHeaderRecipe(bool selected, const InteractionState& state, Theme theme) {
    GroupHeaderRecipe recipe;
    const bool live = state.enabled;
    recipe.text = token(live && (state.hovered || state.pressed) || selected ? u"text-secondary"
                                                                             : u"text-muted",
                        theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.height = profile().groupHeaderHeight;
    recipe.paddingStart = profile().groupHeaderPaddingStart;
    recipe.paddingEnd = profile().groupHeaderPaddingEnd;
    recipe.spacing = profile().groupHeaderGap;
    recipe.nameGap = profile().groupHeaderNameGap;
    recipe.iconSize = profile().groupHeaderIconSize;
    recipe.fontPixels = profile().groupHeaderPixels;
    recipe.fontWeight = profile().groupHeaderWeight;
    recipe.actionSize = profile().groupHeaderActionSize;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const GroupHeaderRecipe& r) {
    return mapOf({{"text", r.text},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"paddingStart", r.paddingStart},
                  {"paddingEnd", r.paddingEnd},
                  {"spacing", r.spacing},
                  {"nameGap", r.nameGap},
                  {"iconSize", r.iconSize},
                  {"fontPixels", r.fontPixels},
                  {"fontWeight", r.fontWeight},
                  {"actionSize", r.actionSize},
                  {"opacity", r.opacity}});
}

RouteTabRecipe routeTabRecipe(bool active, const InteractionState& state, Theme theme) {
    RouteTabRecipe recipe;
    const bool live = state.enabled;
    recipe.text = token(active                                     ? u"accent-text"
                        : live && (state.hovered || state.pressed) ? u"text-main"
                                                                   : u"text-muted",
                        theme);
    recipe.indicator = active ? token(u"accent", theme) : transparent();
    recipe.rule = token(u"panel-border", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.fontPixels = profile().routeTabPixels;
    recipe.paddingTop = profile().routeTabPaddingTop;
    recipe.paddingBottom = profile().routeTabPaddingBottom;
    recipe.indicatorHeight = profile().routeTabIndicator;
    recipe.spacing = profile().routeTabGap;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const RouteTabRecipe& r) {
    return mapOf({{"text", r.text},
                  {"indicator", r.indicator},
                  {"rule", r.rule},
                  {"focusRing", r.focusRing},
                  {"fontPixels", r.fontPixels},
                  {"paddingTop", r.paddingTop},
                  {"paddingBottom", r.paddingBottom},
                  {"indicatorHeight", r.indicatorHeight},
                  {"spacing", r.spacing},
                  {"opacity", r.opacity}});
}

StatusBarRecipe statusBarRecipe(Theme theme) {
    StatusBarRecipe recipe;
    recipe.background = token(u"page-chrome-bg", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.text = token(u"text-muted", theme);
    recipe.height = profile().statusBarHeight;
    recipe.paddingInline = profile().statusBarPaddingInline;
    recipe.fontPixels = profile().statusBarPixels;
    recipe.iconSize = profile().statusBarIconSize;
    recipe.itemGap = profile().statusBarItemGap;
    recipe.groupGap = profile().statusBarGroupGap;
    return recipe;
}

QVariantMap toVariantMap(const StatusBarRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"fontPixels", r.fontPixels},
                  {"iconSize", r.iconSize},
                  {"itemGap", r.itemGap},
                  {"groupGap", r.groupGap}});
}

BreadcrumbRecipe breadcrumbRecipe(Theme theme) {
    BreadcrumbRecipe recipe;
    recipe.text = token(u"text-muted", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.height = profile().breadcrumbHeight;
    recipe.paddingInline = profile().breadcrumbPaddingInline;
    recipe.spacing = profile().breadcrumbGap;
    recipe.fontPixels = profile().breadcrumbPixels;
    recipe.iconSize = profile().breadcrumbIconSize;
    return recipe;
}

QVariantMap toVariantMap(const BreadcrumbRecipe& r) {
    return mapOf({{"text", r.text},
                  {"border", r.border},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"fontPixels", r.fontPixels},
                  {"iconSize", r.iconSize}});
}

PaneHeaderRecipe paneHeaderRecipe(Theme theme) {
    PaneHeaderRecipe recipe;
    recipe.text = token(u"text-main", theme);
    recipe.icon = token(u"text-muted", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.height = profile().paneHeaderHeight;
    recipe.paddingStart = profile().paneHeaderPaddingStart;
    recipe.paddingEnd = profile().paneHeaderPaddingEnd;
    recipe.spacing = profile().paneHeaderGap;
    recipe.fontPixels = profile().paneHeaderPixels;
    recipe.iconSize = profile().paneHeaderIconSize;
    return recipe;
}

QVariantMap toVariantMap(const PaneHeaderRecipe& r) {
    return mapOf({{"text", r.text},
                  {"icon", r.icon},
                  {"border", r.border},
                  {"height", r.height},
                  {"paddingStart", r.paddingStart},
                  {"paddingEnd", r.paddingEnd},
                  {"spacing", r.spacing},
                  {"fontPixels", r.fontPixels},
                  {"iconSize", r.iconSize}});
}

} // namespace ket
