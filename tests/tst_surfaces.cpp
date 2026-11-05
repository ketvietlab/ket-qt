// SPDX-License-Identifier: MPL-2.0
// Surfaces: the recipes and the Qt Widgets command palette, file rows, changed files, diff
// view, background tasks, update cards, theme cards and accent swatch.
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetChangedFiles.h"
#include "KetCommandPalette.h"
#include "KetDiffView.h"
#include "KetFileRow.h"
#include "KetTaskBar.h"
#include "KetThemeCard.h"
#include "KetUpdateCard.h"

#include <QApplication>
#include <QButtonGroup>
#include <QLineEdit>
#include <QProgressBar>
#include <QSignalSpy>
#include <QTest>

using namespace ket;

namespace {

QList<CommandGroup> sampleGroups() {
    return {{QStringLiteral("Files"),
             {{QStringLiteral("rail"), QStringLiteral("NavigationRail.qml"), {}, {}},
              {QStringLiteral("readme"), QStringLiteral("README.md"), {}, {}}}},
            {QStringLiteral("Commands"),
             {{QStringLiteral("theme"), QStringLiteral("Toggle dark theme"), {}, {}}}}};
}

} // namespace

class SurfacesTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void recipesFollowTheKetPlusMock() {
        const Theme light = Theme::Light;
        QCOMPARE(commandPaletteRecipe(light).width, 640.0);
        QCOMPARE(commandPaletteRecipe(light).top, 120.0);
        QCOMPARE(commandResultRecipe(true, light).background,
                 Tokens::color(u"accent-subtle", light));
        QCOMPARE(commandResultRecipe(false, light).background.alpha(), 0);
        QCOMPARE(fileRowRecipe(false, {}, light).height, 29.0);
        QCOMPARE(fileRowRecipe(false, {}, light).folderHeight, 28.0);
        QCOMPARE(fileRowRecipe(true, {}, light).background, Tokens::color(u"accent-subtle", light));
        QCOMPARE(diffRecipe(light).lineHeight, 25.0);
        QCOMPARE(taskRowRecipe(TaskStatus::Failed, false, light).status,
                 Tokens::color(u"danger", light));
        QCOMPARE(taskRowRecipe(TaskStatus::Completed, false, light).status,
                 Tokens::color(u"success", light));
        QCOMPARE(taskRowRecipe(TaskStatus::Running, true, light).border,
                 Tokens::color(u"border-strong", light));
        QCOMPARE(taskRowRecipe(TaskStatus::Running, false, light).minHeight, 34.0);
        QCOMPARE(updateCardRecipe(true, light).border, Tokens::color(u"success-border", light));
        QCOMPARE(updateCardRecipe(false, light).border, Tokens::color(u"panel-border", light));
        QCOMPARE(updateCardRecipe(false, light).headerColumnGap, 10.0);
        // A theme card draws its miniature in the previewed theme, its frame in the current one.
        const ThemeCardRecipe dark = themeCardRecipe(false, Theme::Dark, {}, light);
        QCOMPARE(dark.miniBackground, Tokens::color(u"panel-bg", Theme::Dark));
        QCOMPARE(dark.background, Tokens::color(u"panel-bg", light));
        QCOMPARE(themeCardRecipe(true, light, {}, light).ring, Tokens::color(u"accent", light));
        QCOMPARE(themeCardRecipe(false, light, {}, light).ring.alpha(), 0);
        QCOMPARE(accentSwatchRecipe(light).borderWidth, 3.0);
    }

    void commandPaletteNavigatesAndActivates() {
        QWidget window;
        window.resize(900, 700);
        QLineEdit before(&window);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        before.setFocus();
        CommandPalette palette(&before);
        palette.setGroups(sampleGroups());
        QSignalSpy activated(&palette, &CommandPalette::activated);
        QSignalSpy closed(&palette, &CommandPalette::closed);
        palette.open();
        QVERIFY(palette.isVisible());
        QCOMPARE(palette.geometry(), window.rect());
        QCOMPARE(palette.panel()->width(), 640);
        QCOMPARE(palette.panel()->y(), 120);
        QTRY_VERIFY(palette.searchField()->hasFocus());
        QCOMPARE(palette.highlightedIndex(), 0);
        QTest::keyClick(palette.searchField(), Qt::Key_Up); // wraps to the last item
        QCOMPARE(palette.highlightedIndex(), 2);
        QTest::keyClick(palette.searchField(), Qt::Key_Down);
        QTest::keyClick(palette.searchField(), Qt::Key_Down);
        QCOMPARE(palette.highlightedIndex(), 1);
        QTest::keyClick(palette.searchField(), Qt::Key_Return);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.at(0).at(0).value<CommandItem>().id, QStringLiteral("readme"));
        QVERIFY(!palette.isVisible());
        QCOMPARE(closed.count(), 1);
        QTRY_VERIFY(before.hasFocus()); // focus returns to where it was
    }

    void commandPaletteFiltersThroughTheApp() {
        QWidget window;
        window.resize(900, 360); // too short for the whole list
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        CommandPalette palette(&window);
        palette.setGroups(sampleGroups());
        palette.open();
        QVERIFY(palette.panel()->geometry().bottom() <= window.height() - 25);
        QSignalSpy query(&palette, &CommandPalette::queryChanged);
        QTest::keyClicks(palette.searchField(), QStringLiteral("zz"));
        QCOMPARE(query.count(), 2);
        QCOMPARE(palette.query(), QStringLiteral("zz"));
        palette.setGroups({});
        QSignalSpy activated(&palette, &CommandPalette::activated);
        QTest::keyClick(palette.searchField(), Qt::Key_Return); // nothing to activate
        QCOMPARE(activated.count(), 0);
        QTest::keyClick(palette.searchField(), Qt::Key_Escape);
        QVERIFY(!palette.isVisible());
    }

    void fileRowsIndentAndFold() {
        FileRow root(QStringLiteral("README.md"));
        FileRow nested(QStringLiteral("Composer.qml"), FileRow::Kind::File, 2);
        FileRow folder(QStringLiteral("src"), FileRow::Kind::Folder);
        QCOMPARE(root.indent(), 12.0);
        QCOMPARE(nested.indent(), fileRowRecipe(false, {}, Theme::Light).indentStart +
                                      2 * fileRowRecipe(false, {}, Theme::Light).indentStep);
        QVERIFY(!root.isCheckable());
        QVERIFY(folder.isCheckable());
        QCOMPARE(root.sizeHint().height(), 29);
        QCOMPARE(folder.sizeHint().height(), 28);
        Appearance::instance().setZoom(1.25);
        QCOMPARE(root.sizeHint().height(), 36);
        root.setStatus(QStringLiteral("M"));
        QCOMPARE(root.accessibleDescription(), QStringLiteral("M"));
    }

    void changedFilesTotalAndActivate() {
        ChangedFiles files;
        QCOMPARE(files.title(), QStringLiteral("0 files changed"));
        files.addFile(QStringLiteral("NavigationRail.qml"), QStringLiteral("src"), 2, 2);
        QCOMPARE(files.title(), QStringLiteral("1 file changed"));
        ChangedFileRow* readme = files.addFile(QStringLiteral("README.md"), {}, 3, 1);
        QCOMPARE(files.title(), QStringLiteral("2 files changed"));
        files.setTitle(QStringLiteral("Changes"));
        QCOMPARE(files.title(), QStringLiteral("Changes"));
        QSignalSpy activated(&files, &ChangedFiles::fileActivated);
        files.resize(500, files.sizeHint().height());
        files.show();
        QVERIFY(QTest::qWaitForWindowExposed(&files));
        QTest::mouseClick(readme, Qt::LeftButton);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.at(0).at(0).toInt(), 1);
        QVERIFY(readme->accessibleDescription().contains(QStringLiteral("3 added")));
    }

    void diffParsesUnifiedHunks() {
        const QList<DiffLine> lines = DiffView::parseUnified(QStringLiteral(
            "diff --git a/x b/x\n--- a/x\n+++ b/x\n@@ -10,3 +10,4 @@ fn\n keep\n-old\n+new\n+more\n"
            "\\ No newline at end of file\n"));
        QCOMPARE(lines.size(), 5);
        QCOMPARE(lines.at(0).kind, DiffLine::Kind::Hunk);
        QCOMPARE(lines.at(1).kind, DiffLine::Kind::Context);
        QCOMPARE(lines.at(1).number, 10);
        QCOMPARE(lines.at(2).kind, DiffLine::Kind::Remove);
        QCOMPARE(lines.at(2).number, 11); // the old file's line
        QCOMPARE(lines.at(3).number, 11); // the new file's line
        QCOMPARE(lines.at(4).text, QStringLiteral("more"));
        DiffView view;
        view.setLines(lines);
        const DiffRecipe r = diffRecipe(Theme::Light);
        // A hunk, then a padded run of four lines.
        QVERIFY(view.sizeHint().height() > qRound(4 * r.lineHeight + 2 * r.paddingBlock));
    }

    void taskBarStopsDismissesAndCollapses() {
        TaskBar bar;
        TaskRow* server = bar.addTask(QStringLiteral("npm run dev"));
        TaskRow* tests = bar.addTask(QStringLiteral("ctest"));
        QCOMPARE(bar.toggle()->text(), QStringLiteral("2 background tasks"));
        bar.resize(560, bar.sizeHint().height());
        bar.show();
        QVERIFY(QTest::qWaitForWindowExposed(&bar));
        QVERIFY(server->stopButton()->isVisible());
        QVERIFY(!server->dismissButton()->isVisible());
        QSignalSpy stop(server, &TaskRow::stopRequested);
        QTest::mouseClick(server->stopButton(), Qt::LeftButton);
        QCOMPARE(stop.count(), 1);
        server->setStatus(TaskStatus::Stopping);
        QVERIFY(!server->stopButton()->isEnabled());
        tests->setStatus(TaskStatus::Failed);
        QVERIFY(tests->dismissButton()->isVisible());
        QTest::mouseClick(tests->dismissButton(), Qt::LeftButton);
        QCOMPARE(bar.tasks().size(), 1);
        QCOMPARE(bar.toggle()->text(), QStringLiteral("1 background task"));
        QSignalSpy toggled(&bar, &TaskBar::toggled);
        bar.toggle()->click();
        QVERIFY(!bar.isExpanded());
        QVERIFY(!server->isVisible());
        QCOMPARE(toggled.count(), 1);
    }

    void updateCardShowsSectionsOnDemand() {
        UpdateCard card(QStringLiteral("KetPlus 1.8.0"));
        card.resize(520, card.sizeHint().height());
        card.show();
        QVERIFY(QTest::qWaitForWindowExposed(&card));
        const int bare = card.sizeHint().height();
        QVERIFY(!card.progressBar()->isVisible());
        card.setProgress(44);
        QTRY_VERIFY(card.progressBar()->isVisible());
        QCOMPARE(card.progressBar()->value(), 44);
        card.setProgress(-1);
        QTRY_VERIFY(!card.progressBar()->isVisible());
        QCOMPARE(card.sizeHint().height(), bare);
        auto* restart = new Button(QStringLiteral("Restart now"), ButtonVariant::Primary);
        card.addAction(restart);
        QTRY_VERIFY(restart->isVisible());
        QVERIFY(card.sizeHint().height() > bare);
        QCOMPARE(restart->width(), restart->sizeHint().width()); // actions keep their width
        card.setReady(true);
        QVERIFY(card.isReady());
    }

    void updatePillScalesWithZoom() {
        UpdatePill pill(QStringLiteral("Update available"), QStringLiteral("KetPlus 1.8.0"));
        const int height = pill.sizeHint().height();
        Appearance::instance().setZoom(1.25);
        QVERIFY(pill.sizeHint().height() > height);
        QCOMPARE(pill.accessibleDescription(), QStringLiteral("KetPlus 1.8.0"));
    }

    void themeCardsAreExclusive() {
        QWidget host;
        ThemeCard light(QStringLiteral("Light"), Theme::Light, &host);
        ThemeCard dark(QStringLiteral("Dark"), Theme::Dark, &host);
        QButtonGroup group;
        group.addButton(&light);
        group.addButton(&dark);
        light.setChecked(true);
        dark.click();
        QVERIFY(dark.isChecked());
        QVERIFY(!light.isChecked());
        QCOMPARE(dark.recipe().miniSidebar, Tokens::color(u"sidebar-bg", Theme::Dark));
        QCOMPARE(dark.recipe().border, Tokens::color(u"accent", Theme::Light));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(light.recipe().miniSidebar, Tokens::color(u"sidebar-bg", Theme::Light));
        AccentSwatch swatch;
        QCOMPARE(swatch.sizeHint(), QSize(31, 31)); // 29 + the outer ring
    }
};

QTEST_MAIN(SurfacesTest)
#include "tst_surfaces.moc"
