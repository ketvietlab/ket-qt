// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// KetIcon in every box size and weight, or (`all`) every Két code at 17 px;
// ComponentPages.cpp draws the Widgets twin.
Item {
    id: gallery

    property bool all: false
    readonly property var sizes: [14, 16, 17, 20, 24]
    readonly property var weights: [
        { label: "Light", value: KetTheme.IconWeight.Light },
        { label: "Regular", value: KetTheme.IconWeight.Regular }
    ]

    implicitWidth: all ? everyIcon.implicitWidth : sizeGrid.implicitWidth
    implicitHeight: all ? everyIcon.implicitHeight : sizeGrid.implicitHeight

    GridLayout {
        id: sizeGrid
        visible: !gallery.all
        columns: gallery.sizes.length + 1
        rowSpacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
        columnSpacing: KetTheme.px(KetTheme.t.space6, KetTheme.zoom)

        Item { implicitWidth: 1; implicitHeight: 1 }
        Repeater {
            model: gallery.sizes
            Caption { text: modelData + " px" }
        }
        Repeater {
            model: gallery.weights
            delegate: Caption {
                required property var modelData
                required property int index
                text: modelData.label
                Layout.row: index + 1
                Layout.column: 0
            }
        }
        Repeater {
            model: gallery.weights.length * gallery.sizes.length
            delegate: KetIcon {
                required property int index
                Layout.row: Math.floor(index / gallery.sizes.length) + 1
                Layout.column: index % gallery.sizes.length + 1
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                name: "settings"
                size: gallery.sizes[index % gallery.sizes.length]
                weight: gallery.weights[Math.floor(index / gallery.sizes.length)].value
            }
        }
    }

    GridLayout {
        id: everyIcon
        visible: gallery.all
        columns: 6
        rowSpacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
        columnSpacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

        Repeater {
            model: gallery.all ? KetTheme.iconCodes : []
            delegate: Row {
                required property string modelData
                spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)

                KetIcon { name: modelData; size: 17 }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                    color: KetTheme.t.textSecondary
                    font.family: KetTheme.fontFamily
                    font.pixelSize: KetTheme.px(KetTheme.t.textXs, KetTheme.zoom)
                }
            }
        }
    }

    component Caption: Text {
        color: KetTheme.t.textMuted
        font.family: KetTheme.fontFamily
        font.pixelSize: KetTheme.px(KetTheme.t.textXs, KetTheme.zoom)
        font.weight: 500
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    }
}
