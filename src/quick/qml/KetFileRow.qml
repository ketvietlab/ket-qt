// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A row of the file explorer (`.file-row`), as ket::FileRow draws it: an indented icon and
// name with an optional Git status. A folder row is checkable and shows a chevron; it reads
// as open while checked.
Basic.AbstractButton {
    id: control

    property bool folder: false
    property int depth: 0
    property string iconName
    property string status
    property bool active: false
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.fileRowRecipe(KetTheme.dark, KetTheme.zoom, active, !!previewState.hovered,
                                 !!previewState.pressed, previewState.enabled !== false,
                                 !!previewState.focused)
        : KetTheme.fileRowRecipe(KetTheme.dark, KetTheme.zoom, active, hovered, down, enabled,
                                 visualFocus)
    // `.indent-N`: 12 at the root, then 37, 49, 61…
    readonly property real indent: depth === 0 ? recipe.paddingInline
                                               : recipe.indentStart + depth * recipe.indentStep
    readonly property real gap: folder ? recipe.folderSpacing : recipe.spacing

    hoverEnabled: true
    checkable: folder
    opacity: recipe.opacity
    leftPadding: indent
    rightPadding: recipe.paddingInline
    implicitWidth: leftPadding + rightPadding + (folder ? recipe.chevronSize + gap : 0)
                   + recipe.iconSize + gap + nameText.implicitWidth
    implicitHeight: folder ? recipe.folderHeight : recipe.height
    Accessible.role: folder ? Accessible.TreeItem : Accessible.ListItem
    Accessible.name: text
    Accessible.description: status

    contentItem: Item {
        KetIcon {
            id: chevron
            visible: control.folder
            anchors.verticalCenter: parent.verticalCenter
            name: control.checked ? "chevron-down" : "chevron-right"
            box: control.recipe.chevronSize
            color: control.recipe.text
        }

        KetIcon {
            id: icon
            x: control.folder ? chevron.width + control.gap : 0
            anchors.verticalCenter: parent.verticalCenter
            name: control.iconName !== "" ? control.iconName
                : !control.folder ? "file" : control.checked ? "folder-open" : "folder"
            box: control.recipe.iconSize
            color: control.recipe.text
        }

        Text {
            id: nameText
            x: icon.x + icon.width + control.gap
            width: (statusText.visible ? statusText.x - control.gap : parent.width) - x
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            elide: Text.ElideMiddle
            color: control.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.fontPixels
        }

        Text {
            id: statusText
            visible: control.status !== ""
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: control.status
            color: control.recipe.status
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.statusPixels
        }
    }

    background: Rectangle {
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 0
        border.color: control.recipe.focusRing
    }
}
