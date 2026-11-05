// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Chat extras; `mode` is "usage", "quota", "attachments", "suggestions", "question" or
// "activity" and is set before loading. ChatPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "usage"
    readonly property var screenshots: ["sidebar", "editor", "settings", "dialog", "terminal"]
        .map(name => ({ source: "qrc:/kitchensink/images/screenshot-" + name + ".png",
                        caption: name + ".png" }))

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: ({ usage: usage, quota: quota, attachments: attachments,
                            suggestions: suggestions, question: question,
                            activity: activity })[page.mode]
    }

    // The usage panel from the mock's K.usagePanel().
    component UsagePanel: KetPopover {
        title: "Context & limits"
        footer: [
            KetButton {
                text: "Compact session"; iconName: "layers"
                variant: KetTheme.ButtonVariant.Ghost
            }
        ]

        KetPopoverText { text: "Claude · Opus" }
        Item { width: 1; height: KetTheme.px(12, KetTheme.zoom) }
        KetMeter {
            width: parent.width
            label: "Current context window"; valueText: "24%"; fraction: 0.24
            caption: "32,768 of 136,533 tokens · sample provider snapshot"
        }
        KetPopoverDivider {}
        KetPopoverText { role: "heading"; text: "Account limits" }
        Item { width: 1; height: KetTheme.px(12, KetTheme.zoom) }
        KetMeter { width: parent.width; label: "5-hour usage"; valueText: "18%" }
        Item { width: 1; height: KetTheme.px(12, KetTheme.zoom) }
        KetMeter { width: parent.width; label: "Weekly usage"; valueText: "32%" }
        Item { width: 1; height: KetTheme.px(12, KetTheme.zoom) }
        KetPopoverText {
            text: "Account limits belong to the selected provider, not this conversation."
        }
    }

    Component {
        id: usage
        Item {
            implicitWidth: sample.width + KetTheme.px(32 + 220, KetTheme.zoom)
            implicitHeight: sample.height + 2 * sample.recipe.shadowBlur

            // A static panel, opened in place; the ring on the right opens a live one.
            Item {
                id: sampleHost
                x: sample.recipe.shadowBlur
                y: sample.recipe.shadowBlur
                UsagePanel {
                    id: sample
                    visible: true
                    closePolicy: Basic.Popup.NoAutoClose
                }
            }

            Column {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                spacing: KetTheme.px(4, KetTheme.zoom)

                Text {
                    text: "Click the ring"
                    color: KetTheme.t.textMuted
                    font.family: KetTheme.fontFamily
                    font.pixelSize: KetTheme.px(KetTheme.t.textSm, KetTheme.zoom)
                }
                KetUsageRing {
                    id: ring
                    anchors.right: parent.right
                    value: 0.24
                    onClicked: live.opened ? live.close() : live.openAbove(ring)
                }
            }

            UsagePanel { id: live }
        }
    }

    Component {
        id: quota
        Rectangle {
            implicitWidth: KetTheme.px(280, KetTheme.zoom)
            implicitHeight: quotaColumn.implicitHeight + KetTheme.px(24, KetTheme.zoom)
            color: KetTheme.t.sidebarBg

            Rectangle {
                anchors.right: parent.right
                width: 1; height: parent.height
                color: KetTheme.t.sidebarBorder
            }

            Column {
                id: quotaColumn
                x: KetTheme.px(19, KetTheme.zoom)
                y: KetTheme.px(12, KetTheme.zoom)
                width: parent.width - 2 * x
                spacing: KetTheme.px(16, KetTheme.zoom)

                Repeater {
                    model: [0.32, 0.86, 0.97]
                    KetMeter {
                        required property real modelData
                        width: parent.width
                        compact: true
                        label: "Weekly usage"
                        valueText: Math.round(modelData * 100) + "%"
                        fraction: modelData
                        caption: "Resets Monday 09:00"
                    }
                }
            }
        }
    }

    Component {
        id: attachments
        KetComposer {
            id: field
            property int next: 1
            width: KetTheme.px(620, KetTheme.zoom)
            attachments: [{ name: "design-notes.md" },
                          { name: "sidebar-two-lines.png", icon: "image" },
                          { name: "a-very-long-attachment-name-that-elides-in-the-middle.txt" }]
            leadingTools: [
                KetIconButton {
                    text: "Add images or text files"; iconName: "plus"
                    onClicked: field.attachments = field.attachments.concat(
                        [{ name: "notes-" + field.next++ + ".md" }])
                }
            ]
        }
    }

    Component {
        id: suggestions
        Column {
            width: KetTheme.px(490, KetTheme.zoom)
            spacing: KetTheme.px(8, KetTheme.zoom)
            KetSuggestion {
                width: parent.width; iconName: "branch"
                text: "Review the open pull request"; caption: "Summarise risky changes first"
            }
            KetSuggestion {
                width: parent.width; iconName: "search"
                text: "Find why startup is slow"; caption: "Profile the first 2 seconds"
                previewState: ({ hovered: true })
            }
            KetSuggestion {
                width: parent.width; iconName: "file"
                text: "Write release notes"; caption: "From the commits since the last tag"
            }
        }
    }

    Component {
        id: question
        KetQuestionCard {
            width: KetTheme.px(620, KetTheme.zoom)
            question: "How much detail should each status show?"
            step: "1 / 2"
            footer: [KetButton { text: "Next"; variant: KetTheme.ButtonVariant.Primary }]

            Basic.ButtonGroup { id: answers }
            KetChoice {
                width: parent.width; text: "Status and last update"; checked: true
                kind: KetTheme.ToggleKind.Radio; Basic.ButtonGroup.group: answers
            }
            KetChoice {
                width: parent.width; text: "Status only"
                kind: KetTheme.ToggleKind.Radio; Basic.ButtonGroup.group: answers
            }
            KetTextField { width: parent.width; placeholderText: "Or enter another answer…" }
        }
    }

    Component {
        id: activity
        Column {
            width: KetTheme.px(620, KetTheme.zoom)
            KetActivityGroup {
                width: parent.width
                text: "Explored 4 files, ran 2 commands"
                expanded: true
                KetActivityRow { width: parent.width; text: "Read ConversationRow.qml"; iconName: "file" }
                KetActivityRow {
                    width: parent.width; text: "Ran ctest"; iconName: "terminal"; detail: "8 passed"
                }
                KetActivityRow { width: parent.width; text: "Captured 5 screenshots"; iconName: "image" }
                KetGallery { width: parent.width; images: page.screenshots; maxVisible: 3 }
            }
            KetActivityGroup {
                width: parent.width
                text: "Edited 2 files"
                previewState: ({ hovered: true })
                KetActivityRow { width: parent.width; text: "Edited KetComposer.cpp"; iconName: "edit" }
            }
        }
    }
}
