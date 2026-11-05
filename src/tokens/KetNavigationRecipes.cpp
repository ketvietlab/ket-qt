// SPDX-License-Identifier: MPL-2.0
// Navigation and status recipes: nav rows, kbd, segmented controls, tab strips, progress,
// activity and empty states.
#include "KetRecipes.h"

#include "KetProfile.h"

#include <QtMath>

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

NavItemRecipe navItemRecipe(bool selected, const InteractionState& state, Theme theme) {
    NavItemRecipe recipe;
    const bool live = state.enabled;
    // `.nav-item.is-active` outranks `.nav-item:hover`.
    if (selected)
        recipe.background = token(u"accent-subtle", theme);
    else if (live && (state.hovered || state.pressed))
        recipe.background = token(u"surface-hover", theme);
    else
        recipe.background = transparent();
    recipe.text = token(selected ? u"nav-selected-text" : u"text-secondary", theme);
    recipe.fontWeight = selected ? profile().navItemActiveWeight : profile().buttonFontWeight;
    recipe.focusRing = focusRing(state, theme);
    recipe.countBorder = token(u"border-default", theme);
    recipe.countText = recipe.text;
    recipe.kbdText = token(u"text-muted", theme);
    recipe.height = profile().navItemHeight;
    recipe.radius = profile().navItemRadius;
    recipe.paddingInline = profile().navItemPadding;
    recipe.spacing = profile().navItemGap;
    recipe.iconSize = profile().iconSize;
    recipe.fontPixels = profile().navItemFontPixels;
    recipe.countHeight = profile().navCountHeight;
    recipe.countPadding = profile().navCountPadding;
    recipe.countRadius = profile().navCountRadius;
    recipe.countFontPixels = profile().kbdFontPixels;
    recipe.kbdFontPixels = profile().kbdFontPixels;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

KbdRecipe kbdRecipe(Theme theme) { return {token(u"text-muted", theme), profile().kbdFontPixels}; }

SegmentedRecipe segmentedRecipe(Theme theme) {
    SegmentedRecipe recipe;
    recipe.background = token(u"panel-bg-subtle", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.padding = profile().segmentedPadding;
    recipe.radius = profile().segmentedRadius;
    recipe.spacing = profile().segmentedPadding - 1;
    return recipe;
}

SegmentRecipe segmentRecipe(bool selected, const InteractionState& state, Theme theme) {
    SegmentRecipe recipe;
    recipe.background = selected ? token(u"panel-bg", theme) : transparent();
    recipe.text =
        token(selected || (state.enabled && state.hovered) ? u"text-main" : u"text-muted", theme);
    recipe.focusRing = focusRing(state, theme);
    if (selected)
        recipe.shadow = shadowToken(u"shadow-xs", theme);
    recipe.radius = profile().segmentRadius;
    recipe.paddingInline = profile().segmentPaddingInline;
    recipe.paddingBlock = profile().segmentPaddingBlock;
    recipe.fontPixels = profile().segmentFontPixels;
    recipe.opacity = state.enabled ? 1.0 : profile().disabledOpacity;
    return recipe;
}

TabBarRecipe tabBarRecipe(Theme theme) {
    return {token(u"panel-bg-subtle", theme), token(u"panel-border", theme), profile().tabHeight};
}

TabRecipe tabRecipe(bool selected, const InteractionState& state, Theme theme) {
    TabRecipe recipe;
    recipe.background = selected                         ? token(u"panel-bg", theme)
                        : state.enabled && state.hovered ? token(u"surface-hover", theme)
                                                         : transparent();
    recipe.text =
        token(selected || (state.enabled && state.hovered) ? u"text-main" : u"text-muted", theme);
    recipe.indicator = selected ? token(u"accent", theme) : transparent();
    recipe.focusRing = focusRing(state, theme);
    recipe.height = profile().tabHeight;
    recipe.paddingInline = profile().tabPaddingInline;
    recipe.spacing = profile().tabGap;
    recipe.iconSize = profile().tabIconSize;
    recipe.indicatorHeight = profile().tabIndicator;
    recipe.fontPixels = profile().tabFontPixels;
    recipe.opacity = state.enabled ? 1.0 : profile().disabledOpacity;
    return recipe;
}

ProgressRecipe progressRecipe(Theme theme) {
    ProgressRecipe recipe;
    recipe.track = token(u"border-default", theme);
    recipe.fill = token(u"accent", theme);
    recipe.height = profile().progressHeight;
    recipe.radius = profile().progressHeight / 2;
    recipe.busyFraction = 0.3; // not in the mock, which has no indeterminate bar
    recipe.busyDurationMs = profile().progressBusyDurationMs;
    return recipe;
}

ActivityRecipe activityRecipe(Theme theme, const QColor& color) {
    ActivityRecipe recipe;
    recipe.color = color.isValid() ? color : token(u"success", theme);
    recipe.barWidth = profile().activityBarWidth;
    recipe.barHeight = profile().activityBarHeight;
    recipe.minBarHeight = profile().activityBarMinHeight;
    recipe.spacing = profile().activityGap;
    recipe.radius = profile().activityBarWidth / 2;
    recipe.minOpacity = profile().activityMinOpacity;
    recipe.durationMs = profile().activityDurationMs;
    recipe.staggerMs = profile().activityStaggerMs;
    return recipe;
}

std::pair<qreal, qreal> activityBarFrame(const ActivityRecipe& recipe, int bar, qint64 elapsedMs) {
    if (recipe.durationMs <= 0)
        return {recipe.barHeight, 1.0};
    // A CSS delay holds the first frame, then the bar cycles like the others.
    const qint64 local = elapsedMs - qint64(bar) * recipe.staggerMs;
    if (local < 0)
        return {recipe.barHeight, 1.0};
    const qreal phase = qreal(local % recipe.durationMs) / recipe.durationMs;
    // `ease` between the 0% and 50% frames and back, approximated with a smoothstep.
    const qreal t = phase < 0.5 ? phase * 2 : (1 - phase) * 2;
    const qreal eased = t * t * (3 - 2 * t);
    return {recipe.barHeight + (recipe.minBarHeight - recipe.barHeight) * eased,
            1.0 + (recipe.minOpacity - 1.0) * eased};
}

EmptyStateRecipe emptyStateRecipe(Theme theme) {
    EmptyStateRecipe recipe;
    recipe.icon = token(u"accent-text", theme);
    recipe.title = token(u"text-main", theme);
    recipe.message = token(u"text-muted", theme);
    recipe.iconSize = profile().emptyIconSize;
    recipe.spacing = profile().emptyGap;
    recipe.titlePixels = profile().emptyTitlePixels;
    recipe.titleWeight = qRound(Tokens::number(u"weight-semibold"));
    recipe.messagePixels = profile().emptyMessagePixels;
    recipe.messageMaxWidth = profile().emptyMessageMaxWidth;
    recipe.lineHeight = profile().emptyLineHeight;
    return recipe;
}

QVariantMap toVariantMap(const NavItemRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"focusRing", r.focusRing},
                  {"countBorder", r.countBorder},
                  {"countText", r.countText},
                  {"kbdText", r.kbdText},
                  {"fontWeight", r.fontWeight},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"fontPixels", r.fontPixels},
                  {"countHeight", r.countHeight},
                  {"countPadding", r.countPadding},
                  {"countRadius", r.countRadius},
                  {"countFontPixels", r.countFontPixels},
                  {"kbdFontPixels", r.kbdFontPixels},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const KbdRecipe& r) {
    return mapOf({{"text", r.text}, {"fontPixels", r.fontPixels}});
}

QVariantMap toVariantMap(const SegmentedRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"spacing", r.spacing}});
}

QVariantMap toVariantMap(const SegmentRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"focusRing", r.focusRing},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"fontPixels", r.fontPixels},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const TabBarRecipe& r) {
    return mapOf({{"background", r.background}, {"border", r.border}, {"height", r.height}});
}

QVariantMap toVariantMap(const TabRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"indicator", r.indicator},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"indicatorHeight", r.indicatorHeight},
                  {"fontPixels", r.fontPixels},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const ProgressRecipe& r) {
    return mapOf({{"track", r.track},
                  {"fill", r.fill},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"busyFraction", r.busyFraction},
                  {"busyDurationMs", r.busyDurationMs}});
}

QVariantMap toVariantMap(const ActivityRecipe& r) {
    return mapOf({{"color", r.color},
                  {"barWidth", r.barWidth},
                  {"barHeight", r.barHeight},
                  {"minBarHeight", r.minBarHeight},
                  {"spacing", r.spacing},
                  {"radius", r.radius},
                  {"minOpacity", r.minOpacity},
                  {"durationMs", r.durationMs},
                  {"staggerMs", r.staggerMs},
                  {"bars", r.bars}});
}

QVariantMap toVariantMap(const EmptyStateRecipe& r) {
    return mapOf({{"icon", r.icon},
                  {"title", r.title},
                  {"message", r.message},
                  {"iconSize", r.iconSize},
                  {"spacing", r.spacing},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"messagePixels", r.messagePixels},
                  {"messageMaxWidth", r.messageMaxWidth},
                  {"lineHeight", r.lineHeight}});
}

} // namespace ket
