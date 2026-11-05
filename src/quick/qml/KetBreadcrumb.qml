// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A file path bar (`.file-breadcrumb`), as ket::Breadcrumb draws it: `path` is the folders
// then the file; children are the actions on the right.
Item {
    id: crumb

    property var path: []
    default property alias actions: actionRow.data
    readonly property var recipe: KetTheme.breadcrumbRecipe(KetTheme.dark, KetTheme.zoom)

    implicitHeight: recipe.height
    implicitWidth: KetTheme.px(KetTheme.t.space16 * 6, KetTheme.zoom)
    Accessible.role: Accessible.StaticText
    Accessible.name: path.join("/")

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: crumb.recipe.border
    }

    Row {
        x: crumb.recipe.paddingInline
        y: 0
        height: parent.height - 1
        width: actionRow.x - x - crumb.recipe.spacing
        spacing: crumb.recipe.spacing
        clip: true

        Text {
            anchors.verticalCenter: parent.verticalCenter
            visible: crumb.path.length > 1
            text: crumb.path.slice(0, -1).join(" / ")
            color: crumb.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: crumb.recipe.fontPixels
        }

        KetIcon {
            anchors.verticalCenter: parent.verticalCenter
            visible: crumb.path.length > 1
            name: "chevron-right"
            box: crumb.recipe.iconSize
            color: crumb.recipe.text
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: crumb.path.length > 0 ? crumb.path[crumb.path.length - 1] : ""
            color: crumb.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: crumb.recipe.fontPixels
        }
    }

    Row {
        id: actionRow
        x: parent.width - crumb.recipe.paddingInline - width
        height: parent.height - 1
        spacing: crumb.recipe.spacing
    }
}
