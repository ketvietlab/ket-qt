// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// An assistant answer: Markdown prose and fenced code blocks in order, as ket::MarkdownView
// lays them out.
Column {
    id: view

    property string markdown

    signal linkActivated(string link)

    Repeater {
        model: KetTheme.markdownSegments(view.markdown)

        Loader {
            required property var modelData
            width: view.width
            sourceComponent: modelData.kind === "code" ? codeSegment
                           : modelData.kind === "quote" ? quoteSegment : textSegment

            Component {
                id: textSegment
                KetMarkdownText {
                    width: view.width
                    markdown: modelData.text
                    onLinkActivated: link => view.linkActivated(link)
                }
            }

            // A top-level quote: the quote surface with the accent bar on its leading edge.
            Component {
                id: quoteSegment
                Item {
                    readonly property var recipe: KetTheme.markdownRecipe(KetTheme.dark,
                                                                          KetTheme.zoom)
                    readonly property real margin: recipe.paragraphSpacing
                    width: view.width
                    implicitHeight: quoteText.height + 2 * (recipe.quotePadding + margin)

                    Rectangle {
                        y: parent.margin
                        width: parent.width
                        height: parent.height - 2 * parent.margin
                        color: parent.recipe.quoteBackground
                        Rectangle {
                            width: parent.parent.recipe.quoteBorderWidth
                            height: parent.height
                            color: parent.parent.recipe.quoteBorder
                        }
                    }
                    KetMarkdownText {
                        id: quoteText
                        x: parent.recipe.quoteBorderWidth + parent.recipe.quotePadding
                        y: parent.margin + parent.recipe.quotePadding
                        width: parent.width - x - parent.recipe.quotePadding
                        markdown: modelData.text
                        onLinkActivated: link => view.linkActivated(link)
                    }
                }
            }

            Component {
                id: codeSegment
                KetCodeBlock {
                    width: view.width
                    code: modelData.text
                    language: modelData.language
                }
            }
        }
    }
}
