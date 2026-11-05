// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Segmented control (`.pill-tabs`); it draws ket::segmentedRecipe and ket::segmentRecipe,
// as ket::SegmentedControl does. Arrow keys, Home and End move the selection.
Basic.Control {
    id: control

    property var options: []
    property int currentIndex: 0
    property bool expanding: false
    property int previewHovered: -1
    readonly property var recipe: KetTheme.segmentedRecipe(KetTheme.dark, KetTheme.zoom)

    signal activated(int index)

    function select(index) {
        if (index < 0 || index >= options.length || index === currentIndex)
            return
        currentIndex = index
        activated(index)
    }

    padding: recipe.padding
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    opacity: enabled ? 1 : KetTheme.segmentRecipe(KetTheme.dark, KetTheme.zoom, false, false,
                                                  false, false).opacity
    Accessible.role: Accessible.PageTabList

    Keys.onLeftPressed: select(Math.max(0, currentIndex - 1))
    Keys.onRightPressed: select(Math.min(options.length - 1, currentIndex + 1))
    Keys.onPressed: event => {
        if (event.key === Qt.Key_Home) { select(0); event.accepted = true }
        else if (event.key === Qt.Key_End) { select(options.length - 1); event.accepted = true }
    }

    contentItem: Row {
        spacing: control.recipe.spacing

        Repeater {
            model: control.options

            Item {
                id: segment
                required property int index
                required property string modelData
                readonly property bool selected: index === control.currentIndex
                readonly property var recipe: KetTheme.segmentRecipe(
                    KetTheme.dark, KetTheme.zoom, selected,
                    area.containsMouse || control.previewHovered === index, control.enabled,
                    control.visualFocus && selected)
                readonly property real natural: label.implicitWidth + 2 * recipe.paddingInline

                implicitWidth: natural
                implicitHeight: label.implicitHeight + 2 * recipe.paddingBlock
                width: control.expanding && control.options.length > 0
                       ? (control.availableWidth - (control.options.length - 1) * control.recipe.spacing)
                         / control.options.length
                       : natural
                height: implicitHeight
                Accessible.role: Accessible.PageTab
                Accessible.name: modelData
                Accessible.selected: selected

                KetShadow {
                    anchors.fill: parent
                    visible: segment.selected
                    radius: segment.recipe.radius
                    shadowX: segment.recipe.shadowX
                    shadowY: segment.recipe.shadowY
                    shadowBlur: segment.recipe.shadowBlur
                    shadowColor: segment.recipe.shadowColor
                }

                Rectangle {
                    anchors.fill: parent
                    radius: segment.recipe.radius
                    color: segment.recipe.background
                    border.width: segment.recipe.focusRing.a > 0 ? 2 : 0
                    border.color: segment.recipe.focusRing
                }

                Text {
                    id: label
                    anchors.centerIn: parent
                    text: segment.modelData
                    color: segment.recipe.text
                    font.family: KetTheme.fontFamily
                    font.pixelSize: segment.recipe.fontPixels
                }

                MouseArea {
                    id: area
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: { control.forceActiveFocus(Qt.MouseFocusReason); control.select(segment.index) }
                }
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: 1
        border.color: control.recipe.border
    }
}
