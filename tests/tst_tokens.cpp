// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetRecipes.h"
#include "KetTokens.h"

#include <QGuiApplication>
#include <QTest>

using namespace ket;

class TokensTest final : public QObject {
    Q_OBJECT
  private slots:
    void resolvesThemeBranches() {
        // light-dark() picks a branch per theme; var() chains resolve to the final value.
        QCOMPARE(Tokens::color(u"page-bg", Theme::Light), QColor(QStringLiteral("#f7f5f5")));
        QCOMPARE(Tokens::color(u"page-bg", Theme::Dark), QColor(QStringLiteral("#1b1f24")));
        QCOMPARE(Tokens::color(u"action-primary-bg", Theme::Light),
                 QColor(QStringLiteral("#365fd6")));
    }

    void convertsTranslucentColours() {
        const QColor border = Tokens::color(u"border-default", Theme::Dark);
        QCOMPARE(border.rgb() & 0xffffff, 0xffffffu);
        QVERIFY(border.alpha() > 0 && border.alpha() < 255);
    }

    void convertsLengthsAndDurations() {
        QCOMPARE(Tokens::px(u"control-height-md"), 32.0); // 2rem
        QCOMPARE(Tokens::px(u"control-height-sm"), 28.0);
        QCOMPARE(Tokens::px(u"text-base"), 13.0);
        QCOMPARE(Tokens::ms(u"duration-fast"), 100);
        QVERIFY(qAbs(Tokens::number(u"leading-normal") - 20.0 / 13.0) < 1e-4); // calc(20 / 13)
    }

    void everyColourParses() {
        for (const QString& name : Tokens::names()) {
            if (Tokens::kind(name) != TokenKind::Color)
                continue;
            QVERIFY2(Tokens::color(name, Theme::Light).isValid(), qPrintable(name));
            QVERIFY2(Tokens::color(name, Theme::Dark).isValid(), qPrintable(name));
        }
    }

    void unknownNamesAreEmpty() {
        QVERIFY(!Tokens::contains(u"no-such-token"));
        QVERIFY(!Tokens::color(u"no-such-token", Theme::Light).isValid());
        QCOMPARE(Tokens::px(u"page-bg"), 0.0); // a colour is not a length
    }

    void fontStacksDropGenericFamilies() {
        const QStringList families = Tokens::fontFamilies(u"font-sans");
        QCOMPARE(families.value(0), QStringLiteral("KÉT Sans"));
        QVERIFY(!families.contains(QStringLiteral("sans-serif")));
    }

    void mapsUseCamelCase() {
        const QVariantMap map = Tokens::map(Theme::Dark);
        QCOMPARE(map.value(QStringLiteral("pageBg")).value<QColor>(),
                 QColor(QStringLiteral("#1b1f24")));
        QCOMPARE(map.value(QStringLiteral("controlHeightMd")).toReal(), 32.0);
        QCOMPARE(Tokens::camelCase(u"space-0-5"), QStringLiteral("space05"));
        QVERIFY(Tokens::source().startsWith(QStringLiteral("@ketvietlab/design-system ")));
    }

    void buttonRecipesFollowState() {
        const InteractionState rest;
        const InteractionState hover{true, false, true, false};
        const InteractionState disabled{false, false, false, false};
        const ButtonRecipe primary =
            buttonRecipe(ButtonVariant::Primary, ControlSize::Medium, rest, Theme::Light);
        QCOMPARE(primary.background, Tokens::color(u"action-primary-bg", Theme::Light));
        QCOMPARE(primary.text, Tokens::color(u"action-primary-ink", Theme::Light));
        QCOMPARE(primary.height, 32.0);
        QCOMPARE(buttonRecipe(ButtonVariant::Primary, ControlSize::Medium, hover, Theme::Light)
                     .background,
                 Tokens::color(u"action-primary-hover", Theme::Light));
        QCOMPARE(buttonRecipe(ButtonVariant::Ghost, ControlSize::Small, rest, Theme::Light)
                     .background.alpha(),
                 0);
        QCOMPARE(buttonRecipe(ButtonVariant::Ghost, ControlSize::Small, rest, Theme::Light).height,
                 28.0);
        QCOMPARE(buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, disabled, Theme::Light)
                     .opacity,
                 0.48);
    }

    // The KetPlus mock's `.btn` rules: 5 px radius, strong border on hover, ghost hover on
    // surface-hover, and danger as a secondary button with danger text and border.
    void buttonRecipesFollowTheKetPlusMock() {
        const InteractionState rest;
        const InteractionState hover{true, false, true, false};
        const ButtonRecipe secondary =
            buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, rest, Theme::Light);
        QCOMPARE(secondary.radius, Tokens::px(u"radius-sm"));
        QCOMPARE(secondary.border, Tokens::color(u"border-default", Theme::Light));
        QCOMPARE(secondary.fontWeight, 400);
        QCOMPARE(secondary.spacing, 7.0);
        QCOMPARE(secondary.iconSize, 17.0);
        QCOMPARE(
            buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, hover, Theme::Light).border,
            Tokens::color(u"border-strong", Theme::Light));
        QCOMPARE(
            buttonRecipe(ButtonVariant::Ghost, ControlSize::Medium, hover, Theme::Dark).background,
            Tokens::color(u"surface-hover", Theme::Dark));
        const ButtonRecipe danger =
            buttonRecipe(ButtonVariant::Danger, ControlSize::Medium, hover, Theme::Light);
        QCOMPARE(danger.background, Tokens::color(u"interactive-hover", Theme::Light));
        QCOMPARE(danger.border, Tokens::color(u"danger-border", Theme::Light));
        QCOMPARE(danger.text, Tokens::color(u"danger", Theme::Light));
        const ButtonRecipe ghostDanger =
            buttonRecipe(ButtonVariant::GhostDanger, ControlSize::Medium, rest, Theme::Light);
        QCOMPARE(ghostDanger.background.alpha(), 0);
        QCOMPARE(ghostDanger.text, Tokens::color(u"danger", Theme::Light));
    }

    void iconButtonRecipesAreSquareAndMuted() {
        const InteractionState rest;
        const InteractionState pressed{true, true, true, false};
        const ButtonRecipe medium = iconButtonRecipe(ControlSize::Medium, rest, Theme::Light);
        QCOMPARE(medium.height, 30.0);
        QCOMPARE(medium.background.alpha(), 0);
        QCOMPARE(medium.border.alpha(), 0);
        QCOMPARE(medium.text, Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(iconButtonRecipe(ControlSize::Medium, pressed, Theme::Light).background,
                 Tokens::color(u"interactive-active", Theme::Light));
        const ButtonRecipe small = iconButtonRecipe(ControlSize::Small, rest, Theme::Light);
        QCOMPARE(small.height, Tokens::px(u"control-height-xs"));
        QCOMPARE(small.iconSize, 14.0);
    }

    void badgeRecipesUseToneTokens() {
        const BadgeRecipe danger = badgeRecipe(BadgeTone::Danger, Theme::Dark);
        QCOMPARE(danger.text, Tokens::color(u"badge-danger-text", Theme::Dark));
        QCOMPARE(danger.background, Tokens::color(u"danger-bg", Theme::Dark));
        QCOMPARE(danger.height, Tokens::px(u"line-sm") + 2);
        const BadgeRecipe neutral = badgeRecipe(BadgeTone::Neutral, Theme::Light);
        QCOMPARE(neutral.background, Tokens::color(u"panel-bg", Theme::Light));
        QCOMPARE(neutral.border, Tokens::color(u"panel-border", Theme::Light));
        QCOMPARE(neutral.text, Tokens::color(u"text-secondary", Theme::Light));
    }

    void tagRecipesTintTheAppColour() {
        const QColor blue(QStringLiteral("#2f6fde"));
        const TagRecipe rest = tagRecipe(blue, false, false, Theme::Light);
        QCOMPARE(rest.background.rgb(), blue.rgb());
        QCOMPARE(rest.background.alpha(), qRound(0.13 * 255));
        QCOMPARE(tagRecipe(blue, false, true, Theme::Light).background.alpha(), qRound(0.22 * 255));
        QCOMPARE(rest.text, Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(rest.height, 18.0);
        QCOMPARE(rest.fontPixels, Tokens::px(u"text-2xs"));
        const TagRecipe more = tagRecipe(blue, true, false, Theme::Dark);
        QCOMPARE(more.background, Tokens::color(u"surface-hover", Theme::Dark));
        QCOMPARE(more.text, Tokens::color(u"text-muted", Theme::Dark));
    }

    void appearanceZoomRoundsToWholePixels() {
        Appearance& appearance = Appearance::instance();
        appearance.setZoom(1.25);
        QCOMPARE(appearance.scaled(32), 40.0);
        QCOMPARE(appearance.scaled(13), 16.0); // 16.25 rounds down
        QCOMPARE(appearance.scaled(1), 1.0);   // hairlines stay hairlines
        appearance.setZoom(1.0);
    }

    void appearanceModePinsTheme() {
        Appearance& appearance = Appearance::instance();
        appearance.setMode(Appearance::Mode::Dark);
        QCOMPARE(appearance.theme(), Theme::Dark);
        appearance.setMode(Appearance::Mode::Light);
        QCOMPARE(appearance.theme(), Theme::Light);
    }
};

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    TokensTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_tokens.moc"
