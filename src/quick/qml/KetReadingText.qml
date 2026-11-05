// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Plain wrapped reading text with the transcript's line height, as ket::ReadingText draws.
Text {
    readonly property var recipe: KetTheme.assistantMessageRecipe(KetTheme.dark, KetTheme.zoom)

    wrapMode: Text.Wrap
    textFormat: Text.PlainText
    color: recipe.body
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.bodyPixels
    // A CSS line box: font size × line height (Text's proportional mode scales the font's
    // line spacing instead), as ket::ReadingText lays it out.
    lineHeightMode: Text.FixedHeight
    lineHeight: Math.floor(recipe.bodyPixels * recipe.lineHeight)
}
