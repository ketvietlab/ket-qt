// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Page-level tabs (`.route-tabs`), as ket::RouteTabs draws them: `tabs` is a list of labels.
// Arrow keys move between tabs.
Item {
    id: routeTabs

    property var tabs: []
    property int currentIndex: 0
    readonly property var recipe: KetTheme.routeTabRecipe(KetTheme.dark, KetTheme.zoom, false,
                                                          false, false, true, false)

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: routeTabs.recipe.rule
    }

    Row {
        id: row
        spacing: routeTabs.recipe.spacing

        Repeater {
            id: repeater
            model: routeTabs.tabs

            Basic.AbstractButton {
                id: tab
                required property string modelData
                required property int index
                readonly property var recipe: KetTheme.routeTabRecipe(
                    KetTheme.dark, KetTheme.zoom, index === routeTabs.currentIndex, hovered, down,
                    enabled, visualFocus)
                text: modelData
                hoverEnabled: true
                opacity: recipe.opacity
                implicitWidth: label.implicitWidth
                implicitHeight: recipe.paddingTop + label.implicitHeight + recipe.paddingBottom
                                + recipe.indicatorHeight
                Accessible.role: Accessible.PageTab
                Accessible.selected: index === routeTabs.currentIndex
                onClicked: routeTabs.currentIndex = index
                Keys.onRightPressed: move(1)
                Keys.onLeftPressed: move(-1)

                function move(step) {
                    const next = Math.max(0, Math.min(routeTabs.tabs.length - 1, routeTabs.currentIndex + step))
                    routeTabs.currentIndex = next
                    repeater.itemAt(next).forceActiveFocus(Qt.TabFocusReason)
                }

                contentItem: Item {
                    Text {
                        id: label
                        y: tab.recipe.paddingTop
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: tab.text
                        color: tab.recipe.text
                        font.family: KetTheme.fontFamily
                        font.pixelSize: tab.recipe.fontPixels
                    }
                }

                background: Item {
                    Rectangle {
                        anchors.fill: parent
                        visible: tab.recipe.focusRing.a > 0
                        radius: 3
                        color: "transparent"
                        border.width: 2
                        border.color: tab.recipe.focusRing
                    }

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: tab.recipe.indicatorHeight
                        color: tab.recipe.indicator
                    }
                }
            }
        }
    }
}
