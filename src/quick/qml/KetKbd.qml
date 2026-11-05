// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Keyboard shortcut label (`kbd`); it draws ket::kbdRecipe, as ket::Kbd does.
Text {
    readonly property var recipe: KetTheme.kbdRecipe(KetTheme.dark, KetTheme.zoom)

    color: recipe.text
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels
    textFormat: Text.PlainText
    wrapMode: Text.NoWrap
}
