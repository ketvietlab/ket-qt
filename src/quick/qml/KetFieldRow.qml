// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Layouts
import Ket

// A labelled settings row (`.field-row`): a fixed label column, then the control (the
// default property) with help text or an error below; it draws ket::fieldRowRecipe.
Item {
    id: row

    property string label
    property string help
    property string error
    default property alias control: slot.data
    readonly property var recipe: KetTheme.fieldRowRecipe(KetTheme.dark, KetTheme.zoom)

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    RowLayout {
        id: layout
        anchors.fill: parent
        spacing: row.recipe.gap

        Text {
            Layout.preferredWidth: row.recipe.labelWidth
            Layout.alignment: Qt.AlignTop
            topPadding: row.recipe.labelTop
            text: row.label
            wrapMode: Text.WordWrap
            color: row.recipe.label
            font.family: KetTheme.fontFamily
            font.pixelSize: row.recipe.labelPixels
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: row.recipe.helpGap

            Item {
                id: slot
                Layout.fillWidth: true
                implicitWidth: children.length ? children[0].implicitWidth : 0
                implicitHeight: children.length ? children[0].implicitHeight : 0
                onWidthChanged: if (children.length) children[0].width = width
            }

            Text {
                Layout.fillWidth: true
                visible: text !== ""
                text: row.error !== "" ? row.error : row.help
                wrapMode: Text.WordWrap
                color: row.error !== "" ? row.recipe.error : row.recipe.help
                font.family: KetTheme.fontFamily
                font.pixelSize: row.recipe.helpPixels
            }
        }
    }
}
