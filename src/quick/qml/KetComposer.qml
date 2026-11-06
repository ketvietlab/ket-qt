// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Message composer (`.composer`); it draws ket::composerRecipe, as ket::Composer does.
// Enter sends and Shift+Enter breaks the line. Put the app's controls in `leadingTools`
// and `trailingTools`. While `busy`, the action circle stops the turn until a message is typed.
Item {
    id: composer

    property alias text: input.text
    property alias cursorPosition: input.cursorPosition
    property alias placeholderText: input.placeholderText
    property bool busy: false
    property alias leadingTools: leading.data
    property alias trailingTools: trailing.data
    readonly property bool focused: input.activeFocus
    readonly property var recipe: KetTheme.composerRecipe(KetTheme.dark, KetTheme.zoom, focused)
    readonly property real lineBox: Math.floor(recipe.textPixels * recipe.lineHeight)

    // `@agent/` completion: answer mentionQuery(query) by setting mentionItems
    // ([{ number, title, place }]), which opens the picker; null closes it.
    property var mentionItems: null
    property var mention: null
    readonly property alias mentionPicker: picker
    readonly property alias actionButton: send // send, or stop while busy with nothing typed

    signal submitted(string text)
    signal stopRequested()
    signal mentionQuery(string query)
    signal mentionClosed()
    signal attachmentRemoved(int index, string name)

    // Attachments (`.attachment-chips`) above the input: [{ name, icon }]. Removing a chip
    // drops it from the list and emits attachmentRemoved().
    property var attachments: []

    function removeAttachment(index) {
        if (index < 0 || index >= attachments.length)
            return
        const name = attachments[index].name
        const next = attachments.slice()
        next.splice(index, 1)
        attachments = next
        input.forceActiveFocus()
        attachmentRemoved(index, name)
    }

    function updateMention() {
        const found = input.selectedText === ""
            ? KetTheme.agentMention(input.text, input.cursorPosition) : null
        if (!found) {
            if (mention) {
                mention = null
                mentionItems = null
                mentionClosed()
            }
            return
        }
        if (mention && mention.query === found.query) {
            mention = found
            return
        }
        mention = found
        mentionQuery(found.query)
    }

    function applyMention(item) {
        const found = KetTheme.agentMention(input.text, input.cursorPosition)
        if (found) {
            input.remove(found.start, found.end)
            input.insert(found.start, KetTheme.agentMentionText(item.number))
            input.cursorPosition = found.start + KetTheme.agentMentionText(item.number).length
        }
        mention = null
        mentionItems = null
        input.forceActiveFocus()
    }

    // Arrow keys, Enter, Tab and Escape belong to the open picker.
    function handleMentionKey(event) {
        if (!picker.opened)
            return false
        if (event.key === Qt.Key_Escape) {
            mention = null
            mentionItems = null
            mentionClosed()
            return true
        }
        const count = picker.shownItems.length
        if (event.key === Qt.Key_Down || event.key === Qt.Key_Up) {
            picker.moveHighlight(event.key === Qt.Key_Down ? 1 : -1)
            return count > 0
        }
        if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter
             || event.key === Qt.Key_Tab) && count > 0) {
            applyMention(picker.shownItems[Math.max(0, picker.highlightedIndex)])
            return true
        }
        return false
    }

    KetMentionPicker {
        id: picker
        x: card.x
        y: card.y + recipe.overlap - height
        width: Math.min(recipe.maxWidth, card.width)
        items: composer.mentionItems || []
        visible: composer.mentionItems !== null && composer.mentionItems !== undefined
        onVisibleChanged: if (visible) highlightedIndex = 0
        onActivated: item => composer.applyMention(item)
    }

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

        Flow {
            readonly property var chipRecipe: KetTheme.attachmentChipRecipe(KetTheme.dark,
                                                                            KetTheme.zoom, false)
            Layout.fillWidth: true
            Layout.bottomMargin: visible ? chipRecipe.rowPaddingTop : 0
            visible: composer.attachments.length > 0
            spacing: chipRecipe.rowGap

            Repeater {
                model: composer.attachments

                KetAttachmentChip {
                    required property var modelData
                    required property int index
                    name: modelData.name
                    iconName: modelData.icon || "file"
                    onRemoveRequested: composer.removeAttachment(index)
                }
            }
        }

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
                    Keys.onPressed: event => {
                        if (composer.handleMentionKey(event))
                            event.accepted = true
                        else
                            event.accepted = false
                    }
                    onCursorPositionChanged: composer.updateMention()
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
                        composer.updateMention()
                    }
                    Component.onCompleted: KetTheme.applyLineHeight(textDocument, composer.lineBox)
                }
            }

            // One circle: send, or stop the running turn while the box is empty.
            Basic.AbstractButton {
                id: send
                readonly property var look: KetTheme.composerActionRecipe(
                    KetTheme.dark, KetTheme.zoom, composer.busy, input.text.trim() !== "",
                    hovered, down, visualFocus)
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: Math.max(0, (composer.recipe.minInputHeight - look.size) / 2)
                implicitWidth: look.size
                implicitHeight: look.size
                enabled: look.enabled
                hoverEnabled: true
                Accessible.name: look.stop ? qsTr("Stop turn")
                                 : composer.busy ? qsTr("Send guidance") : qsTr("Send request")
                Basic.ToolTip.visible: hovered
                Basic.ToolTip.delay: 600
                Basic.ToolTip.text: Accessible.name
                onClicked: look.stop ? composer.stopRequested() : composer.submit()

                background: Rectangle {
                    radius: width / 2
                    color: send.look.background
                    border.width: send.look.focusRing.a > 0 ? 2 : 0
                    border.color: send.look.focusRing
                }

                contentItem: Item {
                    KetIcon {
                        visible: !send.look.stop
                        anchors.centerIn: parent
                        name: "arrow-up"
                        box: send.look.iconSize
                        color: send.look.icon
                    }
                    Rectangle {
                        visible: send.look.stop
                        anchors.centerIn: parent
                        width: send.look.stopGlyph
                        height: width
                        radius: send.look.stopRadius
                        color: send.look.icon
                    }
                }
            }
        }

        // Hidden while both groups are empty: apps may keep their controls below the card.
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: composer.recipe.toolbarHeight
            visible: leading.children.length + trailing.children.length > 0
            spacing: composer.recipe.toolbarGap

            Row { id: leading; spacing: composer.recipe.toolbarGap }
            Item { Layout.fillWidth: true }
            Row { id: trailing; spacing: composer.recipe.toolbarGap }
        }
    }
}
