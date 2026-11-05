// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// One tool or command step (`.activity-row`); it draws ket::activityRowRecipe, as
// ket::ActivityRow does. `expandable` makes it checkable with a trailing chevron.
Basic.AbstractButton {
    id: control

    property string iconName
    property string detail
    property bool expandable: false
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.activityRowRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                     previewState.enabled !== false, !!previewState.focused)
        : KetTheme.activityRowRecipe(KetTheme.dark, KetTheme.zoom, hovered || down, enabled,
                                     visualFocus)

    checkable: expandable
    hoverEnabled: true
    implicitHeight: recipe.height
    implicitWidth: leftPadding + rightPadding + row.implicitWidth
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    opacity: recipe.opacity
    Accessible.name: text
    Accessible.description: detail

    contentItem: Item {
        implicitWidth: row.implicitWidth
        implicitHeight: row.implicitHeight

        Row {
            id: row
            anchors.verticalCenter: parent.verticalCenter
            spacing: control.recipe.spacing

            KetIcon {
                anchors.verticalCenter: parent.verticalCenter
                visible: control.iconName !== ""
                name: control.iconName
                box: control.recipe.iconSize
                color: control.recipe.text
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: Math.min(implicitWidth, control.availableWidth
                    - (control.iconName !== "" ? control.recipe.iconSize + row.spacing : 0)
                    - (control.expandable ? control.recipe.chevronSize + row.spacing : 0))
                text: control.text
                elide: Text.ElideRight
                color: control.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.fontPixels
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                visible: control.detail !== ""
                text: control.detail
                elide: Text.ElideRight
                color: control.recipe.detail
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.detailPixels
            }
        }

        KetIcon {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: control.expandable
            name: control.checked ? "chevron-up" : "chevron-down"
            box: control.recipe.chevronSize
            color: control.recipe.text
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 0
        border.color: control.recipe.focusRing
    }
}
