// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Controls.impl as Impl
import Ket

// Square icon-only button (`.icon-btn`); it draws ket::iconButtonRecipe, as ket::IconButton
// does. `text` is the label: the accessible name, and the tooltip when `showToolTip` is set.
Basic.Button {
    id: control

    property int controlSize: KetTheme.ControlSize.Medium
    property bool showToolTip: true
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.iconButtonRecipe(KetTheme.dark, KetTheme.zoom, controlSize,
                                    !!previewState.hovered, !!previewState.pressed,
                                    previewState.enabled !== false, !!previewState.focused)
        : KetTheme.iconButtonRecipe(KetTheme.dark, KetTheme.zoom, controlSize,
                                    hovered, down, enabled, visualFocus)

    implicitWidth: recipe.height
    implicitHeight: recipe.height
    padding: 0
    opacity: recipe.opacity
    hoverEnabled: true
    display: Basic.AbstractButton.IconOnly
    icon.width: recipe.iconSize
    icon.height: recipe.iconSize
    Accessible.name: text

    Basic.ToolTip.visible: showToolTip && hovered && text !== ""
    Basic.ToolTip.text: text
    Basic.ToolTip.delay: 600

    contentItem: Impl.IconImage {
        source: control.icon.source
        sourceSize.width: control.icon.width
        sourceSize.height: control.icon.height
        fillMode: Image.Pad
        color: control.recipe.text
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
