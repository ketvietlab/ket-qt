// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QString>

namespace ket {

// How a product composes the design system: the metrics a recipe takes from the product's
// own design (its mock) rather than from a token. Colours always come from tokens; a
// profile holds sizes, spacing, weights and opacities only.
//
// The kit ships two profiles. designSystem() uses only Két DS tokens. ketPlus() follows the
// KetPlus desktop mock (`ketplus.ketatlas`) and is the default. Another product, such as
// Lampion OS, builds its own Profile (starting from either) and installs it with
// setCurrent() once, before it creates any window: recipes read the current profile, and
// switching it at run time does not repaint existing controls.
struct Profile {
    QString name;

    qreal disabledOpacity = 0;

    qreal controlRadius = 0; // buttons, icon buttons, text fields and selects

    // Buttons and icon buttons.
    qreal buttonGap = 0; // icon to label
    qreal iconSize = 0;  // icon box in buttons and inline icons
    qreal smallIconSize = 0;
    qreal iconButtonSize = 0;
    qreal buttonFontPixels = 0;
    qreal smallButtonFontPixels = 0;
    int buttonFontWeight = 400;

    // Text fields and selects.
    qreal fieldHeight = 0;
    qreal fieldPadding = 0; // inline, including the border
    qreal fieldIconSize = 0;
    qreal fieldIconGap = 0;
    qreal fieldFontPixels = 0;

    // Checkboxes, radios and choice cards.
    qreal toggleSize = 0;
    qreal choiceRadius = 0;
    qreal choicePadding = 0; // including the border
    int choiceTitleWeight = 400;

    // Field rows.
    qreal fieldLabelWidth = 0;
    qreal fieldRowGap = 0;
    qreal fieldLabelTop = 0;
    qreal fieldHelpGap = 0;

    // Tags.
    qreal tagHeight = 0;
    qreal tagRadius = 0;
    qreal tagMaxWidth = 0;
    qreal tagPadding = 0;
    qreal tagOverflowPadding = 0;
    qreal tagTint = 0;      // share of the tag colour in the chip background
    qreal tagHoverTint = 0; // the same for a hovered filter tag

    // Banners (inline notices).
    qreal bannerRadius = 0;
    qreal bannerPaddingInline = 0; // including the border
    qreal bannerPaddingBlock = 0;
    qreal bannerGap = 0;
    int bannerTitleWeight = 400;
    qreal bannerMessageGap = 0;

    // Toasts.
    qreal toastPaddingInline = 0;
    qreal toastPaddingBlock = 0;
    qreal toastGap = 0;
    int toastDurationMs = 0;

    // Dialogs.
    qreal dialogWidth = 0;
    qreal dialogRadius = 0;
    qreal dialogPaddingInline = 0;
    qreal dialogHeaderPaddingBlock = 0;
    qreal dialogFooterPaddingBlock = 0;
    qreal dialogTitlePixels = 0;
    qreal backdropOpacity = 0; // the modal backdrop over the window

    // Menus (context menus, select popups).
    qreal menuMinWidth = 0;
    qreal menuMaxWidth = 0;
    qreal menuPadding = 0;
    qreal menuRadius = 0;
    qreal menuItemHeight = 0;
    qreal menuItemRadius = 0;
    qreal menuItemPaddingStart = 0;
    qreal menuItemPaddingEnd = 0;
    qreal menuItemGap = 0;
    qreal menuItemFontPixels = 0;
    qreal menuCheckSize = 0;
    qreal menuDescriptionGap = 0;
    qreal menuDisabledOpacity = 0;

    // Tooltips.
    qreal tooltipPaddingInline = 0;
    qreal tooltipPaddingBlock = 0;
    qreal tooltipMaxWidth = 0;
    int tooltipDelayMs = 0;

    // Sidebar navigation rows and their trailing count.
    qreal navItemHeight = 0;
    qreal navItemPadding = 0;
    qreal navItemGap = 0;
    qreal navItemRadius = 0;
    qreal navItemFontPixels = 0;
    int navItemActiveWeight = 400;
    qreal navCountHeight = 0;
    qreal navCountPadding = 0;
    qreal navCountRadius = 0;
    qreal kbdFontPixels = 0;

    // Segmented control (pill tabs).
    qreal segmentedPadding = 0;
    qreal segmentedRadius = 0;
    qreal segmentRadius = 0;
    qreal segmentPaddingInline = 0;
    qreal segmentPaddingBlock = 0;
    qreal segmentFontPixels = 0;

    // Underlined tab strip (file tabs; every QTabBar).
    qreal tabHeight = 0;
    qreal tabPaddingInline = 0;
    qreal tabGap = 0;
    qreal tabIconSize = 0;
    qreal tabIndicator = 0;
    qreal tabFontPixels = 0;

    // Progress and activity.
    qreal progressHeight = 0;
    int progressBusyDurationMs = 0; // one sweep of an indeterminate bar
    qreal activityBarWidth = 0;
    qreal activityBarHeight = 0;
    qreal activityBarMinHeight = 0;
    qreal activityGap = 0;
    int activityDurationMs = 0;
    int activityStaggerMs = 0;
    qreal activityMinOpacity = 1.0;

    // Empty states.
    qreal emptyIconSize = 0;
    qreal emptyGap = 0;
    qreal emptyTitlePixels = 0;
    qreal emptyMessagePixels = 0;
    qreal emptyMessageMaxWidth = 0;
    qreal emptyLineHeight = 1.0;

    [[nodiscard]] static Profile designSystem();
    [[nodiscard]] static Profile ketPlus();

    [[nodiscard]] static const Profile& current();
    static void setCurrent(const Profile& profile);
};

} // namespace ket
