// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Centred placeholder for an empty view (`.empty-state`); it draws ket::emptyStateRecipe,
// as ket::EmptyState does. `actionText` adds a button that emits `actionTriggered`.
Item {
    id: empty

    property string iconName
    property string title
    property string message
    property string actionText
    property string actionIcon
    readonly property var recipe: KetTheme.emptyStateRecipe(KetTheme.dark, KetTheme.zoom)

    signal actionTriggered()

    // The natural width comes from the children's natural widths, the message capped at the
    // recipe's maximum, never from their laid-out widths: the message is laid out at the
    // item's width, so measuring it here would make the width depend on itself.
    implicitWidth: Math.max(icon.visible ? icon.implicitWidth : 0,
                            heading.visible ? heading.implicitWidth : 0,
                            body.visible ? Math.min(body.implicitWidth, recipe.messageMaxWidth) : 0,
                            action.visible ? action.implicitWidth : 0)
    implicitHeight: column.implicitHeight

    Column {
        id: column
        anchors.centerIn: parent
        width: Math.min(empty.width > 0 ? empty.width : implicitWidth, implicitWidth)
        spacing: empty.recipe.spacing

        KetIcon {
            id: icon
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.iconName !== ""
            name: empty.iconName
            box: empty.recipe.iconSize
            color: empty.recipe.icon
        }

        Text {
            id: heading
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.title !== ""
            width: Math.min(implicitWidth, column.width)
            text: empty.title
            textFormat: Text.PlainText
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            color: empty.recipe.title
            font.family: KetTheme.fontFamily
            font.pixelSize: empty.recipe.titlePixels
            font.weight: empty.recipe.titleWeight
            Accessible.role: Accessible.Heading
        }

        Text {
            id: body
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.message !== ""
            width: Math.min(implicitWidth, empty.recipe.messageMaxWidth, empty.width)
            text: empty.message
            textFormat: Text.PlainText
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            lineHeight: empty.recipe.lineHeight
            color: empty.recipe.message
            font.family: KetTheme.fontFamily
            font.pixelSize: empty.recipe.messagePixels
        }

        KetButton {
            id: action
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.actionText !== ""
            text: empty.actionText
            iconName: empty.actionIcon
            onClicked: empty.actionTriggered()
        }
    }
}
