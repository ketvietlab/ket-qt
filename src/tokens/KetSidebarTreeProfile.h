// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QtGlobal>

namespace ket {

// The sidebar tree's metrics (Profile::sidebarTree): a repository header, a team branch, a
// section label and one-line conversation rows, nested one level per team. Every column is
// an offset from the tree's left edge at level 0; each deeper level adds indentStep. A
// chevron is centred on its level's guide line, rows highlight from highlightStart, marks
// (folder, team, status) start at markStart and text at textStart, so the text of a header
// and the rows it holds share one edge.
struct SidebarTreeMetrics {
    qreal rowHeight = 0;     // a conversation row, a team branch and a repository header
    qreal sectionHeight = 0; // "Conversations (n)"
    qreal dividerLead = 0;   // space above a repository header that follows another one
    qreal branchGap = 0;     // space after an unfolded team's members
    qreal indentStep = 0;
    qreal guideStart = 0;
    qreal guideWidth = 0;
    qreal highlightStart = 0;
    qreal markStart = 0;
    qreal textStart = 0;
    qreal chevronSize = 0;
    qreal iconSize = 0;
    qreal radius = 0;
    qreal trailingEnd = 0; // the trailing count, dot or actions from the right edge
    qreal trailingGap = 0;
    qreal foldedDotSize = 0; // a folded repository with something waiting
    qreal unreadDotSize = 0; // a finished conversation not read yet
    qreal numberEnd = 0;     // "#number" from the right edge of a row
    qreal titleGap = 0;      // between a row's title and its number
    qreal sectionActionEnd = 0;

    qreal repositoryPixels = 0;
    int repositoryWeight = 400;
    qreal repositoryLetterSpacing = 0; // pixels; the name is drawn in capitals
    qreal teamPixels = 0;
    int teamWeight = 400;
    int teamWaitingWeight = 400; // while a decision waits for the user
    qreal sectionPixels = 0;
    int sectionWeight = 400;
    qreal titlePixels = 0;
    int titleWeight = 400;
    int unreadTitleWeight = 400;
    qreal numberPixels = 0;

    [[nodiscard]] static SidebarTreeMetrics designSystem();
    [[nodiscard]] static SidebarTreeMetrics ketPlus();
};

} // namespace ket
