// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Tooltip; it draws ket::tooltipRecipe, as ket::Style does for QToolTip. Use it as a
// child: KetToolTip { visible: parent.hovered; text: "…" }.
Basic.ToolTip {
    id: tip

    readonly property var recipe: KetTheme.tooltipRecipe(KetTheme.dark, KetTheme.zoom)

    width: Math.min(implicitWidth, recipe.maxWidth)
    delay: recipe.delayMs
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    topPadding: recipe.paddingBlock
    bottomPadding: recipe.paddingBlock
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    contentItem: Text {
        text: tip.text
        font: tip.font
        color: tip.recipe.text
        wrapMode: Text.Wrap
    }

    background: Rectangle {
        radius: tip.recipe.radius
        color: tip.recipe.background
    }
}
