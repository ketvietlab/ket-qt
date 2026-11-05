// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A round avatar (`.avatar`), as ket::Avatar draws it: `source` cropped to the circle, or up
// to two initials from `name`.
Item {
    id: avatar

    property string name
    property url source
    property real diameter: 0 // logical pixels at 100 %; 0 uses the recipe's
    readonly property var recipe: KetTheme.avatarRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property string initials: {
        const words = name.split(" ").filter(word => word.length > 0)
        let result = words.slice(0, 2).map(word => word[0].toUpperCase()).join("")
        if (words.length === 1 && words[0].length > 1)
            result += words[0][1].toUpperCase()
        return result
    }

    implicitWidth: diameter > 0 ? KetTheme.px(diameter, KetTheme.zoom) : recipe.size
    implicitHeight: implicitWidth
    Accessible.role: Accessible.Graphic
    Accessible.name: name

    Rectangle {
        id: circle
        anchors.centerIn: parent
        width: Math.min(avatar.width, avatar.height)
        height: width
        radius: width / 2
        color: avatar.recipe.background

        // The image's natural size for the crop below.
        Image {
            id: probe
            visible: false
            source: avatar.source
            onStatusChanged: if (status === Image.Ready) photo.requestPaint()
        }

        // The image cropped to the circle (object-fit: cover); a Rectangle clips square.
        Canvas {
            id: photo
            anchors.fill: parent
            visible: avatar.source.toString() !== ""
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                if (!isImageLoaded(avatar.source))
                    return
                const w = probe.implicitWidth, h = probe.implicitHeight
                if (w <= 0 || h <= 0)
                    return
                const scale = Math.max(width / w, height / h)
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, width / 2, 0, 2 * Math.PI)
                ctx.clip()
                ctx.drawImage(avatar.source, (width - w * scale) / 2, (height - h * scale) / 2,
                              w * scale, h * scale)
            }
            onImageLoaded: requestPaint()
            onWidthChanged: requestPaint()
            Component.onCompleted: if (visible) loadImage(avatar.source)
            Connections {
                target: avatar
                function onSourceChanged() {
                    photo.loadImage(avatar.source)
                    photo.requestPaint()
                }
            }
        }

        Text {
            anchors.centerIn: parent
            visible: avatar.source.toString() === ""
            text: avatar.initials
            color: avatar.recipe.text
            font.family: KetTheme.fontFamily
            font.pixelSize: Math.max(1, Math.round(avatar.recipe.fontPixels * circle.width
                                                   / avatar.recipe.size))
            font.weight: avatar.recipe.fontWeight
        }
    }

    Rectangle {
        anchors.fill: circle
        radius: circle.radius
        color: "transparent"
        border.width: 1
        border.color: avatar.recipe.border
    }
}
