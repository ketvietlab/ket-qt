// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Context usage as a ring (`.usage-ring`), as ket::UsageRing draws it; `value` is 0..1.
Basic.AbstractButton {
    id: ring

    property real value: 0
    property bool previewHovered: false
    readonly property var recipe: KetTheme.usageRingRecipe(KetTheme.dark, KetTheme.zoom, value,
                                                           hovered || previewHovered)

    implicitWidth: recipe.size
    implicitHeight: recipe.size
    hoverEnabled: true
    padding: 0
    Accessible.name: qsTr("Context %1% used").arg(Math.round(value * 100))

    KetToolTip {
        visible: ring.hovered
        text: ring.Accessible.name
    }

    contentItem: Canvas {
        id: canvas
        readonly property var recipe: ring.recipe
        readonly property real value: ring.value
        onRecipeChanged: requestPaint()
        onValueChanged: requestPaint()

        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            const stroke = recipe.stroke
            const radius = (Math.min(width, height) - stroke) / 2
            ctx.lineWidth = stroke
            ctx.strokeStyle = recipe.track
            ctx.beginPath()
            ctx.arc(width / 2, height / 2, radius, 0, 2 * Math.PI)
            ctx.stroke()
            if (value > 0) {
                ctx.strokeStyle = recipe.arc
                ctx.lineCap = "butt"
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, radius, -Math.PI / 2,
                        -Math.PI / 2 + 2 * Math.PI * Math.min(1, value))
                ctx.stroke()
            }
        }
    }

    background: Rectangle {
        radius: width / 2
        color: "transparent"
        border.width: ring.visualFocus ? 2 : 0
        border.color: KetTheme.t.focusBorder
    }
}
