// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// The accent sample in Settings (`.accent-swatch`), as ket::AccentSwatch draws it.
Rectangle {
    id: swatch

    readonly property var recipe: KetTheme.accentSwatchRecipe(KetTheme.dark, KetTheme.zoom)

    implicitWidth: Math.round(recipe.size) + 2
    implicitHeight: implicitWidth
    radius: recipe.radius + 1
    color: recipe.border
    border.width: 1
    border.color: recipe.ring
    Accessible.role: Accessible.Graphic
    Accessible.name: qsTr("Accent colour")

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1 + swatch.recipe.borderWidth
        radius: Math.max(0, swatch.recipe.radius - swatch.recipe.borderWidth)
        color: swatch.recipe.fill
    }
}
