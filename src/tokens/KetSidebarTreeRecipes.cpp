// SPDX-License-Identifier: MPL-2.0
// Sidebar tree recipes: repository headers, team branches, section labels, one-line rows.
#include "KetSidebarTreeRecipes.h"

#include "KetAppearance.h"
#include "KetProfile.h"

#include <QStringList>

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

const SidebarTreeMetrics& metrics() { return Profile::current().sidebarTree; }

qreal disabledOpacity() { return Profile::current().disabledOpacity; }

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

QColor focusRing(const InteractionState& state, Theme theme) {
    return state.enabled && state.focused ? token(u"focus-border", theme) : transparent();
}

} // namespace

SidebarTreeColumns sidebarTreeColumns(int level, qreal zoom) {
    const SidebarTreeMetrics& m = metrics();
    const qreal indent = m.indentStep * qMax(0, level);
    // Each edge is scaled from its own offset, never summed from scaled parts, so two parts
    // that share a column round to the same pixel.
    SidebarTreeColumns columns;
    columns.guide = Appearance::scale(m.guideStart + indent, zoom);
    columns.chevron = columns.guide - qRound(Appearance::scale(m.chevronSize, zoom) / 2);
    columns.highlight = Appearance::scale(m.highlightStart + indent, zoom);
    columns.mark = Appearance::scale(m.markStart + indent, zoom);
    columns.text = Appearance::scale(m.textStart + indent, zoom);
    return columns;
}

SidebarTreeGuideRecipe sidebarTreeGuideRecipe(Theme theme) {
    return {token(u"sidebar-border", theme), metrics().guideWidth, metrics().branchGap};
}

SidebarRepositoryRecipe sidebarRepositoryRecipe(bool selected, const InteractionState& state,
                                                Theme theme) {
    const SidebarTreeMetrics& m = metrics();
    const bool live = state.enabled;
    SidebarRepositoryRecipe r;
    r.background = live && state.hovered ? token(u"surface-hover", theme) : transparent();
    r.focusRing = focusRing(state, theme);
    r.chevron = token(u"text-muted", theme);
    r.icon = token(u"text-secondary", theme);
    r.name = token(selected ? u"text-main" : u"text-secondary", theme);
    r.dot = token(u"accent-text", theme);
    r.height = m.rowHeight;
    r.dividerLead = m.dividerLead;
    r.radius = m.radius;
    r.chevronSize = m.chevronSize;
    r.iconSize = m.iconSize;
    r.namePixels = m.repositoryPixels;
    r.nameWeight = m.repositoryWeight;
    r.letterSpacing = m.repositoryLetterSpacing;
    r.trailingEnd = m.trailingEnd;
    r.trailingGap = m.trailingGap;
    r.dotSize = m.foldedDotSize;
    r.opacity = live ? 1.0 : disabledOpacity();
    return r;
}

SidebarTeamRecipe sidebarTeamRecipe(bool selected, bool waiting, const InteractionState& state,
                                    Theme theme) {
    const SidebarTreeMetrics& m = metrics();
    const bool live = state.enabled;
    SidebarTeamRecipe r;
    r.highlight = selected                ? token(u"nav-selected", theme)
                  : live && state.hovered ? token(u"surface-hover", theme)
                                          : transparent();
    r.focusRing = focusRing(state, theme);
    r.chevron = token(u"text-muted", theme);
    r.icon = token(selected ? u"accent-text" : u"text-secondary", theme);
    r.name = token(selected ? u"accent-text" : waiting ? u"text-main" : u"text-secondary", theme);
    const ConversationStatusRecipe needsInput =
        conversationStatusRecipe(ConversationStatus::NeedsInput, theme);
    r.waitingIcon = needsInput.icon;
    r.waitingColor = needsInput.iconColor;
    r.height = m.rowHeight;
    r.radius = m.radius;
    r.chevronSize = m.chevronSize;
    r.iconSize = m.iconSize;
    r.namePixels = m.teamPixels;
    r.nameWeight = waiting ? m.teamWaitingWeight : m.teamWeight;
    r.trailingEnd = m.trailingEnd;
    r.trailingGap = m.trailingGap;
    r.branchGap = m.branchGap;
    r.opacity = live ? 1.0 : disabledOpacity();
    return r;
}

SidebarSectionRecipe sidebarSectionRecipe(Theme theme) {
    const SidebarTreeMetrics& m = metrics();
    SidebarSectionRecipe r;
    r.icon = QStringLiteral("chats");
    r.iconColor = token(u"text-muted", theme);
    r.text = token(u"text-secondary", theme);
    r.height = m.sectionHeight;
    r.iconSize = m.iconSize;
    r.pixels = m.sectionPixels;
    r.weight = m.sectionWeight;
    r.actionEnd = m.sectionActionEnd;
    return r;
}

SidebarRowRecipe sidebarRowRecipe(bool selected, bool unread, const InteractionState& state,
                                  Theme theme) {
    const SidebarTreeMetrics& m = metrics();
    // The colours of the two-line row, so a one-line row selects and hovers the same way.
    const ConversationRowRecipe row = conversationRowRecipe(selected, state, theme);
    SidebarRowRecipe r;
    r.background = row.background;
    r.focusRing = row.focusRing;
    r.title = row.title;
    r.number = row.number;
    r.unreadDot = token(u"accent-text", theme);
    r.height = m.rowHeight;
    r.radius = m.radius;
    r.markSize = m.iconSize;
    r.unreadDotSize = m.unreadDotSize;
    r.titlePixels = m.titlePixels;
    r.titleWeight = unread ? m.unreadTitleWeight : m.titleWeight;
    r.numberPixels = m.numberPixels;
    r.numberWeight = m.titleWeight;
    r.numberEnd = m.numberEnd;
    r.titleGap = m.titleGap;
    r.opacity = row.opacity;
    return r;
}

QString sidebarChevronIcon(bool expanded) {
    return expanded ? QStringLiteral("chevron-down") : QStringLiteral("chevron-right");
}

QString sidebarRepositoryDescription(bool expanded, int conversations, int waiting) {
    QString text = expanded ? QStringLiteral("Expanded") : QStringLiteral("Collapsed");
    text += conversations == 1 ? QStringLiteral(", 1 conversation")
                               : QStringLiteral(", %1 conversations").arg(conversations);
    if (waiting > 0)
        text += QStringLiteral(", %1 waiting for you").arg(waiting);
    return text;
}

QString sidebarTeamDescription(int members, int decisions) {
    QString text =
        members == 1 ? QStringLiteral("1 member") : QStringLiteral("%1 members").arg(members);
    if (decisions == 1)
        text += QStringLiteral(", 1 decision waits for you");
    else if (decisions > 1)
        text += QStringLiteral(", %1 decisions wait for you").arg(decisions);
    return text;
}

QString sidebarRowStateText(ConversationStatus status, bool unread, const QString& label,
                            const QString& detail) {
    if (status == ConversationStatus::Completed && unread)
        return QStringLiteral("Completed, unread");
    const QString words = label.isEmpty() ? conversationStatusLabel(status) : label;
    return detail.isEmpty() ? words : words + QStringLiteral(" · ") + detail;
}

QString sidebarRowDescription(const QString& role, const QString& state, const QString& worktree) {
    QStringList parts;
    for (const QString& part : {role, state, worktree})
        if (!part.isEmpty())
            parts.append(part);
    return parts.join(QStringLiteral(", "));
}

QVariantMap toVariantMap(const SidebarTreeColumns& c) {
    return mapOf({{"guide", c.guide},
                  {"chevron", c.chevron},
                  {"highlight", c.highlight},
                  {"mark", c.mark},
                  {"text", c.text}});
}

QVariantMap toVariantMap(const SidebarTreeGuideRecipe& r) {
    return mapOf({{"color", r.color}, {"width", r.width}, {"branchGap", r.branchGap}});
}

QVariantMap toVariantMap(const SidebarRepositoryRecipe& r) {
    return mapOf({{"background", r.background},
                  {"focusRing", r.focusRing},
                  {"chevron", r.chevron},
                  {"icon", r.icon},
                  {"name", r.name},
                  {"dot", r.dot},
                  {"height", r.height},
                  {"dividerLead", r.dividerLead},
                  {"radius", r.radius},
                  {"chevronSize", r.chevronSize},
                  {"iconSize", r.iconSize},
                  {"namePixels", r.namePixels},
                  {"nameWeight", r.nameWeight},
                  {"letterSpacing", r.letterSpacing},
                  {"trailingEnd", r.trailingEnd},
                  {"trailingGap", r.trailingGap},
                  {"dotSize", r.dotSize},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const SidebarTeamRecipe& r) {
    return mapOf({{"highlight", r.highlight},
                  {"focusRing", r.focusRing},
                  {"chevron", r.chevron},
                  {"icon", r.icon},
                  {"name", r.name},
                  {"waitingIcon", r.waitingIcon},
                  {"waitingColor", r.waitingColor},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"chevronSize", r.chevronSize},
                  {"iconSize", r.iconSize},
                  {"namePixels", r.namePixels},
                  {"nameWeight", r.nameWeight},
                  {"trailingEnd", r.trailingEnd},
                  {"trailingGap", r.trailingGap},
                  {"branchGap", r.branchGap},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const SidebarSectionRecipe& r) {
    return mapOf({{"icon", r.icon},
                  {"iconColor", r.iconColor},
                  {"text", r.text},
                  {"height", r.height},
                  {"iconSize", r.iconSize},
                  {"pixels", r.pixels},
                  {"weight", r.weight},
                  {"actionEnd", r.actionEnd}});
}

QVariantMap toVariantMap(const SidebarRowRecipe& r) {
    return mapOf({{"background", r.background},
                  {"focusRing", r.focusRing},
                  {"title", r.title},
                  {"number", r.number},
                  {"unreadDot", r.unreadDot},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"markSize", r.markSize},
                  {"unreadDotSize", r.unreadDotSize},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"numberPixels", r.numberPixels},
                  {"numberWeight", r.numberWeight},
                  {"numberEnd", r.numberEnd},
                  {"titleGap", r.titleGap},
                  {"opacity", r.opacity}});
}

} // namespace ket
