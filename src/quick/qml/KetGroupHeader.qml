// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A sidebar group header (`.repository-group-header`), as ket::GroupHeader draws it: the
// folder icon and name toggle `expanded`; the action shows while hovered or focused.
Item {
    id: header

    property string name
    property bool expanded: true
    property bool selected: false
    property string iconName // empty uses folder-open / folder
    property string actionText: qsTr("New conversation")
    property var previewState: null
    readonly property bool hot: previewState ? !!previewState.hovered
                                             : hover.hovered || toggle.visualFocus || action.visualFocus
    readonly property var recipe: KetTheme.groupHeaderRecipe(
        KetTheme.dark, KetTheme.zoom, selected, hot, toggle.down, enabled, toggle.visualFocus)

    signal actionTriggered()

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 4, KetTheme.zoom)
    implicitHeight: recipe.height

    HoverHandler { id: hover }

    Basic.AbstractButton {
        id: toggle
        x: header.recipe.paddingStart
        width: Math.min(implicitWidth, action.x - x - header.recipe.spacing)
        height: parent.height
        implicitWidth: header.recipe.iconSize + header.recipe.nameGap + nameText.implicitWidth + 2
        checkable: true
        checked: header.expanded
        opacity: header.recipe.opacity
        onToggled: header.expanded = checked
        Accessible.name: header.name
        Accessible.checked: checked

        contentItem: Item {
            KetIcon {
                x: 1
                anchors.verticalCenter: parent.verticalCenter
                name: header.iconName !== "" ? header.iconName
                    : header.expanded ? "folder-open" : "folder"
                box: header.recipe.iconSize
                color: header.recipe.text
            }

            Text {
                id: nameText
                x: 1 + header.recipe.iconSize + header.recipe.nameGap
                width: parent.width - x - 1
                anchors.verticalCenter: parent.verticalCenter
                text: header.name
                elide: Text.ElideRight
                color: header.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: header.recipe.fontPixels
                font.weight: header.recipe.fontWeight
            }
        }

        background: Rectangle {
            radius: KetTheme.px(KetTheme.t.radiusSm, KetTheme.zoom)
            color: "transparent"
            border.width: header.recipe.focusRing.a > 0 ? 2 : 0
            border.color: header.recipe.focusRing
        }
    }

    KetIconButton {
        id: action
        x: header.width - header.recipe.paddingEnd - width
        anchors.verticalCenter: parent.verticalCenter
        width: header.recipe.actionSize
        height: header.recipe.actionSize
        controlSize: KetTheme.ControlSize.Small
        iconName: "plus"
        text: header.actionText
        opacity: header.hot ? 1 : 0
        onClicked: header.actionTriggered()
    }
}
