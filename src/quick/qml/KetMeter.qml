// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A labelled usage meter, as ket::Meter draws it: label and value on one line, an optional
// track (a negative `fraction` hides it) and a muted caption. `compact` is the sidebar's
// `.quota`.
Item {
    id: meter

    property string label
    property string valueText
    property real fraction: -1
    property string caption
    property bool compact: false
    readonly property var recipe: KetTheme.meterRecipe(KetTheme.dark, KetTheme.zoom, compact,
                                                       Math.max(0, fraction))

    implicitWidth: KetTheme.px(KetTheme.t.space16 * 4, KetTheme.zoom)
    implicitHeight: column.implicitHeight
    Accessible.role: Accessible.ProgressBar
    Accessible.name: label
    Accessible.description: valueText

    Column {
        id: column
        width: meter.width

        Item {
            width: parent.width
            height: labelText.implicitHeight

            Text {
                id: labelText
                width: parent.width - valueLabel.implicitWidth - meter.recipe.spacing
                text: meter.label
                elide: Text.ElideRight
                color: meter.recipe.label
                font.family: KetTheme.fontFamily
                font.pixelSize: meter.recipe.labelPixels
            }

            Text {
                id: valueLabel
                anchors.right: parent.right
                text: meter.valueText
                color: meter.recipe.value
                font.family: KetTheme.fontFamily
                font.pixelSize: meter.recipe.labelPixels
                font.weight: meter.recipe.valueWeight
            }
        }

        Item {
            visible: meter.fraction >= 0
            width: parent.width
            height: visible ? meter.recipe.trackSpacingTop + meter.recipe.trackHeight
                              + meter.recipe.trackSpacingBottom : 0

            Rectangle {
                id: track
                y: meter.recipe.trackSpacingTop
                width: parent.width
                height: meter.recipe.trackHeight
                radius: height / 2
                color: meter.recipe.track
                clip: true

                Rectangle {
                    width: parent.width * Math.max(0, meter.fraction)
                    height: parent.height
                    radius: parent.radius
                    color: meter.recipe.fill
                }
            }
        }

        Text {
            visible: meter.caption !== ""
            width: parent.width
            text: meter.caption
            wrapMode: Text.WordWrap
            color: meter.recipe.caption
            font.family: KetTheme.fontFamily
            font.pixelSize: meter.recipe.captionPixels
            lineHeightMode: Text.FixedHeight
            lineHeight: Math.floor(meter.recipe.captionPixels * meter.recipe.captionLineHeight)
        }
    }
}
