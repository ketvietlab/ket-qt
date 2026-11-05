// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// The window status bar (`.statusbar`), as ket::StatusBar draws it: children on the left
// and `trailing` items on the right, usually KetStatusItem.
Rectangle {
    id: bar

    default property alias leading: leadingRow.data
    property alias trailing: trailingRow.data
    readonly property var recipe: KetTheme.statusBarRecipe(KetTheme.dark, KetTheme.zoom)

    implicitHeight: recipe.height
    implicitWidth: leadingRow.implicitWidth + trailingRow.implicitWidth + 2 * recipe.paddingInline
    color: recipe.background

    Rectangle {
        width: parent.width
        height: 1
        color: bar.recipe.border
    }

    Row {
        id: leadingRow
        x: bar.recipe.paddingInline
        y: 1
        height: parent.height - 1
        spacing: bar.recipe.groupGap
    }

    Row {
        id: trailingRow
        x: parent.width - bar.recipe.paddingInline - width
        y: 1
        height: parent.height - 1
        spacing: bar.recipe.groupGap
    }
}
