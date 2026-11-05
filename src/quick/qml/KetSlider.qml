// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A horizontal slider, as ket::Style draws QSlider (ket::sliderRecipe): a rounded track
// filled up to a round handle.
Basic.Slider {
    id: control

    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.sliderRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                !!previewState.pressed, previewState.enabled !== false,
                                !!previewState.focused)
        : KetTheme.sliderRecipe(KetTheme.dark, KetTheme.zoom, hovered, pressed, enabled,
                                visualFocus)

    hoverEnabled: true
    padding: 0
    opacity: recipe.opacity
    implicitWidth: KetTheme.px(KetTheme.t.space16 * 3, KetTheme.zoom)
    implicitHeight: recipe.height

    background: Item {
        x: control.leftPadding + control.recipe.handleSize / 2
        width: control.availableWidth - control.recipe.handleSize
        height: control.height

        Rectangle {
            y: (parent.height - height) / 2
            width: parent.width
            height: control.recipe.trackHeight
            radius: height / 2
            color: control.recipe.track
        }

        Rectangle {
            y: (parent.height - height) / 2
            width: control.visualPosition * parent.width
            height: control.recipe.trackHeight
            radius: height / 2
            color: control.recipe.fill
        }
    }

    handle: Item {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: (control.height - height) / 2
        width: control.recipe.handleSize
        height: width

        Rectangle {
            anchors.fill: parent
            anchors.margins: -2
            visible: control.recipe.focusRing.a > 0
            radius: width / 2
            color: "transparent"
            border.width: 2
            border.color: control.recipe.focusRing
        }

        Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: control.recipe.handle
            border.width: control.recipe.handleBorderWidth
            border.color: control.recipe.handleBorder
        }
    }
}
