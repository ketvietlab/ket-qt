// SPDX-License-Identifier: MPL-2.0
#include "KetProfile.h"

#include "KetTokens.h"

namespace ket {
namespace {

qreal px(QStringView token) { return Tokens::px(token); }

Profile& storage() {
    static Profile profile = Profile::ketPlus();
    return profile;
}

} // namespace

Profile Profile::designSystem() {
    Profile p;
    p.name = QStringLiteral("design-system");
    p.disabledOpacity = Tokens::number(u"opacity-pending");
    p.controlRadius = px(u"control-radius");
    p.buttonGap = px(u"space-1-5");
    p.iconSize = 16;
    p.smallIconSize = 16;
    p.iconButtonSize = px(u"control-height-md");
    p.buttonFontPixels = px(u"action-font-size");
    p.smallButtonFontPixels = px(u"action-font-size");
    p.buttonFontWeight = qRound(Tokens::number(u"weight-medium"));
    p.fieldHeight = px(u"control-height-md");
    p.fieldPadding = px(u"control-padding-inline");
    p.fieldIconSize = 16;
    p.fieldIconGap = px(u"space-2");
    p.fieldFontPixels = px(u"input-font-size");
    p.toggleSize = 16;
    p.choiceRadius = px(u"radius-md");
    p.choicePadding = px(u"space-3");
    p.choiceTitleWeight = qRound(Tokens::number(u"weight-semibold"));
    p.fieldLabelWidth = px(u"space-16") * 3;
    p.fieldRowGap = px(u"space-4");
    p.fieldLabelTop = px(u"control-padding-block");
    p.fieldHelpGap = px(u"space-1-5");
    p.tagHeight = px(u"line-sm");
    p.tagRadius = px(u"radius-xs");
    p.tagMaxWidth = px(u"space-16") * 2;
    p.tagPadding = px(u"space-1-5");
    p.tagOverflowPadding = px(u"space-1-5");
    p.tagTint = 0.13;
    p.tagHoverTint = 0.22;
    p.bannerRadius = px(u"radius-md");
    p.bannerPaddingInline = px(u"space-4");
    p.bannerPaddingBlock = px(u"space-3");
    p.bannerGap = px(u"space-3");
    p.bannerTitleWeight = qRound(Tokens::number(u"weight-semibold"));
    p.bannerMessageGap = px(u"space-0-5");
    p.toastPaddingInline = px(u"space-4");
    p.toastPaddingBlock = px(u"space-3");
    p.toastGap = px(u"space-2");
    p.toastDurationMs = 4000;
    p.dialogWidth = px(u"space-16") * 8;
    p.dialogRadius = px(u"radius-xl");
    p.dialogPaddingInline = px(u"space-6");
    p.dialogHeaderPaddingBlock = px(u"space-4");
    p.dialogFooterPaddingBlock = px(u"space-3");
    p.dialogTitlePixels = px(u"text-lg");
    p.backdropOpacity = 0.24;
    p.menuMinWidth = px(u"space-16") * 3;
    p.menuMaxWidth = px(u"space-16") * 5;
    p.menuPadding = px(u"space-1");
    p.menuRadius = px(u"radius-md");
    p.menuItemHeight = px(u"control-height-md");
    p.menuItemRadius = px(u"radius-sm");
    p.menuItemPaddingStart = px(u"space-2");
    p.menuItemPaddingEnd = px(u"space-3");
    p.menuItemGap = px(u"space-2");
    p.menuItemFontPixels = px(u"text-sm");
    p.menuCheckSize = 16;
    p.menuDescriptionGap = px(u"space-0-5");
    p.menuDisabledOpacity = Tokens::number(u"opacity-pending");
    p.tooltipPaddingInline = px(u"space-2");
    p.tooltipPaddingBlock = px(u"space-1-5");
    p.tooltipMaxWidth = px(u"space-16") * 5;
    p.tooltipDelayMs = 600;
    p.navItemHeight = px(u"control-height-md");
    p.navItemPadding = px(u"space-2");
    p.navItemGap = px(u"space-2");
    p.navItemRadius = px(u"radius-sm");
    p.navItemFontPixels = px(u"text-sm");
    p.navItemActiveWeight = qRound(Tokens::number(u"weight-semibold"));
    p.navCountHeight = px(u"space-5");
    p.navCountPadding = px(u"space-1");
    p.navCountRadius = px(u"radius-sm");
    p.kbdFontPixels = px(u"text-xs");
    p.segmentedPadding = px(u"space-0-5");
    p.segmentedRadius = px(u"radius-md");
    p.segmentRadius = px(u"radius-sm");
    p.segmentPaddingInline = px(u"space-3");
    p.segmentPaddingBlock = px(u"space-1");
    p.segmentFontPixels = px(u"text-sm");
    p.tabHeight = px(u"space-10");
    p.tabPaddingInline = px(u"space-4");
    p.tabGap = px(u"space-2");
    p.tabIconSize = 16;
    p.tabIndicator = 2;
    p.tabFontPixels = px(u"text-sm");
    p.progressHeight = px(u"space-1-5");
    p.progressBusyDurationMs = 1500;
    p.activityBarWidth = 3;
    p.activityBarHeight = 10;
    p.activityBarMinHeight = 4;
    p.activityGap = 3;
    p.activityDurationMs = 1100;
    p.activityStaggerMs = 150;
    p.activityMinOpacity = 0.5;
    p.emptyIconSize = 32;
    p.emptyGap = px(u"space-3");
    p.emptyTitlePixels = px(u"text-xl");
    p.emptyMessagePixels = px(u"text-md");
    p.emptyMessageMaxWidth = px(u"space-16") * 6;
    p.emptyLineHeight = 1.6;
    return p;
}

// The KetPlus desktop mock's rules; literals without a token name their source rule.
Profile Profile::ketPlus() {
    Profile p;
    p.name = QStringLiteral("ketplus");
    p.disabledOpacity = 0.48; // `button:disabled`
    p.controlRadius = px(u"radius-sm");
    p.buttonGap = 7;       // `.btn` gap
    p.iconSize = 17;       // `.icon`; ket::Icons centres the 16 px master
    p.smallIconSize = 14;  // compact header icons; the 14 px master
    p.iconButtonSize = 30; // `.icon-btn`
    // `--kp-text-base` and `--kp-text-md` at the default 15 px interface size.
    p.buttonFontPixels = px(u"text-md");
    p.smallButtonFontPixels = px(u"text-sm");
    p.buttonFontWeight = qRound(Tokens::number(u"weight-normal"));
    p.fieldHeight = 34;   // `input, select` min-height
    p.fieldPadding = 11;  // 10 px padding plus the 1 px border
    p.fieldIconSize = 14; // `.search-field .icon`
    p.fieldIconGap = 9;   // `.search-field` gap
    p.fieldFontPixels = px(u"text-md");
    p.toggleSize = 16;
    p.choiceRadius = 6;        // `.choice`
    p.choicePadding = 12;      // 11 px padding plus the 1 px border
    p.choiceTitleWeight = 550; // `.choice b`
    p.fieldLabelWidth = 180;   // `.field-row` grid column
    p.fieldRowGap = 18;
    p.fieldLabelTop = 8;
    p.fieldHelpGap = 6; // `.error-text` margin-top
    p.tagHeight = 18;   // `.conversation-tag`
    p.tagRadius = 4;
    p.tagMaxWidth = 104;
    p.tagPadding = 6;
    p.tagOverflowPadding = 5; // `.tag-more`
    p.tagTint = 0.13;         // color-mix(tag 13%, transparent)
    p.tagHoverTint = 0.22;
    p.bannerRadius = 6;         // `.banner`
    p.bannerPaddingInline = 17; // 16 px padding plus the 1 px border
    p.bannerPaddingBlock = 14;  // 13 px
    p.bannerGap = 12;
    p.bannerTitleWeight = 550; // `.banner strong`
    p.bannerMessageGap = 3;    // `.banner p`
    p.toastPaddingInline = 18; // `.toast` 17 px plus the border
    p.toastPaddingBlock = 12;  // 11 px
    p.toastGap = 8;
    p.toastDurationMs = 3600;   // K.toast
    p.dialogWidth = 500;        // `.modal`
    p.dialogRadius = 12;        // desktop alignment `.modal`
    p.dialogPaddingInline = 22; // header, body and footer
    p.dialogHeaderPaddingBlock = 18;
    p.dialogFooterPaddingBlock = 14;
    p.dialogTitlePixels = 18; // 16 px at the 15 px interface scale
    p.backdropOpacity = 0.24; // `.modal-backdrop` rgb(18 22 30 / 24%)
    p.menuMinWidth = 210;     // `.desktop-context-menu`
    p.menuMaxWidth = 330;
    p.menuPadding = 6; // 5 px plus the border
    p.menuRadius = 7;
    p.menuItemHeight = 32;
    p.menuItemRadius = 4;
    p.menuItemPaddingStart = 5;
    p.menuItemPaddingEnd = 12;
    p.menuItemGap = 5;
    p.menuItemFontPixels = px(u"text-xs"); // 12 px
    p.menuCheckSize = 14;                  // `.context-check`
    p.menuDescriptionGap = 3;              // `.desktop-context-menu small`
    p.menuDisabledOpacity = 0.65;
    p.tooltipPaddingInline = 8; // the native KetToolTip
    p.tooltipPaddingBlock = 6;
    p.tooltipMaxWidth = 360;
    p.tooltipDelayMs = 600;
    p.navItemHeight = 34; // `.nav-item`
    p.navItemPadding = 9;
    p.navItemGap = 9;
    p.navItemRadius = 5;
    p.navItemFontPixels = px(u"text-md"); // kp-text-base, 14 px
    p.navItemActiveWeight = 550;          // `.nav-item.is-active`
    p.navCountHeight = 19;                // `.nav-count`: 17 px line plus the border
    p.navCountPadding = 6;                // 5 px plus the border
    p.navCountRadius = 4;
    p.kbdFontPixels = px(u"text-xs"); // `kbd` at kp-text-sm, 12 px
    p.segmentedPadding = 4;           // `.pill-tabs` 3 px plus the border
    p.segmentedRadius = 6;
    p.segmentRadius = 4;
    p.segmentPaddingInline = 12;
    p.segmentPaddingBlock = 5;
    p.segmentFontPixels = px(u"text-sm"); // kp-text-md, 13 px
    p.tabHeight = 41;                     // `.file-tabs`
    p.tabPaddingInline = 15;
    p.tabGap = 8;
    p.tabIconSize = 13;
    p.tabIndicator = 2;               // the active tab's bottom border
    p.tabFontPixels = px(u"text-xs"); // kp-text-sm, 12 px
    p.progressHeight = 6;             // `.modal progress`
    p.progressBusyDurationMs = 1500;
    p.activityBarWidth = 3; // `.working-bars i`
    p.activityBarHeight = 10;
    p.activityBarMinHeight = 4; // @keyframes working
    p.activityGap = 3;
    p.activityDurationMs = 1100;
    p.activityStaggerMs = 150; // nth-child delays
    p.activityMinOpacity = 0.5;
    p.emptyIconSize = 35; // `.empty-state > .icon`
    p.emptyGap = 14;
    p.emptyTitlePixels = 21;               // `h2`, 18 px at the 15 px interface scale
    p.emptyMessagePixels = px(u"text-md"); // kp-text-base
    p.emptyMessageMaxWidth = 390;
    p.emptyLineHeight = 1.8;
    return p;
}

const Profile& Profile::current() { return storage(); }

void Profile::setCurrent(const Profile& profile) { storage() = profile; }

} // namespace ket
