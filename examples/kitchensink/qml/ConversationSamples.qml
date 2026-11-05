// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Conversation blocks; `mode` is "rows", "composer" or "transcript" and is set before
// loading. ConversationPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "rows"
    // The mock's tag palette (conversation-tags.css).
    readonly property var tagColors: KetTheme.dark
        ? ["#7aa7ff", "#5fcf95", "#b39dff", "#f0b45a"]
        : ["#2f6fde", "#23875a", "#7a55d6", "#b7791f"]

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "composer" ? composer
                       : page.mode === "transcript" ? transcript : rows
    }

    component Pane: Rectangle {
        property string surface: "panelBg"
        color: KetTheme.t[surface]
        border.width: 1
        border.color: KetTheme.t.panelBorder
    }

    component SampleComposer: KetComposer {
        width: KetTheme.px(620, KetTheme.zoom)
        leadingTools: [
            KetIconButton { text: "Add images or text files"; iconName: "plus" },
            KetButton {
                text: "Ask before edits"; iconName: "shield"
                variant: KetTheme.ButtonVariant.Ghost; controlSize: KetTheme.ControlSize.Small
            }
        ]
        trailingTools: [
            KetButton {
                text: "Claude · Opus"
                variant: KetTheme.ButtonVariant.Ghost; controlSize: KetTheme.ControlSize.Small
            }
        ]
    }

    Component {
        id: rows
        Pane {
            surface: "sidebarBg"
            implicitWidth: KetTheme.px(300, KetTheme.zoom)
            implicitHeight: KetTheme.px(340, KetTheme.zoom)

            Basic.ButtonGroup { id: group }

            Column {
                anchors.fill: parent
                anchors.margins: KetTheme.px(10, KetTheme.zoom)
                spacing: KetTheme.px(2, KetTheme.zoom)

                KetConversationRow {
                    width: parent.width; text: "Navigation rail polish"; number: "4821"
                    status: KetTheme.ConversationStatus.Working
                    tags: [{ label: "ui", color: page.tagColors[0] }]
                    Basic.ButtonGroup.group: group
                }
                KetConversationRow {
                    width: parent.width; text: "Startup time regression"; number: "1307"
                    status: KetTheme.ConversationStatus.Completed; checked: true
                    tags: [{ label: "perf", color: page.tagColors[3] },
                           { label: "review", color: page.tagColors[1] },
                           { label: "account", color: page.tagColors[2] },
                           { label: "ui", color: page.tagColors[0] }]
                    Basic.ButtonGroup.group: group
                }
                KetConversationRow {
                    width: parent.width; text: "Atlas bundle validation"; number: "5590"
                    status: KetTheme.ConversationStatus.NeedsInput
                    Basic.ButtonGroup.group: group
                }
                KetConversationRow {
                    width: parent.width; text: "Sync conflict on resume"; number: "7142"
                    status: KetTheme.ConversationStatus.Error
                    Basic.ButtonGroup.group: group
                }
                KetConversationRow {
                    width: parent.width; text: "Release checklist"; number: "2208"
                    status: KetTheme.ConversationStatus.Stopped
                    previewState: ({ hovered: true }); Basic.ButtonGroup.group: group
                }
                KetConversationRow {
                    width: parent.width; text: "Draft a new conversation"; number: "9031"
                    status: KetTheme.ConversationStatus.NotStarted
                    Basic.ButtonGroup.group: group
                }
            }
        }
    }

    Component {
        id: composer
        Column {
            spacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            SampleComposer {}
            SampleComposer { busy: true; text: "Also keep the +N chip when tags overflow." }
        }
    }

    Component {
        id: transcript
        Pane {
            implicitWidth: KetTheme.px(720, KetTheme.zoom)
            implicitHeight: column.implicitHeight + KetTheme.px(32, KetTheme.zoom)

            Column {
                id: column
                x: KetTheme.px(20, KetTheme.zoom)
                y: KetTheme.px(16, KetTheme.zoom)
                width: parent.width - 2 * x

                KetDayDivider { width: parent.width; text: "Today" }
                KetUserMessage {
                    width: parent.width
                    text: "Make the sidebar rows two lines: the title, then the number and the "
                        + "tags. Hide whole tags that do not fit and count them in +N."
                }
                KetAssistantMessage {
                    width: parent.width
                    name: "Claude"
                    time: "10:42"

                    KetActivityRow {
                        Layout.fillWidth: true; text: "Read 3 files"; iconName: "file"
                        detail: "ConversationRow.qml, conversation-tags.css"
                    }
                    KetActivityRow {
                        Layout.fillWidth: true; text: "Edited ConversationRow.qml"; iconName: "edit"
                        expandable: true; previewState: ({ hovered: true })
                    }
                    KetActivityRow {
                        Layout.fillWidth: true; text: "Ran ctest"; iconName: "terminal"
                        detail: "7 passed"
                    }
                    Item { implicitHeight: KetTheme.px(8, KetTheme.zoom) }
                    KetReadingText {
                        Layout.fillWidth: true
                        text: "Rows now show the number and as many whole tags as fit, with +N "
                            + "for the rest. Without tags the meta line shows the status."
                    }
                }
            }
        }
    }
}
