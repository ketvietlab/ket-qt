// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Checkbox with an inline label; it draws ket::toggleRecipe, as ket::Checkbox does.
Basic.CheckBox {
    id: control

    property var previewState: null
    // A long label wraps under itself, with the indicator on its first line.
    property bool wordWrap: false
    readonly property var recipe: previewState
        ? KetTheme.toggleRecipe(KetTheme.dark, KetTheme.zoom, KetTheme.ToggleKind.Checkbox, checked,
                                !!previewState.hovered, !!previewState.pressed,
                                previewState.enabled !== false, !!previewState.focused)
        : KetTheme.toggleRecipe(KetTheme.dark, KetTheme.zoom, KetTheme.ToggleKind.Checkbox, checked,
                                hovered, down, enabled, visualFocus)

    padding: 2
    spacing: recipe.spacing
    hoverEnabled: true
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    indicator: KetToggleIndicator {
        x: control.leftPadding
        y: control.wordWrap
            ? control.topPadding + Math.max(0, (firstLine.height - height) / 2)
            : (control.height - height) / 2
        recipe: control.recipe
        kind: KetTheme.ToggleKind.Checkbox
    }

    contentItem: Text {
        leftPadding: control.indicator.width + control.spacing
        text: control.text
        font: control.font
        color: control.recipe.label
        verticalAlignment: control.wordWrap ? Text.AlignTop : Text.AlignVCenter
        wrapMode: control.wordWrap ? Text.WordWrap : Text.NoWrap
    }

    FontMetrics {
        id: firstLine
        font: control.font
    }
}
