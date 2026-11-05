// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Animated working indicator (`.working-bars`); it draws ket::activityRecipe with the
// frames of ket::activityBarFrame, as ket::ActivityIndicator does.
Item {
    id: indicator

    property color color: "transparent" // transparent keeps the recipe's success colour
    property bool running: visible
    property real previewTime: -1 // >= 0 freezes the animation, for screenshots
    property real elapsed: 0
    readonly property var recipe: KetTheme.activityRecipe(KetTheme.dark, KetTheme.zoom, color)
    readonly property real time: previewTime >= 0 ? previewTime : elapsed

    implicitWidth: recipe.bars * recipe.barWidth + (recipe.bars - 1) * recipe.spacing
    implicitHeight: recipe.barHeight
    Accessible.role: Accessible.Animation
    Accessible.name: qsTr("Working")

    FrameAnimation {
        running: indicator.running && indicator.previewTime < 0
        onTriggered: indicator.elapsed += frameTime * 1000
    }

    Row {
        anchors.centerIn: parent
        spacing: indicator.recipe.spacing

        Repeater {
            model: indicator.recipe.bars

            Rectangle {
                required property int index
                readonly property var frame: KetTheme.activityBarFrame(indicator.recipe, index,
                                                                       indicator.time)
                anchors.verticalCenter: parent.verticalCenter
                width: indicator.recipe.barWidth
                height: frame[0]
                radius: indicator.recipe.radius
                color: indicator.recipe.color
                opacity: frame[1]
            }
        }
    }
}
