// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// KetButton in every variant and pinned state; ComponentPages.cpp draws the Widgets twin.
GridLayout {
    id: matrix

    property bool small: false
    readonly property var states: [
        { label: "Rest", state: {} },
        { label: "Hover", state: { hovered: true } },
        { label: "Pressed", state: { hovered: true, pressed: true } },
        { label: "Focus", state: { focused: true } },
        { label: "Disabled", state: { enabled: false } }
    ]
    readonly property var variants: [
        { label: "Primary", value: KetTheme.ButtonVariant.Primary },
        { label: "Secondary", value: KetTheme.ButtonVariant.Secondary },
        { label: "Ghost", value: KetTheme.ButtonVariant.Ghost },
        { label: "Danger", value: KetTheme.ButtonVariant.Danger },
        { label: "Ghost danger", value: KetTheme.ButtonVariant.GhostDanger }
    ]

    columns: states.length + 1
    rowSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
    columnSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)

    Item { implicitWidth: 1; implicitHeight: 1 }
    Repeater {
        model: matrix.states
        Caption { text: modelData.label }
    }

    Repeater {
        model: matrix.variants
        delegate: Caption {
            required property var modelData
            required property int index
            text: modelData.label
            Layout.row: index + 1
            Layout.column: 0
        }
    }

    Repeater {
        model: matrix.variants.length * matrix.states.length
        delegate: KetButton {
            required property int index
            readonly property var variantEntry: matrix.variants[Math.floor(index / matrix.states.length)]
            readonly property var stateEntry: matrix.states[index % matrix.states.length]
            Layout.row: Math.floor(index / matrix.states.length) + 1
            Layout.column: index % matrix.states.length + 1
            text: variantEntry.label
            variant: variantEntry.value
            controlSize: matrix.small ? KetTheme.ControlSize.Small : KetTheme.ControlSize.Medium
            previewState: stateEntry.state
            enabled: stateEntry.state.enabled !== false
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
