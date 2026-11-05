// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"
#include "KetBadge.h"
#include "KetButton.h"
#include "KetStyle.h"

#include <QApplication>
#include <QTest>

using namespace ket;

class WidgetsTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void styleIsFusionBasedOnEveryPlatform() {
        auto* style = qobject_cast<Style*>(QApplication::style());
        QVERIFY(style);
        QCOMPARE(style->baseStyle()->name().toLower(), QStringLiteral("fusion"));
    }

    void paletteFollowsAppearance() {
        QCOMPARE(QApplication::palette().color(QPalette::Window),
                 Tokens::color(u"page-bg", Theme::Light));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(QApplication::palette().color(QPalette::Window),
                 Tokens::color(u"page-bg", Theme::Dark));
        QCOMPARE(QApplication::palette().color(QPalette::Text),
                 Tokens::color(u"input-text", Theme::Dark));
    }

    void buttonHeightsComeFromTokens() {
        Button button(QStringLiteral("Save"), ButtonVariant::Primary);
        QCOMPARE(button.sizeHint().height(), 32);
        button.setControlSize(ControlSize::Small);
        QCOMPARE(button.sizeHint().height(), 28);
        Appearance::instance().setZoom(1.25);
        QCOMPARE(button.sizeHint().height(), 35); // 28 * 1.25 = 35
    }

    void buttonRecipeTracksInteraction() {
        Button button(QStringLiteral("Delete"), ButtonVariant::Danger);
        QCOMPARE(button.recipe().text, Tokens::color(u"danger", Theme::Light));
        button.setEnabled(false);
        QCOMPARE(button.recipe().opacity, Tokens::number(u"opacity-pending"));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(button.recipe().border, Tokens::color(u"danger-border", Theme::Dark));
    }

    void buttonRendersWithoutWarnings() {
        Button button(QStringLiteral("Continue"), ButtonVariant::Primary);
        button.resize(button.sizeHint());
        const QImage image = button.grab().toImage();
        QVERIFY(!image.isNull());
        // Below the label, the middle of a primary button is its fill colour.
        const QColor centre = image.pixelColor(image.width() / 2, image.height() - 4);
        QCOMPARE(centre.rgb(), Tokens::color(u"action-primary-bg", Theme::Light).rgb());
    }

    void badgeSizesToText() {
        Badge narrow(QStringLiteral("UI"), BadgeTone::Accent);
        Badge wide(QStringLiteral("Performance"), BadgeTone::Accent);
        QCOMPARE(narrow.sizeHint().height(), qRound(Tokens::px(u"line-sm")));
        QVERIFY(wide.sizeHint().width() > narrow.sizeHint().width());
        QCOMPARE(narrow.recipe().text, Tokens::color(u"accent-text", Theme::Light));
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    WidgetsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_widgets.moc"
