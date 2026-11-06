// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Full-window image viewer (`interactions/lightbox`), as ket::Lightbox draws it: the counter
// and tools over a dark backdrop, the picture on a padded stage, previous/next and the
// caption. `items` is a list of { source, caption, description }. Zoom (tools, keys,
// wheel, pinch, a tap for 2×), pan (drag) and a swipe to the neighbour work on the picture.
// Copy, Download, Print and Fullscreen are the host's: handle toolTriggered(tool). The Qt
// Quick layer has no print dialog; a QApplication host prints with ket::printImage().
Basic.Popup {
    id: lightbox

    property var items: []
    property int currentIndex: 0
    property int tools: KetTheme.LightboxTool.ZoomOut | KetTheme.LightboxTool.ZoomIn
    property bool wraps: true
    readonly property int count: items.length
    readonly property var item: count > 0 ? items[Math.min(currentIndex, count - 1)] : null
    readonly property string description: item ? (item.description || item.caption || "") : ""
    readonly property var recipe: KetTheme.lightboxRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property real toolSize: KetTheme.lightboxToolRecipe(KetTheme.dark, KetTheme.zoom,
                                                                 false, false, true, false).size
    // The view: zoom 1 fits, pan is the picture centre's offset from the stage's.
    readonly property alias zoom: stage.zoom
    readonly property alias panX: stage.panX
    readonly property alias panY: stage.panY
    readonly property int rotationAngle: stage.turns * 90
    readonly property alias fitScale: stage.fitScale
    readonly property alias maximumZoom: stage.maximumZoom
    readonly property alias ready: stage.ready
    readonly property alias image: picture
    readonly property alias closeButton: closeTool

    signal toolTriggered(int tool)

    function openAt(index) {
        if (count === 0)
            return
        currentIndex = Math.max(0, Math.min(index, count - 1))
        stage.reset()
        open()
    }
    function step(delta) {
        if (count < 2)
            return
        let target = currentIndex + delta
        if (wraps)
            target = ((target % count) + count) % count
        else if (target < 0 || target >= count)
            return
        currentIndex = target
    }
    function next() { step(1) }
    function previous() { step(-1) }
    // Toolbar and keyboard steps zoom about the picture's centre, as the design system's do.
    // A step during an animation continues from where it is heading.
    function zoomIn() { stage.show(stage.zoomedAt(stage.goal.zoom + recipe.zoomStep, stage.goalCentre(), stage.goal), true) }
    function zoomOut() { stage.show(stage.zoomedAt(stage.goal.zoom - recipe.zoomStep, stage.goalCentre(), stage.goal), true) }
    function fit() { stage.show({ zoom: 1, x: 0, y: 0 }, true) }
    function toggleActualSize() {
        const actual = 1 / stage.fitScale
        if (Math.abs(stage.goal.zoom - actual) < 1e-3)
            fit()
        else
            stage.show(stage.zoomedAt(actual, stage.goalCentre(), stage.goal), true)
    }
    function rotate() {
        if (!stage.ready)
            return
        stage.turns = (stage.turns + 1) % 4
        stage.show({ zoom: 1, x: 0, y: 0 }, false)
    }
    function trigger(tool) {
        toolTriggered(tool)
        if (tool === KetTheme.LightboxTool.ZoomOut)
            zoomOut()
        else if (tool === KetTheme.LightboxTool.ZoomIn)
            zoomIn()
        else if (tool === KetTheme.LightboxTool.ZoomLevel)
            toggleActualSize()
        else if (tool === KetTheme.LightboxTool.Rotate)
            rotate()
    }

    parent: Basic.Overlay.overlay
    x: 0
    y: 0
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0
    modal: true
    dim: false
    focus: true
    padding: 0
    closePolicy: Basic.Popup.CloseOnEscape
    enter: null
    exit: null
    font.family: KetTheme.fontFamily
    onCurrentIndexChanged: stage.reset()
    onOpened: closeTool.forceActiveFocus()

    background: Rectangle {
        color: lightbox.recipe.backdrop
    }

    contentItem: Item {
        id: layer
        focus: true
        Accessible.role: Accessible.Dialog
        Accessible.name: lightbox.description

        Keys.onPressed: event => {
            event.accepted = true
            if (event.key === Qt.Key_Left)
                lightbox.previous()
            else if (event.key === Qt.Key_Right)
                lightbox.next()
            else if (event.key === Qt.Key_Plus || event.key === Qt.Key_Equal)
                lightbox.zoomIn()
            else if (event.key === Qt.Key_Minus)
                lightbox.zoomOut()
            else if (event.key === Qt.Key_0)
                lightbox.fit()
            else if (event.key === Qt.Key_R && (lightbox.tools & KetTheme.LightboxTool.Rotate))
                lightbox.rotate()
            else
                event.accepted = false
        }

        Item {
            id: toolbar
            width: parent.width
            height: lightbox.toolSize + 2 * lightbox.recipe.toolbarPaddingBlock

            Text {
                x: lightbox.recipe.toolbarPaddingInline
                anchors.verticalCenter: parent.verticalCenter
                visible: lightbox.count > 1
                text: qsTr("%1 / %2").arg(lightbox.currentIndex + 1).arg(lightbox.count)
                color: lightbox.recipe.foreground
                font.family: KetTheme.fontFamily
                font.pixelSize: lightbox.recipe.counterPixels
                font.features: { "tnum": 1 }
                Accessible.role: Accessible.StaticText
                Accessible.name: text
            }

            Row {
                anchors.right: parent.right
                anchors.rightMargin: lightbox.recipe.toolbarPaddingInline
                anchors.verticalCenter: parent.verticalCenter
                spacing: lightbox.recipe.toolbarGap

                Repeater {
                    model: [
                        { tool: KetTheme.LightboxTool.ZoomOut, code: "zoom-out", label: qsTr("Zoom out") },
                        { tool: KetTheme.LightboxTool.ZoomLevel, code: "", label: qsTr("Fit or actual size") },
                        { tool: KetTheme.LightboxTool.ZoomIn, code: "zoom-in", label: qsTr("Zoom in") },
                        // Két Symbol draws `restore` anticlockwise; mirrored it turns clockwise.
                        { tool: KetTheme.LightboxTool.Rotate, code: "restore", label: qsTr("Rotate") },
                        { tool: KetTheme.LightboxTool.Copy, code: "copy", label: qsTr("Copy image") },
                        { tool: KetTheme.LightboxTool.Download, code: "download", label: qsTr("Download") },
                        // Két Symbol has no printer yet; a page stands in until it does.
                        { tool: KetTheme.LightboxTool.Print,
                          code: KetTheme.iconCodes.indexOf("print") >= 0 ? "print" : "file-text",
                          label: qsTr("Print…") },
                        { tool: KetTheme.LightboxTool.Fullscreen, code: "fullscreen", label: qsTr("Full screen") }
                    ]

                    KetLightboxButton {
                        required property var modelData
                        objectName: "lightboxTool" + modelData.tool
                        visible: (lightbox.tools & modelData.tool) !== 0
                        code: modelData.code
                        flipped: modelData.tool === KetTheme.LightboxTool.Rotate
                        label: modelData.label
                        text: modelData.tool === KetTheme.LightboxTool.ZoomLevel
                              ? (stage.ready ? Math.round(stage.fitScale * stage.zoom * 100) + "%" : "—")
                              : ""
                        enabled: {
                            if (modelData.tool === KetTheme.LightboxTool.Fullscreen)
                                return true
                            if (modelData.tool === KetTheme.LightboxTool.ZoomOut)
                                return stage.ready && stage.zoom > 1 + 1e-3
                            if (modelData.tool === KetTheme.LightboxTool.ZoomIn)
                                return stage.ready && stage.zoom < stage.maximumZoom - 1e-3
                            return stage.ready
                        }
                        onClicked: lightbox.trigger(modelData.tool)
                    }
                }

                KetLightboxButton {
                    id: closeTool
                    code: "close"
                    label: qsTr("Close")
                    onClicked: lightbox.close()
                }
            }
        }

        Item {
            id: stage
            y: toolbar.height
            width: parent.width
            height: Math.max(0, parent.height - toolbar.height
                                - (caption.visible ? caption.implicitHeight : 0))
            clip: true

            property real zoom: 1
            property real panX: 0
            property real panY: 0
            property int turns: 0
            property bool animated: false
            readonly property bool ready: picture.status === Image.Ready
            readonly property real paddingInline: lightbox.width <= lightbox.recipe.narrowWidth
                                                  ? lightbox.recipe.narrowStagePaddingInline
                                                  : lightbox.recipe.stagePaddingInline
            readonly property real naturalWidth: ready ? picture.implicitWidth : 0
            readonly property real naturalHeight: ready ? picture.implicitHeight : 0
            readonly property real orientedWidth: turns % 2 ? naturalHeight : naturalWidth
            readonly property real orientedHeight: turns % 2 ? naturalWidth : naturalHeight
            // Fitted picture pixels per image pixel; the picture never fits larger than itself.
            readonly property real fitScale: {
                if (orientedWidth <= 0 || orientedHeight <= 0)
                    return 1
                const fit = Math.min((width - 2 * paddingInline) / orientedWidth,
                                     (height - 2 * lightbox.recipe.stagePaddingBlock) / orientedHeight)
                return fit > 0 ? Math.min(1, fit) : 1
            }
            // The design system's 4× of the fitted picture, and always far enough for 100 %.
            readonly property real maximumZoom: Math.max(lightbox.recipe.maxZoom, 1 / fitScale)
            property var goal: ({ zoom: 1, x: 0, y: 0 }) // where the view is heading

            function goalCentre() { return Qt.point(width / 2 + goal.x, height / 2 + goal.y) }

            function reset() {
                animated = false
                zoom = 1
                panX = 0
                panY = 0
                turns = 0
                goal = { zoom: 1, x: 0, y: 0 }
            }
            function clamped(view) {
                const z = Math.max(1, Math.min(view.zoom, maximumZoom))
                // The picture's edges stop at the stage's.
                const limitX = Math.max(0, (orientedWidth * fitScale * z - width) / 2)
                const limitY = Math.max(0, (orientedHeight * fitScale * z - height) / 2)
                return { zoom: z, x: Math.max(-limitX, Math.min(view.x, limitX)),
                         y: Math.max(-limitY, Math.min(view.y, limitY)) }
            }
            // The image point under `anchor` stays under it.
            function zoomedAt(z, anchor, from) {
                const base = from || { zoom: zoom, x: panX, y: panY }
                z = Math.max(1, Math.min(z, maximumZoom))
                if (z <= 1 + 1e-3)
                    return { zoom: 1, x: 0, y: 0 }
                const qx = (anchor.x - width / 2 - base.x) / base.zoom
                const qy = (anchor.y - height / 2 - base.y) / base.zoom
                return clamped({ zoom: z, x: anchor.x - width / 2 - qx * z,
                                 y: anchor.y - height / 2 - qy * z })
            }
            function show(view, animate) {
                animated = animate && lightbox.recipe.transitionMs > 0 && lightbox.opened
                goal = view
                zoom = view.zoom
                panX = view.x
                panY = view.y
            }
            function contains(point) {
                const w = orientedWidth * fitScale * zoom
                const h = orientedHeight * fitScale * zoom
                return ready && Math.abs(point.x - width / 2 - panX) <= w / 2
                        && Math.abs(point.y - height / 2 - panY) <= h / 2
            }

            Behavior on zoom {
                enabled: stage.animated
                NumberAnimation { duration: lightbox.recipe.transitionMs; easing.type: Easing.Bezier; easing.bezierCurve: [0, 0, 0.58, 1, 1, 1] }
            }
            Behavior on panX {
                enabled: stage.animated
                NumberAnimation { duration: lightbox.recipe.transitionMs; easing.type: Easing.Bezier; easing.bezierCurve: [0, 0, 0.58, 1, 1, 1] }
            }
            Behavior on panY {
                enabled: stage.animated
                NumberAnimation { duration: lightbox.recipe.transitionMs; easing.type: Easing.Bezier; easing.bezierCurve: [0, 0, 0.58, 1, 1, 1] }
            }

            Image {
                id: picture
                source: lightbox.item ? lightbox.item.source || "" : ""
                asynchronous: true
                smooth: true
                mipmap: true
                visible: stage.ready
                width: implicitWidth * stage.fitScale
                height: implicitHeight * stage.fitScale
                x: (stage.width - width) / 2 + stage.panX
                y: (stage.height - height) / 2 + stage.panY
                scale: stage.zoom
                rotation: stage.turns * 90
                Accessible.role: Accessible.Graphic
                Accessible.name: lightbox.description
            }

            KetIcon {
                anchors.centerIn: parent
                visible: picture.status === Image.Error || picture.status === Image.Null
                name: "image"
                box: lightbox.recipe.placeholderIconSize
                color: lightbox.recipe.foreground
                opacity: lightbox.recipe.placeholderOpacity
            }

            // A drag pans, a short press taps (the picture toggles fit and 2×, the empty stage
            // closes), and a long horizontal one at fit size swipes to the neighbour.
            MouseArea {
                id: pointer
                anchors.fill: parent
                property point pressAt
                property point panAt
                property bool moved: false
                cursorShape: stage.zoom > 1 + 1e-3 ? (pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor)
                                                   : Qt.ArrowCursor
                onPressed: mouse => {
                    stage.animated = false
                    pressAt = Qt.point(mouse.x, mouse.y)
                    panAt = Qt.point(stage.panX, stage.panY)
                    moved = false
                }
                onPositionChanged: mouse => {
                    const dx = mouse.x - pressAt.x
                    const dy = mouse.y - pressAt.y
                    if (Math.abs(dx) + Math.abs(dy) > 4)
                        moved = true
                    if (stage.zoom > 1 + 1e-3)
                        stage.show(stage.clamped({ zoom: stage.zoom, x: panAt.x + dx, y: panAt.y + dy }), false)
                }
                onReleased: mouse => {
                    const dx = mouse.x - pressAt.x
                    const dy = mouse.y - pressAt.y
                    const fitted = stage.zoom <= 1 + 1e-3
                    if (!moved) {
                        if (stage.contains(Qt.point(mouse.x, mouse.y)))
                            fitted ? stage.show(stage.zoomedAt(lightbox.recipe.tapZoom, Qt.point(mouse.x, mouse.y)), true)
                                   : lightbox.fit()
                        else
                            lightbox.close()
                    } else if (fitted && Math.abs(dx) > lightbox.recipe.swipeDistance
                               && Math.abs(dx) > Math.abs(dy)) {
                        lightbox.step(dx < 0 ? 1 : -1)
                    }
                }
                // A trackpad scroll pans; a mouse wheel, or a scroll with Ctrl, zooms about
                // the pointer.
                onWheel: wheel => {
                    if (!stage.ready)
                        return
                    stage.animated = false
                    const pixels = wheel.pixelDelta
                    if ((pixels.x !== 0 || pixels.y !== 0) && !(wheel.modifiers & Qt.ControlModifier)) {
                        stage.show(stage.clamped({ zoom: stage.zoom, x: stage.panX + pixels.x,
                                                   y: stage.panY + pixels.y }), false)
                        return
                    }
                    const notches = wheel.angleDelta.y !== 0 ? wheel.angleDelta.y / 120 : pixels.y / 40
                    if (notches !== 0)
                        stage.show(stage.zoomedAt(stage.zoom * Math.pow(lightbox.recipe.wheelStep, notches),
                                                  Qt.point(wheel.x, wheel.y)), false)
                }
            }

            PinchHandler {
                target: null
                onScaleChanged: delta => {
                    if (!stage.ready)
                        return
                    stage.animated = false
                    stage.show(stage.zoomedAt(stage.zoom * delta, centroid.position), false)
                }
            }

            KetLightboxButton {
                objectName: "lightboxPrevious"
                x: lightbox.recipe.navInset
                anchors.verticalCenter: parent.verticalCenter
                visible: lightbox.count > 1
                enabled: lightbox.wraps || lightbox.currentIndex > 0
                code: "chevron-left"
                label: qsTr("Previous image")
                onClicked: lightbox.previous()
            }

            KetLightboxButton {
                objectName: "lightboxNext"
                x: parent.width - lightbox.recipe.navInset - width
                anchors.verticalCenter: parent.verticalCenter
                visible: lightbox.count > 1
                enabled: lightbox.wraps || lightbox.currentIndex < lightbox.count - 1
                code: "chevron-right"
                label: qsTr("Next image")
                onClicked: lightbox.next()
            }
        }

        Text {
            id: caption
            anchors.bottom: parent.bottom
            width: parent.width
            visible: text !== ""
            text: lightbox.item ? lightbox.item.caption || "" : ""
            textFormat: Text.PlainText
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            topPadding: lightbox.recipe.captionPaddingTop
            bottomPadding: lightbox.recipe.captionPaddingBottom
            leftPadding: lightbox.recipe.captionPaddingInline
            rightPadding: lightbox.recipe.captionPaddingInline
            lineHeightMode: Text.FixedHeight
            lineHeight: lightbox.recipe.captionLineHeight
            color: lightbox.recipe.foreground
            font.family: KetTheme.fontFamily
            font.pixelSize: lightbox.recipe.captionPixels
        }
    }
}
