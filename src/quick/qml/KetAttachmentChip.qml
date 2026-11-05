// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A file attached to the composer (`.attachment-chip`), as ket::AttachmentChip draws it.
// Clicking the × or pressing Delete or Backspace while focused emits removeRequested().
FocusScope {
    id: chip

    property string name
    property string iconName: "file"
    readonly property var recipe: KetTheme.attachmentChipRecipe(KetTheme.dark, KetTheme.zoom,
                                                                removeArea.containsMouse)

    signal removeRequested()

    activeFocusOnTab: true
    implicitHeight: recipe.height
    implicitWidth: Math.min(recipe.maxWidth, 2 * recipe.paddingInline
                            + 2 * (recipe.iconSize + recipe.spacing) + nameText.implicitWidth)
    Accessible.role: Accessible.Button
    Accessible.name: name
    Accessible.description: qsTr("Press Delete to remove")
    Keys.onPressed: event => {
        if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace) {
            event.accepted = true
            chip.removeRequested()
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: chip.recipe.radius
        color: chip.recipe.background
        border.width: chip.activeFocus ? 2 : 0
        border.color: KetTheme.t.focusBorder
    }

    KetIcon {
        id: icon
        x: chip.recipe.paddingInline
        anchors.verticalCenter: parent.verticalCenter
        name: chip.iconName
        box: chip.recipe.iconSize
        color: chip.recipe.text
    }

    Text {
        id: nameText
        x: icon.x + icon.width + chip.recipe.spacing
        width: Math.max(0, removeIcon.x - chip.recipe.spacing - x)
        anchors.verticalCenter: parent.verticalCenter
        text: chip.name
        elide: Text.ElideMiddle
        color: chip.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: chip.recipe.fontPixels
    }

    KetIcon {
        id: removeIcon
        x: chip.width - chip.recipe.paddingInline - width
        anchors.verticalCenter: parent.verticalCenter
        name: "close"
        box: chip.recipe.iconSize
        color: chip.recipe.remove

        MouseArea {
            id: removeArea
            anchors.fill: parent
            anchors.margins: -2
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: chip.removeRequested()
        }
    }
}
