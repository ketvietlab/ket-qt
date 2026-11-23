// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A labelled choice among ordered `steps` (`.effort-picker`), as ket::StepSlider draws it: the
// label and current step, a notched track and the first and last steps. The track is a pill
// holding a rounded track with one dot per step; the chosen step is a ringed thumb and the
// track fills up to it. A click or drag picks the nearest step (ket::stepNotchNearest), Left
// and Right move one step, and the focus ring follows keyboard focus only. It is disabled with
// fewer than two steps. `previewState` pins the track's state for the kitchensink.
Column {
    id: picker

    property string label
    property var steps: []
    property int currentIndex: 0
    property var previewState: null
    readonly property string currentStep: steps.length > 0 ? steps[currentIndex] : ""
    readonly property bool interactive: steps.length > 1
    readonly property var recipe: previewState
        ? KetTheme.sliderRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                !!previewState.pressed,
                                previewState.enabled !== false && interactive,
                                !!previewState.focused)
        : KetTheme.sliderRecipe(KetTheme.dark, KetTheme.zoom, notches.hovered,
                                pointer.pressed, interactive, notches.visualFocus)

    function centre(index) {
        return KetTheme.stepNotchCentre(notches.width, recipe.notchInset, steps.length, index)
    }
    function choose(index) {
        const last = Math.max(0, steps.length - 1)
        const next = Math.max(0, Math.min(last, index))
        if (next !== currentIndex)
            currentIndex = next
    }

    width: KetTheme.px(KetTheme.t.space16 * 4, KetTheme.zoom)

    Item {
        width: picker.width
        height: labelText.implicitHeight + picker.recipe.headerGap

        Text {
            id: labelText
            text: picker.label
            color: picker.recipe.label
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.labelPixels
            font.weight: picker.recipe.labelWeight
        }

        Text {
            anchors.right: parent.right
            text: picker.currentStep
            color: picker.recipe.value
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.labelPixels
        }
    }

    Basic.Control {
        id: notches
        objectName: "stepSliderTrack"

        readonly property real start: picker.recipe.notchInset - picker.recipe.notchTrackHeight / 2

        width: picker.width
        height: picker.recipe.notchHeight
        padding: 0
        enabled: picker.interactive
        hoverEnabled: true
        focusPolicy: Qt.StrongFocus
        opacity: picker.recipe.opacity
        Accessible.role: Accessible.Slider
        Accessible.name: picker.label
        Accessible.description: picker.currentStep
        Keys.onLeftPressed: picker.choose(picker.currentIndex - 1)
        Keys.onRightPressed: picker.choose(picker.currentIndex + 1)
        Keys.onPressed: event => {
            if (event.key === Qt.Key_Home) {
                picker.choose(0)
                event.accepted = true
            } else if (event.key === Qt.Key_End) {
                picker.choose(picker.steps.length - 1)
                event.accepted = true
            }
        }

        background: Rectangle {
            radius: height / 2
            color: picker.recipe.notchPill
        }

        contentItem: Item {
            // The track reaches half its height past the end steps, so their dots sit in its
            // round caps.
            Rectangle {
                x: notches.start
                y: (parent.height - height) / 2
                width: Math.max(0, parent.width - notches.start * 2)
                height: picker.recipe.notchTrackHeight
                radius: height / 2
                color: picker.recipe.track
            }

            Rectangle {
                objectName: "stepSliderFill"
                x: notches.start
                y: (parent.height - height) / 2
                width: Math.max(0, Math.min(parent.width - notches.start * 2,
                                            picker.centre(picker.currentIndex) - notches.start
                                                + picker.recipe.notchTrackHeight / 2))
                height: picker.recipe.notchTrackHeight
                radius: height / 2
                color: picker.recipe.fill
            }

            Repeater {
                model: picker.steps.length

                Item {
                    required property int index
                    readonly property bool chosen: index === picker.currentIndex

                    objectName: "stepSliderNotch" + index
                    width: picker.recipe.notchThumbSize
                    height: picker.recipe.notchThumbSize
                    x: picker.centre(index) - width / 2
                    y: (notches.height - height) / 2

                    Rectangle {
                        anchors.fill: parent
                        visible: parent.chosen
                        radius: width / 2
                        color: picker.recipe.handle
                        border.width: picker.recipe.notchRingWidth
                        border.color: picker.recipe.handleBorder
                    }

                    Rectangle {
                        anchors.centerIn: parent
                        width: picker.recipe.notchDotSize
                        height: picker.recipe.notchDotSize
                        radius: width / 2
                        color: parent.chosen ? picker.recipe.fill : picker.recipe.notch
                    }
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: picker.recipe.focusRingWidth / 2
                visible: picker.recipe.focusRing.a > 0
                radius: height / 2
                color: "transparent"
                border.width: picker.recipe.focusRingWidth
                border.color: picker.recipe.focusRing
            }

            MouseArea {
                id: pointer
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                function snap(x) {
                    picker.choose(KetTheme.stepNotchNearest(x, notches.width,
                                                            picker.recipe.notchInset,
                                                            picker.steps.length))
                }
                onPressed: mouse => {
                    notches.forceActiveFocus(Qt.MouseFocusReason)
                    snap(mouse.x)
                }
                onPositionChanged: mouse => { if (pressed) snap(mouse.x) }
            }
        }
    }

    Item {
        width: picker.width
        height: first.implicitHeight + picker.recipe.labelsGap

        Text {
            id: first
            y: picker.recipe.labelsGap
            text: picker.steps.length > 0 ? picker.steps[0] : ""
            color: picker.recipe.rangeLabel
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.rangePixels
        }

        Text {
            anchors.right: parent.right
            y: picker.recipe.labelsGap
            text: picker.steps.length > 1 ? picker.steps[picker.steps.length - 1] : ""
            color: picker.recipe.rangeLabel
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.rangePixels
        }
    }
}
