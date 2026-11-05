// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Ket

// Feedback and overlays; `mode` is "banners", "previews" or "live" and is set before
// loading. OverlayPages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "banners"
    readonly property real gap: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: page.mode === "previews" ? previews
                       : page.mode === "live" ? live : banners
    }

    component SampleMenu: KetMenu {
        KetMenuItem { text: "Open in Finder"; iconName: "folder-open" }
        KetMenuItem { text: "Rename"; iconName: "edit"; shortcut: "F2" }
        KetMenuItem {
            text: "Pin to sidebar"
            checkable: true
            checked: true
            description: "Keep it above recent conversations."
        }
        KetMenuItem { text: "Archive"; enabled: false }
        KetMenuSeparator {}
        KetMenuItem { text: "Delete"; iconName: "trash"; danger: true }
    }

    component RenameDialog: KetDialog {
        title: "Rename conversation"

        KetFieldRow {
            Layout.fillWidth: true
            label: "Name"
            help: "Shown in the sidebar and the window title."

            KetTextField { text: "Release checklist" }
        }

        actions: [
            KetButton { text: "Cancel"; onClicked: self.reject() },
            KetButton {
                text: "Rename"
                variant: KetTheme.ButtonVariant.Primary
                onClicked: self.accept()
            }
        ]
        id: self
    }

    Component {
        id: banners
        Column {
            width: KetTheme.px(520, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)

            KetBanner {
                width: parent.width
                tone: KetTheme.BannerTone.Info
                title: "Update ready"
                message: "KetPlus 2.4 installs when you restart."
            }
            KetBanner {
                width: parent.width
                tone: KetTheme.BannerTone.Warning
                title: "Router unavailable"
                message: "Requests use the direct connection until it returns."
            }
            KetBanner {
                width: parent.width
                tone: KetTheme.BannerTone.Danger
                title: "Sync failed"
                message: "Check your connection and try again."
                actionText: "Retry"
                actionIcon: "refresh"
            }
        }
    }

    // Overlays opened in place; each sits in its own fixed stage.
    Component {
        id: previews
        Column {
            spacing: page.gap

            Row {
                spacing: KetTheme.px(KetTheme.t.space6, KetTheme.zoom)

                Item {
                    id: menuStage
                    implicitWidth: menu.width
                    implicitHeight: menu.height

                    SampleMenu {
                        id: menu
                        parent: menuStage
                        closePolicy: Basic.Popup.NoAutoClose
                        Component.onCompleted: { popup(0, 0); currentIndex = 1 }
                    }
                }

                Item {
                    id: tipStage
                    implicitWidth: tip.width
                    implicitHeight: tip.height

                    KetToolTip {
                        id: tip
                        parent: tipStage
                        x: 0
                        y: 0
                        text: "Copy link"
                        delay: 0
                        timeout: -1
                        visible: true
                        closePolicy: Basic.Popup.NoAutoClose
                    }
                }
            }

            Item {
                id: dialogStage
                implicitWidth: KetTheme.px(620, KetTheme.zoom)
                implicitHeight: KetTheme.px(400, KetTheme.zoom)
                clip: true

                RenameDialog {
                    parent: dialogStage
                    closePolicy: Basic.Popup.NoAutoClose
                    modal: false
                    Component.onCompleted: open()
                }

                Rectangle {
                    anchors.fill: parent
                    z: -1
                    color: KetTheme.dialogRecipe(KetTheme.dark, KetTheme.zoom).backdrop
                }
            }

            Item {
                id: toastStage
                implicitWidth: KetTheme.px(420, KetTheme.zoom)
                implicitHeight: KetTheme.px(96, KetTheme.zoom)

                KetToast {
                    parent: toastStage
                    text: "Settings saved"
                    Component.onCompleted: open()
                }
            }
        }
    }

    Component {
        id: live
        Item {
            implicitWidth: KetTheme.px(640, KetTheme.zoom)
            implicitHeight: KetTheme.px(420, KetTheme.zoom)

            Row {
                spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)

                KetButton { text: "Show toast"; onClicked: toast.show("Settings saved") }
                KetButton { text: "Open dialog"; onClicked: liveDialog.open() }
                KetButton {
                    id: menuButton
                    text: "Context menu"
                    onClicked: liveMenu.popup(menuButton, 0, menuButton.height + 4)
                }
                KetButton {
                    id: tipButton
                    text: "Hover for tooltip"
                    variant: KetTheme.ButtonVariant.Ghost

                    KetToolTip {
                        visible: tipButton.hovered
                        text: "Tooltips use the accent background."
                    }
                }
            }

            KetToast { id: toast }
            RenameDialog { id: liveDialog }
            SampleMenu { id: liveMenu }
        }
    }
}
