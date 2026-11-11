// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// Form controls; `mode` is "fields", "toggles", "choices" or "rows" and is set before
// loading. FormPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "fields"
    readonly property var states: [
        { label: "Rest", state: {} },
        { label: "Hover", state: { hovered: true } },
        { label: "Focus", state: { focused: true } },
        { label: "Disabled", state: { enabled: false } }
    ]
    readonly property real controlWidth: KetTheme.px(KetTheme.t.space16 * 2.5, KetTheme.zoom)

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "toggles" ? toggles
                       : page.mode === "choices" ? choices
                       : page.mode === "rows" ? rows : fields
    }

    Component {
        id: fields
        GridLayout {
            columns: page.states.length + 2
            rowSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            columnSpacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

            Item { implicitWidth: 1; implicitHeight: 1 }
            Repeater { model: page.states; Caption { text: modelData.label } }
            Caption { text: "Invalid" }

            Caption { text: "Text field" }
            Repeater {
                model: page.states
                KetTextField {
                    Layout.preferredWidth: page.controlWidth
                    placeholderText: "Name"
                    previewState: modelData.state
                    enabled: modelData.state.enabled !== false
                }
            }
            KetTextField {
                Layout.preferredWidth: page.controlWidth
                placeholderText: "Name"
                invalid: true
                previewState: ({})
            }

            Caption { text: "Search" }
            Repeater {
                model: page.states
                KetTextField {
                    Layout.preferredWidth: page.controlWidth
                    placeholderText: "Search"
                    leadingIcon: "search"
                    previewState: modelData.state
                    enabled: modelData.state.enabled !== false
                }
            }
            KetTextField {
                Layout.preferredWidth: page.controlWidth
                placeholderText: "Search"
                leadingIcon: "search"
                invalid: true
                previewState: ({})
            }

            Caption { text: "Select" }
            Repeater {
                model: page.states
                KetSelect {
                    Layout.preferredWidth: page.controlWidth
                    model: ["KÉT Sans", "Menlo"]
                    previewState: modelData.state
                    enabled: modelData.state.enabled !== false
                }
            }
            Item { implicitWidth: 1; implicitHeight: 1 }
        }
    }

    Component {
        id: toggles
        GridLayout {
            columns: page.states.length + 1
            rowSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            columnSpacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

            Item { implicitWidth: 1; implicitHeight: 1 }
            Repeater { model: page.states; Caption { text: modelData.label } }

            Repeater {
                model: [
                    { label: "Checkbox · off", radio: false, checked: false },
                    { label: "Checkbox · on", radio: false, checked: true },
                    { label: "Radio · off", radio: true, checked: false },
                    { label: "Radio · on", radio: true, checked: true }
                ]
                delegate: Caption {
                    required property var modelData
                    required property int index
                    text: modelData.label
                    Layout.row: index + 1
                    Layout.column: 0
                }
            }
            Repeater {
                model: 4 * page.states.length
                delegate: Loader {
                    required property int index
                    readonly property int rowIndex: Math.floor(index / page.states.length)
                    readonly property var stateEntry: page.states[index % page.states.length].state
                    Layout.row: rowIndex + 1
                    Layout.column: index % page.states.length + 1
                    sourceComponent: rowIndex >= 2 ? radio : check
                    Component {
                        id: check
                        KetCheckbox {
                            text: "Label"
                            checked: rowIndex % 2 === 1
                            previewState: stateEntry
                            enabled: stateEntry.enabled !== false
                        }
                    }
                    Component {
                        id: radio
                        KetRadio {
                            text: "Label"
                            autoExclusive: false
                            checked: rowIndex % 2 === 1
                            previewState: stateEntry
                            enabled: stateEntry.enabled !== false
                        }
                    }
                }
            }
        }
    }

    Component {
        id: choices
        Column {
            width: KetTheme.px(KetTheme.t.space16 * 7, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)

            KetCheckbox {
                width: parent.width
                wordWrap: true
                text: "A checkbox whose long label wraps under itself in a narrow column"
            }
            KetChoice {
                width: parent.width
                text: "Check for updates automatically"
                description: "Once a day while KetPlus is open."
                checked: true
            }
            KetChoice {
                width: parent.width
                text: "Download updates in the background"
                description: "You still choose when KetPlus restarts."
            }
            Repeater {
                model: [
                    { title: "Read only", text: "The agent can read files and run safe commands." },
                    { title: "Workspace write", text: "Edits inside this worktree; asks before anything else." },
                    { title: "Full access", text: "No sandbox. Use only on a machine you trust." }
                ]
                KetChoice {
                    width: parent.width
                    kind: KetTheme.ToggleKind.Radio
                    text: modelData.title
                    description: modelData.text
                    checked: modelData.title === "Workspace write"
                }
            }
        }
    }

    Component {
        id: rows
        Column {
            width: KetTheme.px(KetTheme.t.space16 * 9, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space5, KetTheme.zoom)

            KetFieldRow {
                width: parent.width
                label: "Interface font"
                help: "KetPlus uses KÉT Sans throughout the interface."
                KetSelect { model: ["KÉT Sans"] }
            }
            KetFieldRow {
                width: parent.width
                label: "Conversation width"
                help: "Applies to Agent Workspace. Atlas keeps its compact reading column."
                KetSelect {
                    model: ["Default · 760 px", "Wide · 864 px", "Extra wide · 1200 px", "Full width"]
                }
            }
            KetFieldRow {
                width: parent.width
                label: "Worktree name"
                error: "Use letters, numbers and dashes only."
                KetTextField { text: "ketplus desktop"; invalid: true }
            }
            KetFieldRow {
                width: parent.width
                stacked: true
                label: "Integration branch (stacked)"
                help: "The label sits above the control in a narrow form."
                KetTextField { text: "integration" }
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
