// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A theme choice in Settings (`.theme-card`), as ket::ThemeCard draws it: a miniature of
// the window in the colours of the `previewDark` theme and the label with a check. The card
// is checkable; put cards in a ButtonGroup.
Basic.AbstractButton {
    id: control

    property bool previewDark: false
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.themeCardRecipe(KetTheme.dark, KetTheme.zoom, checked, previewDark,
                                   !!previewState.hovered, !!previewState.pressed,
                                   previewState.enabled !== false, !!previewState.focused)
        : KetTheme.themeCardRecipe(KetTheme.dark, KetTheme.zoom, checked, previewDark, hovered,
                                   down, enabled, visualFocus)
    // The miniature's sketch follows the mock; it is an illustration, not product chrome.
    readonly property real unit: KetTheme.zoom

    hoverEnabled: true
    checkable: true
    opacity: recipe.opacity
    implicitWidth: recipe.miniSideWidth * 5
    implicitHeight: 2 + recipe.miniMargin + recipe.miniHeight
                    + Math.max(label.implicitHeight, recipe.checkSize)
                    + 2 * recipe.labelPaddingBlock
    Accessible.role: Accessible.RadioButton
    Accessible.name: text

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        readonly property color ring: control.recipe.focusRing.a > 0 ? control.recipe.focusRing
                                                                     : control.recipe.ring
        border.width: ring.a > 0 ? 2 : 1
        border.color: ring.a > 0 ? ring : control.recipe.border
    }

    contentItem: Item {
        Rectangle {
            id: mini
            x: 1 + control.recipe.miniMargin
            y: 1 + control.recipe.miniMargin
            width: parent.width - 2 * x
            height: control.recipe.miniHeight
            radius: 4 * control.unit
            color: control.recipe.miniBackground
            clip: true

            Rectangle {
                id: side
                width: control.recipe.miniSideWidth
                height: parent.height
                color: control.recipe.miniSidebar

                Rectangle {
                    anchors.right: parent.right
                    width: 1
                    height: parent.height
                    color: control.recipe.miniBorder
                }

                Column {
                    x: 7 * control.unit
                    y: 15 * control.unit
                    width: parent.width - 1 - 2 * x
                    spacing: 11 * control.unit

                    Repeater {
                        model: 3

                        Rectangle {
                            required property int index
                            width: parent.width
                            height: 4 * control.unit
                            radius: 3 * control.unit
                            color: index === 1 ? control.recipe.miniAccentBar
                                               : control.recipe.miniBar
                        }
                    }
                }
            }

            Column {
                id: main
                x: side.width + 13 * control.unit
                y: 18 * control.unit
                width: parent.width - x - 13 * control.unit

                Rectangle {
                    width: main.width * 0.6
                    height: 5 * control.unit
                    radius: 2 * control.unit
                    color: control.recipe.miniTitle
                }

                Item { width: 1; height: 8 * control.unit }

                Rectangle {
                    width: main.width * 0.8
                    height: 3 * control.unit
                    color: control.recipe.miniLine
                }

                Item { width: 1; height: 13 * control.unit }

                Row {
                    spacing: 6 * control.unit

                    Repeater {
                        model: 2

                        Rectangle {
                            width: (main.width - 6 * control.unit) / 2
                            height: 25 * control.unit
                            radius: 3 * control.unit
                            color: control.recipe.miniButton
                            border.color: control.recipe.miniButtonBorder
                        }
                    }
                }

                Item { width: 1; height: 13 * control.unit }

                Rectangle {
                    width: main.width * 0.8
                    height: 3 * control.unit
                    color: control.recipe.miniLine
                }
            }

            Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
                border.color: control.recipe.miniBorder
            }
        }

        Text {
            id: label
            x: 1 + control.recipe.labelPaddingInline
            y: mini.y + mini.height + (parent.height - mini.y - mini.height - 1 - height) / 2
            text: control.text
            color: control.recipe.label
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.labelPixels
        }

        KetIcon {
            anchors.right: parent.right
            anchors.rightMargin: 1 + control.recipe.labelPaddingInline
            y: label.y + (label.height - height) / 2
            name: control.checked ? "success" : "circle"
            box: control.recipe.checkSize
            color: control.recipe.check
        }
    }
}
