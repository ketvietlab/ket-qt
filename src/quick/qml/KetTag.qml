// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Label-only chip tinted with an app-owned colour; it draws ket::tagRecipe, as ket::Tag
// does. A filterable tag is clickable; an overflow tag is the muted "+N" chip.
Rectangle {
    id: tag

    property string text
    property color tagColor: "transparent"
    property bool overflow: false
    property bool filterable: false
    // true or false pins the hover state for documentation pages; null follows the pointer.
    property var previewHovered: null
    readonly property bool hovered: previewHovered !== null ? !!previewHovered
                                                             : filterable && mouse.containsMouse
    readonly property var recipe: KetTheme.tagRecipe(KetTheme.dark, KetTheme.zoom, tagColor,
                                                     overflow, hovered)

    signal clicked()

    implicitHeight: recipe.height
    implicitWidth: Math.min(label.implicitWidth + 2 * recipe.paddingInline, recipe.maxWidth)
    radius: recipe.radius
    color: recipe.background

    Text {
        id: label
        anchors.fill: parent
        anchors.leftMargin: tag.recipe.paddingInline
        anchors.rightMargin: tag.recipe.paddingInline
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: tag.text
        elide: Text.ElideRight
        color: tag.recipe.text
        font.family: KetTheme.fontFamily
        font.pixelSize: tag.recipe.fontPixels
        font.weight: tag.recipe.fontWeight
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        enabled: tag.filterable
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: tag.clicked()
    }
}
