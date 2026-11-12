// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Multi-line plain-text form field (`textarea`); it draws ket::textAreaRecipe, as
// ket::TextArea does. It shows `minimumLines` line boxes, grows with the text up to
// `maximumLines` and then scrolls (0 keeps the recipe's counts). Pasting inserts plain text
// and keeps line breaks; Return inserts a line break, Tab moves the focus, and Cmd/Ctrl+Return
// is left to the window (a Shortcut, or a parent's Keys handler).
//
// With `maxLength` above zero an `n / max` count sits below the field; above the limit the
// field is invalid, and the text is never cut.
//
// `@agent/` completion works as in KetComposer: answer mentionQuery(query) by setting
// mentionItems ([{ number, title, place }]); null closes the picker. A chosen mention is drawn
// as a token: Backspace or Delete removes it whole, the caret never stops inside it, and
// `text` stays the plain `@agent/<number>`. Numbers in `unknownMentions` are drawn in the
// danger colours.
Item {
    id: area

    property alias text: input.text
    property alias cursorPosition: input.cursorPosition
    property string placeholderText
    property int minimumLines: 0
    property int maximumLines: 0
    property int maxLength: 0
    property bool invalid: false
    property var previewState: null
    property var mentionItems: null
    property var mention: null
    property var unknownMentions: []
    readonly property alias input: input
    readonly property alias mentions: tokens
    readonly property alias mentionPicker: picker
    readonly property int count: input.length
    readonly property bool overLimit: maxLength > 0 && count > maxLength
    readonly property bool focused: input.activeFocus
    readonly property var recipe: previewState
        ? KetTheme.textAreaRecipe(KetTheme.dark, KetTheme.zoom, !!previewState.hovered,
                                  !!previewState.focused, previewState.enabled !== false,
                                  invalid, overLimit)
        : KetTheme.textAreaRecipe(KetTheme.dark, KetTheme.zoom, hover.hovered, focused,
                                  enabled, invalid, overLimit)
    readonly property int minLines: minimumLines > 0 ? minimumLines : recipe.minLines
    readonly property int maxLines: Math.max(minLines, maximumLines > 0 ? maximumLines
                                                                        : recipe.maxLines)
    // CSS line boxes, whole pixels, as in ket::TextArea.
    readonly property real lineBox: Math.floor(recipe.fontPixels * recipe.lineHeight)
    readonly property int visibleLines: Math.max(minLines, Math.min(maxLines,
        Math.ceil(input.contentHeight / Math.max(1, lineBox) - 0.01)))
    // A fixed-height line sets its glyphs low in the box; the padding moves the difference
    // below the text, as KetComposer does.
    property real leadingShift: 0
    property int lastPosition: 0
    onLineBoxChanged: Qt.callLater(input.reformat)

    signal mentionQuery(string query)
    signal mentionClosed()

    function focusInput() {
        input.forceActiveFocus()
    }

    function updateMention() {
        let found = input.selectedText === ""
            ? KetTheme.agentMention(input.text, input.cursorPosition) : null
        // A drawn token is not a query: the caret only rests at its end.
        if (found && tokens.isToken(found.start))
            found = null
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
        mention = null
        mentionItems = null
        if (found) {
            const position = tokens.insert(found.start, found.end, item.number, item.title || "")
            area.lastPosition = position
            input.cursorPosition = position
        }
        input.forceActiveFocus()
    }

    // Cmd/Ctrl+Return (Control or Meta, as ket::TextArea reads them) belongs to the window.
    function isHostKey(event) {
        return (event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
            && (event.modifiers & (Qt.ControlModifier | Qt.MetaModifier))
    }

    function closeMentions() {
        if (!mention && !mentionItems)
            return
        mention = null
        mentionItems = null
        mentionClosed()
    }

    function handleKey(event) {
        // Not accepted, and TextEdit inserts nothing for it, so it reaches the window.
        if (isHostKey(event))
            return false
        if (picker.visible) {
            if (event.key === Qt.Key_Escape) {
                closeMentions()
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
        }
        if (event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab) {
            const next = input.nextItemInFocusChain(event.key === Qt.Key_Tab
                                                    && !(event.modifiers & Qt.ShiftModifier))
            if (next && next !== input)
                next.forceActiveFocus(event.key === Qt.Key_Tab ? Qt.TabFocusReason
                                                               : Qt.BacktabFocusReason)
            return true
        }
        const backward = event.key === Qt.Key_Backspace
        if ((backward || event.key === Qt.Key_Delete) && event.modifiers === Qt.NoModifier
                && input.selectedText === "") {
            const range = tokens.deletion(input.cursorPosition, backward)
            if (range) {
                area.lastPosition = range.start
                input.remove(range.start, range.end)
                input.cursorPosition = range.start
                return true
            }
        }
        return false
    }

    implicitWidth: 10 * recipe.height
    implicitHeight: frame.height + (maxLength > 0 ? recipe.countGap + countLabel.height : 0)
    opacity: recipe.opacity

    HoverHandler { id: hover }

    KetMentionTokens {
        id: tokens
        document: input.textDocument
        dark: KetTheme.dark
        unknown: area.unknownMentions
    }

    Rectangle {
        id: frame
        width: parent.width
        height: area.visibleLines * area.lineBox + 2 * area.recipe.paddingBlock
        radius: area.recipe.radius
        color: area.recipe.background
        border.width: area.recipe.borderWidth
        border.color: area.recipe.border

        Basic.ScrollView {
            id: scroll
            anchors.fill: parent
            Basic.ScrollBar.horizontal.policy: Basic.ScrollBar.AlwaysOff

            Basic.TextArea {
                id: input
                objectName: "textAreaInput"
                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.PlainText
                leftPadding: area.recipe.paddingInline
                rightPadding: area.recipe.paddingInline
                topPadding: area.recipe.paddingBlock - area.leadingShift
                bottomPadding: area.recipe.paddingBlock + area.leadingShift
                color: area.recipe.text
                selectionColor: KetTheme.t.accentMuted
                selectedTextColor: area.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: area.recipe.fontPixels
                background: null
                Accessible.name: area.Accessible.name !== "" ? area.Accessible.name
                                                             : area.placeholderText
                Accessible.description: [
                    area.maxLength > 0 ? KetTheme.characterCountDescription(area.count,
                                                                            area.maxLength) : "",
                    tokens.spoken].filter(part => part !== "").join(". ")
                Keys.onPressed: event => { event.accepted = area.handleKey(event) }
                // As ket::TextArea: the picker closes when the field loses the focus.
                onActiveFocusChanged: if (!activeFocus) area.closeMentions()
                onCursorPositionChanged: {
                    const snapped = tokens.snap(cursorPosition, area.lastPosition)
                    if (snapped !== cursorPosition) {
                        if (selectedText !== "")
                            moveCursorSelection(snapped, TextEdit.SelectCharacters)
                        else
                            cursorPosition = snapped
                        return
                    }
                    area.lastPosition = cursorPosition
                    area.updateMention()
                }
                // TextEdit has no line height; the document's blocks carry the recipe's.
                property bool formatting: false
                function reformat() {
                    formatting = true
                    KetTheme.applyLineHeight(textDocument, area.lineBox)
                    formatting = false
                    const line = positionToRectangle(0)
                    area.leadingShift = line.y - topPadding - (area.lineBox - line.height) / 2
                    placeholder.y = positionToRectangle(0).y
                }
                onTextChanged: {
                    if (formatting)
                        return
                    reformat()
                    area.updateMention()
                }
                onWidthChanged: Qt.callLater(reformat)
                Component.onCompleted: reformat()

                // Basic's placeholder ignores the document's line height (see KetComposer).
                Text {
                    id: placeholder
                    objectName: "textAreaPlaceholder"
                    x: input.leftPadding
                    width: input.width - input.leftPadding - input.rightPadding
                    text: area.placeholderText
                    color: area.recipe.placeholder
                    font: input.font
                    wrapMode: Text.Wrap
                    lineHeightMode: Text.FixedHeight
                    lineHeight: area.lineBox
                    visible: !input.length && !input.preeditText
                }
            }
        }
    }

    Text {
        id: countLabel
        objectName: "textAreaCount"
        visible: area.maxLength > 0
        anchors.right: parent.right
        y: frame.height + area.recipe.countGap
        height: visible ? Math.ceil(area.recipe.countPixels * area.recipe.lineHeight) : 0
        verticalAlignment: Text.AlignVCenter
        text: KetTheme.characterCountText(area.count, area.maxLength)
        color: area.recipe.count
        font.family: KetTheme.fontFamily
        font.pixelSize: area.recipe.countPixels
        Accessible.ignored: true // the field's description speaks the count
    }

    // Below the field, overlapping its bottom edge; above it when the window has no room.
    KetMentionPicker {
        id: picker
        readonly property var look: KetTheme.mentionPickerRecipe(KetTheme.dark, KetTheme.zoom)
        readonly property real below: frame.height - look.overlap
        readonly property bool roomBelow: !area.Window.window
            || area.mapToItem(null, 0, below).y + height <= area.Window.height
        z: 10
        x: 0
        y: roomBelow ? below : look.overlap - height
        width: Math.min(look.maxWidth, area.width)
        items: area.mentionItems || []
        visible: area.mentionItems !== null && area.mentionItems !== undefined
        onVisibleChanged: if (visible) highlightedIndex = 0
        onActivated: item => area.applyMention(item)
    }
}
