// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The image viewer; `mode` is "thumbs", "buttons", "viewer" or "live" and is set before
// loading. LightboxPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "thumbs"
    readonly property real gap: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
    readonly property int allTools: KetTheme.LightboxTool.ZoomOut | KetTheme.LightboxTool.ZoomLevel
        | KetTheme.LightboxTool.ZoomIn | KetTheme.LightboxTool.Rotate | KetTheme.LightboxTool.Copy
        | KetTheme.LightboxTool.Download | KetTheme.LightboxTool.Print
        | KetTheme.LightboxTool.Fullscreen
    readonly property var screenshots: ["sidebar", "editor", "settings", "dialog", "terminal"]
        .map(name => ({ source: "qrc:/kitchensink/images/screenshot-" + name + ".png",
                        caption: name + ".png" }))

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "buttons" ? buttons
                       : page.mode === "viewer" ? viewer
                       : page.mode === "live" ? live : thumbs
    }

    Component {
        id: thumbs

        Row {
            spacing: page.gap

            Repeater {
                model: [KetTheme.LightboxThumbSize.Small, KetTheme.LightboxThumbSize.Medium,
                        KetTheme.LightboxThumbSize.Large]

                KetLightboxThumb {
                    required property int modelData
                    thumbSize: modelData
                    source: page.screenshots[1].source
                    description: "editor.png"
                }
            }
            KetLightboxThumb {
                source: page.screenshots[1].source
                description: "editor.png"
                previewState: ({ focused: true })
            }
            KetLightboxThumb { text: "No images" }
        }
    }

    Component {
        id: buttons

        Rectangle {
            readonly property real padding: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            implicitWidth: row.implicitWidth + 2 * padding
            implicitHeight: row.implicitHeight + 2 * padding
            radius: KetTheme.px(KetTheme.t.radiusMd, KetTheme.zoom)
            color: KetTheme.lightboxRecipe(KetTheme.dark, KetTheme.zoom).backdrop

            Row {
                id: row
                anchors.centerIn: parent
                spacing: page.gap

                Repeater {
                    model: [{ label: "Default", state: ({}) },
                            { label: "Hover", state: ({ hovered: true }) },
                            { label: "Focus", state: ({ focused: true }) },
                            { label: "Disabled", state: ({ enabled: false }) }]

                    KetLightboxButton {
                        required property var modelData
                        code: "zoom-in"
                        label: modelData.label
                        previewState: modelData.state
                        enabled: modelData.state.enabled !== false
                    }
                }
                KetLightboxButton { code: "restore"; label: "Rotate"; flipped: true }
                KetLightboxButton { label: "Fit or actual size"; text: "100%" }
            }
        }
    }

    Component {
        id: viewer

        Item {
            id: stage
            implicitWidth: 720
            implicitHeight: 460
            clip: true

            KetLightbox {
                parent: stage
                modal: false
                closePolicy: Basic.Popup.NoAutoClose
                tools: page.allTools
                items: page.screenshots
                currentIndex: 1
                Component.onCompleted: open()
            }
        }
    }

    Component {
        id: live

        Row {
            spacing: page.gap

            Repeater {
                model: page.screenshots

                KetLightboxThumb {
                    required property var modelData
                    required property int index
                    source: modelData.source
                    description: modelData.caption
                    onClicked: {
                        viewerWindow.show()
                        liveViewer.openAt(index)
                    }
                }
            }

            // Its own window: the page's view is only as tall as the thumbnails.
            Window {
                id: viewerWindow
                width: 960
                height: 640
                title: "Image viewer"
                color: "transparent"

                KetLightbox {
                    id: liveViewer
                    tools: page.allTools
                    items: page.screenshots
                    onClosed: viewerWindow.close()
                    onToolTriggered: tool => {
                        if (tool === KetTheme.LightboxTool.Print && item)
                            KitchenSinkHost.printImage(item.source, rotationAngle, item.caption)
                    }
                }
            }
        }
    }
}
