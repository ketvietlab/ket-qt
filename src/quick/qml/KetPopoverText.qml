// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Text in a KetPopover body: a section `heading`, a muted wrapped `note` or `body` text,
// as ket::Popover::addHeading() and addNote() style them.
Text {
    id: label

    property string role: "note"
    readonly property var recipe: KetTheme.popoverRecipe(KetTheme.dark, KetTheme.zoom)

    width: parent ? parent.width : implicitWidth
    wrapMode: role === "heading" ? Text.NoWrap : Text.WordWrap
    elide: role === "heading" ? Text.ElideRight : Text.ElideNone
    color: role === "note" ? recipe.muted : role === "heading" ? recipe.title : recipe.text
    font.family: KetTheme.fontFamily
    font.pixelSize: role === "note" ? recipe.notePixels
                  : role === "heading" ? recipe.titlePixels : recipe.bodyPixels
    font.weight: role === "heading" ? recipe.titleWeight : Font.Normal
    lineHeightMode: Text.FixedHeight
    lineHeight: Math.floor(font.pixelSize * (role === "note" ? recipe.noteLineHeight : 1.5))
}
