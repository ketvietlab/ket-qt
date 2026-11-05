// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A labelled choice among ordered `steps` (`.effort-picker`), as ket::StepSlider draws it.
Column {
    id: picker

    property string label
    property var steps: []
    property int currentIndex: 0
    readonly property string currentStep: steps.length > 0 ? steps[currentIndex] : ""
    readonly property var recipe: KetTheme.sliderRecipe(KetTheme.dark, KetTheme.zoom,
                                                        false, false, true, false)

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

    KetSlider {
        width: picker.width
        from: 0
        to: Math.max(0, picker.steps.length - 1)
        stepSize: 1
        snapMode: KetSlider.SnapAlways
        enabled: picker.steps.length > 1
        value: picker.currentIndex
        onMoved: picker.currentIndex = Math.round(value)
        Accessible.name: picker.label
        Accessible.description: picker.currentStep
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
