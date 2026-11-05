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
    return p;
}

const Profile& Profile::current() { return storage(); }

void Profile::setCurrent(const Profile& profile) { storage() = profile; }

} // namespace ket
