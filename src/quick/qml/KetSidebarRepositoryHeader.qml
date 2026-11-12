// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A sidebar tree repository header, as ket::SidebarRepositoryHeader draws it: the chevron, a
// folder and the name in capitals; a dot while folded with something waiting, the
// conversation count, and an add action shown while hovered or focused. Checked is
// unfolded; Return, Enter and Space fold or unfold it.
Basic.AbstractButton {
    id: header

    property string name
    property int conversationCount: -1 // -1 hides the count
    property int waitingCount: 0
    property bool selected: false
    // A header that follows another repository keeps the divider's space above it.
    property bool divided: false
    property string actionText: qsTr("New conversation")
    property var previewState: null
    readonly property bool expanded: checked
    readonly property bool waitingDotShown: waitingCount > 0 && !checked
    readonly property bool hot: previewState ? !!previewState.hovered || !!previewState.focused
                                             : hovered || visualFocus || action.visualFocus
    readonly property var recipe: KetTheme.sidebarRepositoryRecipe(
        KetTheme.dark, KetTheme.zoom, selected,
        previewState ? !!previewState.hovered : hovered, down, enabled,
        previewState ? !!previewState.focused : visualFocus)
    readonly property var columns: KetTheme.sidebarTreeColumns(0, KetTheme.zoom)
    readonly property real lead: divided ? recipe.dividerLead : 0
    // The name's box, from the tree's left edge (ket::SidebarRepositoryHeader::nameRect).
    readonly property real nameLeft: nameLabel.x
    readonly property real markLeft: folder.x

    signal actionTriggered()

    function activate() {
        toggle()
        toggled()
        clicked()
    }

    checkable: true
    checked: true
    hoverEnabled: true
    focusPolicy: Qt.StrongFocus
    padding: 0
    implicitWidth: columns.text + nameLabel.implicitWidth + action.width + count.implicitWidth
                   + recipe.trailingEnd
    implicitHeight: lead + recipe.height
    opacity: recipe.opacity
    Keys.onReturnPressed: activate()
    Keys.onEnterPressed: activate()

    Accessible.role: Accessible.Button
    Accessible.name: name
    Accessible.description: KetTheme.sidebarRepositoryDescription(
        expanded, Math.max(0, conversationCount), waitingCount)
    Accessible.checkable: true
    Accessible.checked: checked

    background: Item {
        Rectangle {
            y: header.lead
            width: parent.width
            height: header.recipe.height
            radius: header.recipe.radius
            color: header.recipe.background
            border.width: header.recipe.focusRing.a > 0 ? 2 : 0
            border.color: header.recipe.focusRing
        }
    }

    contentItem: Item {
        Item {
            id: row
            y: header.lead
            width: parent.width
            height: header.recipe.height

            KetIcon {
                objectName: "sidebarRepositoryChevron"
                x: header.columns.chevron
                anchors.verticalCenter: parent.verticalCenter
                name: KetTheme.sidebarChevronIcon(header.expanded)
                box: header.recipe.chevronSize
                color: header.recipe.chevron
            }

            KetIcon {
                id: folder
                objectName: "sidebarRepositoryMark"
                x: header.columns.mark
                anchors.verticalCenter: parent.verticalCenter
                name: header.expanded ? "folder-open" : "folder"
                box: header.recipe.iconSize
                color: header.recipe.icon
            }

            Text {
                id: nameLabel
                objectName: "sidebarRepositoryName"
                x: header.columns.text
                width: Math.max(0, (header.hot ? action.x
                                    : header.waitingDotShown ? dot.x : row.trailingRight)
                                   - header.recipe.trailingGap - x)
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: header.name
                elide: Text.ElideMiddle
                maximumLineCount: 1
                color: header.recipe.name
                font.family: KetTheme.fontFamily
                font.pixelSize: header.recipe.namePixels
                font.weight: header.recipe.nameWeight
                font.capitalization: Font.AllUppercase
                font.letterSpacing: header.recipe.letterSpacing
            }

            // Where the trailing parts end: the count, or the trailing edge without one.
            readonly property real trailingRight: count.visible
                ? count.x : header.width - header.recipe.trailingEnd

            KetBadge {
                id: count
                objectName: "sidebarRepositoryCount"
                visible: header.conversationCount >= 0
                x: header.width - header.recipe.trailingEnd - width
                anchors.verticalCenter: parent.verticalCenter
                text: String(Math.max(0, header.conversationCount))
            }

            Rectangle {
                id: dot
                objectName: "sidebarRepositoryDot"
                visible: header.waitingDotShown
                x: row.trailingRight - header.recipe.trailingGap - width
                anchors.verticalCenter: parent.verticalCenter
                width: header.recipe.dotSize
                height: width
                radius: width / 2
                color: header.recipe.dot
            }

            KetIconButton {
                id: action
                objectName: "sidebarRepositoryAction"
                x: (header.waitingDotShown ? dot.x : row.trailingRight)
                   - header.recipe.trailingGap - width
                anchors.verticalCenter: parent.verticalCenter
                controlSize: KetTheme.ControlSize.Small
                iconName: "plus"
                text: header.actionText
                // Keeps its place while hidden, so nothing moves when it shows.
                opacity: header.hot ? 1 : 0
                onClicked: header.actionTriggered()
            }
        }
    }
}
