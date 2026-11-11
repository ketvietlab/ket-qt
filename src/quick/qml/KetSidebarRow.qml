// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A one-line sidebar tree conversation row, as ket::SidebarRow draws it: the status mark in
// the state's colour (a dot instead once finished and unread, with a heavier title), the
// title and "#number". Checkable; Return, Enter and Space activate it. Its accessible
// description says the role, the state in words and the worktree.
Basic.AbstractButton {
    id: control

    property int level: 0
    property string number
    property int status: KetTheme.ConversationStatus.NotStarted
    property string statusLabel // empty uses the English status label
    property string statusDetail
    property bool unread: false
    property string role
    property string worktree
    property var previewState: null
    readonly property bool unreadDotShown: unread
                                           && status === KetTheme.ConversationStatus.Completed
    readonly property string stateText: KetTheme.sidebarRowStateText(status, unread, statusLabel,
                                                                     statusDetail)
    readonly property string description: KetTheme.sidebarRowDescription(role, stateText,
                                                                         worktree)
    readonly property var recipe: KetTheme.sidebarRowRecipe(
        KetTheme.dark, KetTheme.zoom, checked, unreadDotShown,
        previewState ? !!previewState.hovered : hovered, down,
        previewState ? previewState.enabled !== false : enabled,
        previewState ? !!previewState.focused : visualFocus)
    readonly property var statusRecipe: KetTheme.conversationStatusRecipe(KetTheme.dark, status)
    readonly property var columns: KetTheme.sidebarTreeColumns(level, KetTheme.zoom)
    readonly property real nameLeft: title.x
    readonly property real markLeft: mark.x
    readonly property real highlightLeft: columns.highlight

    checkable: true
    hoverEnabled: true
    focusPolicy: Qt.StrongFocus
    padding: 0
    implicitWidth: columns.text + title.implicitWidth + recipe.titleGap + numberLabel.implicitWidth
                   + recipe.numberEnd
    implicitHeight: recipe.height
    opacity: recipe.opacity
    Keys.onReturnPressed: { toggle(); toggled(); clicked() }
    Keys.onEnterPressed: { toggle(); toggled(); clicked() }

    Accessible.role: Accessible.Button
    Accessible.name: number === "" ? text : text + ", #" + number
    Accessible.description: description
    Accessible.checkable: true
    Accessible.checked: checked

    // A working icon turns once per spin period on KetTheme's stepped clock.
    readonly property bool spins: statusRecipe.spinning && !unreadDotShown && visible
    property bool holdsClock: false
    function syncClock() {
        if (spins !== holdsClock) {
            holdsClock = spins
            KetTheme.holdAnimationClock(spins)
        }
    }
    onSpinsChanged: syncClock()
    Component.onCompleted: syncClock()
    Component.onDestruction: if (holdsClock) KetTheme.holdAnimationClock(false)

    KetToolTip {
        visible: control.hovered && control.description !== ""
        text: control.description
    }

    background: Item {
        Rectangle {
            objectName: "sidebarRowHighlight"
            x: control.columns.highlight
            width: parent.width - x
            height: parent.height
            radius: control.recipe.radius
            color: control.recipe.background
            border.width: control.recipe.focusRing.a > 0 ? 2 : 0
            border.color: control.recipe.focusRing
        }
    }

    contentItem: Item {
        Item {
            id: mark
            objectName: "sidebarRowMark"
            x: control.columns.mark
            anchors.verticalCenter: parent.verticalCenter
            width: control.recipe.markSize
            height: width

            KetIcon {
                visible: !control.unreadDotShown
                name: control.statusRecipe.icon
                box: control.recipe.markSize
                rotation: control.holdsClock
                          ? 360 * (KetTheme.animationClock % control.statusRecipe.spinMs)
                            / control.statusRecipe.spinMs
                          : 0
                color: control.statusRecipe.iconColor
            }

            Rectangle {
                objectName: "sidebarRowUnreadDot"
                visible: control.unreadDotShown
                anchors.centerIn: parent
                width: control.recipe.unreadDotSize
                height: width
                radius: width / 2
                color: control.recipe.unreadDot
            }
        }

        Text {
            id: title
            objectName: "sidebarRowTitle"
            x: control.columns.text
            width: Math.max(0, (control.number === ""
                                ? control.width - control.recipe.numberEnd
                                : numberLabel.x - control.recipe.titleGap) - x)
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: control.text
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            maximumLineCount: 1
            color: control.recipe.title
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.titlePixels
            font.weight: control.recipe.titleWeight
        }

        Text {
            id: numberLabel
            objectName: "sidebarRowNumber"
            visible: control.number !== ""
            x: control.width - control.recipe.numberEnd - width
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: "#" + control.number
            color: control.recipe.number
            font.family: KetTheme.monoFontFamily
            font.pixelSize: control.recipe.numberPixels
            font.weight: control.recipe.numberWeight
        }
    }
}
