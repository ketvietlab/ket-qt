// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetConversationRecipes.h"

namespace ket {

// The sidebar tree (Profile::sidebarTree): a repository header, a team branch, a section
// label and one-line conversation rows. Level 0 holds what sits directly under a
// repository; each team adds one level for its members.

// The columns of one level, scaled to whole pixels at `zoom`, from the tree's left edge.
// Every part of the tree places itself with these, so a header's text and the text of the
// rows under it share one edge at every zoom.
struct SidebarTreeColumns {
    qreal guide = 0;     // the guide line's left edge; a chevron is centred on it
    qreal chevron = 0;   // the chevron box's left edge
    qreal highlight = 0; // where a row's hover and selection start, right of the guide
    qreal mark = 0;      // the folder, team or status mark
    qreal text = 0;
};

struct SidebarTreeGuideRecipe {
    QColor color;
    qreal width = 0;
    qreal branchGap = 0; // below a team's members
};

// A repository header: chevron, folder, the name in capitals, then a dot when folded with
// something waiting, the conversation count and an add action shown on hover or focus.
struct SidebarRepositoryRecipe {
    QColor background; // hover
    QColor focusRing;
    QColor chevron;
    QColor icon;
    QColor name;
    QColor dot;
    qreal height = 0;
    qreal dividerLead = 0;
    qreal radius = 0;
    qreal chevronSize = 0;
    qreal iconSize = 0;
    qreal namePixels = 0;
    int nameWeight = 400;
    qreal letterSpacing = 0;
    qreal trailingEnd = 0;
    qreal trailingGap = 0;
    qreal dotSize = 0;
    qreal opacity = 1.0;
};

// A team branch: the chevron folds the members, the name opens the team. A waiting decision
// shows the needs-input mark and a heavier name.
struct SidebarTeamRecipe {
    QColor highlight; // selection or hover, from SidebarTreeColumns::highlight
    QColor focusRing;
    QColor chevron;
    QColor icon;
    QColor name;
    QString waitingIcon;
    QColor waitingColor;
    qreal height = 0;
    qreal radius = 0;
    qreal chevronSize = 0;
    qreal iconSize = 0;
    qreal namePixels = 0;
    int nameWeight = 400;
    qreal trailingEnd = 0;
    qreal trailingGap = 0;
    qreal branchGap = 0;
    qreal opacity = 1.0;
};

// "Conversations (n)" with an add action.
struct SidebarSectionRecipe {
    QString icon;
    QColor iconColor;
    QColor text;
    qreal height = 0;
    qreal iconSize = 0;
    qreal pixels = 0;
    int weight = 400;
    qreal actionEnd = 0;
};

// A one-line conversation row: the status mark (or the unread dot), the title, "#number".
struct SidebarRowRecipe {
    QColor background;
    QColor focusRing;
    QColor title;
    QColor number;
    QColor unreadDot;
    qreal height = 0;
    qreal radius = 0;
    qreal markSize = 0;
    qreal unreadDotSize = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal numberPixels = 0; // drawn in the code font
    int numberWeight = 400;
    qreal numberEnd = 0;
    qreal titleGap = 0;
    qreal opacity = 1.0;
};

[[nodiscard]] SidebarTreeColumns sidebarTreeColumns(int level, qreal zoom);
[[nodiscard]] SidebarTreeGuideRecipe sidebarTreeGuideRecipe(Theme theme);
[[nodiscard]] SidebarRepositoryRecipe
sidebarRepositoryRecipe(bool selected, const InteractionState& state, Theme theme);
[[nodiscard]] SidebarTeamRecipe sidebarTeamRecipe(bool selected, bool waiting,
                                                  const InteractionState& state, Theme theme);
[[nodiscard]] SidebarSectionRecipe sidebarSectionRecipe(Theme theme);
[[nodiscard]] SidebarRowRecipe sidebarRowRecipe(bool selected, bool unread,
                                                const InteractionState& state, Theme theme);

// The chevron of a fold: down while unfolded.
[[nodiscard]] QString sidebarChevronIcon(bool expanded);
// The words assistive technology reads, in English; apps translate them.
[[nodiscard]] QString sidebarRepositoryDescription(bool expanded, int conversations, int waiting);
[[nodiscard]] QString sidebarTeamDescription(int members, int decisions);
// "Completed, unread" for a finished row not read yet, else the status label and the detail.
[[nodiscard]] QString sidebarRowStateText(ConversationStatus status, bool unread,
                                          const QString& label, const QString& detail);
// The role, the state in words and the worktree, joined; empty parts are left out.
[[nodiscard]] QString sidebarRowDescription(const QString& role, const QString& state,
                                            const QString& worktree);

[[nodiscard]] QVariantMap toVariantMap(const SidebarTreeColumns& columns);
[[nodiscard]] QVariantMap toVariantMap(const SidebarTreeGuideRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SidebarRepositoryRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SidebarTeamRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SidebarSectionRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SidebarRowRecipe& recipe);

} // namespace ket
