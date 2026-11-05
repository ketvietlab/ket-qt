// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// One muted item in a KetStatusBar: an optional Két icon and its text.
Row {
    id: item

    property string text
    property string iconName
    readonly property var recipe: KetTheme.statusBarRecipe(KetTheme.dark, KetTheme.zoom)

    spacing: recipe.itemGap
    Accessible.role: Accessible.StaticText
    Accessible.name: text

    KetIcon {
        anchors.verticalCenter: parent.verticalCenter
        visible: item.iconName !== ""
        name: item.iconName
        box: item.recipe.iconSize
        color: item.recipe.text
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        visible: item.text !== ""
        text: item.text
        color: item.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: item.recipe.fontPixels
    }
}
