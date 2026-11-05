// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Screenshot thumbnails in an activity (`.activity-gallery`), as ket::Gallery draws them.
// `images` is a list of { source, caption }. With more than `maxVisible` a `+N` tile shows
// the rest; clicking a tile emits activated(index).
Item {
    id: gallery

    property var images: []
    property int maxVisible: 0
    property bool expanded: false
    readonly property var recipe: KetTheme.galleryRecipe(KetTheme.dark, KetTheme.zoom, false)
    readonly property bool limited: !expanded && maxVisible > 0 && images.length > maxVisible
    readonly property var shown: limited ? images.slice(0, maxVisible) : images
    readonly property bool captions: images.some(image => !!image.caption)
    readonly property real tile: recipe.tileSize + 2 * recipe.ringWidth

    signal activated(int index)

    implicitWidth: 2 * recipe.paddingInline + (shown.length + (limited ? 1 : 0)) * (tile + recipe.spacing)
    implicitHeight: flow.implicitHeight + recipe.paddingTop + recipe.paddingBottom

    Flow {
        id: flow
        x: gallery.recipe.paddingInline - gallery.recipe.ringWidth
        y: gallery.recipe.paddingTop - gallery.recipe.ringWidth
        width: gallery.width - 2 * x
        spacing: gallery.recipe.spacing

        Repeater {
            model: gallery.shown

            Basic.AbstractButton {
                id: thumb
                required property var modelData
                required property int index
                readonly property var recipe: KetTheme.galleryRecipe(KetTheme.dark, KetTheme.zoom,
                                                                     hovered || visualFocus)
                width: gallery.tile
                height: gallery.tile + (gallery.captions ? captionText.implicitHeight
                                                           + recipe.captionGap : 0)
                hoverEnabled: true
                text: modelData.caption || ""
                Accessible.name: text !== "" ? text : qsTr("Screenshot")
                onClicked: gallery.activated(index)

                contentItem: Item {}
                background: Item {
                    Rectangle {
                        visible: thumb.recipe.ring.a > 0
                        width: gallery.tile
                        height: gallery.tile
                        radius: thumb.recipe.radius + thumb.recipe.ringWidth
                        color: thumb.recipe.ring
                    }

                    Rectangle {
                        id: frame
                        x: thumb.recipe.ringWidth
                        y: thumb.recipe.ringWidth
                        width: thumb.recipe.tileSize
                        height: thumb.recipe.tileSize
                        radius: thumb.recipe.radius
                        color: thumb.recipe.background
                        clip: true

                        Image {
                            anchors.fill: parent
                            source: thumb.modelData.source || ""
                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                        }
                    }

                    Rectangle {
                        anchors.fill: frame
                        radius: frame.radius
                        color: "transparent"
                        border.width: 1
                        border.color: thumb.recipe.border
                    }

                    Text {
                        id: captionText
                        visible: gallery.captions
                        x: thumb.recipe.ringWidth
                        y: gallery.tile + thumb.recipe.captionGap
                        width: thumb.recipe.tileSize
                        text: thumb.text
                        elide: Text.ElideRight
                        color: thumb.recipe.caption
                        font.family: KetTheme.fontFamily
                        font.pixelSize: thumb.recipe.captionPixels
                    }
                }
            }
        }

        Basic.AbstractButton {
            id: more
            visible: gallery.limited
            readonly property var recipe: KetTheme.galleryRecipe(KetTheme.dark, KetTheme.zoom,
                                                                 hovered || visualFocus)
            width: gallery.tile
            height: gallery.tile
            hoverEnabled: true
            text: "+" + (gallery.images.length - gallery.shown.length)
            Accessible.name: qsTr("Show %1 more").arg(gallery.images.length - gallery.shown.length)
            onClicked: gallery.expanded = true

            contentItem: Item {}
            background: Rectangle {
                x: more.recipe.ringWidth
                y: more.recipe.ringWidth
                width: more.recipe.tileSize
                height: more.recipe.tileSize
                radius: more.recipe.moreRadius
                color: more.recipe.background
                border.width: 1
                border.color: more.recipe.border

                Text {
                    anchors.centerIn: parent
                    text: more.text
                    color: more.recipe.more
                    font.family: KetTheme.fontFamily
                    font.pixelSize: KetTheme.activityRowRecipe(KetTheme.dark, KetTheme.zoom,
                                                               false, true, false).fontPixels
                }
            }
        }
    }
}
