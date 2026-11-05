// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Message composer (`.composer`); it draws ket::composerRecipe, as ket::Composer does.
// Enter sends and Shift+Enter breaks the line. Put the app's controls in `leadingTools`
// and `trailingTools`; `busy` adds the stop button.
Item {
    id: composer

    property alias text: input.text
    property alias placeholderText: input.placeholderText
    property bool busy: false
    property alias leadingTools: leading.data
    property alias trailingTools: trailing.data
    readonly property bool focused: input.activeFocus
    readonly property var recipe: KetTheme.composerRecipe(KetTheme.dark, KetTheme.zoom, focused)
    readonly property real lineBox: Math.floor(recipe.textPixels * recipe.lineHeight)

    signal submitted(string text)
    signal stopRequested()

    function submit() {
        if (input.text.trim() === "")
            return
        const message = input.text
        input.clear()
        submitted(message)
    }

    implicitWidth: KetTheme.px(640, KetTheme.zoom)
    implicitHeight: column.implicitHeight + 2 * (recipe.ringWidth + 1 + recipe.padding)

    Rectangle {
        anchors.fill: parent
        visible: composer.recipe.ring.a > 0
        radius: composer.recipe.radius + composer.recipe.ringWidth
        color: composer.recipe.ring
    }

    KetShadow {
        anchors.fill: card
        visible: composer.recipe.ring.a === 0
        radius: composer.recipe.radius
        shadowX: composer.recipe.shadowX
        shadowY: composer.recipe.shadowY
        shadowBlur: composer.recipe.shadowBlur
        shadowColor: composer.recipe.shadowColor
    }

    Rectangle {
        id: card
        anchors.fill: parent
        anchors.margins: composer.recipe.ringWidth
        radius: composer.recipe.radius
        color: composer.recipe.background
        border.width: 1
        border.color: composer.recipe.border
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.margins: composer.recipe.ringWidth + 1 + composer.recipe.padding
        spacing: 0

        RowLayout {
            spacing: KetTheme.px(KetTheme.t.space1, KetTheme.zoom)

            Basic.ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(composer.recipe.minInputHeight,
                    Math.min(input.implicitHeight, composer.recipe.maxLines * composer.lineBox
                             + 2 * composer.recipe.textPaddingBlock))

                Basic.TextArea {
                    id: input
                    wrapMode: TextEdit.Wrap
                    textFormat: TextEdit.PlainText
                    leftPadding: composer.recipe.textPaddingInline
                    rightPadding: composer.recipe.textPaddingInline
                    topPadding: composer.recipe.textPaddingBlock
                    bottomPadding: composer.recipe.textPaddingBlock
                    color: composer.recipe.text
                    placeholderText: qsTr("Ask for follow-up changes")
                    placeholderTextColor: composer.recipe.placeholder
                    font.family: KetTheme.fontFamily
                    font.pixelSize: composer.recipe.textPixels
                    background: null
                    Keys.onReturnPressed: event => {
                        if (event.modifiers & Qt.ShiftModifier)
                            event.accepted = false
                        else
                            composer.submit()
                    }
                    Keys.onEnterPressed: event => {
                        if (event.modifiers & Qt.ShiftModifier)
                            event.accepted = false
                        else
                            composer.submit()
                    }
                    // TextEdit has no line height; the document's blocks carry the recipe's.
                    // Formatting changes the text again, hence the guard.
                    property bool formatting: false
                    onTextChanged: {
                        if (formatting)
                            return
                        formatting = true
                        KetTheme.applyLineHeight(textDocument, composer.lineBox)
                        formatting = false
                    }
                    Component.onCompleted: KetTheme.applyLineHeight(textDocument, composer.lineBox)
                }
            }

            Basic.AbstractButton {
                id: stop
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: Math.max(0, (composer.recipe.minInputHeight - composer.recipe.sendSize) / 2)
                visible: composer.busy
                implicitWidth: composer.recipe.sendSize
                implicitHeight: composer.recipe.sendSize
                Accessible.name: qsTr("Stop turn")
                onClicked: composer.stopRequested()

                background: Rectangle {
                    radius: KetTheme.px(KetTheme.t.controlRadius, KetTheme.zoom)
                    color: composer.recipe.stopBackground

                    // The mock fills Lucide `square`: an 18/24 square with 2/24 corners.
                    Rectangle {
                        anchors.centerIn: parent
                        width: composer.recipe.stopIconSize * 18 / 24
                        height: width
                        radius: composer.recipe.stopIconSize * 2 / 24
                        color: composer.recipe.stopIcon
                    }
                }
            }

            KetButton {
                id: send
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: Math.max(0, (composer.recipe.minInputHeight - composer.recipe.sendSize) / 2)
                implicitWidth: composer.recipe.sendSize
                implicitHeight: composer.recipe.sendSize
                leftPadding: 0
                rightPadding: 0
                iconName: "arrow-up"
                enabled: input.text.trim() !== ""
                Accessible.name: composer.busy ? qsTr("Send guidance") : qsTr("Send request")
                onClicked: composer.submit()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: composer.recipe.toolbarHeight
            spacing: composer.recipe.toolbarGap

            Row { id: leading; spacing: composer.recipe.toolbarGap }
            Item { Layout.fillWidth: true }
            Row { id: trailing; spacing: composer.recipe.toolbarGap }
        }
    }
}
