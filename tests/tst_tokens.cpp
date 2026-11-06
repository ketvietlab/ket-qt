// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetConversationRecipes.h"
#include "KetMarkdown.h"
#include "KetProfile.h"
#include "KetRecipes.h"
#include "KetSyntax.h"
#include "KetTokens.h"

#include <QFontDatabase>
#include <QGuiApplication>
#include <QTest>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextTable>

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

    void installedFamiliesMatchWithoutAccents() {
        // The KÉT Sans files register as "KET Sans", so names fold case and accents.
        QString installed;
        for (const QString& family : QFontDatabase::families())
            if (family.contains(u'e')) {
                installed = family;
                break;
            }
        if (installed.isEmpty())
            QSKIP("No installed family has an 'e'.");
        QString spelled = installed.toUpper();
        spelled.replace(u'E', QChar(0x00C9)); // É
        QCOMPARE(Tokens::installedFamily({QStringLiteral("No Such Family"), spelled}), installed);
        QVERIFY(Tokens::installedFamily({QStringLiteral("No Such Family")}).isEmpty());
        QCOMPARE(Profile::ketPlus().monoFamilies.value(0), QStringLiteral("KÉT Sans Mono"));
        QCOMPARE(Profile::designSystem().monoFamilies, Tokens::fontFamilies(u"font-mono"));
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

    void fieldRecipesFollowTheKetPlusMock() {
        const FieldRecipe rest = fieldRecipe({}, false, Theme::Light);
        QCOMPARE(rest.background, Tokens::color(u"input-bg", Theme::Light));
        QCOMPARE(rest.border, Tokens::color(u"input-border", Theme::Light));
        QCOMPARE(rest.placeholder, Tokens::color(u"input-placeholder", Theme::Light));
        QCOMPARE(rest.height, 34.0);
        QCOMPARE(rest.radius, Tokens::px(u"radius-sm"));
        QCOMPARE(rest.borderWidth, 1.0);
        const FieldRecipe focused = fieldRecipe({false, false, true, true}, false, Theme::Dark);
        QCOMPARE(focused.border, Tokens::color(u"focus-border", Theme::Dark));
        QCOMPARE(focused.borderWidth, 2.0);
        QCOMPARE(fieldRecipe({}, true, Theme::Light).border,
                 Tokens::color(u"danger", Theme::Light));
        QCOMPARE(fieldRecipe({false, false, false, false}, false, Theme::Light).opacity, 0.48);
    }

    void toggleRecipesMarkTheCheckedState() {
        const ToggleRecipe off = toggleRecipe(ToggleKind::Checkbox, false, {}, Theme::Light);
        QCOMPARE(off.mark.alpha(), 0);
        QCOMPARE(off.border, Tokens::color(u"border-strong", Theme::Light));
        QCOMPARE(off.size, 16.0);
        const ToggleRecipe on = toggleRecipe(ToggleKind::Checkbox, true, {}, Theme::Light);
        QCOMPARE(on.background, Tokens::color(u"accent", Theme::Light));
        QCOMPARE(on.mark, Tokens::color(u"action-primary-ink", Theme::Light));
        const ToggleRecipe radio = toggleRecipe(ToggleKind::Radio, true, {}, Theme::Dark);
        QCOMPARE(radio.radius, 8.0);
        QCOMPARE(radio.background, Tokens::color(u"input-bg", Theme::Dark));
        QCOMPARE(radio.mark, Tokens::color(u"accent", Theme::Dark));
    }

    void choiceAndFieldRowRecipes() {
        const ChoiceRecipe off = choiceRecipe(false, {}, Theme::Light);
        QCOMPARE(off.border, Tokens::color(u"panel-border", Theme::Light));
        QCOMPARE(off.background.alpha(), 0);
        const ChoiceRecipe on = choiceRecipe(true, {}, Theme::Light);
        QCOMPARE(on.background, Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(on.border, Tokens::color(u"accent-border", Theme::Light));
        QCOMPARE(on.titleWeight, 550);
        const FieldRowRecipe row = fieldRowRecipe(Theme::Light);
        QCOMPARE(row.labelWidth, 180.0);
        QCOMPARE(row.label, Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(row.error, Tokens::color(u"danger", Theme::Light));
    }

    void recipesFollowTheCurrentProfile() {
        QCOMPARE(Profile::current().name, QStringLiteral("ketplus"));
        const ButtonRecipe ketPlus =
            buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, {}, Theme::Light);
        QCOMPARE(ketPlus.radius, Tokens::px(u"radius-sm"));

        Profile::setCurrent(Profile::designSystem());
        const ButtonRecipe base =
            buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, {}, Theme::Light);
        QCOMPARE(base.radius, Tokens::px(u"control-radius"));
        QCOMPARE(base.fontPixels, Tokens::px(u"action-font-size"));
        QCOMPARE(base.fontWeight, 500);
        QCOMPARE(buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium,
                              {false, false, false, false}, Theme::Light)
                     .opacity,
                 Tokens::number(u"opacity-pending"));
        QCOMPARE(fieldRecipe({}, false, Theme::Light).height, Tokens::px(u"control-height-md"));
        // Colours never come from a profile.
        QCOMPARE(base.background, ketPlus.background);

        // A product profile starts from a shipped one and changes what its design needs.
        Profile glass = Profile::designSystem();
        glass.name = QStringLiteral("lampion-glass");
        glass.controlRadius = Tokens::px(u"radius-lg");
        Profile::setCurrent(glass);
        QCOMPARE(fieldRecipe({}, false, Theme::Dark).radius, Tokens::px(u"radius-lg"));

        Profile::setCurrent(Profile::ketPlus());
        QCOMPARE(
            buttonRecipe(ButtonVariant::Secondary, ControlSize::Medium, {}, Theme::Light).radius,
            ketPlus.radius);
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

    void shadowTokensParse() {
        const Shadow shadow = shadowToken(u"shadow-md", Theme::Light);
        QCOMPARE(shadow.x, 0.0);
        QCOMPARE(shadow.y, 8.0);
        QCOMPARE(shadow.blur, 20.0);
        QVERIFY(shadow.color.isValid() && shadow.color.alpha() > 0);
        QVERIFY(!shadowToken(u"no-such-shadow", Theme::Light).color.isValid());
    }

    void overlayRecipesFollowTheKetPlusMock() {
        const BannerRecipe danger = bannerRecipe(BannerTone::Danger, Theme::Light);
        QCOMPARE(danger.background, Tokens::color(u"danger-bg", Theme::Light));
        QCOMPARE(danger.icon, QStringLiteral("warning"));
        QCOMPARE(danger.radius, 6.0);
        QCOMPARE(danger.titleWeight, 550);
        QCOMPARE(bannerRecipe(BannerTone::Info, Theme::Dark).text,
                 Tokens::color(u"accent-text", Theme::Dark));

        const ToastRecipe toast = toastRecipe(Theme::Light);
        QCOMPARE(toast.border, Tokens::color(u"positive-border", Theme::Light));
        QCOMPARE(toast.durationMs, 3600);

        const DialogRecipe dialog = dialogRecipe(Theme::Dark);
        QCOMPARE(dialog.width, 500.0);
        QCOMPARE(dialog.radius, 12.0);
        QCOMPARE(dialog.backdrop.alpha(), qRound(0.24 * 255));
        QCOMPARE(dialog.footer, Tokens::color(u"surface-raised", Theme::Dark));

        const MenuRecipe menu = menuRecipe(Theme::Light);
        QCOMPARE(menu.minWidth, 210.0);
        QCOMPARE(menu.border, Tokens::color(u"border-strong", Theme::Light));
        const MenuItemRecipe idle = menuItemRecipe(false, true, MenuItemTone::Normal, Theme::Light);
        QCOMPARE(idle.background.alpha(), 0);
        QCOMPARE(idle.height, 32.0);
        const MenuItemRecipe hot = menuItemRecipe(true, true, MenuItemTone::Normal, Theme::Light);
        QCOMPARE(hot.background, Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(hot.text, Tokens::color(u"accent-text", Theme::Light));
        QCOMPARE(menuItemRecipe(true, true, MenuItemTone::Danger, Theme::Light).text,
                 Tokens::color(u"danger", Theme::Light));
        const MenuItemRecipe elevated =
            menuItemRecipe(false, true, MenuItemTone::Warning, Theme::Light);
        QCOMPARE(elevated.text, Tokens::color(u"warning", Theme::Light));
        QCOMPARE(elevated.description, elevated.text);
        QCOMPARE(menu.sectionText, Tokens::color(u"text-muted", Theme::Light));
        QCOMPARE(menu.sectionHeight, 24.0);
        const MenuItemRecipe off = menuItemRecipe(true, false, MenuItemTone::Normal, Theme::Light);
        QCOMPARE(off.background.alpha(), 0);
        QCOMPARE(off.opacity, 0.65);

        const TooltipRecipe tip = tooltipRecipe(Theme::Light);
        QCOMPARE(tip.background, Tokens::color(u"accent", Theme::Light));
        QCOMPARE(tip.maxWidth, 360.0);
    }

    void navigationRecipesFollowTheKetPlusMock() {
        const NavItemRecipe idle = navItemRecipe(false, {}, Theme::Light);
        QCOMPARE(idle.height, 34.0);
        QCOMPARE(idle.background.alpha(), 0);
        QCOMPARE(idle.text, Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(navItemRecipe(false, {true, false, true, false}, Theme::Light).background,
                 Tokens::color(u"surface-hover", Theme::Light));
        const NavItemRecipe active = navItemRecipe(true, {true, false, true, false}, Theme::Dark);
        QCOMPARE(active.background, Tokens::color(u"accent-subtle", Theme::Dark));
        QCOMPARE(active.text, Tokens::color(u"nav-selected-text", Theme::Dark));
        QCOMPARE(active.fontWeight, 550);

        QCOMPARE(segmentedRecipe(Theme::Light).radius, 6.0);
        const SegmentRecipe on = segmentRecipe(true, {}, Theme::Light);
        QCOMPARE(on.background, Tokens::color(u"panel-bg", Theme::Light));
        QVERIFY(on.shadow.color.isValid());
        QCOMPARE(segmentRecipe(false, {}, Theme::Light).text,
                 Tokens::color(u"text-muted", Theme::Light));

        QCOMPARE(tabBarRecipe(Theme::Light).height, 41.0);
        const TabRecipe tab = tabRecipe(true, {}, Theme::Light);
        QCOMPARE(tab.indicator, Tokens::color(u"accent", Theme::Light));
        QCOMPARE(tab.indicatorHeight, 2.0);
        QCOMPARE(tabRecipe(false, {}, Theme::Light).indicator.alpha(), 0);

        QCOMPARE(progressRecipe(Theme::Light).height, 6.0);
        QCOMPARE(emptyStateRecipe(Theme::Light).iconSize, 35.0);
        QCOMPARE(emptyStateRecipe(Theme::Light).icon, Tokens::color(u"accent-text", Theme::Light));
    }

    void activityBarsFollowTheWorkingKeyframes() {
        const ActivityRecipe r = activityRecipe(Theme::Light);
        QCOMPARE(r.color, Tokens::color(u"success", Theme::Light));
        QCOMPARE(activityRecipe(Theme::Light, QColor(Qt::red)).color, QColor(Qt::red));
        QCOMPARE(activityBarFrame(r, 0, 0).first, 10.0);
        const auto middle = activityBarFrame(r, 0, r.durationMs / 2);
        QCOMPARE(middle.first, 4.0);
        QCOMPARE(middle.second, 0.5);
        // The second bar waits for its delay, then runs the same cycle.
        QCOMPARE(activityBarFrame(r, 1, r.durationMs / 2 - 1).first,
                 activityBarFrame(r, 0, r.durationMs / 2 - 1 - r.staggerMs).first);
        QCOMPARE(activityBarFrame(r, 2, 100).first, 10.0);
    }

    void conversationRecipesFollowTheKetPlusMock() {
        const ConversationRowRecipe row = conversationRowRecipe(false, {}, Theme::Light);
        QCOMPARE(row.height, 48.0);
        QCOMPARE(row.background.alpha(), 0);
        QCOMPARE(row.titleWeight, 400);
        const ConversationRowRecipe selected = conversationRowRecipe(true, {}, Theme::Light);
        QCOMPARE(selected.background, Tokens::color(u"surface-hover", Theme::Light));
        QCOMPARE(selected.titleWeight, 500);
        QCOMPARE(row.metaWeight, 400);
        QCOMPARE(row.tagMaxWidth, 84.0);

        const ConversationStatusRecipe working =
            conversationStatusRecipe(ConversationStatus::Working, Theme::Light);
        QVERIFY(working.spinning);
        QCOMPARE(working.iconColor, Tokens::color(u"text-main", Theme::Light));
        QCOMPARE(conversationStatusRecipe(ConversationStatus::NeedsInput, Theme::Light).labelColor,
                 Tokens::color(u"warning", Theme::Light));
        QCOMPARE(conversationStatusLabel(ConversationStatus::Error),
                 QStringLiteral("Reply failed"));
        // A finished turn that waits for the user is neither the tick nor the blocked pause.
        const ConversationStatusRecipe awaiting =
            conversationStatusRecipe(ConversationStatus::AwaitingReply, Theme::Dark);
        QCOMPARE(awaiting.icon, QStringLiteral("help"));
        QCOMPARE(awaiting.iconColor, Tokens::color(u"accent-text", Theme::Dark));
        QCOMPARE(awaiting.labelColor, awaiting.iconColor);
        QVERIFY(!awaiting.spinning);
        QVERIFY(awaiting.icon !=
                conversationStatusRecipe(ConversationStatus::Completed, Theme::Dark).icon);
        QVERIFY(awaiting.icon !=
                conversationStatusRecipe(ConversationStatus::NeedsInput, Theme::Dark).icon);
        QCOMPARE(conversationStatusLabel(ConversationStatus::AwaitingReply),
                 QStringLiteral("Waiting for you"));

        QCOMPARE(composerRecipe(false, Theme::Light).border,
                 Tokens::color(u"border-default", Theme::Light));
        const ComposerRecipe focused = composerRecipe(true, Theme::Light);
        QCOMPARE(focused.border, Tokens::color(u"accent-border", Theme::Light));
        QCOMPARE(focused.ring, Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(focused.radius, 12.0);

        // The user's bubble is tinted; the action circle is a circle.
        QCOMPARE(userMessageRecipe(Theme::Light).background,
                 Tokens::color(u"accent-subtle", Theme::Light));
        const ComposerActionRecipe send =
            composerActionRecipe(ComposerAction::Send, true, {}, Theme::Light);
        QCOMPARE(send.background, Tokens::color(u"accent", Theme::Light));
        QCOMPARE(send.size, 32.0);
        QCOMPARE(composerRecipe(false, Theme::Light).attachIconSize, 20.0);
        QCOMPARE(composerAction(true, false), ComposerAction::Stop);
        QCOMPARE(composerAction(true, true), ComposerAction::Send);
        QCOMPARE(composerActionRecipe(ComposerAction::Send, false, {}, Theme::Light).icon,
                 Tokens::color(u"text-disabled", Theme::Light));
        QCOMPARE(userMessageRecipe(Theme::Light).fontPixels, 15.0);
        QCOMPARE(assistantMessageRecipe(Theme::Light).avatarSize, 25.0);
        QCOMPARE(dayDividerRecipe(Theme::Light).marginBottom, 22.0);
        QCOMPARE(activityRowRecipe({true, false, true, false}, Theme::Light).background,
                 Tokens::color(u"surface-hover", Theme::Light));
    }

    void markdownSplitsProseAndFences() {
        const QList<MarkdownSegment> segments =
            splitMarkdown(u"Intro **bold**\n\n```cpp\nint x = 1;\n```\nAfter\n~~~\nunclosed");
        const QList<MarkdownSegment> quoted = splitMarkdown(u"Lead\n> One\n>two\n\nTail");
        QCOMPARE(quoted.size(), 3);
        QCOMPARE(quoted.at(1).kind, MarkdownSegment::Kind::Quote);
        QCOMPARE(quoted.at(1).text, QStringLiteral("One\ntwo"));
        QCOMPARE(quoted.at(2).text, QStringLiteral("Tail"));
        QCOMPARE(segments.size(), 4);
        QCOMPARE(segments.at(0).kind, MarkdownSegment::Kind::Text);
        QCOMPARE(segments.at(1).kind, MarkdownSegment::Kind::Code);
        QCOMPARE(segments.at(1).language, QStringLiteral("cpp"));
        QCOMPARE(segments.at(1).text, QStringLiteral("int x = 1;"));
        QCOMPARE(segments.at(2).text, QStringLiteral("After"));
        QCOMPARE(segments.at(3).text, QStringLiteral("unclosed"));
        // A shorter or different marker does not close a fence.
        const QList<MarkdownSegment> nested = splitMarkdown(u"````\n```\nstill code\n````");
        QCOMPARE(nested.size(), 1);
        QCOMPARE(nested.at(0).text, QStringLiteral("```\nstill code"));
    }

    void markdownTakesTheAnswerStyle() {
        QTextDocument document;
        document.setMarkdown(
            QStringLiteral("### Plan\n\nBody with `code` and **bold**.\n\n- one\n- two"));
        styleMarkdown(&document, markdownRecipe(Theme::Light), 1.0, QFont(),
                      QStringLiteral("Menlo"));
        const QTextBlock heading = document.begin();
        QCOMPARE(heading.begin().fragment().charFormat().intProperty(QTextFormat::FontPixelSize),
                 20);
        QCOMPARE(heading.blockFormat().bottomMargin(), 7.0);
        const QTextBlock body = heading.next();
        QCOMPARE(body.blockFormat().lineHeight(), qreal(qFloor(15 * 1.75)));
        QCOMPARE(body.blockFormat().bottomMargin(), 16.0);
        bool mono = false;
        bool strong = false;
        for (auto it = body.begin(); !it.atEnd(); ++it) {
            const QTextCharFormat chars = it.fragment().charFormat();
            mono |= chars.fontFamilies().toStringList().contains(QStringLiteral("Menlo"));
            strong |= chars.fontWeight() == 550;
        }
        QVERIFY(mono);
        QVERIFY(strong);
        const QTextBlock first = body.next();
        QVERIFY(first.textList());
        QCOMPARE(first.blockFormat().bottomMargin(), 5.0); // the list gap, not a paragraph
        QCOMPARE(document.indentWidth(), 17.0);

        QTextDocument quoted;
        quoted.setMarkdown(QStringLiteral("Before\n\n> Quoted"));
        styleMarkdown(&quoted, markdownRecipe(Theme::Light), 1.0, QFont(), QString());
        const QTextBlockFormat quote = quoted.lastBlock().blockFormat();
        QCOMPARE(quote.background().color(), Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(quote.leftMargin(), 14.0);

        // h1 and h2 tighten; h3 keeps normal spacing.
        QTextDocument headings;
        headings.setMarkdown(QStringLiteral("# One\n\n## Two\n\n### Three"));
        styleMarkdown(&headings, markdownRecipe(Theme::Light), 1.0, QFont(), QString());
        const auto spacing = [](const QTextBlock& block) {
            return block.begin().fragment().charFormat().fontLetterSpacing();
        };
        QCOMPARE(spacing(headings.begin()), 24 * -0.02);
        QCOMPARE(spacing(headings.begin().next()), 21 * -0.02);
        QCOMPARE(spacing(headings.lastBlock()), 0.0);

        // Tables draw one quiet hairline instead of Qt's default grey frame.
        QTextDocument tabled;
        tabled.setMarkdown(QStringLiteral("| A | B |\n| --- | --- |\n| 1 | 2 |"));
        styleMarkdown(&tabled, markdownRecipe(Theme::Dark), 1.0, QFont(), QString());
        const QList<QTextFrame*> frames = tabled.rootFrame()->childFrames();
        QCOMPARE(frames.size(), 1);
        const QTextTableFormat table = qobject_cast<QTextTable*>(frames.first())->format();
        QCOMPARE(table.borderBrush().color(), Tokens::color(u"panel-border", Theme::Dark));
        QVERIFY(table.borderCollapse());
        QCOMPARE(table.border(), 1.0);
        QCOMPARE(table.cellPadding(), 8.0);
    }

    void syntaxColoursFollowTheLanguage() {
        const auto roles = [](const QList<SyntaxSpan>& spans) {
            QList<std::tuple<qsizetype, qsizetype, SyntaxRole>> out;
            for (const SyntaxSpan& span : spans)
                out.append({span.start, span.length, span.role});
            return out;
        };
        using R = SyntaxRole;
        int state = 0;
        // Keywords, strings and a trailing comment; the number inside the comment stays grey.
        QCOMPARE(roles(highlightLine(u"const id = \"ok\"; // note 42", u"js", state)),
                 (QList<std::tuple<qsizetype, qsizetype, R>>{
                     {0, 5, R::Keyword}, {11, 4, R::String}, {17, 10, R::Comment}}));
        // QML keys and types; numbers.
        QCOMPARE(
            roles(highlightLine(u"    width: 42", u"qml", state)),
            (QList<std::tuple<qsizetype, qsizetype, R>>{{4, 5, R::Attribute}, {11, 2, R::Number}}));
        QCOMPARE(roles(highlightLine(u"Item {", u"qml", state)),
                 (QList<std::tuple<qsizetype, qsizetype, R>>{{0, 4, R::Type}}));
        // A JSON key is not a string value.
        QCOMPARE(
            roles(highlightLine(u"{\"a\": \"b\"}", u"json", state)),
            (QList<std::tuple<qsizetype, qsizetype, R>>{{1, 3, R::Attribute}, {6, 3, R::String}}));
        // Shell flags and variables.
        QCOMPARE(
            roles(highlightLine(u"cmake --build $DIR", u"sh", state)),
            (QList<std::tuple<qsizetype, qsizetype, R>>{{6, 7, R::Attribute}, {14, 4, R::Type}}));
        // Unknown or plain fences stay uncoloured.
        QVERIFY(highlightLine(u"const x = 1", u"text", state).isEmpty());
        QVERIFY(highlightLine(u"const x = 1", u"", state).isEmpty());
        // A block comment carries over lines.
        const QList<SyntaxSpan> block = highlightCode(u"a /* one\ntwo */ return", u"cpp");
        QCOMPARE(roles(block), (QList<std::tuple<qsizetype, qsizetype, R>>{
                                   {2, 6, R::Comment}, {9, 6, R::Comment}, {16, 6, R::Keyword}}));
        // Rich text per line: escaped, spaces kept, colours from the recipe tokens.
        const SyntaxRecipe recipe = syntaxRecipe(Theme::Dark);
        QCOMPARE(recipe.keyword, Tokens::color(u"accent-text", Theme::Dark));
        QCOMPARE(recipe.string, Tokens::color(u"chart-2", Theme::Dark));
        const QStringList lines = highlightedLines(u"if (a < b)\n  x", u"cpp", recipe);
        QCOMPARE(lines.size(), 2);
        QVERIFY(lines.at(0).startsWith(QStringLiteral("<font color='%1'>if</font>")
                                           .arg(recipe.keyword.name(QColor::HexArgb))));
        QVERIFY(lines.at(0).contains(QStringLiteral("&lt;")));
        QCOMPARE(lines.at(1), QStringLiteral("&nbsp;&nbsp;x"));
        // The document highlighter colours a TextEdit's blocks the same way.
        QTextDocument document(QStringLiteral("return 1;"));
        auto* highlighter = new SyntaxHighlighter(&document);
        highlighter->setRecipe(recipe);
        highlighter->setLanguage(QStringLiteral("cpp"));
        const QList<QTextLayout::FormatRange> formats = document.begin().layout()->formats();
        QCOMPARE(formats.size(), 2);
        QCOMPARE(formats.at(0).format.foreground().color(), recipe.keyword);
        QCOMPARE(formats.at(1).format.foreground().color(), recipe.number);
    }

    void agentMentionsFollowTheMock() {
        const QString text = QStringLiteral("Ask @agent/48 about it");
        const auto mention = agentMention(text, 13);
        QVERIFY(mention);
        QCOMPARE(mention->start, 4);
        QCOMPARE(mention->end, 13);
        QCOMPARE(mention->query, QStringLiteral("48"));
        QVERIFY(agentMention(u"Ask @agent/", 11));
        QCOMPARE(agentMention(u"Ask @agent/", 11)->query, QString());
        QVERIFY(!agentMention(u"Ask @file.cpp", 13)); // a plain @ mentions a file
        QVERIFY(!agentMention(u"mail@agent/1", 12));  // not after a space or bracket
        QVERIFY(agentMention(u"(@agent/1", 9));
        QCOMPARE(agentMention(u"@agent/AB", 9)->query, QStringLiteral("ab"));
        QCOMPARE(agentMentionText(u"4821"), QStringLiteral("@agent/4821 "));
    }

    void richRecipesFollowTheKetPlusMock() {
        QCOMPARE(codeBlockRecipe(Theme::Light).background,
                 Tokens::color(u"panel-bg-subtle", Theme::Light));
        QCOMPARE(codeBlockRecipe(Theme::Light).padding, 17.0);
        QCOMPARE(mentionPickerRecipe(Theme::Light).maxWidth, 520.0);
        QCOMPARE(mentionPickerRecipe(Theme::Light).border,
                 Tokens::color(u"border-strong", Theme::Light));
        QCOMPARE(mentionOptionRecipe(true, Theme::Light).title,
                 Tokens::color(u"accent-text", Theme::Light));
        QCOMPARE(mentionOptionRecipe(false, Theme::Light).height, 34.0);
        QCOMPARE(usageRingRecipe(0.3, false, Theme::Light).arc,
                 Tokens::color(u"text-muted", Theme::Light));
        QCOMPARE(usageRingRecipe(0.3, true, Theme::Light).arc,
                 Tokens::color(u"accent", Theme::Light));
        QCOMPARE(usageRingRecipe(0.85, false, Theme::Light).arc,
                 Tokens::color(u"warning", Theme::Light));
        QCOMPARE(usageRingRecipe(0.97, false, Theme::Light).arc,
                 Tokens::color(u"danger", Theme::Light));
        QCOMPARE(usageRingRecipe(0, false, Theme::Light).size, 19.0);
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
