// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Surfaces; `mode` is "palette", "explorer", "changes", "tasks", "updates" or "appearance"
// and is set before loading. SurfacePages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "palette"

    implicitWidth: loader.implicitWidth
    implicitHeight: loader.implicitHeight

    Loader {
        id: loader
        sourceComponent: ({ palette: palette, explorer: explorer, changes: changes, tasks: tasks,
                            updates: updates, appearance: appearance })[page.mode]
    }

    readonly property string sampleDiff: "diff --git a/src/NavigationRail.qml b/src/NavigationRail.qml\n"
        + "--- a/src/NavigationRail.qml\n+++ b/src/NavigationRail.qml\n"
        + "@@ -12,6 +12,7 @@ Item {\n     id: rail\n     property bool compact: false\n"
        + "-    width: compact ? 48 : 220\n+    width: compact ? KetTheme.px(48) : KetTheme.px(220)\n"
        + "+    clip: true\n \n     Column {\n         spacing: 4\n"
        + "@@ -40,3 +41,3 @@ Item {\n-        color: \"#f4f4f5\"\n"
        + "+        color: KetTheme.t.sidebarBg\n     }\n"

    Component {
        id: palette
        Item {
            id: stage
            implicitWidth: KetTheme.px(720, KetTheme.zoom)
            implicitHeight: KetTheme.px(460, KetTheme.zoom)
            clip: true

            Rectangle {
                anchors.fill: parent
                color: spotlight.recipe.backdrop
            }

            KetCommandPalette {
                id: spotlight
                parent: stage
                modal: false
                closePolicy: Basic.Popup.NoAutoClose
                groups: [
                    { title: "Files", items: [
                        { id: "rail", title: "NavigationRail.qml",
                          detail: "src/components/NavigationRail.qml", iconName: "file-code" },
                        { id: "readme", title: "README.md", detail: "README.md",
                          iconName: "file" }] },
                    { title: "Commands", items: [
                        { id: "theme", title: "Toggle dark theme", detail: "Appearance",
                          iconName: "moon" },
                        { id: "terminal", title: "New terminal", detail: "Workspace · ⌃`",
                          iconName: "terminal" }] },
                    { title: "Conversations", items: [
                        { id: "chat", title: "Refine the sidebar spacing",
                          detail: "develop · 2 hours ago", iconName: "bot" }] }
                ]
                Component.onCompleted: open()
            }
        }
    }

    Component {
        id: explorer
        Column {
            width: KetTheme.px(280, KetTheme.zoom)
            Repeater {
                model: [
                    { name: "src", folder: true, depth: 0, status: "", open: true },
                    { name: "components", folder: true, depth: 1, status: "", open: true },
                    { name: "NavigationRail.qml", folder: false, depth: 2, status: "M",
                      active: true },
                    { name: "Composer.qml", folder: false, depth: 2, status: "A" },
                    { name: "assets", folder: true, depth: 1, status: "", open: false },
                    { name: "README.md", folder: false, depth: 0, status: "" }
                ]
                KetFileRow {
                    required property var modelData
                    width: parent.width
                    text: modelData.name
                    folder: modelData.folder
                    depth: modelData.depth
                    status: modelData.status
                    checked: !!modelData.open
                    active: !!modelData.active
                }
            }
        }
    }

    Component {
        id: changes
        Column {
            width: KetTheme.px(560, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            KetChangedFiles {
                width: parent.width
                files: [
                    { name: "NavigationRail.qml", path: "src/components", added: 2, removed: 2 },
                    { name: "Composer.qml", path: "src/components", added: 48, removed: 0 },
                    { name: "README.md", path: "", added: 3, removed: 1 }
                ]
                note: "Not committed"
                footer: KetButton { text: "Review changes" }
            }
            KetDiffView {
                id: diff
                width: parent.width
                lines: parseUnified(page.sampleDiff)
            }
        }
    }

    Component {
        id: tasks
        KetTaskBar {
            id: bar
            width: KetTheme.px(560, KetTheme.zoom)
            tasks: [
                { title: "npm run dev", meta: "localhost:5173 · 2m 14s", current: true,
                  status: KetTheme.TaskStatus.Running },
                { title: "Review the diff", meta: "Completed in 48s", kindIcon: "bot",
                  status: KetTheme.TaskStatus.Completed },
                { title: "ctest --test-dir build", meta: "2 tests failed",
                  status: KetTheme.TaskStatus.Failed }
            ]
            onStopRequested: index => {
                const next = tasks.slice()
                next[index] = Object.assign({}, next[index],
                                            { status: KetTheme.TaskStatus.Stopped })
                tasks = next
            }
            onDismissRequested: index => tasks = tasks.filter((task, i) => i !== index)
        }
    }

    Component {
        id: updates
        Column {
            width: KetTheme.px(520, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
            KetUpdateCard {
                width: parent.width
                title: "KetPlus 1.8.0"
                badges: KetBadge { text: "Stable"; tone: KetTheme.BadgeTone.Accent }
                detail: "Downloading · 42 MB of 96 MB"
                progress: 44
            }
            KetUpdateCard {
                width: parent.width
                title: "KetPlus 1.8.0"
                ready: true
                badges: KetBadge { text: "Ready"; tone: KetTheme.BadgeTone.Success }
                detail: "Restart to finish installing."
                notes: ["Command palette searches conversations",
                        "Background tasks keep running after a restart"]
                actions: [
                    KetButton { text: "Restart now"; variant: KetTheme.ButtonVariant.Primary },
                    KetButton { text: "Later"; variant: KetTheme.ButtonVariant.Ghost }
                ]
            }
            KetUpdatePill {
                width: KetTheme.px(240, KetTheme.zoom)
                text: "Update available"
                detail: "KetPlus 1.8.0"
            }
        }
    }

    Component {
        id: appearance
        Column {
            width: KetTheme.px(520, KetTheme.zoom)
            spacing: KetTheme.px(KetTheme.t.space4, KetTheme.zoom)
            Basic.ButtonGroup { id: themes }
            Row {
                width: parent.width
                spacing: KetTheme.px(KetTheme.t.space3, KetTheme.zoom)
                KetThemeCard {
                    width: (parent.width - parent.spacing) / 2
                    text: "Light"
                    checked: true
                    Basic.ButtonGroup.group: themes
                }
                KetThemeCard {
                    width: (parent.width - parent.spacing) / 2
                    text: "Dark"
                    previewDark: true
                    Basic.ButtonGroup.group: themes
                }
            }
            Row {
                spacing: KetTheme.px(KetTheme.t.space25, KetTheme.zoom)
                KetAccentSwatch {}
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Indigo"
                    color: KetTheme.t.textMain
                    font.family: KetTheme.fontFamily
                    font.pixelSize: KetTheme.px(KetTheme.t.textMd, KetTheme.zoom)
                    font.weight: Font.DemiBold
                }
            }
        }
    }
}
