// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Diff lines (`.diff-lines`, `.diff-hunk`), as ket::DiffView draws them: hunk headers and
// numbered lines with a sign column and add/remove tints. `lines` is a list of
// { kind: "context" | "add" | "remove" | "hunk", number, text }; parseUnified() turns a
// unified diff into that list. The view only presents lines; it computes no diff.
Column {
    id: view

    property var lines: []
    readonly property var recipe: KetTheme.diffRecipe(KetTheme.dark, KetTheme.zoom)

    function parseUnified(diff) {
        const result = []
        let oldLine = 0
        let newLine = 0
        let inHunk = false
        for (const line of diff.split("\n")) {
            const match = /^@@ -(\d+)(?:,\d+)? \+(\d+)/.exec(line)
            if (match) {
                oldLine = Number(match[1])
                newLine = Number(match[2])
                inHunk = true
                result.push({ kind: "hunk", number: 0, text: line })
                continue
            }
            if (!inHunk || line.startsWith("\\ "))
                continue
            if (line.startsWith("+")) {
                result.push({ kind: "add", number: newLine++, text: line.slice(1) })
            } else if (line.startsWith("-")) {
                result.push({ kind: "remove", number: oldLine++, text: line.slice(1) })
            } else if (line.startsWith(" ") || line === "") {
                result.push({ kind: "context", number: newLine++, text: line.slice(1) })
                ++oldLine
            } else {
                inHunk = false
            }
        }
        while (result.length > 0 && result[result.length - 1].kind === "context"
               && result[result.length - 1].text === "")
            result.pop()
        return result
    }

    Accessible.role: Accessible.List
    Accessible.name: qsTr("Diff")

    Repeater {
        model: view.lines

        Rectangle {
            id: row
            required property var modelData
            required property int index
            readonly property bool hunk: modelData.kind === "hunk"
            readonly property bool add: modelData.kind === "add"
            readonly property bool remove: modelData.kind === "remove"
            readonly property bool previousHunk: index > 0 && view.lines[index - 1].kind === "hunk"
            readonly property bool last: index === view.lines.length - 1
            // `.diff-lines` pads each run of lines above and below.
            readonly property real padTop: (hunk && index > 0 && !previousHunk)
                                           || (!hunk && (index === 0 || previousHunk))
                                           ? view.recipe.paddingBlock : 0
            readonly property real padBottom: !hunk && last ? view.recipe.paddingBlock : 0
            readonly property real lineHeight: hunk
                ? 2 * view.recipe.hunkPaddingBlock + code.implicitHeight : view.recipe.lineHeight

            width: view.width
            implicitWidth: (hunk ? 2 * view.recipe.hunkPaddingInline
                                 : view.recipe.numberWidth + view.recipe.signWidth
                                   + view.recipe.codeGap + view.recipe.paddingEnd)
                           + code.implicitWidth
            height: padTop + lineHeight + padBottom
            color: view.recipe.background

            Rectangle {
                y: row.padTop
                width: parent.width
                height: row.lineHeight
                color: row.hunk ? view.recipe.hunkBackground
                     : row.add ? view.recipe.addBackground
                     : row.remove ? view.recipe.removeBackground : "transparent"

                Text {
                    visible: !row.hunk && row.modelData.number > 0
                    width: view.recipe.numberWidth - view.recipe.numberGap
                    height: parent.height
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    text: row.modelData.number
                    color: view.recipe.number
                    font.family: KetTheme.monoFontFamily
                    font.pixelSize: view.recipe.fontPixels
                }

                Text {
                    visible: !row.hunk
                    x: view.recipe.numberWidth
                    width: view.recipe.signWidth
                    height: parent.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: row.add ? "+" : row.remove ? "−" : ""
                    color: row.add ? view.recipe.addSign : row.remove ? view.recipe.removeSign
                                                                      : view.recipe.sign
                    font.family: KetTheme.monoFontFamily
                    font.pixelSize: view.recipe.fontPixels
                }

                Text {
                    id: code
                    x: row.hunk ? view.recipe.hunkPaddingInline
                                : view.recipe.numberWidth + view.recipe.signWidth
                                  + view.recipe.codeGap
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: row.modelData.text
                    textFormat: Text.PlainText
                    color: row.hunk ? view.recipe.hunkText : view.recipe.text
                    font.family: KetTheme.monoFontFamily
                    font.pixelSize: view.recipe.fontPixels
                }
            }
        }
    }
}
