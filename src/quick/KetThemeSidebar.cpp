// SPDX-License-Identifier: MPL-2.0
// KetTheme's sidebar tree recipes (KetSidebarTreeRecipes).
#include "KetTheme.h"
#include "KetThemeDetail.h"

#include "KetSidebarTreeRecipes.h"

namespace ket {
namespace {

using detail::scaleLengths;
using detail::themeOf;

} // namespace

QVariantMap QuickTheme::sidebarTreeColumns(int level, qreal zoom) const {
    // Already scaled: each column is rounded from its own offset, as the widgets do.
    return toVariantMap(ket::sidebarTreeColumns(level, zoom));
}

QVariantMap QuickTheme::sidebarTreeGuideRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::sidebarTreeGuideRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"width", u"branchGap"});
    return recipe;
}

QVariantMap QuickTheme::sidebarRepositoryRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                                bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    const SidebarRepositoryRecipe r = ket::sidebarRepositoryRecipe(selected, state, themeOf(dark));
    QVariantMap recipe = toVariantMap(r);
    scaleLengths(recipe, zoom,
                 {u"height", u"dividerLead", u"radius", u"chevronSize", u"iconSize", u"namePixels",
                  u"trailingEnd", u"trailingGap", u"dotSize"});
    // Tracking follows the zoom exactly, as in the widget; it is below a pixel.
    recipe.insert(QStringLiteral("letterSpacing"), r.letterSpacing * zoom);
    return recipe;
}

QVariantMap QuickTheme::sidebarTeamRecipe(bool dark, qreal zoom, bool selected, bool waiting,
                                          bool hovered, bool pressed, bool enabled,
                                          bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::sidebarTeamRecipe(selected, waiting, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"chevronSize", u"iconSize", u"namePixels", u"trailingEnd",
                  u"trailingGap", u"branchGap"});
    return recipe;
}

QVariantMap QuickTheme::sidebarSectionRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::sidebarSectionRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"height", u"iconSize", u"pixels", u"actionEnd"});
    return recipe;
}

QVariantMap QuickTheme::sidebarRowRecipe(bool dark, qreal zoom, bool selected, bool unread,
                                         bool hovered, bool pressed, bool enabled,
                                         bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::sidebarRowRecipe(selected, unread, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"markSize", u"unreadDotSize", u"titlePixels",
                  u"numberPixels", u"numberEnd", u"titleGap"});
    return recipe;
}

QString QuickTheme::sidebarChevronIcon(bool expanded) const {
    return ket::sidebarChevronIcon(expanded);
}

QString QuickTheme::sidebarRepositoryDescription(bool expanded, int conversations,
                                                 int waiting) const {
    return ket::sidebarRepositoryDescription(expanded, conversations, waiting);
}

QString QuickTheme::sidebarTeamDescription(int members, int decisions) const {
    return ket::sidebarTeamDescription(members, decisions);
}

QString QuickTheme::sidebarRowStateText(int status, bool unread, const QString& label,
                                        const QString& detail) const {
    return ket::sidebarRowStateText(ket::ConversationStatus(status), unread, label, detail);
}

QString QuickTheme::sidebarRowDescription(const QString& role, const QString& state,
                                          const QString& worktree) const {
    return ket::sidebarRowDescription(role, state, worktree);
}

} // namespace ket
