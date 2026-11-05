// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

namespace ket {

// A horizontal slider (`input[type=range]` with `accent-color`), drawn by ket::Style for every
// horizontal QSlider: the filled track, a round handle, and the `.effort-picker` header and
// `.range-labels` for ket::StepSlider.
struct SliderRecipe {
    QColor track;
    QColor fill;
    QColor handle;
    QColor handleBorder;
    QColor focusRing;
    QColor label;
    QColor value;
    QColor rangeLabel;
    qreal height = 0;
    qreal trackHeight = 0;
    qreal handleSize = 0;
    qreal handleBorderWidth = 0;
    qreal labelPixels = 0;
    int labelWeight = 0;
    qreal rangePixels = 0;
    qreal headerGap = 0;
    qreal labelsGap = 0;
    qreal opacity = 0;
};
[[nodiscard]] SliderRecipe sliderRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const SliderRecipe& recipe);

// A round avatar (`.avatar`): initials or an image on the raised surface.
struct AvatarRecipe {
    QColor background;
    QColor border;
    QColor text;
    qreal size = 0;
    qreal fontPixels = 0;
    int fontWeight = 0;
};
[[nodiscard]] AvatarRecipe avatarRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const AvatarRecipe& recipe);

// A two-line sidebar item (`.tree-item`): icon, title, detail or activity line, and an optional
// count. Selected items take the accent.
struct TreeItemRecipe {
    QColor background;
    QColor title;
    QColor icon;
    QColor detail;
    QColor count;
    QColor working;
    QColor waiting;
    QColor focusRing;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal radius = 0;
    qreal iconSize = 0;
    qreal iconOffset = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal detailPixels = 0;
    qreal activityPixels = 0;
    qreal activityIconSize = 0;
    qreal activityGap = 0;
    qreal activityOffset = 0;
    qreal countPixels = 0;
    qreal opacity = 0;
};
[[nodiscard]] TreeItemRecipe treeItemRecipe(bool selected, const InteractionState& state,
                                            Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const TreeItemRecipe& recipe);

// A sidebar group header (`.repository-group-header`): a folder icon and the name that toggle
// the group, and an action button that shows on hover.
struct GroupHeaderRecipe {
    QColor text;
    QColor focusRing;
    qreal height = 0;
    qreal paddingStart = 0;
    qreal paddingEnd = 0;
    qreal spacing = 0;
    qreal nameGap = 0;
    qreal iconSize = 0;
    qreal fontPixels = 0;
    int fontWeight = 0;
    qreal actionSize = 0;
    qreal opacity = 0;
};
[[nodiscard]] GroupHeaderRecipe groupHeaderRecipe(bool selected, const InteractionState& state,
                                                  Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const GroupHeaderRecipe& recipe);

// Page-level tabs (`.route-tabs`): text tabs over a hairline; the active tab takes the accent
// and a 2 px underline.
struct RouteTabRecipe {
    QColor text;
    QColor indicator;
    QColor rule;
    QColor focusRing;
    qreal fontPixels = 0;
    qreal paddingTop = 0;
    qreal paddingBottom = 0;
    qreal indicatorHeight = 0;
    qreal spacing = 0;
    qreal opacity = 0;
};
[[nodiscard]] RouteTabRecipe routeTabRecipe(bool active, const InteractionState& state,
                                            Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const RouteTabRecipe& recipe);

// The window status bar (`.statusbar`): muted items with small icons, left and right.
struct StatusBarRecipe {
    QColor background;
    QColor border;
    QColor text;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal fontPixels = 0;
    qreal iconSize = 0;
    qreal itemGap = 0;
    qreal groupGap = 0;
};
[[nodiscard]] StatusBarRecipe statusBarRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const StatusBarRecipe& recipe);

// A file path bar (`.file-breadcrumb`): the folders, a chevron and the file, then the app's
// actions.
struct BreadcrumbRecipe {
    QColor text;
    QColor border;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal fontPixels = 0;
    qreal iconSize = 0;
};
[[nodiscard]] BreadcrumbRecipe breadcrumbRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const BreadcrumbRecipe& recipe);

// A side pane's header (`.context-tabs`): an icon and title, then the pane's actions.
struct PaneHeaderRecipe {
    QColor text;
    QColor icon;
    QColor border;
    qreal height = 0;
    qreal paddingStart = 0;
    qreal paddingEnd = 0;
    qreal spacing = 0;
    qreal fontPixels = 0;
    qreal iconSize = 0;
};
[[nodiscard]] PaneHeaderRecipe paneHeaderRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const PaneHeaderRecipe& recipe);

} // namespace ket
