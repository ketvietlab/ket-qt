// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A side pane's header (`.context-tabs`), as ket::PaneHeader draws it: the icon and title,
// then the pane's actions (children) on the right.
Item {
    id: header

    property string title
    property string iconName
    default property alias actions: actionRow.data
    readonly property var recipe: KetTheme.paneHeaderRecipe(KetTheme.dark, KetTheme.zoom)

    implicitHeight: recipe.height
    implicitWidth: KetTheme.px(KetTheme.t.space16 * 5, KetTheme.zoom)
    Accessible.role: Accessible.Heading
    Accessible.name: title

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: header.recipe.border
    }

    Row {
        x: header.recipe.paddingStart
        height: parent.height - 1
        width: actionRow.x - x - header.recipe.spacing
        spacing: header.recipe.spacing

        KetIcon {
            anchors.verticalCenter: parent.verticalCenter
            visible: header.iconName !== ""
            name: header.iconName
            box: header.recipe.iconSize
            color: header.recipe.icon
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - (header.iconName !== "" ? header.recipe.iconSize + parent.spacing : 0)
            text: header.title
            elide: Text.ElideMiddle
            color: header.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: header.recipe.fontPixels
        }
    }

    Row {
        id: actionRow
        x: parent.width - header.recipe.paddingEnd - width
        height: parent.height - 1
        spacing: KetTheme.px(KetTheme.t.space1, KetTheme.zoom)
    }
}
