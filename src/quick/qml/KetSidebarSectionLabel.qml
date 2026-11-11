// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A sidebar tree section label, "Conversations (n)" with an add action, as
// ket::SidebarSectionLabel draws it. The label is a heading; its action is a focus stop.
Item {
    id: section

    property string text
    property int count: -1 // -1 leaves the count out
    property int level: 0
    property string actionText: qsTr("New conversation")
    readonly property string displayText: count < 0 ? text : text + " (" + count + ")"
    readonly property var recipe: KetTheme.sidebarSectionRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property var columns: KetTheme.sidebarTreeColumns(level, KetTheme.zoom)
    readonly property real nameLeft: label.x
    readonly property real markLeft: icon.x

    signal actionTriggered()

    implicitWidth: columns.text + label.implicitWidth + action.width + 2 * recipe.actionEnd
    implicitHeight: recipe.height

    Accessible.role: Accessible.Heading
    Accessible.name: count < 0 ? text : text + ", " + count

    KetIcon {
        id: icon
        objectName: "sidebarSectionMark"
        x: section.columns.mark
        anchors.verticalCenter: parent.verticalCenter
        name: section.recipe.icon
        box: section.recipe.iconSize
        color: section.recipe.iconColor
    }

    Text {
        id: label
        objectName: "sidebarSectionText"
        x: section.columns.text
        width: Math.max(0, action.x - section.recipe.actionEnd - x)
        height: parent.height
        verticalAlignment: Text.AlignVCenter
        text: section.displayText
        elide: Text.ElideRight
        maximumLineCount: 1
        color: section.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: section.recipe.pixels
        font.weight: section.recipe.weight
        Accessible.ignored: true
    }

    KetIconButton {
        id: action
        objectName: "sidebarSectionAction"
        x: section.width - section.recipe.actionEnd - width
        anchors.verticalCenter: parent.verticalCenter
        controlSize: KetTheme.ControlSize.Small
        iconName: "plus"
        text: section.actionText
        onClicked: section.actionTriggered()
    }
}
