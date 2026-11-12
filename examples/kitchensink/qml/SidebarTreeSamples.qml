// SPDX-License-Identifier: MPL-2.0
pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// The sidebar tree; `mode` is "tree" or "states" and is set before loading.
// SidebarTreePages.cpp draws the Widgets twin.
Item {
    id: page

    property string mode: "tree"
    readonly property real inset: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)

    implicitWidth: KetTheme.px(KetTheme.t.sidebarWidth, KetTheme.zoom) + 2 * inset
    implicitHeight: loader.implicitHeight + 2 * inset

    Rectangle {
        anchors.fill: parent
        color: KetTheme.t.sidebarBg
        border.width: 1
        border.color: KetTheme.t.sidebarBorder
    }

    Loader {
        id: loader
        x: page.inset
        y: page.inset
        width: page.width - 2 * page.inset
        sourceComponent: page.mode === "states" ? stateSamples : treeSample
    }

    Basic.ButtonGroup { id: rows }

    Component {
        id: treeSample
        Column {
            KetSidebarRepositoryHeader {
                id: repository
                width: parent.width
                name: "ketplus"
                conversationCount: 6
                selected: true
            }
            KetSidebarTreeGuide {
                width: parent.width
                visible: repository.expanded
                KetSidebarSectionLabel { text: "Conversations"; count: 2 }
                KetSidebarRow {
                    text: "Fix the transcript anchors"; number: "2035"; checked: true
                    status: KetTheme.ConversationStatus.Working
                    worktree: "fix/transcript-anchors"; Basic.ButtonGroup.group: rows
                }
                KetSidebarRow {
                    text: "Release notes for 1.4"; number: "2110"
                    status: KetTheme.ConversationStatus.NeedsInput
                    statusDetail: "Allow git push?"; Basic.ButtonGroup.group: rows
                }
                KetSidebarTeamBranch {
                    id: team
                    name: "Mobile Refactor"; memberCount: 3; decisionCount: 1
                }
                KetSidebarTreeGuide {
                    level: 1
                    visible: team.expanded
                    KetSidebarRow {
                        level: 1; text: "Lead Mobile Refactor"; number: "9692"; role: "Leader"
                        status: KetTheme.ConversationStatus.AwaitingReply
                        Basic.ButtonGroup.group: rows
                    }
                    KetSidebarRow {
                        level: 1; text: "ADR and contract for teams"; number: "3651"
                        status: KetTheme.ConversationStatus.Completed; unread: true
                        role: "Developer"; worktree: "team/3/t-3"; Basic.ButtonGroup.group: rows
                    }
                    KetSidebarRow {
                        level: 1; text: "Reviewer"; number: "6913"; role: "Reviewer"
                        status: KetTheme.ConversationStatus.Error; Basic.ButtonGroup.group: rows
                    }
                }
            }
            KetSidebarRepositoryHeader {
                width: parent.width
                name: "ket-qt"
                divided: true
                conversationCount: 2
                waitingCount: 1
                checked: false
            }
        }
    }

    Component {
        id: stateSamples
        Column {
            KetSidebarRepositoryHeader {
                width: parent.width; name: "Hovered"; conversationCount: 1
                previewState: ({ hovered: true })
            }
            KetSidebarRepositoryHeader {
                width: parent.width; name: "Focused"; conversationCount: 1
                previewState: ({ focused: true })
            }
            KetSidebarTeamBranch {
                width: parent.width; name: "Team, hovered"; memberCount: 2
                previewState: ({ hovered: true })
            }
            KetSidebarTeamBranch {
                width: parent.width; name: "Team, focused"; memberCount: 2
                previewState: ({ focused: true })
            }
            KetSidebarTeamBranch {
                width: parent.width; name: "Team, selected"; memberCount: 2; selected: true
            }
            KetSidebarRow { width: parent.width; text: "Not started"; number: "1024" }
            KetSidebarRow {
                width: parent.width; text: "Hovered"; number: "1024"
                status: KetTheme.ConversationStatus.Stopped; previewState: ({ hovered: true })
            }
            KetSidebarRow {
                width: parent.width; text: "Focused"; number: "1024"
                status: KetTheme.ConversationStatus.Working; previewState: ({ focused: true })
            }
            KetSidebarRow {
                width: parent.width; text: "Selected"; number: "1024"; checked: true
                status: KetTheme.ConversationStatus.Completed
            }
            KetSidebarRow {
                width: parent.width; text: "Finished, unread"; number: "1024"; unread: true
                status: KetTheme.ConversationStatus.Completed
            }
            KetSidebarRow {
                width: parent.width; text: "Disabled"; number: "1024"; enabled: false
            }
        }
    }
}
