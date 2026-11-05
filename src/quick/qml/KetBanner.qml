// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// Inline notice (`.banner`); it draws ket::bannerRecipe, as ket::Banner does. Set
// `actionText` to show the trailing button; `actionTriggered` reports a click.
Rectangle {
    id: banner

    property int tone: KetTheme.BannerTone.Info
    property string title
    property string message
    property string actionText
    property string actionIcon
    readonly property var recipe: KetTheme.bannerRecipe(KetTheme.dark, KetTheme.zoom, tone)

    signal actionTriggered()

    implicitWidth: row.implicitWidth + 2 * recipe.paddingInline
    implicitHeight: row.implicitHeight + 2 * recipe.paddingBlock
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border
    Accessible.role: Accessible.AlertMessage
    Accessible.name: title

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.leftMargin: banner.recipe.paddingInline
        anchors.rightMargin: banner.recipe.paddingInline
        anchors.topMargin: banner.recipe.paddingBlock
        anchors.bottomMargin: banner.recipe.paddingBlock
        spacing: banner.recipe.spacing

        KetIcon {
            Layout.alignment: Qt.AlignTop
            name: banner.recipe.icon
            box: banner.recipe.iconSize
            color: banner.recipe.text
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: banner.recipe.messageGap

            Text {
                Layout.fillWidth: true
                text: banner.title
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: banner.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: banner.recipe.titlePixels
                font.weight: banner.recipe.titleWeight
            }

            Text {
                Layout.fillWidth: true
                visible: banner.message !== ""
                text: banner.message
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: banner.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: banner.recipe.messagePixels
            }
        }

        KetButton {
            Layout.alignment: Qt.AlignVCenter
            visible: banner.actionText !== ""
            text: banner.actionText
            iconName: banner.actionIcon
            controlSize: KetTheme.ControlSize.Small
            onClicked: banner.actionTriggered()
        }
    }
}
