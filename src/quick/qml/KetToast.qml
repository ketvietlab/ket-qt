// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Transient confirmation (`.toast`) centred near the bottom of the window; it draws
// ket::toastRecipe, as ket::Toast does. Call show(text) to display it.
Basic.Popup {
    id: toast

    property string text
    readonly property var recipe: KetTheme.toastRecipe(KetTheme.dark, KetTheme.zoom)

    function show(message) {
        if (message !== undefined)
            text = message
        open()
        timer.restart()
    }

    parent: Basic.Overlay.overlay
    x: parent ? Math.round((parent.width - width) / 2) : 0
    y: parent ? parent.height - recipe.bottomOffset - height : 0
    closePolicy: Basic.Popup.NoAutoClose
    focus: false
    modal: false
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    topPadding: recipe.paddingBlock
    bottomPadding: recipe.paddingBlock
    enter: null
    exit: null

    Timer {
        id: timer
        interval: toast.recipe.durationMs
        onTriggered: toast.close()
    }

    contentItem: Row {
        spacing: toast.recipe.spacing
        Accessible.role: Accessible.AlertMessage
        Accessible.name: toast.text

        KetIcon {
            anchors.verticalCenter: parent.verticalCenter
            name: "success"
            box: toast.recipe.iconSize
            color: toast.recipe.icon
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: toast.text
            textFormat: Text.PlainText
            color: toast.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: toast.recipe.fontPixels
        }
    }

    background: Item {
        KetShadow {
            anchors.fill: parent
            radius: toast.recipe.radius
            shadowX: toast.recipe.shadowX
            shadowY: toast.recipe.shadowY
            shadowBlur: toast.recipe.shadowBlur
            shadowColor: toast.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: toast.recipe.radius
            color: toast.recipe.background
            border.width: 1
            border.color: toast.recipe.border
        }
    }
}
