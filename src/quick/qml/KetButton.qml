// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Controls.impl as Impl
import Ket

// Design-system button for Qt Quick; it draws ket::buttonRecipe, as ket::Button does.
// Icons are tinted with the text colour, like the mock's `currentColor` icons.
Basic.Button {
    id: control

    property int variant: KetTheme.ButtonVariant.Secondary
    property int controlSize: KetTheme.ControlSize.Medium
    // A fixed state such as { hovered: true } for documentation pages and screenshot tests;
    // null follows the live interaction state.
    property var previewState: null
    // A Két Symbol code drawn by KetIcon; icon.source still takes any other monochrome icon.
    property string iconName
    property int iconWeight: KetTheme.IconWeight.Light
    readonly property bool hasIcon: iconName !== "" || icon.source.toString() !== ""
    readonly property var recipe: previewState
        ? KetTheme.buttonRecipe(KetTheme.dark, KetTheme.zoom, variant, controlSize,
                                !!previewState.hovered, !!previewState.pressed,
                                previewState.enabled !== false, !!previewState.focused)
        : KetTheme.buttonRecipe(KetTheme.dark, KetTheme.zoom, variant, controlSize,
                                hovered, down, enabled, visualFocus)

    implicitHeight: recipe.height
    implicitWidth: Math.max(implicitHeight, implicitContentWidth + leftPadding + rightPadding)
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    topPadding: 0
    bottomPadding: 0
    spacing: recipe.spacing
    opacity: recipe.opacity
    hoverEnabled: true
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels
    font.weight: recipe.fontWeight
    icon.width: recipe.iconSize
    icon.height: recipe.iconSize

    contentItem: Row {
        spacing: control.hasIcon && control.text !== "" ? control.spacing : 0

        KetIcon {
            anchors.verticalCenter: parent.verticalCenter
            visible: control.iconName !== ""
            name: control.iconName
            weight: control.iconWeight
            box: control.recipe.iconSize
            color: control.recipe.text
        }

        Impl.IconImage {
            anchors.verticalCenter: parent.verticalCenter
            visible: control.iconName === "" && control.icon.source.toString() !== ""
            source: control.icon.source
            sourceSize.width: control.icon.width
            sourceSize.height: control.icon.height
            width: visible ? control.icon.width : 0
            height: control.icon.height
            color: control.recipe.text
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            visible: control.text !== ""
            text: control.text
            font: control.font
            color: control.recipe.text
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.border.a > 0 ? 1 : 0
        border.color: control.recipe.border

        Rectangle {
            anchors.fill: parent
            visible: control.recipe.focusRing.a > 0
            radius: parent.radius
            color: "transparent"
            border.width: 2
            border.color: control.recipe.focusRing
        }
    }
}
