// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Read-only Markdown prose styled by ket::markdownRecipe, as ket::MarkdownText is. Links are
// reported through linkActivated(); selection and copying work as in any TextEdit.
TextEdit {
    id: prose

    property string markdown

    function restyle() {
        text = markdown
        KetTheme.styleMarkdown(textDocument, KetTheme.dark, KetTheme.zoom)
    }

    readOnly: true
    selectByMouse: true
    wrapMode: TextEdit.Wrap
    textFormat: TextEdit.MarkdownText
    font.family: KetTheme.fontFamily
    color: KetTheme.markdownRecipe(KetTheme.dark, KetTheme.zoom).text
    selectionColor: KetTheme.t.accentSubtle
    selectedTextColor: KetTheme.t.accentText
    onMarkdownChanged: restyle()
    Component.onCompleted: restyle()

    Connections {
        target: KetTheme
        function onThemeChanged() { prose.restyle() }
        function onZoomChanged() { prose.restyle() }
    }

    HoverHandler {
        cursorShape: prose.hoveredLink !== "" ? Qt.PointingHandCursor : Qt.IBeamCursor
    }
}
