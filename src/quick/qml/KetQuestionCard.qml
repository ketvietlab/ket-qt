// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// The agent asking for input (`.question-card`), as ket::QuestionCard draws it: the label
// row with an optional `step`, the question, then the answer controls (children) and the
// buttons in `footer`.
Rectangle {
    id: card

    property string question
    property string label: qsTr("Your input is needed")
    property string step
    default property alias content: body.data
    property alias footer: footerRow.data
    readonly property var recipe: KetTheme.questionCardRecipe(KetTheme.dark, KetTheme.zoom)

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 8, KetTheme.zoom)
    implicitHeight: column.implicitHeight + 2 * (recipe.padding + 1)
    radius: recipe.radius
    color: recipe.background
    border.width: 1
    border.color: recipe.border
    Accessible.role: Accessible.Grouping
    Accessible.name: question

    Column {
        id: column
        x: card.recipe.padding + 1
        y: card.recipe.padding + 1
        width: card.width - 2 * x

        Item {
            width: parent.width
            height: labelText.implicitHeight

            Row {
                spacing: KetTheme.px(KetTheme.t.space15, KetTheme.zoom)

                KetIcon {
                    anchors.verticalCenter: parent.verticalCenter
                    name: "help"
                    box: card.recipe.iconSize
                    color: card.recipe.label
                }

                Text {
                    id: labelText
                    text: card.label
                    color: card.recipe.label
                    font.family: KetTheme.fontFamily
                    font.pixelSize: card.recipe.labelPixels
                    font.weight: card.recipe.labelWeight
                }
            }

            Text {
                anchors.right: parent.right
                visible: card.step !== ""
                text: card.step
                color: card.recipe.step
                font.family: KetTheme.fontFamily
                font.pixelSize: card.recipe.labelPixels
            }
        }

        Text {
            width: parent.width
            topPadding: card.recipe.headingSpacing
            bottomPadding: card.recipe.headingSpacing
            text: card.question
            wrapMode: Text.WordWrap
            textFormat: Text.PlainText
            color: card.recipe.heading
            font.family: KetTheme.fontFamily
            font.pixelSize: card.recipe.headingPixels
            font.weight: card.recipe.headingWeight
        }

        Column {
            id: body
            width: parent.width
            spacing: card.recipe.controlGap
            bottomPadding: children.length > 0 ? card.recipe.controlGap : 0
        }

        Row {
            id: footerRow
            width: parent.width
            spacing: card.recipe.spacing
            layoutDirection: Qt.RightToLeft
        }
    }
}
