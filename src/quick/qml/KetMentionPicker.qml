// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The `@agent/` completion menu (`.mention-menu`), as ket::MentionPicker draws it. `items`
// is a list of { number, title, place }. It never takes focus; KetComposer forwards keys.
Basic.Popup {
    id: picker

    property var items: []
    property int highlightedIndex: 0
    property string title: qsTr("Conversations")
    property string emptyText: qsTr("No conversation matches")
    readonly property var recipe: KetTheme.mentionPickerRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property var shownItems: items.slice(0, recipe.maxItems)

    signal activated(var item)

    function moveHighlight(step) {
        const count = shownItems.length
        if (count > 0)
            highlightedIndex = ((Math.max(0, highlightedIndex) + step) % count + count) % count
    }

    padding: recipe.padding
    focus: false
    closePolicy: Basic.Popup.NoAutoClose
    enter: null
    exit: null

    contentItem: Column {
        Text {
            leftPadding: picker.recipe.titlePaddingInline
            topPadding: picker.recipe.titlePaddingTop
            bottomPadding: picker.recipe.titlePaddingBottom
            text: picker.title
            color: picker.recipe.title
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.titlePixels
            font.weight: picker.recipe.titleWeight
        }

        Text {
            visible: picker.shownItems.length === 0
            padding: KetTheme.mentionOptionRecipe(KetTheme.dark, KetTheme.zoom, false).paddingInline
            text: picker.emptyText
            color: picker.recipe.empty
            font.family: KetTheme.fontFamily
            font.pixelSize: picker.recipe.emptyPixels
        }

        Repeater {
            model: picker.shownItems

            Rectangle {
                id: option
                required property var modelData
                required property int index
                readonly property var recipe: KetTheme.mentionOptionRecipe(
                    KetTheme.dark, KetTheme.zoom, index === picker.highlightedIndex)
                width: picker.availableWidth
                height: recipe.height
                radius: recipe.radius
                color: recipe.background
                Accessible.role: Accessible.ListItem
                Accessible.name: "@agent/" + modelData.number + " " + modelData.title
                Accessible.selected: index === picker.highlightedIndex

                Text {
                    id: number
                    x: option.recipe.paddingInline
                    anchors.verticalCenter: parent.verticalCenter
                    text: "@agent/" + option.modelData.number
                    color: option.recipe.number
                    font.family: KetTheme.monoFontFamily
                    font.pixelSize: option.recipe.numberPixels
                }

                Text {
                    id: place
                    anchors.right: parent.right
                    anchors.rightMargin: option.recipe.paddingInline
                    anchors.verticalCenter: parent.verticalCenter
                    text: option.modelData.place || ""
                    color: option.recipe.place
                    font.family: KetTheme.fontFamily
                    font.pixelSize: option.recipe.placePixels
                }

                Text {
                    x: number.x + number.width + option.recipe.spacing
                    width: Math.max(0, place.x - option.recipe.spacing - x)
                    anchors.verticalCenter: parent.verticalCenter
                    text: option.modelData.title
                    elide: Text.ElideRight
                    color: option.recipe.title
                    font.family: KetTheme.fontFamily
                    font.pixelSize: option.recipe.fontPixels
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: picker.highlightedIndex = option.index
                    onClicked: picker.activated(option.modelData)
                }
            }
        }
    }

    background: Item {
        KetShadow {
            anchors.fill: parent
            radius: picker.recipe.radius
            shadowX: picker.recipe.shadowX
            shadowY: picker.recipe.shadowY
            shadowBlur: picker.recipe.shadowBlur
            shadowColor: picker.recipe.shadowColor
        }

        Rectangle {
            anchors.fill: parent
            radius: picker.recipe.radius
            color: picker.recipe.background
            border.width: 1
            border.color: picker.recipe.border
        }
    }
}
