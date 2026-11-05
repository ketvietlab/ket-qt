// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// An assistant message (`.assistant-message`); it draws ket::assistantMessageRecipe, as
// ket::AssistantMessage does. Children form the body; `text` adds a KetReadingText body.
// The avatar defaults to a Két Symbol `bot` tile; set `avatarSource` for the app's mark.
Item {
    id: message

    property string name
    property string time
    property string text
    property url avatarSource
    default property alias body: bodyColumn.data
    readonly property var recipe: KetTheme.assistantMessageRecipe(KetTheme.dark, KetTheme.zoom)

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 8, KetTheme.zoom)
    implicitHeight: Math.max(avatar.height, column.implicitHeight) + recipe.spacingAfter

    Rectangle {
        id: avatar
        width: message.recipe.avatarSize
        height: width
        radius: message.recipe.avatarRadius
        color: message.avatarSource.toString() === "" ? message.recipe.avatarBackground : "transparent"
        clip: true

        KetIcon {
            anchors.centerIn: parent
            visible: message.avatarSource.toString() === ""
            name: "bot"
            box: message.recipe.avatarIconSize
            color: message.recipe.avatarIcon
        }

        Image {
            anchors.fill: parent
            visible: message.avatarSource.toString() !== ""
            source: message.avatarSource
            sourceSize: Qt.size(width, height)
        }
    }

    ColumnLayout {
        id: column
        x: avatar.width + message.recipe.spacing
        width: message.width - x
        spacing: 0

        Row {
            Layout.bottomMargin: message.recipe.metaBottom
            spacing: message.recipe.metaGap

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: message.name
                color: message.recipe.name
                font.family: KetTheme.fontFamily
                font.pixelSize: message.recipe.namePixels
                font.weight: message.recipe.nameWeight
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                visible: message.time !== ""
                text: message.time
                color: message.recipe.time
                font.family: KetTheme.fontFamily
                font.pixelSize: message.recipe.timePixels
            }
        }

        KetReadingText {
            Layout.fillWidth: true
            visible: message.text !== ""
            text: message.text
        }

        ColumnLayout {
            id: bodyColumn
            Layout.fillWidth: true
            spacing: 0
        }
    }
}
