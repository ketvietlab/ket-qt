// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Sidebar navigation row (`.nav-item`); it draws ket::navItemRecipe, as ket::NavItem does.
// `count` (>= 0) shows `.nav-count`; otherwise `shortcut` shows a kbd. Use a ButtonGroup
// or `checked` for the selected row.
Basic.AbstractButton {
    id: control

    property string iconName
    property int count: -1
    property string shortcut
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.navItemRecipe(KetTheme.dark, KetTheme.zoom, checked, !!previewState.hovered,
                                 !!previewState.pressed, previewState.enabled !== false,
                                 !!previewState.focused)
        : KetTheme.navItemRecipe(KetTheme.dark, KetTheme.zoom, checked, hovered, down, enabled,
                                 visualFocus)

    checkable: true
    hoverEnabled: true
    implicitHeight: recipe.height
    implicitWidth: leftPadding + rightPadding + row.implicitWidth
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels
    font.weight: recipe.fontWeight
    Accessible.role: Accessible.PageTab
    Accessible.name: text

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
                id: label
                anchors.verticalCenter: parent.verticalCenter
                width: Math.min(implicitWidth, Math.max(0, control.availableWidth
                    - (control.iconName !== "" ? control.recipe.iconSize + row.spacing : 0)
                    - (trailing.visible ? trailing.width + row.spacing : 0)))
                text: control.text
                font: control.font
                color: control.recipe.text
                elide: Text.ElideRight
            }
        }

        Rectangle {
            id: trailing
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: control.count >= 0 || control.shortcut !== ""
            width: control.count >= 0 ? trailingText.implicitWidth + 2 * control.recipe.countPadding
                                      : trailingText.implicitWidth
            height: control.count >= 0 ? control.recipe.countHeight : trailingText.implicitHeight
            radius: control.recipe.countRadius
            color: "transparent"
            border.width: control.count >= 0 ? 1 : 0
            border.color: control.recipe.countBorder

            Text {
                id: trailingText
                anchors.centerIn: parent
                text: control.count >= 0 ? String(control.count) : control.shortcut
                color: control.count >= 0 ? control.recipe.countText : control.recipe.kbdText
                font.family: KetTheme.fontFamily
                font.pixelSize: control.count >= 0 ? control.recipe.countFontPixels
                                                   : control.recipe.kbdFontPixels
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background

        Rectangle {
            anchors.fill: parent
            visible: control.recipe.focusRing.a > 0
            radius: parent.radius
            color: "transparent"
            border.width: 2
            border.color: control.recipe.focusRing
        }
    }
}
