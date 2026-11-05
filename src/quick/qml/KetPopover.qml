// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// An anchored panel (`.usage-popover`), as ket::Popover draws it: a title with a close
// button, the body (children) and an optional right-aligned `footer`. openAbove(anchor)
// places it above the anchor, right-aligned; Esc and a press outside close it. Use
// KetPopoverText and KetPopoverDivider for the body's type.
Basic.Popup {
    id: popover

    property string title
    default property alias content: body.data
    property alias footer: footerRow.data
    property Item anchorItem: null
    readonly property var recipe: KetTheme.popoverRecipe(KetTheme.dark, KetTheme.zoom)

    function openAbove(anchor) {
        anchorItem = anchor
        place()
        open()
    }

    function place() {
        if (!anchorItem || !parent)
            return
        const corner = parent.mapFromItem(anchorItem, anchorItem.width, 0)
        x = Math.max(0, corner.x - width)
        y = Math.max(0, corner.y - recipe.offset - height)
    }

    width: recipe.width
    padding: recipe.padding + 1
    closePolicy: Basic.Popup.CloseOnEscape | Basic.Popup.CloseOnPressOutsideParent
    focus: true
    enter: null
    exit: null
    onHeightChanged: place()

    contentItem: Column {
        spacing: 0
        Accessible.role: Accessible.Pane
        Accessible.name: popover.title

        Item {
            width: parent.width
            height: Math.max(titleText.implicitHeight, close.implicitHeight) + popover.recipe.headerGap

            Text {
                id: titleText
                width: parent.width - close.width
                text: popover.title
                elide: Text.ElideRight
                color: popover.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: popover.recipe.titlePixels
                font.weight: popover.recipe.titleWeight
            }

            KetIconButton {
                id: close
                anchors.right: parent.right
                controlSize: KetTheme.ControlSize.Small
                iconName: "close"
                text: qsTr("Close")
                onClicked: popover.close()
            }
        }

        Column {
            id: body
            width: parent.width
        }

        Item {
            width: parent.width
            visible: footerRow.children.length > 0
            height: visible ? footerRow.implicitHeight + popover.recipe.footerGap : 0

            Row {
                id: footerRow
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)
            }
        }
    }

    background: Item {
        KetShadow {
            anchors.fill: parent
            radius: popover.recipe.radius
            shadowX: popover.recipe.shadowX
            shadowY: popover.recipe.shadowY
            shadowBlur: popover.recipe.shadowBlur
            shadowColor: popover.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: popover.recipe.radius
            color: popover.recipe.background
            border.width: 1
            border.color: popover.recipe.border
        }
    }
}
