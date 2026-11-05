// SPDX-License-Identifier: MPL-2.0
import QtQuick

// A CSS-like box shadow under a rounded rectangle, drawn with concentric translucent
// layers as the Widgets layer does, so it needs no shader effect. Fill the shadowed item.
Item {
    id: shadow

    property real radius
    property real shadowX
    property real shadowY
    property real shadowBlur
    property color shadowColor: "transparent"
    readonly property int layers: Math.max(4, Math.min(16, Math.round(shadowBlur / 2)))

    Repeater {
        model: shadow.shadowBlur > 0 && shadow.shadowColor.a > 0 ? shadow.layers : 0

        // Equal layers from half the blur inside the edge to half outside it, as
        // ket::detail::paintShadow draws them.
        Rectangle {
            readonly property real grow: shadow.shadowBlur / 2
                                         * (2 * (index + 0.5) / shadow.layers - 1)
            x: shadow.shadowX - grow
            y: shadow.shadowY - grow
            width: Math.max(0, shadow.width + 2 * grow)
            height: Math.max(0, shadow.height + 2 * grow)
            radius: Math.max(0, shadow.radius + grow)
            color: Qt.rgba(shadow.shadowColor.r, shadow.shadowColor.g, shadow.shadowColor.b,
                           shadow.shadowColor.a / shadow.layers)
        }
    }
}
