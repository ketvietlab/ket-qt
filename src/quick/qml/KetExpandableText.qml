// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Read-only multi-line text folded to `foldedLines` lines (0 keeps the recipe's count), with a
// Show more / Show less button when it is longer; it draws ket::expandableTextRecipe, as
// ket::ExpandableText does. Line breaks are kept. Each `@agent/<number>` is a link drawn as a
// mention token: a click, or Return or Space on the mention that has keyboard focus (the
// arrow keys move it), emits mentionActivated(number). `mentionTitles` ({ number: title })
// names the conversations for assistive technology.
Item {
    id: root

    property string text
    property int foldedLines: 0
    property bool expanded: false
    property var unknownMentions: []
    property var mentionTitles: ({})
    property int focusedMention: -1
    readonly property var recipe: KetTheme.expandableTextRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property int folded: foldedLines > 0 ? foldedLines : recipe.foldedLines
    readonly property var mentions: KetTheme.agentMentions(text)
    readonly property real lineBox: Math.floor(recipe.fontPixels * recipe.lineHeight)
    readonly property int lineCount: body.lineCount
    readonly property bool foldable: lineCount > folded
    readonly property int visibleLineCount: expanded ? lineCount : Math.min(lineCount, folded)
    readonly property alias toggleButton: toggle

    signal mentionActivated(string number)

    function spoken(index) {
        const number = mentions[index].number
        return KetTheme.spokenMention(number, mentionTitles[number] || "",
                                      unknownMentions.indexOf(number) >= 0)
    }

    function setFocusedMention(index) {
        index = index < 0 || mentions.length === 0 ? -1 : Math.min(index, mentions.length - 1)
        focusedMention = index
        // A mention behind the fold opens the text, so the focus stays visible.
        if (index >= 0 && !expanded
                && Math.floor(body.positionToRectangle(mentions[index].start).y / lineBox)
                   >= folded)
            expanded = true
    }

    onTextChanged: focusedMention = -1
    implicitWidth: KetTheme.px(320, KetTheme.zoom)
    implicitHeight: clipper.height + (foldable ? recipe.toggleGap + toggle.implicitHeight : 0)
    activeFocusOnTab: mentions.length > 0
    onActiveFocusChanged: if (activeFocus && focusedMention < 0) setFocusedMention(0)

    Accessible.role: Accessible.StaticText
    Accessible.name: text
    Accessible.description: {
        if (activeFocus && focusedMention >= 0)
            return spoken(focusedMention)
        const names = []
        for (let i = 0; i < mentions.length; ++i)
            names.push(spoken(i))
        return names.length ? qsTr("Mentions: %1").arg(names.join(", ")) : ""
    }

    Keys.onPressed: event => {
        const count = mentions.length
        if (count === 0)
            return
        if (event.key === Qt.Key_Left || event.key === Qt.Key_Up) {
            setFocusedMention(focusedMention <= 0 ? count - 1 : focusedMention - 1)
            event.accepted = true
        } else if (event.key === Qt.Key_Right || event.key === Qt.Key_Down) {
            setFocusedMention(focusedMention + 1 >= count ? 0 : focusedMention + 1)
            event.accepted = true
        } else if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter
                    || event.key === Qt.Key_Space) && focusedMention >= 0) {
            mentionActivated(mentions[focusedMention].number)
            event.accepted = true
        }
    }

    Item {
        id: clipper
        width: parent.width
        height: Math.max(1, root.visibleLineCount) * root.lineBox
        clip: true

        TextEdit {
            id: body
            objectName: "expandableTextBody"
            width: parent.width
            readOnly: true
            activeFocusOnPress: false
            selectByMouse: false
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.RichText
            text: KetTheme.mentionLinkedHtml(root.text, KetTheme.dark, root.unknownMentions,
                                             root.activeFocus ? root.focusedMention : -1)
            color: root.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: root.recipe.fontPixels
            Accessible.ignored: true
            property bool formatting: false
            function reformat() {
                formatting = true
                KetTheme.applyLineHeight(textDocument, root.lineBox)
                formatting = false
            }
            onTextChanged: if (!formatting) reformat()
            Component.onCompleted: reformat()
            onLinkActivated: link => {
                const number = KetTheme.mentionLinkNumber(link)
                if (number !== "")
                    root.mentionActivated(number)
            }

            HoverHandler {
                cursorShape: body.hoveredLink !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }

    KetButton {
        id: toggle
        y: clipper.height + root.recipe.toggleGap
        visible: root.foldable
        variant: KetTheme.ButtonVariant.Ghost
        controlSize: KetTheme.ControlSize.Small
        text: root.expanded ? qsTr("Show less") : qsTr("Show more")
        onClicked: root.expanded = !root.expanded
    }
}
