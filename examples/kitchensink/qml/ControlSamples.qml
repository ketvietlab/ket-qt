// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Controls and chrome; `mode` is "sliders", "avatars", "sidebar", "tabs" or "chrome" and is
// set before loading. ControlPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "sliders"

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: ({ sliders: sliders, avatars: avatars, sidebar: sidebar, tabs: tabs,
                            chrome: chrome })[page.mode]
    }

    component Surface: Rectangle {
        property string surface: "panelBg"
        color: KetTheme.t[surface]
        border.width: 1
        border.color: KetTheme.t.panelBorder
    }

    Component {
        id: sliders
        Column {
            width: KetTheme.px(420, KetTheme.zoom)
            spacing: KetTheme.px(20, KetTheme.zoom)
            KetStepSlider {
                width: parent.width
                label: "Reasoning effort"
                steps: ["low", "medium", "high", "xhigh"]
                currentIndex: 2
            }
            Row {
                spacing: KetTheme.px(24, KetTheme.zoom)
                KetSlider { width: (parent.parent.width - parent.spacing) / 2; from: 0; to: 100; value: 30 }
                KetSlider { width: (parent.parent.width - parent.spacing) / 2; from: 0; to: 100; value: 70; enabled: false }
            }
        }
    }

    Component {
        id: avatars
        Row {
            spacing: KetTheme.px(12, KetTheme.zoom)
            KetAvatar { anchors.verticalCenter: parent.verticalCenter; name: "Kieu Duy" }
            KetAvatar { anchors.verticalCenter: parent.verticalCenter; name: "Jay" }
            KetAvatar { anchors.verticalCenter: parent.verticalCenter; name: "Linh Tran" }
            KetAvatar { anchors.verticalCenter: parent.verticalCenter; name: "Kieu Duy"; diameter: 40 }
            KetAvatar {
                anchors.verticalCenter: parent.verticalCenter
                name: "Sidebar"; source: "qrc:/kitchensink/images/screenshot-sidebar.png"
            }
        }
    }

    Component {
        id: sidebar
        Surface {
            surface: "sidebarBg"
            implicitWidth: KetTheme.px(300, KetTheme.zoom)
            implicitHeight: KetTheme.px(330, KetTheme.zoom)

            Basic.ButtonGroup { id: trees }

            Column {
                anchors.fill: parent
                anchors.margins: KetTheme.px(10, KetTheme.zoom)
                spacing: KetTheme.px(3, KetTheme.zoom)

                KetGroupHeader { width: parent.width; name: "ketplus"; selected: true }
                KetTreeItem {
                    width: parent.width; text: "develop"; iconName: "branch"
                    detail: "~/dev/ketplus"; count: "3"; checked: true
                    Basic.ButtonGroup.group: trees
                }
                KetTreeItem {
                    width: parent.width; text: "feat/new-organization"; iconName: "branch"
                    activity: "working"; activityText: "Working · 2 conversations"
                    Basic.ButtonGroup.group: trees
                }
                KetTreeItem {
                    width: parent.width; text: "fix/sync-resume"; iconName: "branch"
                    activity: "waiting"; activityText: "Waiting for input"
                    Basic.ButtonGroup.group: trees
                }
                KetTreeItem {
                    width: parent.width; text: "release/1.4"; iconName: "branch"
                    previewState: ({ hovered: true })
                    Basic.ButtonGroup.group: trees
                }
                Item { width: 1; height: KetTheme.px(7, KetTheme.zoom) }
                KetGroupHeader {
                    width: parent.width; name: "ket-viewer · studio-mac"
                    iconName: "display"; expanded: false
                }
                Item { width: 1; height: KetTheme.px(7, KetTheme.zoom) }
                // Activatable: the icon folds, the name opens the group's page.
                KetGroupHeader {
                    width: parent.width; name: "Checkout"; iconName: "users"
                    activatable: true
                }
            }
        }
    }

    Component {
        id: tabs
        KetRouteTabs {
            width: KetTheme.px(520, KetTheme.zoom)
            tabs: ["General", "Providers", "Appearance", "Updates"]
            currentIndex: 1
        }
    }

    Component {
        id: chrome
        Surface {
            implicitWidth: KetTheme.px(620, KetTheme.zoom)
            implicitHeight: KetTheme.px(180, KetTheme.zoom)

            Column {
                width: parent.width
                KetPaneHeader {
                    width: parent.width; title: "ConversationRow.qml"; iconName: "file-code"
                    KetIconButton {
                        anchors.verticalCenter: parent.verticalCenter
                        controlSize: KetTheme.ControlSize.Small; iconName: "close"; text: "Close pane"
                    }
                }
                KetBreadcrumb {
                    width: parent.width
                    path: ["modules", "ai", "src", "agent", "qml", "ConversationRow.qml"]
                    KetButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "View diff"; iconName: "compare"
                        variant: KetTheme.ButtonVariant.Ghost; controlSize: KetTheme.ControlSize.Small
                    }
                }
            }

            KetStatusBar {
                anchors.bottom: parent.bottom
                width: parent.width
                trailing: [
                    KetStatusItem { anchors.verticalCenter: parent.verticalCenter; text: "UTF-8" },
                    KetStatusItem { anchors.verticalCenter: parent.verticalCenter; text: "LF" },
                    KetStatusItem { anchors.verticalCenter: parent.verticalCenter; text: "KétPlus Pro" },
                    KetStatusItem { anchors.verticalCenter: parent.verticalCenter; iconName: "keyboard" }
                ]
                KetStatusItem {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Local workspace"; iconName: "success"
                }
            }
        }
    }
}
