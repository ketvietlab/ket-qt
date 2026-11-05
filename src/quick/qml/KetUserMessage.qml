// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// The user's message (`.user-bubble`); it draws ket::userMessageRecipe, as ket::UserMessage
// does: an accent bubble at the trailing edge, at most the recipe's share of the width.
Item {
    id: message

    property string text
    readonly property var recipe: KetTheme.userMessageRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property alias bubble: bubble

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 8, KetTheme.zoom)
    implicitHeight: bubble.height + recipe.spacingAfter
    Accessible.role: Accessible.StaticText
    Accessible.name: text

    Rectangle {
        id: bubble
        anchors.right: parent.right
        width: label.width + 2 * message.recipe.paddingInline
        height: label.height + 2 * message.recipe.paddingBlock
        radius: message.recipe.radius
        color: message.recipe.background

        Text {
            id: label
            x: message.recipe.paddingInline
            y: message.recipe.paddingBlock
            width: Math.min(implicitWidth + 1,
                            message.width * message.recipe.maxWidth - 2 * message.recipe.paddingInline)
            text: message.text
            textFormat: Text.PlainText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: message.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: message.recipe.fontPixels
            // A CSS line box, as in KetReadingText.
            lineHeightMode: Text.FixedHeight
            lineHeight: Math.floor(message.recipe.fontPixels * message.recipe.lineHeight)
        }
    }
}
