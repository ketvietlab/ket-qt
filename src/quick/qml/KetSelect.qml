// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Drop-down select (`select`); the closed control draws ket::fieldRecipe, as ket::Select does.
Basic.ComboBox {
    id: control

    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                               !!previewState.focused, previewState.enabled !== false, false)
        : KetTheme.fieldRecipe(KetTheme.dark, KetTheme.zoom, hovered, visualFocus, enabled, false)

    implicitHeight: recipe.height
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline + recipe.iconSize + recipe.spacing
    hoverEnabled: true
    opacity: recipe.opacity
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels

    contentItem: Text {
        text: control.displayText
        font: control.font
        color: control.recipe.text
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: KetIcon {
        x: control.width - control.recipe.paddingInline - width
        anchors.verticalCenter: parent.verticalCenter
        name: "chevron-down"
        box: control.recipe.iconSize
        color: control.recipe.icon
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.borderWidth
        border.color: control.recipe.border
    }

    // The open list is a menu (`.desktop-context-menu`), as ket::Style draws QComboBox's.
    readonly property var menuRecipe: KetTheme.menuRecipe(KetTheme.dark, KetTheme.zoom)

    delegate: KetMenuItem {
        required property var model
        required property int index
        width: ListView.view ? ListView.view.width : implicitWidth
        text: model[control.textRole] !== undefined ? model[control.textRole] : modelData
        property var modelData: model.modelData
        checkable: true
        checked: control.currentIndex === index
        highlighted: control.highlightedIndex === index
    }

    popup: Basic.Popup {
        y: control.height + KetTheme.px(4, KetTheme.zoom)
        width: Math.max(control.width, control.menuRecipe.minWidth)
        implicitHeight: Math.min(contentItem.implicitHeight + 2 * padding,
                                 control.Window.height - control.menuRecipe.padding * 4)
        padding: control.menuRecipe.padding
        enter: null
        exit: null

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            boundsBehavior: Flickable.StopAtBounds
        }

        background: Item {
            KetShadow {
                anchors.fill: parent
                radius: control.menuRecipe.radius
                shadowX: control.menuRecipe.shadowX
                shadowY: control.menuRecipe.shadowY
                shadowBlur: control.menuRecipe.shadowBlur
                shadowColor: control.menuRecipe.shadowColor
            }

            Rectangle {
                anchors.fill: parent
                radius: control.menuRecipe.radius
                color: control.menuRecipe.background
                border.width: 1
                border.color: control.menuRecipe.border
            }
        }
    }
}
