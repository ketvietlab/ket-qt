// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A round button over the image viewer's backdrop (`[data-ui="lightbox-tool"]`), as
// ket::LightboxButton draws it: a Két Symbol `code`, or with no code a pill with `text`
// (the zoom percentage). `label` is the accessible name and the tooltip.
Basic.AbstractButton {
    id: control

    property string code
    property string label
    property bool flipped: false // a clockwise Rotate from the anticlockwise `restore`
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.lightboxToolRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                      !!previewState.pressed, previewState.enabled !== false,
                                      !!previewState.focused)
        : KetTheme.lightboxToolRecipe(KetTheme.dark, KetTheme.zoom, hovered, pressed, enabled,
                                      visualFocus)

    implicitWidth: code === ""
                   ? KetTheme.lightboxRecipe(KetTheme.dark, KetTheme.zoom).zoomLevelWidth
                   : recipe.size
    implicitHeight: recipe.size
    hoverEnabled: true
    focusPolicy: Qt.StrongFocus
    opacity: recipe.opacity
    Accessible.name: label !== "" ? label : text

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }

    KetToolTip {
        visible: control.hovered && control.label !== ""
        text: control.label
    }

    background: Rectangle {
        radius: height / 2
        color: control.recipe.background
        // Inside the bounds, as ket::LightboxButton draws it.
        border.width: control.recipe.focusRing.a > 0 ? control.recipe.focusWidth : 0
        border.color: control.recipe.focusRing
    }

    contentItem: Item {
        KetIcon {
            anchors.centerIn: parent
            visible: control.code !== ""
            name: control.code
            box: control.recipe.iconSize
            color: control.recipe.glyph
            transform: Scale {
                origin.x: control.recipe.iconSize / 2
                xScale: control.flipped ? -1 : 1
            }
        }

        Text {
            anchors.fill: parent
            visible: control.code === ""
            text: control.text
            color: control.recipe.glyph
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: KetTheme.fontFamily
            font.pixelSize: KetTheme.lightboxRecipe(KetTheme.dark, KetTheme.zoom).counterPixels
            font.features: { "tnum": 1 }
        }
    }
}
