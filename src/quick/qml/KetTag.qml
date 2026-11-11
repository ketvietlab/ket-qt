// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Label-only chip tinted with an app-owned colour; it draws ket::tagRecipe, as ket::Tag
// does. A filterable tag is clickable; an overflow tag is the muted "+N" chip; a locked tag
// (ket::lockedTagRecipe) reads as fixed, leads with a lock and is never clicked.
Rectangle {
    id: tag

    property string text
    property color tagColor: "transparent"
    property bool overflow: false
    property bool filterable: false
    property bool locked: false
    // true or false pins the hover state for documentation pages; null follows the pointer.
    property var previewHovered: null
    readonly property bool hovered: previewHovered !== null ? !!previewHovered
                                                             : filterable && mouse.containsMouse
    readonly property var recipe: locked && !overflow
        ? KetTheme.lockedTagRecipe(KetTheme.dark, KetTheme.zoom, tagColor)
        : KetTheme.tagRecipe(KetTheme.dark, KetTheme.zoom, tagColor, overflow, hovered)
    readonly property real iconWidth: recipe.iconCode !== "" ? recipe.iconSize + recipe.iconGap : 0

    signal clicked()

    implicitHeight: recipe.height
    // Rows may cap chips tighter than the recipe (`.conversation-tags .conversation-tag`).
    property real maxWidth: recipe.maxWidth
    implicitWidth: Math.min(label.implicitWidth + 2 * recipe.paddingInline + iconWidth, maxWidth)
    Accessible.role: Accessible.StaticText
    Accessible.name: text
    Accessible.description: locked ? qsTr("Locked") : ""

    KetIcon {
        visible: tag.iconWidth > 0
        x: tag.recipe.paddingInline
        anchors.verticalCenter: parent.verticalCenter
        name: tag.recipe.iconCode
        box: tag.recipe.iconSize
        color: tag.recipe.text
    }
    radius: recipe.radius
    color: recipe.background

    Text {
        id: label
        anchors.fill: parent
        anchors.leftMargin: tag.recipe.paddingInline + tag.iconWidth
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
        enabled: tag.filterable && !tag.locked
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: tag.clicked()
    }
}
