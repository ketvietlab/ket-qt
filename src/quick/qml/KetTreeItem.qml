// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A two-line sidebar item (`.tree-item`), as ket::TreeItem draws it. `activity` is "",
// "working" or "waiting"; `checked` is the mock's `.selected`.
Basic.AbstractButton {
    id: control

    property string iconName
    property string detail
    property string activity
    property string activityText
    property string count
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.treeItemRecipe(KetTheme.dark, KetTheme.zoom, checked, !!previewState.hovered,
                                  !!previewState.pressed, previewState.enabled !== false,
                                  !!previewState.focused)
        : KetTheme.treeItemRecipe(KetTheme.dark, KetTheme.zoom, checked, hovered, down, enabled,
                                  visualFocus)
    readonly property bool hasActivity: activity === "working" || activity === "waiting"

    checkable: true
    hoverEnabled: true
    opacity: recipe.opacity
    leftPadding: recipe.paddingInline
    rightPadding: recipe.paddingInline
    topPadding: recipe.paddingBlock
    bottomPadding: recipe.paddingBlock
    implicitWidth: leftPadding + rightPadding + recipe.iconSize + recipe.spacing
                   + titleText.implicitWidth
    implicitHeight: topPadding + bottomPadding + lines.implicitHeight
    Accessible.name: text
    Accessible.description: hasActivity ? activityText : detail

    contentItem: Item {
        KetIcon {
            visible: control.iconName !== ""
            y: control.recipe.iconOffset
            name: control.iconName
            box: control.recipe.iconSize
            color: control.recipe.icon
        }

        Text {
            id: countText
            anchors.right: parent.right
            visible: control.count !== ""
            text: control.count
            color: control.recipe.count
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.countPixels
        }

        Column {
            id: lines
            x: control.recipe.iconSize + control.recipe.spacing
            width: parent.width - x - (countText.visible ? countText.width + control.recipe.spacing : 0)

            Text {
                id: titleText
                width: parent.width
                text: control.text
                elide: Text.ElideRight
                color: control.recipe.title
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.titlePixels
                font.weight: control.recipe.titleWeight
            }

            Text {
                visible: !control.hasActivity && control.detail !== ""
                width: parent.width
                text: control.detail
                elide: Text.ElideRight
                color: control.recipe.detail
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.detailPixels
            }

            Row {
                visible: control.hasActivity
                topPadding: control.recipe.activityOffset
                spacing: control.recipe.activityGap
                readonly property color tone: control.activity === "working"
                                              ? control.recipe.working : control.recipe.waiting

                KetIcon {
                    anchors.verticalCenter: activityLabel.verticalCenter
                    name: control.activity === "working" ? "loading" : "pause-circle"
                    box: control.recipe.activityIconSize
                    color: parent.tone
                }

                Text {
                    id: activityLabel
                    width: lines.width - control.recipe.activityIconSize - parent.spacing
                    text: control.activityText
                    elide: Text.ElideRight
                    color: parent.tone
                    font.family: KetTheme.fontFamily
                    font.pixelSize: control.recipe.activityPixels
                }
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 0
        border.color: control.recipe.focusRing
    }
}
