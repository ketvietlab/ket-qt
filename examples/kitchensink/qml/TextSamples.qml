// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// Multi-line text; `mode` is "states", "live" or "expandable" and is set before loading.
// TextPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "states"
    readonly property string placeholder: "Add context, links or @agent/ mentions"
    readonly property var states: [
        { label: "Rest", state: {} },
        { label: "Hover", state: { hovered: true } },
        { label: "Focus", state: { focused: true } },
        { label: "Disabled", state: { enabled: false } },
        { label: "Invalid", state: {}, invalid: true }
    ]
    readonly property var conversations: [
        { number: "2035", title: "Refine navigation", place: "desktop" },
        { number: "9410", title: "Review the startup flow", place: "develop" },
        { number: "4821", title: "Prepare the release", place: "desktop" }
    ]
    readonly property string longText: "Ship the new navigation and the repository picker.\n"
        + "@agent/2035 owns the sidebar; ask @agent/9410 about the startup flow.\n"
        + "Settings keep their sections, with zoom and text size applied at once.\n"
        + "The release notes stay in English.\n"
        + "Every change lands behind review, and the leader merges."

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "live" ? live
                       : page.mode === "expandable" ? expandable : statesMatrix
    }

    Component {
        id: statesMatrix
        GridLayout {
            columns: page.states.length
            columnSpacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            rowSpacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
            Repeater { model: page.states; Caption { text: modelData.label } }
            Repeater {
                model: page.states
                KetTextArea {
                    required property var modelData
                    required property int index
                    Layout.preferredWidth: KetTheme.px(KetTheme.t.space16 * 3.5, KetTheme.zoom)
                    Layout.alignment: Qt.AlignTop
                    placeholderText: page.placeholder
                    previewState: modelData.state
                    enabled: modelData.state.enabled !== false
                    invalid: !!modelData.invalid
                    text: index === 1 ? "Line breaks\nare kept" : ""
                }
            }
        }
    }

    Component {
        id: live
        Column {
            width: KetTheme.px(KetTheme.t.space16 * 9, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            KetTextArea {
                id: context
                width: parent.width
                Accessible.name: "Context"
                placeholderText: page.placeholder
                maxLength: 160
                text: "Ask @agent/2035 how the sidebar groups conversations.\n"
                      + "Keep the release notes short; @agent/9999 is unknown."
                unknownMentions: ["9999"]
                Component.onCompleted: mentions.setTitle("2035", "Refine navigation")
                onMentionQuery: query => mentionItems = page.conversations.filter(
                    item => item.number.startsWith(query)
                            || item.title.toLowerCase().indexOf(query) >= 0)
            }
            KetTextArea {
                width: parent.width
                Accessible.name: "Summary"
                minimumLines: 2
                maxLength: 40
                text: "A summary that runs past its limit of forty characters."
            }
        }
    }

    Component {
        id: expandable
        Column {
            width: KetTheme.px(KetTheme.t.space16 * 9, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            Repeater {
                model: [false, true]
                KetExpandableText {
                    required property bool modelData
                    width: parent.width
                    text: page.longText
                    mentionTitles: ({ "2035": "Refine navigation" })
                    unknownMentions: ["9410"]
                    expanded: modelData
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
