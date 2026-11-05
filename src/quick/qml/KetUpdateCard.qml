// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// An update in Settings (`.update-card`), as ket::UpdateCard draws it: the title with
// `badges`, a detail, an optional progress bar, the release notes and `actions` (the row
// shows once it has an action). A ready update takes the success border.
Rectangle {
    id: card

    property string title
    property string detail
    property var notes: []
    property int progress: -1 // negative hides the bar
    property bool ready: false
    property alias badges: badgeRow.data
    property alias actions: actionRow.data
    readonly property var recipe: KetTheme.updateCardRecipe(KetTheme.dark, KetTheme.zoom, ready)

    implicitWidth: 420
    implicitHeight: column.implicitHeight + 2 * recipe.paddingBlock + 2
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border
    Accessible.role: Accessible.Grouping
    Accessible.name: title

    Column {
        id: column
        x: card.recipe.paddingInline + 1
        y: card.recipe.paddingBlock + 1
        width: parent.width - 2 * x

        Row {
            spacing: card.recipe.headerColumnGap

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: card.title
                color: card.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: card.recipe.titlePixels
                font.weight: card.recipe.titleWeight
            }

            Row {
                id: badgeRow
                anchors.verticalCenter: parent.verticalCenter
                spacing: card.recipe.headerColumnGap
            }
        }

        Text {
            visible: card.detail !== ""
            width: parent.width
            topPadding: card.recipe.headerGap
            text: card.detail
            wrapMode: Text.Wrap
            color: card.recipe.detail
            font.family: KetTheme.fontFamily
            font.pixelSize: card.recipe.detailPixels
        }

        Item {
            visible: card.progress >= 0
            width: parent.width
            height: card.recipe.sectionGap + bar.implicitHeight

            KetProgressBar {
                id: bar
                anchors.bottom: parent.bottom
                width: parent.width
                value: Math.max(0, Math.min(100, card.progress)) / 100
            }
        }

        Column {
            visible: card.notes.length > 0
            width: parent.width
            topPadding: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            leftPadding: card.recipe.notesIndent

            Repeater {
                model: card.notes

                Text {
                    required property string modelData
                    width: parent.width - card.recipe.notesIndent
                    text: "•  " + modelData
                    wrapMode: Text.Wrap
                    // CSS line-height: a multiple of the font size, not of the line.
                    lineHeightMode: Text.FixedHeight
                    lineHeight: Math.round(card.recipe.notesPixels * card.recipe.notesLineHeight)
                    color: card.recipe.notes
                    font.family: KetTheme.fontFamily
                    font.pixelSize: card.recipe.notesPixels
                }
            }
        }

        Item {
            width: parent.width
            visible: actionRow.children.length > 0
            height: visible ? card.recipe.actionsTop + actionRow.implicitHeight : 0

            Row {
                id: actionRow
                anchors.bottom: parent.bottom
                spacing: card.recipe.actionsGap
            }
        }
    }
}
