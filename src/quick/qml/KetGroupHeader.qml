// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A sidebar group header (`.repository-group-header`), as ket::GroupHeader draws it: the
// folder icon and name toggle `expanded`; the action shows while hovered or focused. With
// `activatable`, the icon alone folds and the name is a control of its own that emits
// activated(); they are separate focus stops with their own accessible names.
Item {
    id: header

    property string name
    property bool expanded: true
    property bool selected: false
    property string iconName // empty uses folder-open / folder
    property string actionText: qsTr("New conversation")
    property bool activatable: false
    property var previewState: null
    readonly property bool hot: previewState ? !!previewState.hovered
                                             : hover.hovered || toggle.visualFocus || action.visualFocus
                                               || nameButton.visualFocus
    readonly property var recipe: KetTheme.groupHeaderRecipe(
        KetTheme.dark, KetTheme.zoom, selected, hot, toggle.down, enabled, toggle.visualFocus)
    readonly property var nameRecipe: KetTheme.groupHeaderRecipe(
        KetTheme.dark, KetTheme.zoom, selected, hot, nameButton.down, enabled,
        nameButton.visualFocus)

    signal actionTriggered()
    signal activated()

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 4, KetTheme.zoom)
    implicitHeight: recipe.height

    HoverHandler { id: hover }

    Basic.AbstractButton {
        id: toggle
        objectName: "groupHeaderFold"
        x: header.recipe.paddingStart
        width: Math.min(implicitWidth, action.x - x - header.recipe.spacing)
        height: parent.height
        implicitWidth: header.activatable
            ? header.recipe.iconSize + 2
            : header.recipe.iconSize + header.recipe.nameGap + nameText.implicitWidth + 2
        checkable: true
        checked: header.expanded
        opacity: header.recipe.opacity
        onToggled: header.expanded = checked
        Accessible.name: !header.activatable ? header.name
            : header.expanded ? qsTr("Fold %1").arg(header.name)
                              : qsTr("Unfold %1").arg(header.name)
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
                visible: !header.activatable
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

    Basic.AbstractButton {
        id: nameButton
        objectName: "groupHeaderName"
        visible: header.activatable
        x: toggle.x + toggle.width + header.recipe.nameGap
        width: Math.max(0, Math.min(implicitWidth, action.x - x - header.recipe.spacing))
        height: parent.height
        implicitWidth: nameLabel.implicitWidth + 4
        activeFocusOnTab: header.activatable
        opacity: header.nameRecipe.opacity
        onClicked: header.activated()
        Accessible.role: Accessible.Button
        Accessible.name: header.name

        contentItem: Text {
            id: nameLabel
            leftPadding: 2
            rightPadding: 2
            verticalAlignment: Text.AlignVCenter
            text: header.name
            elide: Text.ElideRight
            color: header.nameRecipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: header.nameRecipe.fontPixels
            font.weight: header.nameRecipe.fontWeight
        }

        background: Rectangle {
            radius: KetTheme.px(KetTheme.t.radiusSm, KetTheme.zoom)
            color: "transparent"
            border.width: header.nameRecipe.focusRing.a > 0 ? 2 : 0
            border.color: header.nameRecipe.focusRing
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
