// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Modal dialog over the window (`.modal` on `.modal-backdrop`); it draws ket::dialogRecipe,
// as ket::Dialog does. Children form the body; put footer buttons in `actions`.
Basic.Dialog {
    id: dialog

    default property alias body: bodyColumn.data
    property alias actions: actionRow.data
    readonly property var recipe: KetTheme.dialogRecipe(KetTheme.dark, KetTheme.zoom)

    parent: Basic.Overlay.overlay
    modal: true
    focus: true
    closePolicy: Basic.Popup.CloseOnEscape | Basic.Popup.CloseOnPressOutside
    width: parent ? Math.min(recipe.width, parent.width - 2 * recipe.margin) : recipe.width
    height: parent ? Math.min(implicitHeight, parent.height - 2 * recipe.margin) : implicitHeight
    x: parent ? Math.round((parent.width - width) / 2) : 0
    y: parent ? Math.round((parent.height - height) / 2) : 0
    padding: recipe.bodyPadding
    font.family: KetTheme.fontFamily
    enter: null
    exit: null
    // A backdrop click or Escape rejects, as the close button does. done() may close the
    // popup before it emits, so the check runs once the current handlers have finished.
    property bool resultSet: false
    onAboutToShow: resultSet = false
    onAccepted: resultSet = true
    onRejected: resultSet = true
    onClosed: Qt.callLater(() => { if (!resultSet) reject() })

    Basic.Overlay.modal: Rectangle {
        color: dialog.recipe.backdrop
    }

    header: Item {
        implicitHeight: headerRow.implicitHeight + 2 * dialog.recipe.headerPaddingBlock + 1

        RowLayout {
            id: headerRow
            anchors.fill: parent
            anchors.leftMargin: dialog.recipe.paddingInline
            anchors.rightMargin: dialog.recipe.paddingInline
            anchors.topMargin: dialog.recipe.headerPaddingBlock
            anchors.bottomMargin: dialog.recipe.headerPaddingBlock + 1
            spacing: dialog.recipe.footerGap

            Text {
                Layout.fillWidth: true
                text: dialog.title
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: dialog.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: dialog.recipe.titlePixels
                font.weight: dialog.recipe.titleWeight
                Accessible.role: Accessible.Heading
            }

            KetIconButton {
                text: qsTr("Close")
                iconName: "close"
                showToolTip: false
                onClicked: dialog.reject()
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: dialog.recipe.divider
        }
    }

    contentItem: ColumnLayout {
        id: bodyColumn
        spacing: 0
    }

    footer: Rectangle {
        visible: actionRow.children.length > 0
        implicitHeight: visible ? actionRow.implicitHeight + 2 * dialog.recipe.footerPaddingBlock + 1
                                : 0
        color: dialog.recipe.footer
        // Square top edge under the body; the card's rounded clip shapes the bottom.
        bottomLeftRadius: dialog.recipe.radius - 1
        bottomRightRadius: dialog.recipe.radius - 1

        Rectangle {
            width: parent.width
            height: 1
            color: dialog.recipe.divider
        }

        Row {
            id: actionRow
            anchors.right: parent.right
            anchors.rightMargin: dialog.recipe.paddingInline
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 0.5
            spacing: dialog.recipe.footerGap
        }
    }

    background: Item {
        KetShadow {
            anchors.fill: parent
            radius: dialog.recipe.radius
            shadowX: dialog.recipe.shadowX
            shadowY: dialog.recipe.shadowY
            shadowBlur: dialog.recipe.shadowBlur
            shadowColor: dialog.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: dialog.recipe.radius
            color: dialog.recipe.background
            border.width: 1
            border.color: dialog.recipe.border
        }
    }
}
