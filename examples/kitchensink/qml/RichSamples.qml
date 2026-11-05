// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Rich transcript blocks; `mode` is "answer", "mentions" or "usage" and is set before
// loading. RichPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "answer"
    readonly property var conversations: [
        { number: "4821", title: "Navigation rail polish", place: "ketplus · develop" },
        { number: "4310", title: "Startup time regression", place: "ketplus · perf/startup" },
        { number: "5590", title: "Atlas bundle validation", place: "ketplus-atlas · main" },
        { number: "7142", title: "Sync conflict on resume", place: "ketplus · fix/sync" }
    ]

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "mentions" ? mentions
                       : page.mode === "usage" ? usage : answer
    }

    Component {
        id: answer
        KetMarkdown {
            width: KetTheme.px(640, KetTheme.zoom)
            markdown: "### Plan\n\n"
                + "The sidebar now shows **two lines** per conversation: the title, then the "
                + "number and as many\nwhole tags as fit. See "
                + "[ADR 0028](https://ketplus.app/docs/adr-0028) for the reasoning.\n\n"
                + "- Hide tags that do not fit and count them in `+N`.\n"
                + "- Keep the status icon when a row has no tags.\n\n"
                + "```sh\ncmake --build build && ctest --test-dir build --output-on-failure\n```\n\n"
                + "> Hidden tags stay searchable from the sidebar filter."
        }
    }

    Component {
        id: mentions
        Item {
            implicitWidth: KetTheme.px(620, KetTheme.zoom)
            implicitHeight: sample.height + KetTheme.px(16, KetTheme.zoom) + field.height
                + KetTheme.px(240, KetTheme.zoom)

            // A static picker; the composer below opens a live one when you type @agent/.
            // Room for the shadow, as ket::MentionPicker::shadowMargins() leaves it.
            Item {
                id: sample
                readonly property real inset: picker.recipe.shadowBlur
                width: picker.width + 2 * inset
                height: picker.height + 2 * inset
                KetMentionPicker {
                    id: picker
                    x: sample.inset
                    y: sample.inset
                    width: Math.min(recipe.maxWidth, KetTheme.px(620, KetTheme.zoom))
                    items: page.conversations
                    highlightedIndex: 1
                    visible: true
                }
            }

            KetComposer {
                id: field
                anchors.bottom: parent.bottom
                width: parent.width
                placeholderText: "Type @agent/ to mention a conversation"
                onMentionQuery: query => {
                    mentionItems = page.conversations.filter(item =>
                        item.number.startsWith(query)
                        || item.title.toLowerCase().includes(query))
                }
            }
        }
    }

    Component {
        id: usage
        Row {
            spacing: KetTheme.px(KetTheme.t.space5, KetTheme.zoom)
            Repeater {
                model: [{ value: 0.12, hovered: false }, { value: 0.46, hovered: true },
                        { value: 0.84, hovered: false }, { value: 0.97, hovered: false }]
                Row {
                    required property var modelData
                    spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
                    KetUsageRing {
                        anchors.verticalCenter: parent.verticalCenter
                        value: modelData.value
                        previewHovered: modelData.hovered
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: Math.round(modelData.value * 100) + "%"
                            + (modelData.hovered ? " · hover" : "")
                        color: KetTheme.t.textMuted
                        font.pixelSize: KetTheme.px(KetTheme.t.textSm, KetTheme.zoom)
                    }
                }
            }
        }
    }
}
