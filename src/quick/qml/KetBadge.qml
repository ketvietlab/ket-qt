// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Non-interactive status label; it draws ket::badgeRecipe, as ket::Badge does.
Rectangle {
    id: badge

    property string text
    property int tone: KetTheme.BadgeTone.Neutral
    readonly property var recipe: KetTheme.badgeRecipe(KetTheme.dark, KetTheme.zoom, tone)

    implicitHeight: recipe.height
    implicitWidth: label.implicitWidth + 2 * recipe.paddingInline
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border

    Text {
        id: label
        anchors.centerIn: parent
        text: badge.text
        color: badge.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: badge.recipe.fontPixels
        font.weight: badge.recipe.fontWeight
    }
}
