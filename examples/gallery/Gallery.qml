// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// The Qt Quick half of the gallery; main.cpp draws the same components with Widgets.
Rectangle {
    color: KetTheme.t.pageBg
    implicitWidth: 520
    implicitHeight: 140

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: KetTheme.px(16, KetTheme.zoom)
        spacing: KetTheme.px(8, KetTheme.zoom)

        Text {
            text: "Qt Quick"
            color: KetTheme.t.textSecondary
            font.family: KetTheme.fontFamily
            font.pixelSize: KetTheme.px(KetTheme.t.textSm, KetTheme.zoom)
        }
        RowLayout {
            spacing: KetTheme.px(8, KetTheme.zoom)
            KetButton { text: "Primary"; variant: KetTheme.ButtonVariant.Primary }
            KetButton { text: "Secondary" }
            KetButton { text: "Ghost"; variant: KetTheme.ButtonVariant.Ghost }
            KetButton { text: "Delete"; variant: KetTheme.ButtonVariant.Danger }
            KetButton { text: "Disabled"; enabled: false }
        }
        RowLayout {
            spacing: KetTheme.px(6, KetTheme.zoom)
            KetBadge { text: "Neutral" }
            KetBadge { text: "Accent"; tone: KetTheme.BadgeTone.Accent }
            KetBadge { text: "Info"; tone: KetTheme.BadgeTone.Info }
            KetBadge { text: "Success"; tone: KetTheme.BadgeTone.Success }
            KetBadge { text: "Warning"; tone: KetTheme.BadgeTone.Warning }
            KetBadge { text: "Danger"; tone: KetTheme.BadgeTone.Danger }
        }
    }
}
