// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Underlined tab strip (`.file-tabs`); it draws ket::tabBarRecipe, as ket::Style draws
// QTabBar. Fill it with KetTabButton.
Basic.TabBar {
    id: control

    readonly property var recipe: KetTheme.tabBarRecipe(KetTheme.dark, KetTheme.zoom)

    implicitHeight: recipe.height
    spacing: 0

    contentItem: ListView {
        model: control.contentModel
        currentIndex: control.currentIndex
        spacing: control.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem
        highlightMoveDuration: 0
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: 40
        preferredHighlightEnd: width - 40
    }

    background: Rectangle {
        color: control.recipe.background

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: control.recipe.border
        }
    }
}
