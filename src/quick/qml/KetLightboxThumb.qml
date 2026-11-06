// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Effects
import Ket

// The thumbnail that opens the image viewer (`[data-ui="lightbox-thumb"]`), as
// ket::LightboxThumb draws it: `source` cropped to a square tile. Without a source it is
// the empty tile (`[data-ui="lightbox-empty"]`) and shows `text`. `description` is the
// accessible name.
Basic.AbstractButton {
    id: control

    property url source
    property string description
    property int thumbSize: KetTheme.LightboxThumbSize.Medium
    property var previewState: null
    readonly property bool empty: source.toString() === ""
    readonly property var recipe: KetTheme.lightboxThumbRecipe(
        KetTheme.dark, KetTheme.zoom, thumbSize,
        previewState ? !!previewState.focused : visualFocus)

    implicitWidth: recipe.size
    implicitHeight: recipe.size
    enabled: !empty
    focusPolicy: Qt.StrongFocus
    Accessible.name: description !== "" ? description : text

    HoverHandler {
        cursorShape: control.empty ? Qt.ArrowCursor : Qt.PointingHandCursor
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background

        // `object-fit: cover`, masked to the rounded tile at the image's own resolution.
        Image {
            id: picture
            anchors.fill: parent
            visible: false
            source: control.source
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            smooth: true
            mipmap: true
        }

        Item {
            id: mask
            anchors.fill: parent
            visible: false
            layer.enabled: true
            layer.smooth: true

            Rectangle {
                anchors.fill: parent
                radius: control.recipe.radius
            }
        }

        MultiEffect {
            anchors.fill: parent
            visible: !control.empty
            source: picture
            maskEnabled: true
            maskSource: mask
            // Soft mask edges: an antialiased curve rather than a stepped one.
            maskThresholdMin: 0.5
            maskSpreadAtMin: 1
        }
    }

    contentItem: Item {
        Rectangle {
            anchors.fill: parent
            radius: control.recipe.radius
            color: "transparent"
            border.width: 1
            border.color: control.recipe.border
        }

        Text {
            anchors.fill: parent
            anchors.margins: 4
            visible: control.empty
            text: control.text
            color: control.recipe.text
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.textPixels
            lineHeightMode: Text.FixedHeight
            lineHeight: control.recipe.lineHeight
        }

        Rectangle {
            anchors.fill: parent
            visible: control.recipe.focusRing.a > 0
            radius: control.recipe.radius
            color: "transparent"
            border.width: control.recipe.focusWidth
            border.color: control.recipe.focusRing
        }
    }
}
