// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Drop-down select (`select`); the closed control draws ket::fieldRecipe, as ket::Select does.
Basic.ComboBox {
    id: control

    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                               !!previewState.focused, previewState.enabled !== false, false)
        : KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, hovered, visualFocus, enabled, false)

    implicitHeight: recipe.height
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline + recipe.iconSize + recipe.spacing
    hoverEnabled: true
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    contentItem: Text {
        text: control.displayText
        font: control.font
        color: control.recipe.text
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: KetIcon {
        x: control.width - control.recipe.paddingInline - width
        anchors.verticalCenter: parent.verticalCenter
        name: "chevron-down"
        box: control.recipe.iconSize
        color: control.recipe.icon
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.borderWidth
        border.color: control.recipe.border
    }
}
