// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Divider between menu groups (`.context-separator`).
Basic.MenuSeparator {
    id: separator

    readonly property var recipe: KetTheme.menuRecipe(KetTheme.dark, KetTheme.zoom)

    padding: 0
    topPadding: recipe.separatorMargin
    bottomPadding: recipe.separatorMargin

    contentItem: Rectangle {
        implicitHeight: 1
        color: separator.recipe.separator
    }
}
