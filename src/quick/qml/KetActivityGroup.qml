// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A collapsible run of transcript steps (`.activity-group`), as ket::ActivityGroup draws it:
// the summary toggles `expanded`; the children (KetActivityRow, KetGallery…) sit behind a
// hairline.
Column {
    id: group

    property string text
    property bool expanded: false
    property var previewState: null
    default property alias rows: rowColumn.data
    readonly property var recipe: KetTheme.activityGroupRecipe(KetTheme.dark, KetTheme.zoom,
                                                               false, false, true, false)

    Basic.AbstractButton {
        id: summary
        readonly property var recipe: group.previewState
            ? KetTheme.activityGroupRecipe(KetTheme.dark, KetTheme.zoom,
                                           !!group.previewState.hovered, false,
                                           group.previewState.enabled !== false,
                                           !!group.previewState.focused)
            : KetTheme.activityGroupRecipe(KetTheme.dark, KetTheme.zoom, hovered || down,
                                           down, enabled, visualFocus)

        width: group.width
        implicitHeight: recipe.height
        implicitWidth: label.implicitWidth + recipe.spacing + recipe.chevronSize
        hoverEnabled: true
        checkable: true
        checked: group.expanded
        opacity: recipe.opacity
        onToggled: group.expanded = checked
        Accessible.name: group.text
        Accessible.checked: checked

        contentItem: Item {
            Text {
                id: label
                width: Math.min(implicitWidth,
                                summary.width - summary.recipe.spacing - summary.recipe.chevronSize)
                anchors.verticalCenter: parent.verticalCenter
                text: group.text
                elide: Text.ElideRight
                color: summary.recipe.text
                font.family: KetTheme.fontFamily
                font.pixelSize: summary.recipe.fontPixels
            }

            KetIcon {
                x: label.width + summary.recipe.spacing
                anchors.verticalCenter: parent.verticalCenter
                name: summary.checked ? "chevron-up" : "chevron-down"
                box: summary.recipe.chevronSize
                color: summary.recipe.text
            }
        }

        background: Rectangle {
            radius: summary.recipe.radius
            color: summary.recipe.background
            border.width: summary.recipe.focusRing.a > 0 ? 2 : 0
            border.color: summary.recipe.focusRing
        }
    }

    Item {
        visible: group.expanded
        width: group.width
        height: visible ? rowColumn.implicitHeight + group.recipe.childrenSpacingTop
                          + group.recipe.childrenSpacingBottom + 2 * inset : 0
        readonly property real inset: KetTheme.px(KetTheme.t.space05, KetTheme.zoom)

        Rectangle {
            x: Math.round(group.recipe.childrenIndent)
            y: group.recipe.childrenSpacingTop
            width: 1
            height: parent.height - group.recipe.childrenSpacingTop
                    - group.recipe.childrenSpacingBottom
            color: group.recipe.rule
        }

        Column {
            id: rowColumn
            x: Math.round(group.recipe.childrenIndent) + 1 + group.recipe.childrenPadding
            y: group.recipe.childrenSpacingTop + parent.inset
            width: parent.width - x
            spacing: group.recipe.childrenGap
        }
    }
}
