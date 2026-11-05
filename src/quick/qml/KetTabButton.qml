// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// One tab in a KetTabBar (`.file-tabs > a`); it draws ket::tabRecipe. `closable` adds a
// close button that emits `closeRequested`.
Basic.TabButton {
    id: control

    property string iconName
    property bool closable: false
    property var previewState: null
    readonly property var barRecipe: KetTheme.tabBarRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property var recipe: previewState
        ? KetTheme.tabRecipe(KetTheme.dark, KetTheme.zoom, checked, !!previewState.hovered,
                             previewState.enabled !== false, !!previewState.focused)
        : KetTheme.tabRecipe(KetTheme.dark, KetTheme.zoom, checked, hovered, enabled, visualFocus)

    signal closeRequested()

    width: implicitWidth
    implicitWidth: leftPadding + rightPadding + row.implicitWidth
    implicitHeight: recipe.height
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    hoverEnabled: true
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    contentItem: Item {
        implicitWidth: row.implicitWidth
        implicitHeight: row.implicitHeight

        Row {
            id: row
            anchors.verticalCenter: parent.verticalCenter
            spacing: control.recipe.spacing

            KetIcon {
                anchors.verticalCenter: parent.verticalCenter
                visible: control.iconName !== ""
                name: control.iconName
                box: control.recipe.iconSize
                color: control.recipe.text
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: control.text
                font: control.font
                color: control.recipe.text
            }

            Item {
                visible: control.closable
                anchors.verticalCenter: parent.verticalCenter
                width: KetTheme.px(KetTheme.t.space5, KetTheme.zoom)
                height: width

                Rectangle {
                    anchors.fill: parent
                    radius: KetTheme.px(KetTheme.t.radiusSm, KetTheme.zoom)
                    color: closeArea.containsMouse ? KetTheme.t.surfaceHover : "transparent"
                }

                KetIcon {
                    anchors.centerIn: parent
                    name: "close"
                    box: control.recipe.iconSize
                    color: control.recipe.text
                }

                MouseArea {
                    id: closeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: control.closeRequested()
                    Accessible.role: Accessible.Button
                    Accessible.name: qsTr("Close %1").arg(control.text)
                }
            }
        }
    }

    background: Rectangle {
        color: control.recipe.background

        Rectangle {
            anchors.right: parent.right
            width: 1
            height: parent.height
            color: control.barRecipe.border
        }

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: control.checked ? control.recipe.indicatorHeight : 1
            color: control.checked ? control.recipe.indicator : control.barRecipe.border
        }

        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: control.recipe.indicatorHeight
            visible: control.recipe.focusRing.a > 0
            color: "transparent"
            border.width: 2
            border.color: control.recipe.focusRing
        }
    }
}
