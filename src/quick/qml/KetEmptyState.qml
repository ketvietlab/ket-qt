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

    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    Column {
        id: column
        anchors.centerIn: parent
        width: Math.min(empty.width > 0 ? empty.width : implicitWidth, implicitWidth)
        spacing: empty.recipe.spacing

        KetIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.iconName !== ""
            name: empty.iconName
            box: empty.recipe.iconSize
            color: empty.recipe.icon
        }

        Text {
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
            anchors.horizontalCenter: parent.horizontalCenter
            visible: empty.actionText !== ""
            text: empty.actionText
            iconName: empty.actionIcon
            onClicked: empty.actionTriggered()
        }
    }
}
