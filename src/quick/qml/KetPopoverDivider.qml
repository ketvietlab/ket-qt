// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// `.settings-divider` in a KetPopover body: a hairline with the recipe's spacing around it.
Item {
    readonly property var recipe: KetTheme.popoverRecipe(KetTheme.dark, KetTheme.zoom)

    width: parent ? parent.width : 0
    implicitHeight: 2 * Math.round(recipe.dividerSpacing) + 1

    Rectangle {
        y: Math.round(parent.recipe.dividerSpacing)
        width: parent.width
        height: 1
        color: parent.recipe.divider
    }
}
