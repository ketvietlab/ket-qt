// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QString>
#include <QStringList>

namespace ket {

// How a product composes the design system: the metrics a recipe takes from the product's
// own design (its mock) rather than from a token. Colours always come from tokens; a
// profile holds sizes, spacing, weights, opacities and the preferred code font only.
//
// The kit ships two profiles. designSystem() uses only Két DS tokens. ketPlus() follows the
// KetPlus desktop mock (`ketplus.ketatlas`) and is the default. Another product, such as
// Lampion OS, builds its own Profile (starting from either) and installs it with
// setCurrent() once, before it creates any window: recipes read the current profile, and
// switching it at run time does not repaint existing controls.
struct Profile {
    QString name;

    // The code font, in order of preference (diffs, code blocks, inline code, paths).
    // monoFamily() picks the first installed one. The kit bundles no fonts: an app that may
    // ship one registers it with QFontDatabase before it creates any window.
    QStringList monoFamilies;
    [[nodiscard]] QString monoFamily() const; // empty when none is installed

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
    qreal menuSectionHeight = 0; // a group heading row
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
    int conversationMetaWeight = 400;
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
    qreal composerSendIconSize = 0; // the arrow in the action circle
    qreal composerStopGlyph = 0;    // the stop square's side
    qreal composerStopGlyphRadius = 0;
    qreal composerAttachIconSize = 0; // the leading plus, larger than toolbar icons
    qreal composerToolbarHeight = 0;
    qreal composerToolbarGap = 0;

    // Transcript.
    qreal readingPixels = 0;
    qreal readingLineHeight = 1.0;
    qreal userBubbleRadius = 0;
    qreal userBubblePaddingInline = 0;
    qreal userBubblePaddingBlock = 0;
    qreal userBubbleMaxWidth = 1.0; // fraction of the transcript width
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

    // Chat extras: popover, meters, attachment chips, suggestions, question cards,
    // activity groups and galleries.
    qreal popoverWidth = 0;
    qreal popoverPadding = 0;
    qreal popoverRadius = 0;
    qreal popoverOffset = 0;
    qreal popoverTitlePixels = 0;
    int popoverTitleWeight = 0;
    qreal popoverHeaderGap = 0;
    qreal popoverBodyPixels = 0;
    qreal popoverNotePixels = 0;
    qreal popoverNoteLineHeight = 0;
    qreal popoverDividerSpacing = 0;
    qreal popoverFooterGap = 0;
    qreal meterLabelPixels = 0;
    qreal meterCompactLabelPixels = 0;
    int meterValueWeight = 0;
    int meterCompactValueWeight = 0;
    qreal meterGap = 0;
    qreal meterTrackHeight = 0;
    qreal meterCompactTrackHeight = 0;
    qreal meterTrackSpacingTop = 0;
    qreal meterTrackSpacingBottom = 0;
    qreal meterCompactTrackSpacingTop = 0;
    qreal meterCompactTrackSpacingBottom = 0;
    qreal meterCaptionPixels = 0;
    qreal meterCompactCaptionPixels = 0;
    qreal meterCaptionLineHeight = 0;
    qreal chipHeight = 0;
    qreal chipPaddingInline = 0;
    qreal chipRadius = 0;
    qreal chipGap = 0;
    qreal chipIconSize = 0;
    qreal chipPixels = 0;
    qreal chipMaxWidth = 0;
    qreal chipRowGap = 0;
    qreal chipRowPaddingInline = 0;
    qreal chipRowPaddingTop = 0;
    qreal suggestionPaddingBlock = 0;
    qreal suggestionPaddingInline = 0;
    qreal suggestionRadius = 0;
    qreal suggestionGap = 0;
    qreal suggestionArrowSize = 0;
    qreal suggestionTitlePixels = 0;
    int suggestionTitleWeight = 0;
    qreal suggestionCaptionPixels = 0;
    qreal suggestionCaptionGap = 0;
    qreal questionPadding = 0;
    qreal questionRadius = 0;
    qreal questionGap = 0;
    qreal questionLabelPixels = 0;
    int questionLabelWeight = 0;
    qreal questionHeadingPixels = 0;
    int questionHeadingWeight = 0;
    qreal questionHeadingSpacing = 0;
    qreal questionControlGap = 0;
    qreal activityGroupHeight = 0;
    qreal activityGroupGap = 0;
    qreal activityGroupChevronSize = 0;
    qreal activityChildrenIndent = 0;
    qreal activityChildrenPadding = 0;
    qreal activityChildrenGap = 0;
    qreal activityChildrenSpacingTop = 0;
    qreal activityChildrenSpacingBottom = 0;
    qreal galleryTileSize = 0;
    qreal galleryGap = 0;
    qreal galleryPaddingInline = 0;
    qreal galleryPaddingTop = 0;
    qreal galleryPaddingBottom = 0;
    qreal galleryMoreRadius = 0;
    qreal galleryRingWidth = 0;
    qreal galleryCaptionPixels = 0;
    qreal galleryCaptionGap = 0;

    // Controls and chrome: slider, avatar, tree items, group headers, route tabs,
    // status bar, breadcrumb and pane header.
    qreal sliderHeight = 0;
    qreal sliderTrackHeight = 0;
    qreal sliderHandleSize = 0;
    qreal sliderHandleBorder = 0;
    qreal sliderLabelPixels = 0;
    int sliderLabelWeight = 0;
    qreal sliderRangePixels = 0;
    qreal sliderHeaderGap = 0;
    qreal sliderLabelsGap = 0;
    qreal avatarDiameter = 0;
    qreal avatarPixels = 0;
    int avatarWeight = 0;
    qreal treeItemPaddingBlock = 0;
    qreal treeItemPaddingInline = 0;
    qreal treeItemGap = 0;
    qreal treeItemRadius = 0;
    qreal treeItemIconOffset = 0;
    qreal treeItemTitlePixels = 0;
    int treeItemTitleWeight = 0;
    qreal treeItemDetailPixels = 0;
    qreal treeItemActivityPixels = 0;
    qreal treeItemActivityIconSize = 0;
    qreal treeItemActivityGap = 0;
    qreal treeItemActivityOffset = 0;
    qreal groupHeaderHeight = 0;
    qreal groupHeaderPaddingStart = 0;
    qreal groupHeaderPaddingEnd = 0;
    qreal groupHeaderGap = 0;
    qreal groupHeaderNameGap = 0;
    qreal groupHeaderIconSize = 0;
    qreal groupHeaderPixels = 0;
    int groupHeaderWeight = 0;
    qreal groupHeaderActionSize = 0;
    qreal routeTabPixels = 0;
    qreal routeTabPaddingTop = 0;
    qreal routeTabPaddingBottom = 0;
    qreal routeTabIndicator = 0;
    qreal routeTabGap = 0;
    qreal statusBarHeight = 0;
    qreal statusBarPaddingInline = 0;
    qreal statusBarPixels = 0;
    qreal statusBarIconSize = 0;
    qreal statusBarItemGap = 0;
    qreal statusBarGroupGap = 0;
    qreal breadcrumbHeight = 0;
    qreal breadcrumbPaddingInline = 0;
    qreal breadcrumbGap = 0;
    qreal breadcrumbPixels = 0;
    qreal breadcrumbIconSize = 0;
    qreal paneHeaderHeight = 0;
    qreal paneHeaderPaddingStart = 0;
    qreal paneHeaderPaddingEnd = 0;
    qreal paneHeaderGap = 0;
    qreal paneHeaderPixels = 0;
    qreal paneHeaderIconSize = 0;

    // Surfaces: command palette, file rows, changed files, diffs, background tasks,
    // updates and theme choices.
    qreal paletteWidth = 0;
    qreal paletteTop = 0;
    qreal paletteMaxListHeight = 0;
    qreal paletteInputPixels = 0;
    qreal paletteSearchIconSize = 0;
    qreal paletteSearchPaddingBlock = 0;
    qreal paletteSearchPaddingInline = 0;
    qreal paletteSearchGap = 0;
    qreal paletteInputPaddingBlock = 0;
    qreal paletteGroupPixels = 0;
    int paletteGroupWeight = 0;
    qreal paletteGroupLetterSpacing = 0;
    qreal paletteGroupPaddingTop = 0;
    qreal paletteGroupPaddingInline = 0;
    qreal paletteGroupPaddingBottom = 0;
    qreal paletteListPaddingInline = 0;
    qreal paletteListPaddingBottom = 0;
    qreal paletteFooterPaddingBlock = 0;
    qreal paletteFooterPaddingInline = 0;
    qreal paletteFooterGap = 0;
    qreal paletteFooterPixels = 0;
    qreal paletteEmptyIconSize = 0;
    qreal paletteEmptyHeight = 0;
    qreal paletteResultPaddingBlock = 0;
    qreal paletteResultPaddingInline = 0;
    qreal paletteResultRadius = 0;
    qreal paletteTrailingSize = 0;
    qreal paletteResultPixels = 0;
    int paletteResultWeight = 0;
    qreal paletteDetailGap = 0;
    qreal fileRowHeight = 0;
    qreal folderRowHeight = 0;
    qreal fileRowPaddingInline = 0;
    qreal fileRowGap = 0;
    qreal folderRowGap = 0;
    qreal fileRowIconSize = 0;
    qreal fileRowChevronSize = 0;
    qreal fileRowPixels = 0;
    qreal fileRowStatusPixels = 0;
    qreal fileRowIndentStart = 0;
    qreal fileRowIndentStep = 0;
    qreal changedRadius = 0;
    qreal changedPaddingBlock = 0;
    qreal changedPaddingInline = 0;
    qreal changedHeadingGap = 0;
    qreal changedRowPaddingBlock = 0;
    qreal changedRowGap = 0;
    qreal changedFooterPaddingBlock = 0;
    qreal changedArrowSize = 0;
    qreal changedTextPixels = 0;
    qreal changedTitlePixels = 0;
    int changedTitleWeight = 0;
    int changedNameWeight = 0;
    qreal changedPathPixels = 0;
    qreal changedLineHeight = 0;
    qreal diffLineHeight = 0;
    qreal diffNumberWidth = 0;
    qreal diffNumberGap = 0;
    qreal diffSignWidth = 0;
    qreal diffCodeGap = 0;
    qreal diffHunkPaddingBlock = 0;
    qreal diffHunkPaddingInline = 0;
    qreal diffPaddingBlock = 0;
    qreal diffPaddingEnd = 0;
    qreal taskRowHeight = 0;
    qreal taskRowPaddingStart = 0;
    qreal taskRowPaddingEnd = 0;
    qreal taskRowGap = 0;
    qreal taskRowRadius = 0;
    qreal taskRowIconSize = 0;
    qreal taskRowPixels = 0;
    qreal taskMetaPixels = 0;
    qreal taskBarHeight = 0;
    qreal taskBarPaddingInline = 0;
    qreal taskBarRadius = 0;
    qreal taskBarIconSize = 0;
    qreal taskBarChevronSize = 0;
    qreal taskBarGap = 0;
    qreal taskBarRowGap = 0;
    qreal updatePaddingBlock = 0;
    qreal updatePaddingInline = 0;
    qreal updateTitlePixels = 0;
    int updateTitleWeight = 0;
    qreal updateHeaderGap = 0;
    qreal updateHeaderColumnGap = 0;
    qreal updateSectionGap = 0;
    qreal updateNotesLineHeight = 0;
    qreal updateNotesIndent = 0;
    qreal updateActionsGap = 0;
    qreal updateActionsTop = 0;
    qreal updatePillPaddingBlock = 0;
    qreal updatePillPaddingInline = 0;
    qreal updatePillRadius = 0;
    qreal updatePillGap = 0;
    qreal updatePillIconSize = 0;
    qreal themeMiniHeight = 0;
    qreal themeMiniMargin = 0;
    qreal themeMiniSideWidth = 0;
    qreal themeLabelPaddingBlock = 0;
    qreal themeLabelPaddingInline = 0;
    qreal accentSwatchRadius = 0;
    qreal accentSwatchBorder = 0;

    [[nodiscard]] static Profile designSystem();
    [[nodiscard]] static Profile ketPlus();

    [[nodiscard]] static const Profile& current();
    static void setCurrent(const Profile& profile);
};

} // namespace ket
