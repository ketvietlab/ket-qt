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

    // Sidebar conversation rows (two lines: title, then number and tags or status).
    qreal conversationRowHeight = 0;
    qreal conversationRowPaddingInline = 0;
    qreal conversationRowPaddingBlock = 0;
    qreal conversationRowRadius = 0;
    qreal conversationRowGap = 0;
    qreal conversationLineGap = 0;
    qreal conversationTitlePixels = 0;
    qreal conversationTitleLineHeight = 0; // pixels
    int conversationTitleWeight = 400;
    int conversationSelectedTitleWeight = 400;
    qreal conversationNumberPixels = 0;
    qreal conversationMetaPixels = 0;
    qreal conversationMetaHeight = 0;
    qreal conversationMetaGap = 0;
    qreal conversationTagGap = 0;
    qreal conversationTagMaxWidth = 0;
    qreal conversationIndicatorSize = 0;
    qreal conversationIndicatorTop = 0;
    int conversationSpinMs = 0;

    // Composer.
    qreal composerRadius = 0;
    qreal composerPadding = 0;
    qreal composerRingWidth = 0;
    qreal composerTextPixels = 0;
    qreal composerLineHeight = 1.0;
    qreal composerTextPaddingInline = 0;
    qreal composerTextPaddingBlock = 0;
    qreal composerMinInputHeight = 0;
    int composerMaxLines = 0;
    qreal composerSendSize = 0;
    qreal composerStopIconSize = 0;
    qreal composerToolbarHeight = 0;
    qreal composerToolbarGap = 0;

    // Transcript.
    qreal readingPixels = 0;
    qreal readingLineHeight = 1.0;
    qreal userBubbleRadius = 0;
    qreal userBubblePaddingInline = 0;
    qreal userBubblePaddingBlock = 0;
    qreal userBubbleMaxWidth = 1.0; // fraction of the transcript width
    qreal userBubbleShade = 0;      // how much of the accent stays when mixed with black
    qreal messageSpacing = 0;
    qreal avatarSize = 0;
    qreal avatarRadius = 0;
    qreal assistantGap = 0;
    qreal messageNamePixels = 0;
    qreal messageTimePixels = 0;
    qreal messageMetaGap = 0;
    qreal messageMetaBottom = 0;
    qreal dayDividerPixels = 0;
    qreal dayDividerGap = 0;
    qreal dayDividerMarginTop = 0;
    qreal dayDividerMarginBottom = 0;
    qreal activityRowHeight = 0;
    qreal activityRowPaddingInline = 0;
    qreal activityRowGap = 0;
    qreal activityRowPixels = 0;
    qreal activityRowIconSize = 0;
    qreal activityRowChevronSize = 0;

    // Markdown in the transcript (`.answer`) and code blocks.
    qreal markdownHeading1Pixels = 0;
    qreal markdownHeading2Pixels = 0;
    qreal markdownHeading3Pixels = 0;
    qreal markdownHeadingLineHeight = 1.0;
    qreal markdownHeadingSpacingAfter = 0;
    qreal markdownParagraphSpacing = 0;
    qreal markdownListIndent = 0;
    qreal markdownListItemGap = 0;
    int markdownStrongWeight = 700;
    qreal markdownQuotePadding = 0;
    qreal markdownQuoteBorder = 0;
    qreal codePixels = 0;
    qreal codeLineHeight = 1.0;
    qreal codePadding = 0;
    qreal codeRadius = 0;
    qreal codeMargin = 0;

    // Mention picker (`.mention-menu`).
    qreal mentionMaxWidth = 0;
    qreal mentionPadding = 0; // including the border
    qreal mentionRadius = 0;
    qreal mentionOverlap = 0; // how far the menu overlaps the composer's top edge
    qreal mentionTitlePixels = 0;
    int mentionTitleWeight = 400;
    qreal mentionTitlePaddingInline = 0;
    qreal mentionTitlePaddingTop = 0;
    qreal mentionTitlePaddingBottom = 0;
    qreal mentionOptionHeight = 0;
    qreal mentionOptionPadding = 0;
    qreal mentionOptionRadius = 0;
    qreal mentionOptionGap = 0;
    qreal mentionOptionPixels = 0;
    qreal mentionPlacePixels = 0;
    int mentionMaxItems = 0;

    // Usage ring in the composer toolbar.
    qreal usageRingSize = 0;
    qreal usageRingStroke = 0;
    qreal usageWarningAt = 1.0;
    qreal usageDangerAt = 1.0;

    [[nodiscard]] static Profile designSystem();
    [[nodiscard]] static Profile ketPlus();

    [[nodiscard]] static const Profile& current();
    static void setCurrent(const Profile& profile);
};

} // namespace ket
