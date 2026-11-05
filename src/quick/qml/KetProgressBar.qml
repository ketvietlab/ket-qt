// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Progress bar (`progress`); it draws ket::progressRecipe, as ket::Style draws QProgressBar.
// `indeterminate` sweeps a segment across the track.
Basic.ProgressBar {
    id: control

    readonly property var recipe: KetTheme.progressRecipe(KetTheme.dark, KetTheme.zoom)

    implicitHeight: recipe.height
    implicitWidth: KetTheme.px(KetTheme.t.space16 * 3, KetTheme.zoom)
    opacity: enabled ? 1 : KetTheme.navItemRecipe(KetTheme.dark, KetTheme.zoom, false, false,
                                                  false, false, false).opacity

    background: Rectangle {
        implicitHeight: control.recipe.height
        radius: control.recipe.radius
        color: control.recipe.track
    }

    contentItem: Item {
        implicitHeight: control.recipe.height
        clip: true

        Rectangle {
            visible: !control.indeterminate
            width: control.visualPosition * parent.width
            height: parent.height
            radius: control.recipe.radius
            color: control.recipe.fill
        }

        Rectangle {
            id: sweep
            visible: control.indeterminate
            width: parent.width * control.recipe.busyFraction
            height: parent.height
            radius: control.recipe.radius
            color: control.recipe.fill

            NumberAnimation on x {
                running: control.indeterminate && control.visible
                loops: Animation.Infinite
                from: -sweep.width
                to: sweep.parent.width
                duration: control.recipe.busyDurationMs
            }
        }
    }
}
