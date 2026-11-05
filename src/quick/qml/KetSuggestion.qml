// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A starter prompt on the welcome screen (`.suggestion`), as ket::Suggestion draws it.
Basic.AbstractButton {
    id: control

    property string caption
    property string iconName
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.suggestionRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                    !!previewState.pressed, previewState.enabled !== false,
                                    !!previewState.focused)
        : KetTheme.suggestionRecipe(KetTheme.dark, KetTheme.zoom, hovered, down, enabled,
                                    visualFocus)

    hoverEnabled: true
    opacity: recipe.opacity
    leftPadding: recipe.paddingInline + 1
    rightPadding: recipe.paddingInline + 1
    topPadding: recipe.paddingBlock + 1
    bottomPadding: recipe.paddingBlock + 1
    implicitWidth: leftPadding + rightPadding + recipe.iconSize + recipe.arrowSize
                   + 2 * recipe.spacing + Math.max(titleText.implicitWidth, captionText.implicitWidth)
    implicitHeight: topPadding + bottomPadding
                    + Math.max(recipe.iconSize, texts.implicitHeight)
    Accessible.name: text
    Accessible.description: caption

    contentItem: Item {
        KetIcon {
            id: icon
            visible: control.iconName !== ""
            anchors.verticalCenter: parent.verticalCenter
            name: control.iconName
            box: control.recipe.iconSize
            color: control.recipe.icon
        }

        Column {
            id: texts
            x: icon.visible ? icon.width + control.recipe.spacing : 0
            width: parent.width - x - control.recipe.arrowSize - control.recipe.spacing
            anchors.verticalCenter: parent.verticalCenter
            spacing: control.recipe.captionGap

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
                id: captionText
                visible: control.caption !== ""
                width: parent.width
                text: control.caption
                elide: Text.ElideRight
                color: control.recipe.caption
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.captionPixels
            }
        }

        KetIcon {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            name: "arrow-right"
            box: control.recipe.arrowSize
            color: control.recipe.arrow
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 1
        border.color: control.recipe.focusRing.a > 0 ? control.recipe.focusRing : control.recipe.border
    }
}
