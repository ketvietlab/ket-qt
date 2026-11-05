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

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QSignalSpy>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QTest>

using namespace ket;

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
