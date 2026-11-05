// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Live KetIconButtons next to KetButtons with icons; hover them for tooltips.
Row {
    spacing: KetTheme.px(KetTheme.t.space1, KetTheme.zoom)

    KetIconButton { text: "New conversation"; icon.source: "qrc:/qt/qml/KetKitchenSink/icons/plus.svg" }
    KetIconButton { text: "More actions"; icon.source: "qrc:/qt/qml/KetKitchenSink/icons/more.svg" }
    KetIconButton { text: "Close"; icon.source: "qrc:/qt/qml/KetKitchenSink/icons/close.svg" }
    Item { width: KetTheme.px(KetTheme.t.space3, KetTheme.zoom); height: 1 }
    KetButton {
        text: "Save changes"
        variant: KetTheme.ButtonVariant.Primary
        icon.source: "qrc:/qt/qml/KetKitchenSink/icons/check.svg"
    }
    KetButton {
        text: "Tag this flow"
        variant: KetTheme.ButtonVariant.Ghost
        icon.source: "qrc:/qt/qml/KetKitchenSink/icons/plus.svg"
    }
}
