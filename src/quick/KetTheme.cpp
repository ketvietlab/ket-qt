// SPDX-License-Identifier: MPL-2.0
#include "KetTheme.h"
#include "KetThemeDetail.h"

#include "KetAppearance.h"
#include "KetConversationRecipes.h"
#include "KetIcons.h"
#include "KetMarkdown.h"
#include "KetProfile.h"
#include "KetRecipes.h"
#include "KetRichRecipes.h"

#include <QFontDatabase>
#include <QGuiApplication>
#include <QQuickTextDocument>
#include <QTextCursor>
#include <QTextDocument>

namespace ket {
namespace {

using detail::kShadowLengths;
using detail::scaleLengths;
using detail::themeOf;

static_assert(int(QuickTheme::ButtonVariant::GhostDanger) == int(ButtonVariant::GhostDanger));
static_assert(int(QuickTheme::ControlSize::Medium) == int(ControlSize::Medium));
static_assert(int(QuickTheme::BadgeTone::Danger) == int(BadgeTone::Danger));
static_assert(int(QuickTheme::IconWeight::Regular) == int(ket::IconWeight::Regular));
static_assert(int(QuickTheme::ToggleKind::Radio) == int(ket::ToggleKind::Radio));
static_assert(int(QuickTheme::BannerTone::Danger) == int(ket::BannerTone::Danger));
static_assert(int(QuickTheme::ConversationStatus::Stopped) ==
              int(ket::ConversationStatus::Stopped));

} // namespace

QuickTheme::QuickTheme(QObject* parent) : QObject(parent) {
    Appearance& appearance = Appearance::instance();
    tokens_ = Tokens::map(appearance.theme());
    connect(&appearance, &Appearance::themeChanged, this, [this](Theme theme) {
        tokens_ = Tokens::map(theme);
        emit themeChanged();
    });
    connect(&appearance, &Appearance::zoomChanged, this, &QuickTheme::zoomChanged);
}

QuickTheme* QuickTheme::create(QQmlEngine*, QJSEngine*) { return new QuickTheme; }

bool QuickTheme::dark() const { return Appearance::instance().dark(); }

qreal QuickTheme::zoom() const { return Appearance::instance().zoom(); }

// Without an installed design-system family, fall back to the platform fonts, as the
// Widgets layer does, rather than letting QML pick an unrelated default.
QString QuickTheme::fontFamily() const {
    const QString family = Tokens::availableFamily(u"font-sans");
    return family.isEmpty() ? QGuiApplication::font().family() : family;
}

QString QuickTheme::monoFontFamily() const {
    const QString family = Profile::current().monoFamily();
    return family.isEmpty() ? QFontDatabase::systemFont(QFontDatabase::FixedFont).family() : family;
}

QString QuickTheme::source() const { return Tokens::source(); }

QStringList QuickTheme::iconCodes() const { return Icons::codes(); }

QVariantMap QuickTheme::buttonRecipe(bool dark, qreal zoom, int variant, int size, bool hovered,
                                     bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::buttonRecipe(
        ket::ButtonVariant(variant), ket::ControlSize(size), state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"iconSize", u"spacing", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::iconButtonRecipe(bool dark, qreal zoom, int size, bool hovered,
                                         bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::iconButtonRecipe(ket::ControlSize(size), state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"iconSize", u"spacing", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::badgeRecipe(bool dark, qreal zoom, int tone) const {
    QVariantMap recipe = toVariantMap(ket::badgeRecipe(ket::BadgeTone(tone), themeOf(dark)));
    scaleLengths(recipe, zoom, {u"height", u"radius", u"paddingInline", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::tagRecipe(bool dark, qreal zoom, const QColor& color, bool overflow,
                                  bool hovered) const {
    QVariantMap recipe = toVariantMap(ket::tagRecipe(color, overflow, hovered, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"maxWidth", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::fieldRecipe(bool dark, qreal zoom, bool hovered, bool focused, bool enabled,
                                    bool invalid) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::fieldRecipe(state, invalid, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"iconSize", u"spacing", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::toggleRecipe(bool dark, qreal zoom, int kind, bool checked, bool hovered,
                                     bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe =
        toVariantMap(ket::toggleRecipe(ket::ToggleKind(kind), checked, state, themeOf(dark)));
    scaleLengths(recipe, zoom, {u"size", u"spacing", u"fontPixels"});
    // The radius follows the scaled size so a radio stays round.
    recipe.insert(QStringLiteral("radius"),
                  kind == int(ket::ToggleKind::Radio)
                      ? recipe.value(QStringLiteral("size")).toReal() / 2
                      : Appearance::scale(recipe.value(QStringLiteral("radius")).toReal(), zoom));
    return recipe;
}

QVariantMap QuickTheme::choiceRecipe(bool dark, qreal zoom, bool checked, bool hovered,
                                     bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::choiceRecipe(checked, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"padding", u"radius", u"spacing", u"titlePixels", u"descriptionPixels"});
    return recipe;
}

QVariantMap QuickTheme::fieldRowRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::fieldRowRecipe(themeOf(dark)));
    scaleLengths(
        recipe, zoom,
        {u"labelWidth", u"gap", u"labelTop", u"labelPixels", u"helpPixels", u"helpGap", u"rowGap"});
    return recipe;
}

QVariantMap QuickTheme::bannerRecipe(bool dark, qreal zoom, int tone) const {
    QVariantMap recipe = toVariantMap(ket::bannerRecipe(ket::BannerTone(tone), themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingInline", u"paddingBlock", u"spacing", u"iconSize",
                  u"titlePixels", u"messagePixels", u"messageGap"});
    return recipe;
}

QVariantMap QuickTheme::toastRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::toastRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingInline", u"paddingBlock", u"spacing", u"iconSize",
                  u"fontPixels", u"bottomOffset"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::dialogRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::dialogRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"width", u"radius", u"paddingInline", u"headerPaddingBlock", u"bodyPadding",
                  u"footerPaddingBlock", u"footerGap", u"titlePixels", u"margin"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::menuRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::menuRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"minWidth", u"maxWidth", u"padding", u"radius", u"separatorMargin",
                  u"sectionHeight", u"sectionPixels"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::menuItemRecipe(bool dark, qreal zoom, bool highlighted, bool enabled,
                                       int tone) const {
    QVariantMap recipe = toVariantMap(
        ket::menuItemRecipe(highlighted, enabled, ket::MenuItemTone(tone), themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingStart", u"paddingEnd", u"spacing", u"checkSize",
                  u"iconSize", u"fontPixels", u"descriptionPixels", u"descriptionGap"});
    return recipe;
}

QVariantMap QuickTheme::tooltipRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::tooltipRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingInline", u"paddingBlock", u"maxWidth", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::navItemRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                      bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::navItemRecipe(selected, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"spacing", u"iconSize", u"fontPixels",
                  u"countHeight", u"countPadding", u"countRadius", u"countFontPixels",
                  u"kbdFontPixels"});
    return recipe;
}

QVariantMap QuickTheme::kbdRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::kbdRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::segmentedRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::segmentedRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"padding", u"radius", u"spacing"});
    return recipe;
}

QVariantMap QuickTheme::segmentRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                      bool enabled, bool focused) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::segmentRecipe(selected, state, themeOf(dark)));
    scaleLengths(recipe, zoom, {u"radius", u"paddingInline", u"paddingBlock", u"fontPixels"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::tabBarRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::tabBarRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"height"});
    return recipe;
}

QVariantMap QuickTheme::tabRecipe(bool dark, qreal zoom, bool selected, bool hovered, bool enabled,
                                  bool focused) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::tabRecipe(selected, state, themeOf(dark)));
    scaleLengths(
        recipe, zoom,
        {u"height", u"paddingInline", u"spacing", u"iconSize", u"indicatorHeight", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::progressRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::progressRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"height"});
    recipe.insert(QStringLiteral("radius"), recipe.value(QStringLiteral("height")).toReal() / 2);
    return recipe;
}

QVariantMap QuickTheme::activityRecipe(bool dark, qreal zoom, const QColor& color) const {
    // A transparent colour (the QML default) keeps the recipe's success colour.
    QVariantMap recipe =
        toVariantMap(ket::activityRecipe(themeOf(dark), color.alpha() > 0 ? color : QColor()));
    scaleLengths(recipe, zoom, {u"barWidth", u"barHeight", u"minBarHeight", u"spacing"});
    recipe.insert(QStringLiteral("radius"), recipe.value(QStringLiteral("barWidth")).toReal() / 2);
    return recipe;
}

QVariantList QuickTheme::activityBarFrame(const QVariantMap& map, int bar, qreal elapsedMs) const {
    ActivityRecipe recipe;
    recipe.barHeight = map.value(QStringLiteral("barHeight")).toReal();
    recipe.minBarHeight = map.value(QStringLiteral("minBarHeight")).toReal();
    recipe.minOpacity = map.value(QStringLiteral("minOpacity")).toReal();
    recipe.durationMs = map.value(QStringLiteral("durationMs")).toInt();
    recipe.staggerMs = map.value(QStringLiteral("staggerMs")).toInt();
    const auto [height, opacity] = ket::activityBarFrame(recipe, bar, qint64(elapsedMs));
    return {height, opacity};
}

QVariantMap QuickTheme::emptyStateRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::emptyStateRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"iconSize", u"spacing", u"titlePixels", u"messagePixels", u"messageMaxWidth"});
    return recipe;
}

QVariantMap QuickTheme::conversationRowRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                              bool enabled, bool focused) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::conversationRowRecipe(selected, state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"paddingBlock", u"radius", u"spacing", u"lineGap",
                  u"titlePixels", u"titleLineHeight", u"numberPixels", u"metaPixels", u"metaHeight",
                  u"metaGap", u"tagGap", u"tagMaxWidth", u"indicatorSize", u"indicatorTop"});
    return recipe;
}

QVariantMap QuickTheme::conversationStatusRecipe(bool dark, int status) const {
    return toVariantMap(
        ket::conversationStatusRecipe(ket::ConversationStatus(status), themeOf(dark)));
}

QString QuickTheme::conversationStatusLabel(int status) const {
    return ket::conversationStatusLabel(ket::ConversationStatus(status));
}

// The values match ket::ComposerAction: 0 sends, 1 stops.
QVariantMap QuickTheme::composerActionRecipe(bool dark, qreal zoom, bool busy, bool hasText,
                                             bool hovered, bool pressed, bool focused) const {
    const ket::ComposerAction action = ket::composerAction(busy, hasText);
    const bool enabled = action == ket::ComposerAction::Stop || hasText;
    QVariantMap recipe = toVariantMap(ket::composerActionRecipe(
        action, enabled, {hovered, pressed, enabled, focused}, themeOf(dark)));
    recipe.insert(QStringLiteral("stop"), action == ket::ComposerAction::Stop);
    recipe.insert(QStringLiteral("enabled"), enabled);
    scaleLengths(recipe, zoom, {u"size", u"iconSize", u"stopGlyph", u"stopRadius"});
    return recipe;
}

QVariantMap QuickTheme::composerRecipe(bool dark, qreal zoom, bool focused) const {
    QVariantMap recipe = toVariantMap(ket::composerRecipe(focused, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"padding", u"textPixels", u"textPaddingInline", u"textPaddingBlock",
                  u"minInputHeight", u"sendSize", u"attachIconSize", u"toolbarHeight",
                  u"toolbarGap"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::userMessageRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::userMessageRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"radius", u"paddingInline", u"paddingBlock", u"fontPixels", u"spacingAfter"});
    return recipe;
}

QVariantMap QuickTheme::assistantMessageRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::assistantMessageRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"avatarSize", u"avatarRadius", u"avatarIconSize", u"spacing", u"namePixels",
                  u"timePixels", u"metaGap", u"metaBottom", u"bodyPixels", u"spacingAfter"});
    return recipe;
}

QVariantMap QuickTheme::dayDividerRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::dayDividerRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"fontPixels", u"spacing", u"marginTop", u"marginBottom"});
    return recipe;
}

QVariantMap QuickTheme::activityRowRecipe(bool dark, qreal zoom, bool hovered, bool enabled,
                                          bool focused) const {
    const InteractionState state{hovered, false, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::activityRowRecipe(state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"spacing", u"radius", u"fontPixels",
                  u"detailPixels", u"iconSize", u"chevronSize"});
    return recipe;
}

void QuickTheme::applyLineHeight(QQuickTextDocument* document, qreal pixels) const {
    if (!document || !document->textDocument())
        return;
    QTextBlockFormat format;
    format.setLineHeight(pixels, QTextBlockFormat::FixedHeight);
    QTextCursor cursor(document->textDocument());
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}

QVariantMap QuickTheme::markdownRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::markdownRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"bodyPixels", u"heading1Pixels", u"heading2Pixels", u"heading3Pixels",
                  u"headingSpacingAfter", u"paragraphSpacing", u"listIndent", u"listItemGap",
                  u"quotePadding", u"quoteBorderWidth", u"codePixels"});
    return recipe;
}

QVariantMap QuickTheme::codeBlockRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::codeBlockRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom, {u"padding", u"radius", u"margin", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::mentionPickerRecipe(bool dark, qreal zoom) const {
    QVariantMap recipe = toVariantMap(ket::mentionPickerRecipe(themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"maxWidth", u"padding", u"radius", u"overlap", u"titlePixels",
                  u"titlePaddingInline", u"titlePaddingTop", u"titlePaddingBottom",
                  u"emptyPixels"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::mentionOptionRecipe(bool dark, qreal zoom, bool highlighted) const {
    QVariantMap recipe = toVariantMap(ket::mentionOptionRecipe(highlighted, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"paddingInline", u"radius", u"spacing", u"fontPixels",
                  u"numberPixels", u"placePixels"});
    return recipe;
}

QVariantMap QuickTheme::usageRingRecipe(bool dark, qreal zoom, qreal used, bool hovered) const {
    QVariantMap recipe = toVariantMap(ket::usageRingRecipe(used, hovered, themeOf(dark)));
    scaleLengths(recipe, zoom, {u"size", u"stroke"});
    return recipe;
}

QVariantList QuickTheme::markdownSegments(const QString& markdown) const {
    QVariantList segments;
    for (const MarkdownSegment& segment : splitMarkdown(markdown))
        segments.append(
            QVariantMap{{QStringLiteral("kind"),
                         segment.kind == MarkdownSegment::Kind::Code    ? QStringLiteral("code")
                         : segment.kind == MarkdownSegment::Kind::Quote ? QStringLiteral("quote")
                                                                        : QStringLiteral("text")},
                        {QStringLiteral("text"), segment.text},
                        {QStringLiteral("language"), segment.language}});
    return segments;
}

void QuickTheme::styleMarkdown(QQuickTextDocument* document, bool dark, qreal zoom) const {
    if (!document)
        return;
    QFont base = QGuiApplication::font();
    base.setFamilies({fontFamily()});
    ket::styleMarkdown(document->textDocument(), ket::markdownRecipe(themeOf(dark)), zoom, base,
                       monoFontFamily());
}

QVariant QuickTheme::agentMention(const QString& text, int cursor) const {
    const std::optional<AgentMention> mention = ket::agentMention(text, cursor);
    if (!mention)
        return {};
    return QVariantMap{{QStringLiteral("start"), int(mention->start)},
                       {QStringLiteral("end"), int(mention->end)},
                       {QStringLiteral("query"), mention->query}};
}

QString QuickTheme::agentMentionText(const QString& number) const {
    return ket::agentMentionText(number);
}

QString QuickTheme::iconSource(const QString& code, qreal box, int weight) const {
    if (code.isEmpty())
        return {};
    return QLatin1String("qrc") + Icons::resourcePath(code, box, ket::IconWeight(weight));
}

qreal QuickTheme::iconDrawSize(qreal box) const { return Icons::drawSize(box); }

qreal QuickTheme::iconOffset(qreal box, qreal devicePixelRatio) const {
    return Icons::offset(box, devicePixelRatio);
}

QString QuickTheme::iconForLucide(const QString& lucide) const {
    return Icons::codeForLucide(lucide);
}

qreal QuickTheme::px(qreal logicalPixels, qreal zoom) const {
    return Appearance::scale(logicalPixels, zoom);
}

} // namespace ket
