// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.impl as Impl
import Ket

// A Két Symbol icon drawn as ket::Icon draws it: the master for the box, at its own size,
// centred on whole device pixels and tinted with `color`.
Item {
    id: icon

    property string name // Két code; KetTheme.iconForLucide() maps a Lucide name
    property real size: 16 // logical pixels at 100% zoom
    // The zoomed box; kit components set it from their recipe instead of `size`.
    property int weight: KetTheme.IconWeight.Light
    property color color: KetTheme.t.textMain
    property real box: KetTheme.px(size, KetTheme.zoom)
    readonly property real drawSize: KetTheme.iconDrawSize(box)
    readonly property real offset: KetTheme.iconOffset(box, Screen.devicePixelRatio)

    implicitWidth: box
    implicitHeight: box

    Impl.IconImage {
        x: icon.offset
        y: icon.offset
        width: icon.drawSize
        height: icon.drawSize
        sourceSize.width: icon.drawSize
        sourceSize.height: icon.drawSize
        source: KetTheme.iconSource(icon.name, icon.box, icon.weight)
        color: icon.color
    }
}
