// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A bordered option with a checkbox or radio, a title and an optional description
// (`.choice`); it draws ket::choiceRecipe, as ket::Choice does.
Basic.AbstractButton {
    id: control

    property string description
    property int kind: KetTheme.ToggleKind.Checkbox
    property var previewState: null
    readonly property bool liveHovered: previewState ? !!previewState.hovered : hovered
    readonly property bool liveEnabled: previewState ? previewState.enabled !== false : enabled
    readonly property var recipe: KetTheme.choiceRecipe(
        KetTheme.dark, KetTheme.zoom, checked, liveHovered,
        previewState ? !!previewState.pressed : down, liveEnabled,
        previewState ? !!previewState.focused : visualFocus)
    // The card shows focus; the indicator inside does not repeat it.
    readonly property var toggle: KetTheme.toggleRecipe(
        KetTheme.dark, KetTheme.zoom, kind, checked, liveHovered,
        previewState ? !!previewState.pressed : down, liveEnabled, false)

    checkable: true
    autoExclusive: kind === KetTheme.ToggleKind.Radio
    hoverEnabled: true
    opacity: recipe.opacity
    padding: recipe.padding
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    contentItem: Row {
        spacing: control.recipe.spacing

        KetToggleIndicator {
            anchors.verticalCenter: parent.verticalCenter
            recipe: control.toggle
            kind: control.kind
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: Math.max(0, control.availableWidth - control.toggle.size - control.recipe.spacing)

            Text {
                width: parent.width
                text: control.text
                wrapMode: Text.WordWrap
                color: control.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.titlePixels
                font.weight: control.recipe.titleWeight
            }
            Text {
                width: parent.width
                visible: control.description !== ""
                text: control.description
                wrapMode: Text.WordWrap
                color: control.recipe.description
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.descriptionPixels
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 1
        border.color: control.recipe.focusRing.a > 0 ? control.recipe.focusRing
                                                     : control.recipe.border
    }
}
