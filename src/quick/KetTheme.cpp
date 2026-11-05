// SPDX-License-Identifier: MPL-2.0
#include "KetTheme.h"

#include "KetAppearance.h"
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

static_assert(int(QuickTheme::ButtonVariant::Danger) == int(ButtonVariant::Danger));
static_assert(int(QuickTheme::ControlSize::Medium) == int(ControlSize::Medium));
static_assert(int(QuickTheme::BadgeTone::Danger) == int(BadgeTone::Danger));

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

QVariantMap QuickTheme::buttonRecipe(bool dark, qreal zoom, int variant, int size, bool hovered,
                                     bool pressed, bool enabled, bool focused) const {
    const InteractionState state{hovered, pressed, enabled, focused};
    QVariantMap recipe = toVariantMap(ket::buttonRecipe(
        ket::ButtonVariant(variant), ket::ControlSize(size), state, themeOf(dark)));
    scaleLengths(recipe, zoom,
                 {u"height", u"radius", u"paddingInline", u"iconSize", u"spacing", u"fontPixels"});
    return recipe;
}

QVariantMap QuickTheme::badgeRecipe(bool dark, qreal zoom, int tone) const {
    QVariantMap recipe = toVariantMap(ket::badgeRecipe(ket::BadgeTone(tone), themeOf(dark)));
    scaleLengths(recipe, zoom, {u"height", u"radius", u"paddingInline", u"fontPixels"});
    return recipe;
}

qreal QuickTheme::px(qreal logicalPixels, qreal zoom) const {
    return Appearance::scale(logicalPixels, zoom);
}

} // namespace ket
