// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The sidebar's update notice (`.update-pill`), as ket::UpdatePill draws it: a download
// icon, the title in the accent and a detail.
Basic.AbstractButton {
    id: control

    property string detail
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.updatePillRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                    !!previewState.pressed, previewState.enabled !== false,
                                    !!previewState.focused)
        : KetTheme.updatePillRecipe(KetTheme.dark, KetTheme.zoom, hovered, down, enabled,
                                    visualFocus)

    hoverEnabled: true
    opacity: recipe.opacity
    leftPadding: recipe.paddingInline + 1
    rightPadding: recipe.paddingInline + 1
    topPadding: recipe.paddingBlock + 1
    bottomPadding: recipe.paddingBlock + 1
    implicitWidth: leftPadding + rightPadding + recipe.iconSize + recipe.spacing
                   + titleText.implicitWidth
    implicitHeight: topPadding + bottomPadding + texts.implicitHeight
    Accessible.name: text
    Accessible.description: detail

    contentItem: Item {
        KetIcon {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            name: "download"
            box: control.recipe.iconSize
            color: control.recipe.title
        }

        Column {
            id: texts
            x: icon.width + control.recipe.spacing
            width: parent.width - x
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: titleText
                width: parent.width
                text: control.text
                elide: Text.ElideRight
                color: control.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.titlePixels
                font.weight: control.recipe.titleWeight
            }

            Text {
                visible: control.detail !== ""
                width: parent.width
                text: control.detail
                elide: Text.ElideRight
                color: control.recipe.detail
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.detailPixels
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 1
        border.color: control.recipe.focusRing.a > 0 ? control.recipe.focusRing
                                                     : control.recipe.border
    }
}
