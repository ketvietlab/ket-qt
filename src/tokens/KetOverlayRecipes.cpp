// SPDX-License-Identifier: MPL-2.0
// Feedback and overlay recipes: banners, toasts, dialogs, menus and tooltips.
#include "KetRecipes.h"

#include "KetProfile.h"

#include <QRegularExpression>

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

QColor transparent() { return QColor(Qt::transparent); }

const Profile& profile() { return Profile::current(); }

} // namespace

Shadow shadowToken(QStringView name, Theme theme) {
    static const QRegularExpression pattern(QStringLiteral(
        R"(^(-?[\d.]+)(?:px)? (-?[\d.]+)(?:px)? ([\d.]+)(?:px)?(?: -?[\d.]+px)? (#[0-9a-fA-F]+))"));
    Shadow shadow;
    const QRegularExpressionMatch match = pattern.match(Tokens::raw(name, theme));
    if (!match.hasMatch())
        return shadow;
    shadow.x = match.captured(1).toDouble();
    shadow.y = match.captured(2).toDouble();
    shadow.blur = match.captured(3).toDouble();
    shadow.color = QColor::fromString(match.captured(4));
    return shadow;
}

BannerRecipe bannerRecipe(BannerTone tone, Theme theme) {
    BannerRecipe recipe;
    switch (tone) {
    case BannerTone::Info:
        recipe.background = token(u"accent-subtle", theme);
        recipe.border = token(u"accent-border", theme);
        recipe.text = token(u"accent-text", theme);
        recipe.icon = QStringLiteral("info");
        break;
    case BannerTone::Warning:
        recipe.background = token(u"warning-bg", theme);
        recipe.border = token(u"warning-border", theme);
        recipe.text = token(u"warning", theme);
        recipe.icon = QStringLiteral("info");
        break;
    case BannerTone::Danger:
        recipe.background = token(u"danger-bg", theme);
        recipe.border = token(u"danger-border", theme);
        recipe.text = token(u"danger", theme);
        recipe.icon = QStringLiteral("warning");
        break;
    }
    recipe.radius = profile().bannerRadius;
    recipe.paddingInline = profile().bannerPaddingInline;
    recipe.paddingBlock = profile().bannerPaddingBlock;
    recipe.spacing = profile().bannerGap;
    recipe.iconSize = profile().iconSize;
    recipe.titlePixels = profile().buttonFontPixels;
    recipe.titleWeight = profile().bannerTitleWeight;
    recipe.messagePixels = profile().smallButtonFontPixels;
    recipe.messageGap = profile().bannerMessageGap;
    return recipe;
}

ToastRecipe toastRecipe(Theme theme) {
    ToastRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"positive-border", theme);
    recipe.text = token(u"text-main", theme);
    recipe.icon = token(u"success", theme);
    recipe.shadow = shadowToken(u"shadow-sm", theme);
    recipe.radius = Tokens::px(u"radius-md");
    recipe.paddingInline = profile().toastPaddingInline;
    recipe.paddingBlock = profile().toastPaddingBlock;
    recipe.spacing = profile().toastGap;
    recipe.iconSize = profile().iconSize;
    recipe.fontPixels = profile().buttonFontPixels;
    recipe.bottomOffset = Tokens::px(u"space-10");
    recipe.durationMs = profile().toastDurationMs;
    return recipe;
}

DialogRecipe dialogRecipe(Theme theme) {
    DialogRecipe recipe;
    // The backdrop darkens in both themes: the light theme's main text colour, translucent.
    recipe.backdrop = token(u"text-main", Theme::Light);
    recipe.backdrop.setAlphaF(float(profile().backdropOpacity));
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"border-default", theme);
    recipe.divider = token(u"panel-border", theme);
    recipe.footer = token(u"surface-raised", theme);
    recipe.title = token(u"text-main", theme);
    recipe.shadow = shadowToken(u"shadow-popover", theme);
    recipe.width = profile().dialogWidth;
    recipe.radius = profile().dialogRadius;
    recipe.paddingInline = profile().dialogPaddingInline;
    recipe.headerPaddingBlock = profile().dialogHeaderPaddingBlock;
    recipe.bodyPadding = profile().dialogPaddingInline;
    recipe.footerPaddingBlock = profile().dialogFooterPaddingBlock;
    recipe.footerGap = Tokens::px(u"space-2");
    recipe.titlePixels = profile().dialogTitlePixels;
    recipe.titleWeight = qRound(Tokens::number(u"weight-semibold"));
    recipe.margin = Tokens::px(u"space-6");
    return recipe;
}

MenuRecipe menuRecipe(Theme theme) {
    MenuRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"border-strong", theme);
    recipe.separator = token(u"panel-border", theme);
    recipe.sectionText = token(u"text-muted", theme);
    recipe.shadow = shadowToken(u"shadow-md", theme);
    recipe.minWidth = profile().menuMinWidth;
    recipe.maxWidth = profile().menuMaxWidth;
    recipe.padding = profile().menuPadding;
    recipe.radius = profile().menuRadius;
    recipe.separatorMargin = profile().menuPadding - 1;
    recipe.sectionHeight = profile().menuSectionHeight;
    recipe.sectionPixels = Tokens::px(u"text-2xs");
    return recipe;
}

MenuItemRecipe menuItemRecipe(bool highlighted, bool enabled, MenuItemTone tone, Theme theme) {
    MenuItemRecipe recipe;
    const bool live = enabled && highlighted;
    recipe.background = live ? token(u"accent-subtle", theme) : transparent();
    if (!enabled)
        recipe.text = token(u"text-muted", theme);
    else if (tone == MenuItemTone::Danger)
        recipe.text = token(u"danger", theme);
    else if (tone == MenuItemTone::Warning)
        recipe.text = token(u"warning", theme);
    else
        recipe.text = token(live ? u"accent-text" : u"text-main", theme);
    // A warning keeps its colour on both lines, so the elevated choice reads at a glance.
    if (enabled && tone == MenuItemTone::Warning)
        recipe.description = recipe.text;
    else
        recipe.description =
            live && tone == MenuItemTone::Normal ? recipe.text : token(u"text-muted", theme);
    recipe.height = profile().menuItemHeight;
    recipe.radius = profile().menuItemRadius;
    recipe.paddingStart = profile().menuItemPaddingStart;
    recipe.paddingEnd = profile().menuItemPaddingEnd;
    recipe.spacing = profile().menuItemGap;
    recipe.checkSize = profile().menuCheckSize;
    recipe.iconSize = profile().smallIconSize;
    recipe.fontPixels = profile().menuItemFontPixels;
    recipe.descriptionPixels = Tokens::px(u"text-2xs");
    recipe.descriptionGap = profile().menuDescriptionGap;
    recipe.opacity = enabled ? 1.0 : profile().menuDisabledOpacity;
    return recipe;
}

TooltipRecipe tooltipRecipe(Theme theme) {
    TooltipRecipe recipe;
    recipe.background = token(u"accent", theme);
    recipe.text = token(u"action-primary-ink", theme);
    recipe.radius = Tokens::px(u"radius-sm");
    recipe.paddingInline = profile().tooltipPaddingInline;
    recipe.paddingBlock = profile().tooltipPaddingBlock;
    recipe.maxWidth = profile().tooltipMaxWidth;
    recipe.fontPixels = profile().smallButtonFontPixels;
    recipe.delayMs = profile().tooltipDelayMs;
    return recipe;
}

namespace {

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

} // namespace

QVariantMap toVariantMap(const BannerRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"icon", r.icon},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"messagePixels", r.messagePixels},
                  {"messageGap", r.messageGap}});
}

QVariantMap toVariantMap(const ToastRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"text", r.text},
                  {"icon", r.icon},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"fontPixels", r.fontPixels},
                  {"bottomOffset", r.bottomOffset},
                  {"durationMs", r.durationMs}});
}

QVariantMap toVariantMap(const DialogRecipe& r) {
    return mapOf({{"backdrop", r.backdrop},
                  {"background", r.background},
                  {"border", r.border},
                  {"divider", r.divider},
                  {"footer", r.footer},
                  {"title", r.title},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"width", r.width},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"headerPaddingBlock", r.headerPaddingBlock},
                  {"bodyPadding", r.bodyPadding},
                  {"footerPaddingBlock", r.footerPaddingBlock},
                  {"footerGap", r.footerGap},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"margin", r.margin}});
}

QVariantMap toVariantMap(const MenuRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"separator", r.separator},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"minWidth", r.minWidth},
                  {"maxWidth", r.maxWidth},
                  {"padding", r.padding},
                  {"radius", r.radius},
                  {"separatorMargin", r.separatorMargin},
                  {"sectionText", r.sectionText},
                  {"sectionHeight", r.sectionHeight},
                  {"sectionPixels", r.sectionPixels}});
}

QVariantMap toVariantMap(const MenuItemRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"description", r.description},
                  {"height", r.height},
                  {"radius", r.radius},
                  {"paddingStart", r.paddingStart},
                  {"paddingEnd", r.paddingEnd},
                  {"spacing", r.spacing},
                  {"checkSize", r.checkSize},
                  {"iconSize", r.iconSize},
                  {"fontPixels", r.fontPixels},
                  {"descriptionPixels", r.descriptionPixels},
                  {"descriptionGap", r.descriptionGap},
                  {"opacity", r.opacity}});
}

QVariantMap toVariantMap(const TooltipRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"radius", r.radius},
                  {"paddingInline", r.paddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"maxWidth", r.maxWidth},
                  {"fontPixels", r.fontPixels},
                  {"delayMs", r.delayMs}});
}

} // namespace ket
