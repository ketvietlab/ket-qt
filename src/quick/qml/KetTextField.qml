// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Single-line text field (`input`); it draws ket::fieldRecipe, as ket::TextField does.
// `leadingIcon: "search"` makes the search field.
Basic.TextField {
    id: control

    property string leadingIcon
    property bool invalid: false
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                               !!previewState.focused, previewState.enabled !== false, invalid)
        : KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, hovered, activeFocus, enabled,
                               invalid)

    implicitHeight: recipe.height
    implicitWidth: 10 * recipe.height
    leftPadding: recipe.paddingInline + (leadingIcon !== "" ? recipe.iconSize + recipe.spacing : 0)
    rightPadding: recipe.paddingInline
    topPadding: 0
    bottomPadding: 0
    verticalAlignment: TextInput.AlignVCenter
    hoverEnabled: true
    opacity: recipe.opacity
    color: recipe.text
    placeholderTextColor: recipe.placeholder
    selectionColor: KetTheme.t.accentMuted
    selectedTextColor: recipe.text
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.borderWidth
        border.color: control.recipe.border

        KetIcon {
            visible: control.leadingIcon !== ""
            x: control.recipe.paddingInline
            anchors.verticalCenter: parent.verticalCenter
            name: control.leadingIcon
            box: control.recipe.iconSize
            color: control.recipe.icon
        }
    }
}
