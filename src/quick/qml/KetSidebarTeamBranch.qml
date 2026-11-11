// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// A sidebar tree team branch, as ket::SidebarTeamBranch draws it: the chevron folds the
// members and the name opens the team, two focus stops. While a decision waits, the
// needs-input mark shows and the name is heavier. The member count trails; add and menu
// actions show while hovered or focused. `level` is the members' level: the chevron sits on
// that level's guide and the highlight starts right of it.
Item {
    id: branch

    property string name
    property int level: 1
    property bool expanded: true
    property int memberCount: 0
    property int decisionCount: 0
    property bool selected: false
    property var previewState: null
    readonly property bool waiting: decisionCount > 0
    readonly property bool hot: previewState ? !!previewState.hovered || !!previewState.focused
                                             : hover.hovered || fold.visualFocus
                                               || nameButton.visualFocus || add.visualFocus
                                               || menu.visualFocus
    readonly property var recipe: KetTheme.sidebarTeamRecipe(
        KetTheme.dark, KetTheme.zoom, selected, waiting,
        previewState ? !!previewState.hovered : nameButton.hovered, nameButton.down, enabled,
        previewState ? !!previewState.focused : nameButton.visualFocus)
    readonly property var foldRecipe: KetTheme.sidebarTeamRecipe(
        KetTheme.dark, KetTheme.zoom, selected, waiting, fold.hovered, fold.down, enabled,
        !previewState && fold.visualFocus)
    readonly property var columns: KetTheme.sidebarTreeColumns(level, KetTheme.zoom)
    readonly property real nameLeft: nameButton.x + nameLabel.x
    readonly property real markLeft: nameButton.x + teamIcon.x
    readonly property real highlightLeft: nameButton.x

    signal activated()
    signal addTriggered()
    signal menuTriggered()

    implicitWidth: columns.text + nameLabel.implicitWidth + add.width * 2 + count.implicitWidth
                   + recipe.trailingEnd
    implicitHeight: recipe.height
    opacity: recipe.opacity

    HoverHandler { id: hover }

    Basic.AbstractButton {
        id: fold
        objectName: "sidebarTeamFold"
        // As far left of the chevron as the highlight is right of it.
        readonly property real pad: Math.max(0, branch.columns.highlight - branch.columns.chevron
                                                - branch.recipe.chevronSize)
        x: branch.columns.chevron - pad
        width: branch.columns.highlight - x
        height: parent.height
        padding: 0
        checkable: true
        checked: branch.expanded
        hoverEnabled: true
        focusPolicy: Qt.StrongFocus
        onToggled: branch.expanded = checked
        Keys.onReturnPressed: { toggle(); toggled(); clicked() }
        Keys.onEnterPressed: { toggle(); toggled(); clicked() }
        Accessible.role: Accessible.Button
        Accessible.name: branch.expanded ? qsTr("Fold %1").arg(branch.name)
                                         : qsTr("Unfold %1").arg(branch.name)
        Accessible.checkable: true
        Accessible.checked: checked

        background: Rectangle {
            radius: KetTheme.px(KetTheme.t.radiusSm, KetTheme.zoom)
            color: "transparent"
            border.width: branch.foldRecipe.focusRing.a > 0 ? 2 : 0
            border.color: branch.foldRecipe.focusRing
        }
        contentItem: Item {
            KetIcon {
                objectName: "sidebarTeamChevron"
                x: branch.columns.chevron - fold.x
                anchors.verticalCenter: parent.verticalCenter
                name: KetTheme.sidebarChevronIcon(branch.expanded)
                box: branch.recipe.chevronSize
                color: branch.recipe.chevron
            }
        }
    }

    Basic.AbstractButton {
        id: nameButton
        objectName: "sidebarTeamName"
        x: branch.columns.highlight
        width: parent.width - x
        height: parent.height
        padding: 0
        hoverEnabled: true
        focusPolicy: Qt.StrongFocus
        onClicked: branch.activated()
        Keys.onReturnPressed: clicked()
        Keys.onEnterPressed: clicked()
        Accessible.role: Accessible.Button
        Accessible.name: qsTr("Team %1").arg(branch.name)
        Accessible.description: KetTheme.sidebarTeamDescription(branch.memberCount,
                                                                 branch.decisionCount)

        background: Rectangle {
            radius: branch.recipe.radius
            color: branch.recipe.highlight
            border.width: branch.recipe.focusRing.a > 0 ? 2 : 0
            border.color: branch.recipe.focusRing
        }
        contentItem: Item {
            KetIcon {
                id: teamIcon
                objectName: "sidebarTeamMark"
                x: branch.columns.mark - nameButton.x
                anchors.verticalCenter: parent.verticalCenter
                name: "users"
                box: branch.recipe.iconSize
                color: branch.recipe.icon
            }
            Text {
                id: nameLabel
                objectName: "sidebarTeamNameText"
                x: branch.columns.text - nameButton.x
                width: Math.max(0, (branch.hot ? add.x
                                    : branch.waiting ? waitingMark.x : count.x)
                                   - branch.recipe.trailingGap - nameButton.x - x)
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: branch.name
                elide: Text.ElideRight
                maximumLineCount: 1
                color: branch.recipe.name
                font.family: KetTheme.fontFamily
                font.pixelSize: branch.recipe.namePixels
                font.weight: branch.recipe.nameWeight
            }
        }
    }

    KetBadge {
        id: count
        objectName: "sidebarTeamCount"
        x: branch.width - branch.recipe.trailingEnd - width
        anchors.verticalCenter: parent.verticalCenter
        text: String(branch.memberCount)
    }

    KetIcon {
        id: waitingMark
        objectName: "sidebarTeamWaiting"
        visible: branch.waiting
        x: count.x - branch.recipe.trailingGap - width
        anchors.verticalCenter: parent.verticalCenter
        name: branch.recipe.waitingIcon
        box: branch.recipe.iconSize
        color: branch.recipe.waitingColor
    }

    // Declared before the menu: Tab reaches add, then the menu, as in the widget.
    KetIconButton {
        id: add
        objectName: "sidebarTeamAdd"
        x: menu.x - branch.recipe.trailingGap - width
        anchors.verticalCenter: parent.verticalCenter
        controlSize: KetTheme.ControlSize.Small
        iconName: "plus"
        text: qsTr("Add member")
        opacity: branch.hot ? 1 : 0
        onClicked: branch.addTriggered()
    }

    KetIconButton {
        id: menu
        objectName: "sidebarTeamMenu"
        x: (branch.waiting ? waitingMark.x : count.x) - branch.recipe.trailingGap - width
        anchors.verticalCenter: parent.verticalCenter
        controlSize: KetTheme.ControlSize.Small
        iconName: "more"
        text: qsTr("Team actions")
        opacity: branch.hot ? 1 : 0
        onClicked: branch.menuTriggered()
    }
}
