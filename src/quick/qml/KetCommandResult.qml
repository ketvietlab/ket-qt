// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// One result in KetCommandPalette (`.spotlight-result`): an icon, the title over a detail
// and, when highlighted, the Return key; otherwise an arrow.
Basic.AbstractButton {
    id: control

    property string title
    property string detail
    property string iconName
    property bool highlighted
    property color keyBorder: recipe.trailing
    readonly property var recipe: KetTheme.commandResultRecipe(KetTheme.dark, KetTheme.zoom,
                                                               highlighted)

    hoverEnabled: true
    focusPolicy: Qt.NoFocus
    text: title
    padding: recipe.paddingBlock
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    implicitWidth: leftPadding + rightPadding + titleText.implicitWidth
    implicitHeight: topPadding + bottomPadding + titleText.implicitHeight + recipe.detailGap
                    + detailText.implicitHeight
    Accessible.role: Accessible.ListItem
    Accessible.name: title
    Accessible.description: detail

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
            x: icon.visible ? icon.width + control.recipe.spacing : 0
            width: trailing.x - x - control.recipe.spacing
            spacing: control.recipe.detailGap

            Text {
                id: titleText
                width: parent.width
                text: control.title
                elide: Text.ElideRight
                color: control.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.titlePixels
                font.weight: control.recipe.titleWeight
            }

            Text {
                id: detailText
                width: parent.width
                text: control.detail
                elide: Text.ElideMiddle
                color: control.recipe.detail
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.detailPixels
            }
        }

        Item {
            id: trailing
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: control.highlighted ? enterBox.width : control.recipe.trailingSize
            height: control.highlighted ? enterBox.height : control.recipe.trailingSize

            Rectangle {
                id: enterBox
                visible: control.highlighted
                width: enterText.implicitWidth + 8
                height: enterText.implicitHeight + 4
                radius: 3
                color: "transparent"
                border.color: control.keyBorder

                Text {
                    id: enterText
                    anchors.centerIn: parent
                    text: "↵"
                    color: control.recipe.trailing
                    font.family: KetTheme.fontFamily
                    font.pixelSize: control.recipe.detailPixels
                }
            }

            KetIcon {
                visible: !control.highlighted
                name: "arrow-up-right"
                box: control.recipe.trailingSize
                color: control.recipe.trailing
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
    }
}
