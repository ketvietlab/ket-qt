// SPDX-License-Identifier: MPL-2.0
// Controls and chrome: the recipes and the Qt Widgets slider, avatar, tree items, group
// headers, route tabs, status bar, breadcrumb and pane header.
#include "KetAppearance.h"
#include "KetAvatar.h"
#include "KetButton.h"
#include "KetChrome.h"
#include "KetRouteTabs.h"
#include "KetStepSlider.h"
#include "KetStyle.h"
#include "KetTreeItem.h"

#include <QAccessible>
#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QSignalSpy>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QTest>

using namespace ket;

namespace {

bool near(const QColor& actual, const QColor& expected, int tolerance = 6) {
    return qAbs(actual.red() - expected.red()) <= tolerance &&
           qAbs(actual.green() - expected.green()) <= tolerance &&
           qAbs(actual.blue() - expected.blue()) <= tolerance;
}

// `over` composited on the opaque `under`, as a translucent token lands on the pill.
QColor over(const QColor& top, const QColor& under) {
    const qreal a = top.alphaF();
    return QColor::fromRgbF(top.redF() * a + under.redF() * (1 - a),
                            top.greenF() * a + under.greenF() * (1 - a),
                            top.blueF() * a + under.blueF() * (1 - a));
}

} // namespace

class ControlsTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void recipesFollowTheKetPlusMock() {
        const InteractionState hovered{true, false, true, false};
        QCOMPARE(sliderRecipe({}, Theme::Light).fill, Tokens::color(u"accent", Theme::Light));
        QCOMPARE(sliderRecipe(hovered, Theme::Light).fill,
                 Tokens::color(u"accent-hover", Theme::Light));
        QCOMPARE(avatarRecipe(Theme::Light).size, 29.0);
        QCOMPARE(treeItemRecipe(true, {}, Theme::Light).background,
                 Tokens::color(u"accent-subtle", Theme::Light));
        QCOMPARE(treeItemRecipe(true, {}, Theme::Light).title,
                 Tokens::color(u"accent-text", Theme::Light));
        QCOMPARE(treeItemRecipe(false, hovered, Theme::Light).background,
                 Tokens::color(u"surface-hover", Theme::Light));
        QCOMPARE(groupHeaderRecipe(false, {}, Theme::Light).text,
                 Tokens::color(u"text-muted", Theme::Light));
        QCOMPARE(groupHeaderRecipe(true, {}, Theme::Light).text,
                 Tokens::color(u"text-secondary", Theme::Light));
        QCOMPARE(routeTabRecipe(true, {}, Theme::Light).indicator,
                 Tokens::color(u"accent", Theme::Light));
        QCOMPARE(routeTabRecipe(false, {}, Theme::Light).indicator.alpha(), 0);
        QCOMPARE(statusBarRecipe(Theme::Light).height, 26.0);
        QCOMPARE(breadcrumbRecipe(Theme::Light).height, 33.0);
        QCOMPARE(paneHeaderRecipe(Theme::Light).height, 40.0);
    }

    void styleDrawsHorizontalSliders() {
        QSlider slider(Qt::Horizontal);
        slider.setRange(0, 100);
        slider.setValue(50);
        slider.resize(216, slider.sizeHint().height());
        QCOMPARE(slider.sizeHint().height(), 20);
        QStyleOptionSlider option;
        option.initFrom(&slider);
        option.orientation = Qt::Horizontal;
        option.minimum = 0;
        option.maximum = 100;
        option.sliderPosition = 50;
        option.rect = slider.rect();
        const QRect handle = slider.style()->subControlRect(QStyle::CC_Slider, &option,
                                                            QStyle::SC_SliderHandle, &slider);
        QCOMPARE(handle.size(), QSize(16, 16));
        QCOMPARE(handle.center().x(), 107); // (216 - 16) / 2 + 8 - 1
        QTest::mouseClick(&slider, Qt::LeftButton, {}, QPoint(200, 10));
        QVERIFY(slider.value() > 50);
        QVERIFY(!slider.grab().isNull());
    }

    void stepSlidersReportTheirStep() {
        StepSlider picker(QStringLiteral("Reasoning effort"));
        picker.setSteps({QStringLiteral("low"), QStringLiteral("medium"), QStringLiteral("high")});
        QSignalSpy changed(&picker, &StepSlider::currentIndexChanged);
        picker.setCurrentIndex(2);
        QCOMPARE(picker.currentStep(), QStringLiteral("high"));
        QCOMPARE(changed.count(), 1);
        QCOMPARE(picker.slider()->accessibleDescription(), QStringLiteral("high"));
        picker.setSteps({QStringLiteral("only")});
        QVERIFY(!picker.slider()->isEnabled());
    }

    void stepNotchesSnapToTheNearestCentre() {
        // Four steps across 200 px with 18 px insets: the ends sit on the insets.
        QCOMPARE(stepNotchCentre(200, 18, 4, 0), 18.0);
        QCOMPARE(stepNotchCentre(200, 18, 4, 3), 182.0);
        QCOMPARE(stepNotchCentre(200, 18, 4, 1), 18.0 + 164.0 / 3);
        QCOMPARE(stepNotchCentre(200, 18, 4, 9), 182.0); // clamped
        QCOMPARE(stepNotchCentre(200, 18, 1, 0), 100.0); // one step sits in the middle
        QCOMPARE(stepNotchNearest(0, 200, 18, 4), 0);
        QCOMPARE(stepNotchNearest(-50, 200, 18, 4), 0);
        QCOMPARE(stepNotchNearest(400, 200, 18, 4), 3);
        const qreal gap = 164.0 / 3;
        QCOMPARE(stepNotchNearest(18 + gap * 1.4, 200, 18, 4), 1);
        QCOMPARE(stepNotchNearest(18 + gap * 1.6, 200, 18, 4), 2);
        QCOMPARE(stepNotchNearest(100, 200, 18, 1), 0);
        QCOMPARE(stepNotchNearest(100, 200, 18, 0), 0);
    }

    void stepSliderRecipesFollowTheNotchedMock() {
        for (const Theme theme : {Theme::Light, Theme::Dark}) {
            const SliderRecipe r = sliderRecipe({}, theme);
            QCOMPARE(r.notchPill, Tokens::color(u"surface-hover", theme));
            QCOMPARE(r.notch, Tokens::color(u"text-muted", theme));
            QCOMPARE(r.track, Tokens::color(u"border-default", theme));
            QCOMPARE(r.handleBorder, Tokens::color(u"accent", theme));
            QCOMPARE(r.notchHeight, 36.0);
            QCOMPARE(r.notchTrackHeight, 14.0);
            QCOMPARE(r.notchInset, 18.0);
            QCOMPARE(r.notchDotSize, 6.0);
            QCOMPARE(r.notchThumbSize, 26.0);
            QCOMPARE(r.notchRingWidth, 3.0);
            // The end dots sit inside the track's round caps, and the thumb inside the pill.
            QVERIFY(r.notchInset - r.notchTrackHeight / 2 > 0);
            QVERIFY(r.notchInset >= r.notchThumbSize / 2);
            QVERIFY(r.notchThumbSize < r.notchHeight);
        }
        const InteractionState focused{false, false, true, true};
        QCOMPARE(sliderRecipe(focused, Theme::Light).focusRing,
                 Tokens::color(u"focus-border", Theme::Light));
        QCOMPARE(sliderRecipe({}, Theme::Light).focusRing.alpha(), 0);
    }

    void stepSlidersDrawTheirNotches_data() {
        QTest::addColumn<bool>("dark");
        QTest::newRow("light") << false;
        QTest::newRow("dark") << true;
    }

    void stepSlidersDrawTheirNotches() {
        QFETCH(bool, dark);
        Appearance::instance().setMode(dark ? Appearance::Mode::Dark : Appearance::Mode::Light);
        const Theme theme = dark ? Theme::Dark : Theme::Light;
        StepSlider picker(QStringLiteral("Reasoning effort"));
        picker.setSteps({QStringLiteral("low"), QStringLiteral("medium"), QStringLiteral("high"),
                         QStringLiteral("xhigh")});
        picker.setCurrentIndex(2);
        picker.resize(300, picker.sizeHint().height());
        picker.show();
        QVERIFY(QTest::qWaitForWindowExposed(&picker));
        QSlider* track = picker.slider();
        QCOMPARE(track->width(), 300);
        QCOMPARE(track->height(), 36);
        const SliderRecipe r = sliderRecipe({}, theme);
        const QImage image = track->grab().toImage();
        const int middle = 18;
        const auto at = [&](int index) { return qRound(stepNotchCentre(300, 18, 4, index)); };
        // The pill, just inside its left end.
        QVERIFY(near(image.pixelColor(4, middle), r.notchPill));
        // The empty track between the chosen step and the last one.
        const int gap = (at(2) + 13 + at(3) - 3) / 2;
        QVERIFY(near(image.pixelColor(gap, middle), over(r.track, r.notchPill)));
        // The filled track between the first two steps.
        QVERIFY(near(image.pixelColor((at(0) + at(1)) / 2, middle), r.fill));
        // An unchosen step's dot, the chosen step's thumb, ring and dot.
        QVERIFY(near(image.pixelColor(at(3), middle), r.notch, 24));
        QVERIFY(near(image.pixelColor(at(0), middle), r.notch, 24));
        QVERIFY(near(image.pixelColor(at(2) + 7, middle), r.handle));
        QVERIFY(near(image.pixelColor(at(2) + 12, middle), r.handleBorder, 24));
        QVERIFY(near(image.pixelColor(at(2), middle), r.fill, 24));
        // The theme switches without reopening the slider.
        Appearance::instance().setMode(dark ? Appearance::Mode::Light : Appearance::Mode::Dark);
        const SliderRecipe other = sliderRecipe({}, dark ? Theme::Light : Theme::Dark);
        QVERIFY(near(track->grab().toImage().pixelColor(4, middle), other.notchPill));
    }

    void stepSlidersSnapAndAnnounceTheirStep() {
        StepSlider picker(QStringLiteral("Reasoning effort"));
        picker.setSteps({QStringLiteral("low"), QStringLiteral("medium"), QStringLiteral("high"),
                         QStringLiteral("xhigh")});
        picker.resize(300, picker.sizeHint().height());
        picker.show();
        QVERIFY(QTest::qWaitForWindowExposed(&picker));
        QSlider* track = picker.slider();
        const int middle = track->height() / 2;
        // A click between two notches picks the nearer one.
        QTest::mouseClick(track, Qt::LeftButton, {},
                          QPoint(qRound(stepNotchCentre(300, 18, 4, 1)) + 20, middle));
        QCOMPARE(picker.currentIndex(), 1);
        QTest::mouseClick(track, Qt::LeftButton, {}, QPoint(298, middle));
        QCOMPARE(picker.currentIndex(), 3);
        // A drag follows the pointer from notch to notch.
        QTest::mousePress(track, Qt::LeftButton, {}, QPoint(2, middle));
        QCOMPARE(picker.currentIndex(), 0);
        QTest::mouseMove(track, QPoint(qRound(stepNotchCentre(300, 18, 4, 2)) - 10, middle));
        QCOMPARE(picker.currentIndex(), 2);
        QTest::mouseRelease(track, Qt::LeftButton, {},
                            QPoint(qRound(stepNotchCentre(300, 18, 4, 2)) - 10, middle));
        QVERIFY(!track->isSliderDown());
        // Left and Right move one step.
        track->setFocus(Qt::TabFocusReason);
        QTest::keyClick(track, Qt::Key_Right);
        QCOMPARE(picker.currentIndex(), 3);
        QTest::keyClick(track, Qt::Key_Left);
        QTest::keyClick(track, Qt::Key_Left);
        QCOMPARE(picker.currentStep(), QStringLiteral("medium"));
        // It reads as a slider named after the label, with the step as its description.
        QAccessibleInterface* accessible = QAccessible::queryAccessibleInterface(track);
        QVERIFY(accessible);
        QCOMPARE(accessible->role(), QAccessible::Slider);
        QCOMPARE(accessible->text(QAccessible::Name), QStringLiteral("Reasoning effort"));
        QCOMPARE(accessible->text(QAccessible::Description), QStringLiteral("medium"));
        QVERIFY(accessible->valueInterface());
        QCOMPARE(accessible->valueInterface()->currentValue().toInt(), 1);
        // The track follows the zoom.
        Appearance::instance().setZoom(1.25);
        QCOMPARE(track->height(), 45);
    }

    void avatarsTakeInitials() {
        QCOMPARE(Avatar::initialsOf(QStringLiteral("Kieu Duy")), QStringLiteral("KD"));
        QCOMPARE(Avatar::initialsOf(QStringLiteral("Jay")), QStringLiteral("JA"));
        QCOMPARE(Avatar::initialsOf(QStringLiteral("a b c")), QStringLiteral("AB"));
        Avatar avatar(QStringLiteral("Kieu Duy"));
        QCOMPARE(avatar.sizeHint(), QSize(29, 29));
        avatar.setDiameter(40);
        QCOMPARE(avatar.sizeHint(), QSize(40, 40));
        Appearance::instance().setZoom(1.25);
        QCOMPARE(avatar.sizeHint(), QSize(50, 50));
    }

    void treeItemsShowActivityOrDetail() {
        TreeItem item(QStringLiteral("develop"), QStringLiteral("branch"));
        const int single = item.sizeHint().height();
        QVERIFY(single > 2 * 9 + 13); // padding 9 around a 13 px title line
        item.setDetail(QStringLiteral("~/dev/ketplus"));
        QVERIFY(item.sizeHint().height() > single);
        item.setActivity(QStringLiteral("Working"), TreeActivity::Working);
        QCOMPARE(item.accessibleDescription(), QStringLiteral("Working"));
        item.setCheckable(true);
        item.click();
        QVERIFY(item.isChecked());
        item.resize(280, item.sizeHint().height());
        QVERIFY(!item.grab().isNull());
    }

    void groupHeadersToggleAndRevealTheirAction() {
        QWidget window;
        window.resize(400, 300);
        auto* header = new GroupHeader(QStringLiteral("ketplus"), &window);
        header->setGeometry(60, 140, 280, 24); // away from the cursor at the origin
        QCursor::setPos(window.mapToGlobal(QPoint(5, 5)));
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        QCOMPARE(header->height(), 24);
        // The window focuses the name first, which shows the action; without focus or
        // hover it hides.
        header->toggle()->clearFocus();
        QTRY_VERIFY(!header->action()->isVisible());
        QSignalSpy toggled(header, &GroupHeader::toggled);
        QSignalSpy action(header, &GroupHeader::actionTriggered);
        header->toggle()->click();
        QVERIFY(!header->isExpanded());
        QCOMPARE(toggled.count(), 1);
        header->toggle()->setFocus();
        QTRY_VERIFY(header->action()->isVisible());
        header->action()->click();
        QCOMPARE(action.count(), 1);
    }

    void activatableGroupHeadersSplitTheNameFromTheFold() {
        QWidget window;
        window.resize(400, 300);
        auto* header = new GroupHeader(QStringLiteral("Checkout"), &window);
        header->setGeometry(60, 140, 280, 24);
        QCursor::setPos(window.mapToGlobal(QPoint(5, 5)));
        QVERIFY(!header->nameButton()->isVisibleTo(header));
        header->setActivatable(true);
        header->setIconCode(QStringLiteral("users"));
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        QVERIFY(header->nameButton()->isVisible());
        // Two stops, and each says what it does.
        QCOMPARE(header->toggle()->accessibleName(), QStringLiteral("Fold Checkout"));
        QCOMPARE(header->nameButton()->accessibleName(), QStringLiteral("Checkout"));
        QVERIFY(header->toggle()->width() < header->nameButton()->x());
        QSignalSpy toggled(header, &GroupHeader::toggled);
        QSignalSpy activated(header, &GroupHeader::activated);
        header->nameButton()->click();
        QCOMPARE(activated.count(), 1);
        QCOMPARE(toggled.count(), 0);
        QVERIFY(header->isExpanded());
        header->toggle()->click();
        QCOMPARE(toggled.count(), 1);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(header->toggle()->accessibleName(), QStringLiteral("Unfold Checkout"));
        // The keyboard reaches both: Tab from the fold lands on the name, Space activates.
        header->toggle()->setFocus();
        QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), header->nameButton());
        QTRY_VERIFY(header->action()->isVisible());
        QTest::keyClick(header->nameButton(), Qt::Key_Space);
        QCOMPARE(activated.count(), 2);
        // Plain again: the name folds.
        header->setActivatable(false);
        QVERIFY(!header->nameButton()->isVisibleTo(header));
        QCOMPARE(header->toggle()->accessibleName(), QStringLiteral("Checkout"));
    }

    void routeTabsSwitchWithClicksAndArrows() {
        RouteTabs tabs;
        for (const char* label : {"General", "Providers", "Updates"})
            tabs.addTab(QString::fromLatin1(label));
        QCOMPARE(tabs.currentIndex(), 0);
        QSignalSpy changed(&tabs, &RouteTabs::currentChanged);
        tabs.tab(2)->click();
        QCOMPARE(tabs.currentIndex(), 2);
        QTest::keyClick(tabs.tab(2), Qt::Key_Left);
        QCOMPARE(tabs.currentIndex(), 1);
        QCOMPARE(changed.count(), 2);
        // padding 10 + line + padding 12 + the 2 px underline
        QVERIFY(tabs.tab(0)->sizeHint().height() > 24);
    }

    void chromeBarsTakeTheirHeights() {
        StatusBar status;
        StatusItem* item =
            status.addItem(QStringLiteral("Local workspace"), QStringLiteral("success"));
        status.addItem(QStringLiteral("UTF-8"), {}, StatusBar::Side::Right);
        QCOMPARE(status.height(), 26);
        QVERIFY(item->sizeHint().width() > 11 + 5);
        Breadcrumb crumb;
        crumb.setPath({QStringLiteral("modules"), QStringLiteral("ai"), QStringLiteral("x.qml")});
        crumb.actions()->addWidget(new Button(QStringLiteral("View diff")));
        QCOMPARE(crumb.height(), 33);
        QCOMPARE(crumb.accessibleName(), QStringLiteral("modules/ai/x.qml"));
        PaneHeader pane(QStringLiteral("ConversationRow.qml"), QStringLiteral("file-code"));
        QCOMPARE(pane.height(), 40);
        for (QWidget* widget : {static_cast<QWidget*>(&status), static_cast<QWidget*>(&crumb),
                                static_cast<QWidget*>(&pane)}) {
            widget->resize(600, widget->height());
            QVERIFY(!widget->grab().isNull());
        }
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    ControlsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_controls.moc"
