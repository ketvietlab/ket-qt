// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Navigation and status; `mode` is "nav", "tabs" or "status" and is set before loading.
// NavigationPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "nav"
    readonly property real gap: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "tabs" ? tabs : page.mode === "status" ? status : nav
    }

    component Caption: Text {
        color: KetTheme.t.textMuted
        font.family: KetTheme.fontFamily
        font.pixelSize: KetTheme.px(KetTheme.t.textXs, KetTheme.zoom)
        font.weight: 500
    }

    component Pane: Rectangle {
        property string surface: "panelBg"
        color: KetTheme.t[surface]
        border.width: 1
        border.color: KetTheme.t.panelBorder
    }

    Component {
        id: nav
        Pane {
            surface: "sidebarBg"
            implicitWidth: KetTheme.px(260, KetTheme.zoom)
            implicitHeight: KetTheme.px(260, KetTheme.zoom)

            Basic.ButtonGroup { id: group }

            Column {
                anchors.fill: parent
                anchors.margins: KetTheme.px(10, KetTheme.zoom)
                spacing: KetTheme.px(4, KetTheme.zoom)

                KetNavItem {
                    width: parent.width; text: "New conversation"; iconName: "compose"
                    shortcut: "⌘N"; Basic.ButtonGroup.group: group
                }
                KetNavItem {
                    width: parent.width; text: "Search"; iconName: "search"; shortcut: "⌘K"
                    Basic.ButtonGroup.group: group
                }
                KetNavItem {
                    width: parent.width; text: "Conversations"; iconName: "chat"; checked: true
                    Basic.ButtonGroup.group: group
                }
                KetNavItem {
                    width: parent.width; text: "Worktrees"; iconName: "folder"; count: 4
                    Basic.ButtonGroup.group: group
                }
                KetNavItem {
                    width: parent.width; text: "Atlas"; iconName: "map"
                    previewState: ({ hovered: true }); Basic.ButtonGroup.group: group
                }
                KetNavItem {
                    width: parent.width; text: "Archived"; iconName: "archive"; enabled: false
                    Basic.ButtonGroup.group: group
                }
            }
        }
    }

    Component {
        id: tabs
        Column {
            spacing: page.gap

            Row {
                spacing: page.gap
                KetSegmentedControl { options: ["Claude", "Codex", "Cursor"]; previewHovered: 2 }
                KetSegmentedControl { options: ["Map", "List"]; currentIndex: 1 }
                KetSegmentedControl { options: ["On", "Off"]; enabled: false }
            }

            KetSegmentedControl {
                width: KetTheme.px(460, KetTheme.zoom)
                expanding: true
                options: ["Claude", "Codex", "Cursor", "Gemini"]
            }

            Pane {
                surface: "panelBgSubtle"
                width: KetTheme.px(620, KetTheme.zoom)
                height: strip.implicitHeight

                KetTabBar {
                    id: strip
                    KetTabButton { text: "NavigationRail.qml"; iconName: "file-code"; closable: true }
                    KetTabButton { text: "README.md"; iconName: "file"; closable: true }
                    KetTabButton { text: "ThemeManager.cpp"; iconName: "file-code"; closable: true }
                }
            }
        }
    }

    Component {
        id: status
        GridLayout {
            columns: 2
            rowSpacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            columnSpacing: page.gap

            Caption { text: "0%" }
            KetProgressBar { Layout.preferredWidth: KetTheme.px(320, KetTheme.zoom); value: 0 }
            Caption { text: "40%" }
            KetProgressBar { Layout.preferredWidth: KetTheme.px(320, KetTheme.zoom); value: 0.4 }
            Caption { text: "100%" }
            KetProgressBar { Layout.preferredWidth: KetTheme.px(320, KetTheme.zoom); value: 1 }
            Caption { text: "Busy" }
            KetProgressBar { Layout.preferredWidth: KetTheme.px(320, KetTheme.zoom); indeterminate: true }
            Caption { text: "Disabled" }
            KetProgressBar {
                Layout.preferredWidth: KetTheme.px(320, KetTheme.zoom); value: 0.6; enabled: false
            }
            Caption { text: "Working" }
            Row {
                spacing: page.gap
                KetActivityIndicator {}
                KetActivityIndicator { previewTime: 400 }
                KetActivityIndicator { color: KetTheme.t.accent }
            }

            Pane {
                Layout.columnSpan: 2
                Layout.preferredWidth: KetTheme.px(560, KetTheme.zoom)
                Layout.preferredHeight: KetTheme.px(300, KetTheme.zoom)

                KetEmptyState {
                    anchors.fill: parent
                    iconName: "success"
                    title: "Your worktree is clean."
                    message: "There are no uncommitted changes in this snapshot."
                    actionText: "Back to editor"
                    actionIcon: "arrow-left"
                }
            }
        }
    }
}
