// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// The checkbox or radio indicator of ket::toggleRecipe; shared by KetCheckbox, KetRadio
// and KetChoice.
Rectangle {
    id: indicator

    required property var recipe
    property int kind: KetTheme.ToggleKind.Checkbox

    implicitWidth: recipe.size
    implicitHeight: recipe.size
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border

    KetIcon {
        anchors.centerIn: parent
        visible: indicator.kind === KetTheme.ToggleKind.Checkbox && indicator.recipe.mark.a > 0
        name: "check"
        weight: KetTheme.IconWeight.Regular
        box: indicator.recipe.size
        color: indicator.recipe.mark
    }

    Rectangle {
        anchors.centerIn: parent
        visible: indicator.kind === KetTheme.ToggleKind.Radio && indicator.recipe.mark.a > 0
        width: indicator.width * 0.375
        height: width
        radius: width / 2
        color: indicator.recipe.mark
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: -2
        visible: indicator.recipe.focusRing.a > 0
        radius: indicator.radius + 2
        color: "transparent"
        border.width: 2
        border.color: indicator.recipe.focusRing
    }
}
