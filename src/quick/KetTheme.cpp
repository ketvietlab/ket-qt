// SPDX-License-Identifier: MPL-2.0
#include "KetTheme.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetRecipes.h"

#include <QFontDatabase>
#include <QGuiApplication>

namespace ket {
namespace {

Theme themeOf(bool dark) { return dark ? Theme::Dark : Theme::Light; }

void scaleLengths(QVariantMap& recipe, qreal zoom, std::initializer_list<QStringView> keys) {
    for (QStringView key : keys) {
        const QString name = key.toString();
        recipe.insert(name, Appearance::scale(recipe.value(name).toReal(), zoom));
    }
}

static_assert(int(QuickTheme::ButtonVariant::GhostDanger) == int(ButtonVariant::GhostDanger));
static_assert(int(QuickTheme::ControlSize::Medium) == int(ControlSize::Medium));
static_assert(int(QuickTheme::BadgeTone::Danger) == int(BadgeTone::Danger));
static_assert(int(QuickTheme::IconWeight::Regular) == int(ket::IconWeight::Regular));
static_assert(int(QuickTheme::ToggleKind::Radio) == int(ket::ToggleKind::Radio));
static_assert(int(QuickTheme::BannerTone::Danger) == int(ket::BannerTone::Danger));

constexpr std::initializer_list<QStringView> kShadowLengths = {u"shadowX", u"shadowY",
                                                               u"shadowBlur"};

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
    const QString family = Tokens::availableFamily(u"font-mono");
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
                 {u"minWidth", u"maxWidth", u"padding", u"radius", u"separatorMargin"});
    scaleLengths(recipe, zoom, kShadowLengths);
    return recipe;
}

QVariantMap QuickTheme::menuItemRecipe(bool dark, qreal zoom, bool highlighted, bool enabled,
                                       bool danger) const {
    QVariantMap recipe =
        toVariantMap(ket::menuItemRecipe(highlighted, enabled, danger, themeOf(dark)));
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
