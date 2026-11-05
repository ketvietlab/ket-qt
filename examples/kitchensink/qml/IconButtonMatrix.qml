// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// KetIconButton in each size and pinned state; ComponentPages.cpp draws the Widgets twin.
Item {
    id: page

    readonly property var states: [
        { label: "Rest", state: {} },
        { label: "Hover", state: { hovered: true } },
        { label: "Pressed", state: { hovered: true, pressed: true } },
        { label: "Focus", state: { focused: true } },
        { label: "Disabled", state: { enabled: false } }
    ]
    readonly property var sizes: [
        { label: "Medium", value: KetTheme.ControlSize.Medium },
        { label: "Small", value: KetTheme.ControlSize.Small }
    ]

    implicitWidth: matrix.implicitWidth
    implicitHeight: matrix.implicitHeight

    GridLayout {
        id: matrix
        columns: page.states.length + 1
        rowSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
        columnSpacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

        Item { implicitWidth: 1; implicitHeight: 1 }
        Repeater {
            model: page.states
            Caption { text: modelData.label }
        }
        Repeater {
            model: page.sizes
            delegate: Caption {
                required property var modelData
                required property int index
                text: modelData.label
                Layout.row: index + 1
                Layout.column: 0
            }
        }
        Repeater {
            model: page.sizes.length * page.states.length
            delegate: KetIconButton {
                required property int index
                readonly property var stateEntry: page.states[index % page.states.length]
                Layout.row: Math.floor(index / page.states.length) + 1
                Layout.column: index % page.states.length + 1
                text: "Close"
                icon.source: "qrc:/qt/qml/KetKitchenSink/icons/close.svg"
                controlSize: page.sizes[Math.floor(index / page.states.length)].value
                previewState: stateEntry.state
                enabled: stateEntry.state.enabled !== false
                showToolTip: false
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
