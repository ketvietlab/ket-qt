// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The command palette (`.spotlight`), as ket::CommandPalette draws it: a search field, the
// results in titled groups and a footer of key hints, over a dimmed window. `groups` is a
// list of { title, items: [{ id, title, detail, iconName }] }; the app filters them as
// `query` changes. Up and Down move the highlight, Return activates and Escape closes.
Basic.Popup {
    id: spotlight

    property var groups: []
    property alias query: field.text
    property alias placeholderText: field.placeholderText
    property int highlightedIndex: 0
    property string emptyTitle: qsTr("No results")
    property string emptyMessage: qsTr("Try another word or a file name.")
    readonly property var recipe: KetTheme.commandPaletteRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property var rows: {
        const result = []
        let index = 0
        for (const group of groups) {
            if (!group.items || group.items.length === 0)
                continue
            if (group.title)
                result.push({ item: -1, title: group.title })
            for (const item of group.items)
                result.push({ item: index++, title: item.title, detail: item.detail || "",
                              iconName: item.iconName || "", data: item })
        }
        return result
    }
    readonly property int itemCount: rows.filter(row => row.item >= 0).length

    signal activated(var item)

    function setHighlightedIndex(index) {
        highlightedIndex = itemCount === 0 ? 0 : ((index % itemCount) + itemCount) % itemCount
        for (let i = 0; i < rows.length; ++i)
            if (rows[i].item === highlightedIndex)
                list.positionViewAtIndex(i, ListView.Contain)
    }

    function activate(index) {
        const row = rows.find(row => row.item === index)
        if (!row)
            return
        close()
        activated(row.data)
    }

    onGroupsChanged: highlightedIndex = Math.max(0, Math.min(highlightedIndex, itemCount - 1))
    onAboutToShow: {
        field.clear()
        highlightedIndex = 0
    }
    onOpened: field.forceActiveFocus()

    parent: Basic.Overlay.overlay
    modal: true
    focus: true
    closePolicy: Basic.Popup.CloseOnEscape | Basic.Popup.CloseOnPressOutside
    width: parent ? Math.min(recipe.width, parent.width - 50) : recipe.width
    x: parent ? Math.round((parent.width - width) / 2) : 0
    y: parent ? Math.min(recipe.top, Math.max(25, parent.height - height - 25)) : recipe.top
    padding: 1
    enter: null
    exit: null

    Basic.Overlay.modal: Rectangle {
        color: spotlight.recipe.backdrop
    }

    contentItem: Column {
        Accessible.role: Accessible.Dialog
        Accessible.name: qsTr("Command palette")

        Basic.TextField {
            id: field
            width: parent.width
            leftPadding: spotlight.recipe.searchPaddingInline + spotlight.recipe.searchIconSize
                         + spotlight.recipe.searchGap
            rightPadding: spotlight.recipe.searchPaddingInline
            topPadding: spotlight.recipe.searchPaddingBlock + spotlight.recipe.inputPaddingBlock
            bottomPadding: spotlight.recipe.searchPaddingBlock + spotlight.recipe.inputPaddingBlock + 1
            placeholderText: qsTr("Search commands, files and conversations")
            placeholderTextColor: spotlight.recipe.placeholder
            color: spotlight.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: spotlight.recipe.inputPixels
            Accessible.name: qsTr("Search")
            onTextChanged: spotlight.highlightedIndex = 0
            Keys.onDownPressed: spotlight.setHighlightedIndex(spotlight.highlightedIndex + 1)
            Keys.onUpPressed: spotlight.setHighlightedIndex(spotlight.highlightedIndex - 1)
            Keys.onReturnPressed: spotlight.activate(spotlight.highlightedIndex)
            Keys.onEnterPressed: spotlight.activate(spotlight.highlightedIndex)

            background: Item {
                KetIcon {
                    x: spotlight.recipe.searchPaddingInline
                    anchors.verticalCenter: parent.verticalCenter
                    name: "search"
                    box: spotlight.recipe.searchIconSize
                    color: spotlight.recipe.searchIcon
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: spotlight.recipe.rule
                }
            }
        }

        ListView {
            id: list
            width: parent.width
            // The list scrolls past its maximum or when the window is too short for it.
            height: Math.max(0, Math.min(spotlight.rows.length === 0
                                         ? spotlight.recipe.emptyHeight : contentHeight,
                                         spotlight.recipe.maxListHeight,
                                         spotlight.parent ? spotlight.parent.height - 50 - 2
                                                            - field.height - footerBar.height
                                                          : Infinity))
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: spotlight.rows
            footer: Item { height: spotlight.recipe.listPaddingBottom }
            leftMargin: spotlight.recipe.listPaddingInline
            rightMargin: spotlight.recipe.listPaddingInline

            delegate: Loader {
                required property var modelData
                width: ListView.view.width - spotlight.recipe.listPaddingInline * 2
                sourceComponent: modelData.item < 0 ? groupLabel : result

                Component {
                    id: groupLabel

                    Text {
                        topPadding: spotlight.recipe.groupPaddingTop
                        bottomPadding: spotlight.recipe.groupPaddingBottom
                        leftPadding: spotlight.recipe.groupPaddingInline
                        text: modelData.title.toUpperCase()
                        color: spotlight.recipe.group
                        font.family: KetTheme.fontFamily
                        font.pixelSize: spotlight.recipe.groupPixels
                        font.weight: spotlight.recipe.groupWeight
                        font.letterSpacing: spotlight.recipe.groupLetterSpacing
                    }
                }

                Component {
                    id: result

                    KetCommandResult {
                        title: modelData.title
                        detail: modelData.detail
                        iconName: modelData.iconName
                        highlighted: modelData.item === spotlight.highlightedIndex
                        keyBorder: spotlight.recipe.rule
                        onHoveredChanged: if (hovered) spotlight.highlightedIndex = modelData.item
                        onClicked: spotlight.activate(modelData.item)
                    }
                }
            }

            Column {
                visible: spotlight.rows.length === 0
                anchors.centerIn: parent
                spacing: 0

                KetIcon {
                    anchors.horizontalCenter: parent.horizontalCenter
                    name: "search"
                    box: spotlight.recipe.emptyIconSize
                    color: spotlight.recipe.placeholder
                }

                Item { width: 1; height: 15 }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: spotlight.emptyTitle
                    color: spotlight.recipe.text
                    font.family: KetTheme.fontFamily
                    font.pixelSize: spotlight.recipe.emptyTitlePixels
                    font.weight: Font.DemiBold
                }

                Item { width: 1; height: 9 }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: spotlight.emptyMessage
                    color: spotlight.recipe.placeholder
                    font.family: KetTheme.fontFamily
                    font.pixelSize: spotlight.recipe.emptyTextPixels
                }
            }
        }

        // `.spotlight > footer`: key hints.
        Item {
            id: footerBar
            width: parent.width
            implicitHeight: hints.implicitHeight + 2 * spotlight.recipe.footerPaddingBlock + 4

            Rectangle {
                width: parent.width
                height: 1
                color: spotlight.recipe.rule
            }

            Row {
                id: hints
                x: spotlight.recipe.footerPaddingInline
                anchors.verticalCenter: parent.verticalCenter
                spacing: spotlight.recipe.footerGap

                Repeater {
                    model: [["↑↓", qsTr("to navigate")], ["↵", qsTr("to open")],
                            ["esc", qsTr("to close")]]

                    Row {
                        required property var modelData
                        spacing: 4

                        Rectangle {
                            width: keyText.implicitWidth + 6
                            height: keyText.implicitHeight + 4
                            radius: 3
                            color: spotlight.recipe.background
                            border.color: spotlight.recipe.rule

                            Text {
                                id: keyText
                                anchors.centerIn: parent
                                text: modelData[0]
                                color: spotlight.recipe.footer
                                font.family: KetTheme.fontFamily
                                font.pixelSize: spotlight.recipe.footerPixels
                            }
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData[1]
                            color: spotlight.recipe.footer
                            font.family: KetTheme.fontFamily
                            font.pixelSize: spotlight.recipe.footerPixels
                        }
                    }
                }
            }
        }
    }

    background: Item {
        KetShadow {
            anchors.fill: parent
            radius: spotlight.recipe.radius
            shadowX: spotlight.recipe.shadowX
            shadowY: spotlight.recipe.shadowY
            shadowBlur: spotlight.recipe.shadowBlur
            shadowColor: spotlight.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: spotlight.recipe.radius
            color: spotlight.recipe.background
            border.width: 1
            border.color: spotlight.recipe.border
        }
    }
}
