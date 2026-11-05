// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Radio button with an inline label; it draws ket::toggleRecipe, as ket::Radio does.
Basic.RadioButton {
    id: control

    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.toggleRecipe(KetTheme.dark, KetTheme.zoom, KetTheme.ToggleKind.Radio, checked,
                                !!previewState.hovered, !!previewState.pressed,
                                previewState.enabled !== false, !!previewState.focused)
        : KetTheme.toggleRecipe(KetTheme.dark, KetTheme.zoom, KetTheme.ToggleKind.Radio, checked,
                                hovered, down, enabled, visualFocus)

    padding: 2
    spacing: recipe.spacing
    hoverEnabled: true
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    indicator: KetToggleIndicator {
        x: control.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        recipe: control.recipe
        kind: KetTheme.ToggleKind.Radio
    }

    contentItem: Text {
        leftPadding: control.indicator.width + control.spacing
        text: control.text
        font: control.font
        color: control.recipe.label
        verticalAlignment: Text.AlignVCenter
    }
}
