// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A background task (`.bg-row`), as ket::TaskRow draws it: a status icon, the kind
// (`terminal` for a shell, `bot` for an agent), the title, a meta line and Stop while the
// task runs or Dismiss once it ends.
Rectangle {
    id: row

    property string title
    property string meta
    property string kindIcon: "terminal"
    property int status: KetTheme.TaskStatus.Running
    property bool current: false
    readonly property var recipe: KetTheme.taskRowRecipe(KetTheme.dark, KetTheme.zoom, status,
                                                         current)
    readonly property bool live: status === KetTheme.TaskStatus.Running
                                 || status === KetTheme.TaskStatus.Stopping

    signal stopRequested()
    signal dismissRequested()

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 5, KetTheme.zoom)
    implicitHeight: recipe.minHeight
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border
    Accessible.role: Accessible.ListItem
    Accessible.name: title
    Accessible.description: meta

    Row {
        id: icons
        x: row.recipe.paddingStart
        anchors.verticalCenter: parent.verticalCenter
        spacing: row.recipe.spacing

        KetIcon {
            name: row.live ? "loading"
                : row.status === KetTheme.TaskStatus.Completed ? "success"
                : row.status === KetTheme.TaskStatus.Failed ? "error" : "stop-circle"
            box: row.recipe.iconSize
            color: row.recipe.status
        }

        KetIcon {
            name: row.kindIcon
            box: row.recipe.iconSize
            color: row.recipe.kind
        }
    }

    // `.bg-title` keeps its width; the meta line takes what is left.
    Text {
        id: titleText
        x: icons.x + icons.width + row.recipe.spacing
        width: Math.min(implicitWidth, action.x - row.recipe.spacing - x)
        anchors.verticalCenter: parent.verticalCenter
        text: row.title
        elide: Text.ElideRight
        color: row.recipe.title
        font.family: KetTheme.fontFamily
        font.pixelSize: row.recipe.titlePixels
    }

    Text {
        x: titleText.x + titleText.width + row.recipe.spacing
        width: Math.max(0, action.x - row.recipe.spacing - x)
        visible: row.meta !== "" && width > 0
        anchors.verticalCenter: parent.verticalCenter
        text: row.meta
        elide: Text.ElideRight
        color: row.recipe.meta
        font.family: KetTheme.fontFamily
        font.pixelSize: row.recipe.metaPixels
    }

    KetIconButton {
        id: action
        anchors.right: parent.right
        anchors.rightMargin: row.recipe.paddingEnd
        anchors.verticalCenter: parent.verticalCenter
        width: row.recipe.actionSize
        height: row.recipe.actionSize
        controlSize: KetTheme.ControlSize.Small
        iconName: row.live ? "stop" : "close"
        text: row.live ? qsTr("Stop %1").arg(row.title) : qsTr("Dismiss")
        enabled: !row.live || row.status === KetTheme.TaskStatus.Running
        onClicked: row.live ? row.stopRequested() : row.dismissRequested()
    }
}
