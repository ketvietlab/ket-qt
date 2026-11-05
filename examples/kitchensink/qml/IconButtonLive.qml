// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Live KetIconButtons next to KetButtons with icons; hover them for tooltips.
Row {
    spacing: KetTheme.px(KetTheme.t.space1, KetTheme.zoom)

    KetIconButton { text: "New conversation"; iconName: "plus" }
    KetIconButton { text: "More actions"; iconName: "more" }
    KetIconButton { text: "Close"; iconName: "close" }
    Item { width: KetTheme.px(KetTheme.t.space3, KetTheme.zoom); height: 1 }
    KetButton {
        text: "Save changes"
        variant: KetTheme.ButtonVariant.Primary
        iconName: "check"
    }
    KetButton {
        text: "Tag this flow"
        variant: KetTheme.ButtonVariant.Ghost
        iconName: "plus"
    }
}
