// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The background task strip above the composer (`.bg-bar`), as ket::TaskBar draws it: a
// toggle with the count, then one KetTaskRow per task while expanded. `tasks` is a list of
// { title, meta, kindIcon, status, current }; the app removes a task on dismissRequested.
Column {
    id: bar

    property var tasks: []
    property bool expanded: true
    readonly property var toggleRecipe: KetTheme.taskBarRecipe(
        KetTheme.dark, KetTheme.zoom, toggle.hovered, toggle.down, toggle.enabled,
        toggle.visualFocus)

    signal stopRequested(int index)
    signal dismissRequested(int index)

    spacing: toggleRecipe.rowGap

    Basic.AbstractButton {
        id: toggle
        hoverEnabled: true
        checkable: true
        checked: bar.expanded
        onToggled: bar.expanded = checked
        text: bar.tasks.length === 1 ? qsTr("1 background task")
                                     : qsTr("%1 background tasks").arg(bar.tasks.length)
        opacity: bar.toggleRecipe.opacity
        leftPadding: bar.toggleRecipe.paddingInline
        rightPadding: bar.toggleRecipe.paddingInline
        implicitWidth: leftPadding + rightPadding + content.implicitWidth
        implicitHeight: bar.toggleRecipe.height
        Accessible.role: Accessible.Button
        Accessible.name: text

        contentItem: Row {
            id: content
            spacing: bar.toggleRecipe.spacing

            KetIcon {
                anchors.verticalCenter: parent.verticalCenter
                name: "loading"
                box: bar.toggleRecipe.iconSize
                color: bar.toggleRecipe.text
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: toggle.text
                color: bar.toggleRecipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: bar.toggleRecipe.fontPixels
            }

            KetIcon {
                anchors.verticalCenter: parent.verticalCenter
                name: toggle.checked ? "chevron-down" : "chevron-up"
                box: bar.toggleRecipe.chevronSize
                color: bar.toggleRecipe.text
            }
        }

        background: Rectangle {
            radius: bar.toggleRecipe.radius
            color: bar.toggleRecipe.background
            border.width: bar.toggleRecipe.focusRing.a > 0 ? 2 : 0
            border.color: bar.toggleRecipe.focusRing
        }
    }

    Repeater {
        model: bar.expanded ? bar.tasks : []

        KetTaskRow {
            required property var modelData
            required property int index
            width: bar.width
            title: modelData.title
            meta: modelData.meta || ""
            kindIcon: modelData.kindIcon || "terminal"
            status: modelData.status !== undefined ? modelData.status
                                                   : KetTheme.TaskStatus.Running
            current: !!modelData.current
            onStopRequested: bar.stopRequested(index)
            onDismissRequested: bar.dismissRequested(index)
        }
    }
}
