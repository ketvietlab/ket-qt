// SPDX-License-Identifier: MPL-2.0
// Profile::sidebarTree for the two shipped profiles.
#include "KetSidebarTreeProfile.h"

#include "KetTokens.h"

namespace ket {
namespace {

qreal px(QStringView token) { return Tokens::px(token); }
int weight(QStringView token) { return qRound(Tokens::number(token)); }

} // namespace

SidebarTreeMetrics SidebarTreeMetrics::designSystem() {
    SidebarTreeMetrics m;
    m.rowHeight = px(u"sidebar-item-height");
    m.sectionHeight = px(u"sidebar-child-height");
    m.dividerLead = px(u"space-1-5");
    m.branchGap = px(u"space-0-5");
    m.indentStep = px(u"sidebar-indent");
    m.chevronSize = px(u"space-3");
    m.guideStart = px(u"space-2") + m.chevronSize / 2;
    m.guideWidth = 1; // a hairline
    m.highlightStart = px(u"space-5");
    m.markStart = px(u"space-6");
    m.iconSize = px(u"space-4");
    m.textStart = m.markStart + m.iconSize + px(u"space-1");
    m.radius = px(u"radius-md");
    m.trailingEnd = px(u"space-1-5");
    m.trailingGap = px(u"space-1-5");
    m.foldedDotSize = px(u"space-1-5");
    m.unreadDotSize = px(u"space-2");
    m.numberEnd = px(u"space-2");
    m.titleGap = px(u"space-2-5");
    m.sectionActionEnd = px(u"space-1");
    m.repositoryPixels = px(u"text-xs");
    m.repositoryWeight = weight(u"weight-semibold");
    m.teamPixels = px(u"text-sm");
    m.teamWeight = weight(u"weight-medium");
    m.teamWaitingWeight = weight(u"weight-semibold");
    m.sectionPixels = px(u"text-xs");
    m.sectionWeight = weight(u"weight-medium");
    m.titlePixels = px(u"text-sm");
    m.titleWeight = weight(u"weight-normal");
    m.unreadTitleWeight = weight(u"weight-semibold");
    m.numberPixels = px(u"text-2xs");
    return m;
}

// The sidebar tree of the KetPlus Qt mock (mockups/qt/qml/shell: SidebarRepoHeader,
// TeamFolder, SidebarSection and SidebarRow).
SidebarTreeMetrics SidebarTreeMetrics::ketPlus() {
    SidebarTreeMetrics m = designSystem();
    m.indentStep = 18;
    m.chevronSize = 10;
    m.guideStart = 13;
    m.highlightStart = 20;
    m.markStart = 24;
    m.iconSize = 14; // the 14 px master
    m.textStart = 44;
    m.radius = 6;
    m.repositoryLetterSpacing = 0.4;
    return m;
}

} // namespace ket
