// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A fenced code block: monospace text on a subtle rounded surface with a copy button, as
// ket::CodeBlock draws it. Long lines wrap.
Item {
    id: block

    property string code
    property string language
    property bool copied: false
    readonly property var recipe: KetTheme.codeBlockRecipe(KetTheme.dark, KetTheme.zoom)

    signal copiedToClipboard()

    function copy() {
        clipboard.text = code
        clipboard.selectAll()
        clipboard.copy()
        copied = true
        reset.restart()
        copiedToClipboard()
    }

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 8, KetTheme.zoom)
    implicitHeight: surface.height + 2 * recipe.margin
    Accessible.role: Accessible.StaticText
    Accessible.name: language !== "" ? qsTr("%1 code").arg(language) : qsTr("Code")

    // A hidden editor carries the text to the clipboard (QML has no clipboard API).
    TextEdit { id: clipboard; visible: false }

    Timer { id: reset; interval: 1500; onTriggered: block.copied = false }

    Rectangle {
        id: surface
        y: block.recipe.margin
        width: block.width
        height: codeText.height + 2 * block.recipe.padding
        radius: block.recipe.radius
        color: block.recipe.background
        border.width: 1
        border.color: block.recipe.border

        TextEdit {
            id: codeText
            x: block.recipe.padding
            y: block.recipe.padding
            width: surface.width - 2 * block.recipe.padding - copyButton.width
            readOnly: true
            selectByMouse: true
            text: block.code
            textFormat: TextEdit.PlainText
            wrapMode: TextEdit.WrapAnywhere
            color: block.recipe.text
            selectionColor: KetTheme.t.accentSubtle
            selectedTextColor: KetTheme.t.accentText
            font.family: KetTheme.monoFontFamily
            font.pixelSize: block.recipe.fontPixels
            // Formatting changes the text again, hence the guard.
            property bool formatting: false
            function format() {
                if (formatting)
                    return
                formatting = true
                KetTheme.applyLineHeight(
                    textDocument, Math.floor(block.recipe.fontPixels * block.recipe.lineHeight))
                formatting = false
            }
            Component.onCompleted: format()
            onTextChanged: format()
        }

        KetIconButton {
            id: copyButton
            anchors.right: parent.right
            anchors.rightMargin: Math.round(block.recipe.padding / 2)
            y: Math.round(block.recipe.padding / 2)
            controlSize: KetTheme.ControlSize.Small
            iconName: block.copied ? "check" : "copy"
            text: qsTr("Copy code")
            onClicked: block.copy()
        }
    }
}
