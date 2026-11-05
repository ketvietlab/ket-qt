// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The changed-files card under an agent turn (`.changed-files`), as ket::ChangedFiles draws
// it: a heading with the totals, one row per file and a footer for actions and a note.
// `files` is a list of { name, path, added, removed }; a click emits fileActivated(index).
Item {
    id: card

    property var files: []
    property string title
    property string note
    property alias footer: footerRow.data
    readonly property var recipe: KetTheme.changedFilesRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property int addedTotal: files.reduce((sum, file) => sum + (file.added || 0), 0)
    readonly property int removedTotal: files.reduce((sum, file) => sum + (file.removed || 0), 0)

    signal fileActivated(int index)

    implicitWidth: 400
    implicitHeight: column.implicitHeight + 2
    Accessible.role: Accessible.Grouping
    Accessible.name: heading.title

    component Counts: Row {
        property int added
        property int removed
        spacing: KetTheme.t.space15

        Text {
            text: "+" + parent.added
            color: card.recipe.added
            font.family: KetTheme.fontFamily
            font.pixelSize: card.recipe.textPixels
        }

        Text {
            text: "−" + parent.removed
            color: card.recipe.removed
            font.family: KetTheme.fontFamily
            font.pixelSize: card.recipe.textPixels
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: card.recipe.radius
        color: card.recipe.background
        border.width: 1
        border.color: card.recipe.border
    }

    Column {
        id: column
        x: 1
        y: 1
        width: parent.width - 2

        // `.changed-heading`: only the top corners follow the card's radius.
        Item {
            id: heading
            readonly property string title: card.title !== "" ? card.title
                : card.files.length === 1 ? qsTr("1 file changed")
                : qsTr("%1 files changed").arg(card.files.length)
            width: parent.width
            implicitHeight: headingText.implicitHeight + 2 * card.recipe.paddingBlock
            clip: true

            Rectangle {
                width: parent.width
                height: parent.height + radius
                radius: Math.max(0, card.recipe.radius - 1)
                color: card.recipe.headingBackground
            }

            KetIcon {
                id: headingIcon
                x: card.recipe.paddingInline
                anchors.verticalCenter: parent.verticalCenter
                name: "edit"
                box: card.recipe.iconSize
                color: card.recipe.icon
            }

            Text {
                id: headingText
                x: headingIcon.x + headingIcon.width + card.recipe.headingGap
                width: totals.x - card.recipe.headingGap - x
                anchors.verticalCenter: parent.verticalCenter
                text: heading.title
                elide: Text.ElideRight
                color: card.recipe.heading
                font.family: KetTheme.fontFamily
                font.pixelSize: card.recipe.titlePixels
                font.weight: card.recipe.titleWeight
            }

            Counts {
                id: totals
                anchors.right: parent.right
                anchors.rightMargin: card.recipe.paddingInline
                anchors.verticalCenter: parent.verticalCenter
                added: card.addedTotal
                removed: card.removedTotal
            }
        }

        Repeater {
            model: card.files

            Basic.AbstractButton {
                id: row
                required property var modelData
                required property int index
                readonly property real nameBox: Math.floor(card.recipe.titlePixels
                                                           * card.recipe.lineHeight)
                readonly property real pathBox: modelData.path
                    ? Math.floor(card.recipe.pathPixels * card.recipe.lineHeight) : 0
                width: column.width
                implicitHeight: Math.ceil(2 * card.recipe.rowPaddingBlock + nameBox + pathBox) + 1
                hoverEnabled: true
                text: modelData.name
                Accessible.role: Accessible.Button
                Accessible.name: modelData.name
                Accessible.description: qsTr("%1, %2 added, %3 removed").arg(modelData.path || "")
                                        .arg(modelData.added || 0).arg(modelData.removed || 0)
                onClicked: card.fileActivated(index)

                background: Rectangle {
                    color: row.hovered || row.down ? card.recipe.rowHover : "transparent"
                    border.width: row.visualFocus ? 2 : 0
                    border.color: KetTheme.t.focusBorder

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: card.recipe.border
                    }
                }

                contentItem: Item {
                    KetIcon {
                        id: fileIcon
                        x: card.recipe.paddingInline
                        anchors.verticalCenter: parent.verticalCenter
                        name: "file-code"
                        box: card.recipe.iconSize
                        color: card.recipe.icon
                    }

                    Column {
                        x: fileIcon.x + fileIcon.width + card.recipe.rowGap
                        width: counts.x - card.recipe.rowGap - x
                        y: (parent.height + 1 - row.nameBox - row.pathBox) / 2

                        Text {
                            width: parent.width
                            height: row.nameBox
                            verticalAlignment: Text.AlignVCenter
                            text: row.modelData.name
                            elide: Text.ElideMiddle
                            color: card.recipe.name
                            font.family: KetTheme.fontFamily
                            font.pixelSize: card.recipe.titlePixels
                            font.weight: card.recipe.nameWeight
                        }

                        Text {
                            visible: row.pathBox > 0
                            width: parent.width
                            height: row.pathBox
                            verticalAlignment: Text.AlignVCenter
                            text: row.modelData.path || ""
                            elide: Text.ElideLeft
                            color: card.recipe.path
                            font.family: KetTheme.fontFamily
                            font.pixelSize: card.recipe.pathPixels
                        }
                    }

                    Counts {
                        id: counts
                        anchors.right: arrow.left
                        anchors.rightMargin: card.recipe.rowGap
                        anchors.verticalCenter: parent.verticalCenter
                        added: row.modelData.added || 0
                        removed: row.modelData.removed || 0
                    }

                    KetIcon {
                        id: arrow
                        anchors.right: parent.right
                        anchors.rightMargin: card.recipe.paddingInline
                        anchors.verticalCenter: parent.verticalCenter
                        name: "arrow-up-right"
                        box: card.recipe.arrowSize
                        color: card.recipe.icon
                    }
                }
            }
        }

        // The footer: actions on the left, the note on the right.
        Item {
            width: parent.width
            implicitHeight: Math.max(footerRow.implicitHeight, noteText.implicitHeight)
                            + 2 * card.recipe.footerPaddingBlock + 1

            Rectangle {
                width: parent.width
                height: 1
                color: card.recipe.border
            }

            Row {
                id: footerRow
                x: card.recipe.paddingInline
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 0.5
                spacing: card.recipe.headingGap
            }

            Text {
                id: noteText
                anchors.right: parent.right
                anchors.rightMargin: card.recipe.paddingInline
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 0.5
                text: card.note
                color: card.recipe.footer
                font.family: KetTheme.fontFamily
                font.pixelSize: card.recipe.pathPixels
            }
        }
    }
}
