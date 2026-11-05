// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Context menu (`.desktop-context-menu`); it draws ket::menuRecipe, as ket::Style does
// for QMenu. Fill it with KetMenuItem and KetMenuSeparator.
Basic.Menu {
    id: menu

    readonly property var recipe: KetTheme.menuRecipe(KetTheme.dark, KetTheme.zoom)
    // Reserve the check column when any item is checkable, as QMenu does.
    readonly property bool hasCheckableItems: {
        for (let i = 0; i < count; ++i) {
            const item = itemAt(i)
            if (item && item.checkable)
                return true
        }
        return false
    }

    // The widest item's natural width; the list view does not report one.
    readonly property real itemsWidth: {
        let width = 0
        for (let i = 0; i < count; ++i) {
            const item = itemAt(i)
            if (item)
                width = Math.max(width, item.implicitWidth)
        }
        return width
    }

    padding: recipe.padding
    implicitWidth: Math.max(recipe.minWidth,
                            Math.min(recipe.maxWidth, itemsWidth + leftPadding + rightPadding))
    overlap: recipe.padding
    enter: null
    exit: null
    delegate: KetMenuItem {}

    background: Item {
        implicitWidth: menu.recipe.minWidth

        KetShadow {
            anchors.fill: parent
            radius: menu.recipe.radius
            shadowX: menu.recipe.shadowX
            shadowY: menu.recipe.shadowY
            shadowBlur: menu.recipe.shadowBlur
            shadowColor: menu.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: menu.recipe.radius
            color: menu.recipe.background
            border.width: 1
            border.color: menu.recipe.border
        }
    }
}
