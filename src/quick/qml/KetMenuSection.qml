// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A muted group heading inside a KetMenu, such as the composer's "Mode"; ket::Style draws
// QMenu::addSection the same way. It is not a command: keyboard navigation skips it.
Basic.MenuSeparator {
    id: section

    property string text
    readonly property var recipe: KetTheme.menuRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property var item: KetTheme.menuItemRecipe(KetTheme.dark, KetTheme.zoom, false, true, 0)

    padding: 0
    leftPadding: item.paddingStart
    rightPadding: item.paddingEnd
    implicitHeight: recipe.sectionHeight
    Accessible.role: Accessible.StaticText
    Accessible.name: text

    contentItem: Text {
        text: section.text
        color: section.recipe.sectionText
        font.family: KetTheme.fontFamily
        font.pixelSize: section.recipe.sectionPixels
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
