// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A date between turns (`.day-divider`); it draws ket::dayDividerRecipe, as ket::DayDivider does.
Item {
    id: divider

    property string text
    readonly property var recipe: KetTheme.dayDividerRecipe(KetTheme.dark, KetTheme.zoom)

    implicitWidth: label.implicitWidth + 4 * recipe.spacing
    implicitHeight: label.implicitHeight + recipe.marginTop + recipe.marginBottom

    Text {
        id: label
        x: Math.round((divider.width - width) / 2)
        y: divider.recipe.marginTop
        text: divider.text
        color: divider.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: divider.recipe.fontPixels
    }

    Rectangle {
        x: 0
        y: Math.floor(label.y + label.height / 2)
        width: Math.max(0, label.x - (divider.text !== "" ? divider.recipe.spacing : 0))
        height: 1
        color: divider.recipe.line
    }

    Rectangle {
        x: label.x + label.width + (divider.text !== "" ? divider.recipe.spacing : 0)
        y: Math.floor(label.y + label.height / 2)
        width: Math.max(0, divider.width - x)
        height: 1
        color: divider.recipe.line
    }
}
