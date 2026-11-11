// SPDX-License-Identifier: MPL-2.0
// The sidebar tree in both layers, against the same recipes: a repository header, a team
// branch, a section label and one-line rows share their text and mark edges at every zoom.
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetProfile.h"
#include "KetSidebarTree.h"
#include "KetTokens.h"

#include <QAccessible>
#include <QApplication>
#include <QBoxLayout>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

int qmlWarnings = 0;
QtMessageHandler previousHandler = nullptr;

void countQmlWarnings(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    if (type != QtDebugMsg && type != QtInfoMsg &&
        (message.contains(QLatin1String("qrc:/")) || message.contains(QLatin1String(".qml:"))))
        ++qmlWarnings;
    previousHandler(type, context, message);
}

int px(qreal length, qreal zoom) { return qRound(Appearance::scale(length, zoom)); }

// The tree of the mock: a repository, its conversations, and a team with two members.
struct WidgetTree {
    QWidget root;
    SidebarRepositoryHeader* repository = new SidebarRepositoryHeader(QStringLiteral("ketplus"));
    SidebarTreeGuide* repositoryGuide = new SidebarTreeGuide(0);
    SidebarSectionLabel* section = new SidebarSectionLabel(QStringLiteral("Conversations"));
    SidebarRow* row = new SidebarRow(QStringLiteral("Fix the sidebar"));
    SidebarTeamBranch* team = new SidebarTeamBranch(QStringLiteral("Mobile Refactor"));
    SidebarTreeGuide* members = new SidebarTreeGuide(1);
    SidebarRow* leader = new SidebarRow(QStringLiteral("Lead Mobile Refactor"));
    SidebarRow* developer = new SidebarRow(QStringLiteral("Sidebar tree parts in ket-qt"));

    WidgetTree() {
        auto* column = new QVBoxLayout(&root);
        column->setContentsMargins(0, 0, 0, 0);
        column->setSpacing(0);
        column->addWidget(repository);
        column->addWidget(repositoryGuide);
        column->addStretch(1);
        repository->setConversationCount(3);
        section->setCount(1);
        row->setNumber(QStringLiteral("3651"));
        team->setMemberCount(2);
        team->setDecisionCount(1);
        for (SidebarRow* member : {leader, developer})
            member->setLevel(1);
        leader->setNumber(QStringLiteral("9692"));
        developer->setNumber(QStringLiteral("3651"));
        developer->setStatus(ConversationStatus::Completed);
        developer->setUnread(true);
        repositoryGuide->addPart(section);
        repositoryGuide->addPart(row);
        repositoryGuide->addPart(team);
        repositoryGuide->addPart(members);
        members->addPart(leader);
        members->addPart(developer);
        root.resize(280, 400);
    }

    [[nodiscard]] int left(const QWidget* part, const QRect& box) const {
        return part->mapTo(&root, box.topLeft()).x();
    }
};

} // namespace

class SidebarTreeTest final : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase() { previousHandler = qInstallMessageHandler(countQmlWarnings); }

    void init() {
        Profile::setCurrent(Profile::ketPlus());
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void cleanup() {
        const int warnings = std::exchange(qmlWarnings, 0);
        QCOMPARE(warnings, 0);
        Appearance::instance().setZoom(1.0);
    }

    // The mock's columns (mockups/qt/qml/shell), one level per team.
    void columnsFollowTheMock() {
        const SidebarTreeColumns top = sidebarTreeColumns(0, 1.0);
        QCOMPARE(top.guide, 13.0);
        QCOMPARE(top.chevron, 8.0);
        QCOMPARE(top.highlight, 20.0);
        QCOMPARE(top.mark, 24.0);
        QCOMPARE(top.text, 44.0);
        const SidebarTreeColumns team = sidebarTreeColumns(1, 1.0);
        QCOMPARE(team.guide, 31.0);
        QCOMPARE(team.chevron, 26.0);
        QCOMPARE(team.highlight, 38.0);
        QCOMPARE(team.mark, 42.0);
        QCOMPARE(team.text, 62.0);
        // Lampion and other products start from the design system: its columns keep the
        // same order, so a chevron sits on its guide and rows highlight right of it.
        Profile::setCurrent(Profile::designSystem());
        for (int level : {0, 1, 2}) {
            const SidebarTreeColumns c = sidebarTreeColumns(level, 1.25);
            QVERIFY(c.chevron < c.guide);
            QVERIFY(c.guide < c.highlight);
            QVERIFY(c.highlight < c.mark);
            QVERIFY(c.mark < c.text);
        }
        QVERIFY(Profile::current().sidebarTree.rowHeight > 0);
    }

    void recipesUseTokensInBothThemes() {
        for (Theme theme : {Theme::Light, Theme::Dark}) {
            const InteractionState rest;
            const InteractionState hover{true, false, true, false};
            const InteractionState focus{false, false, true, true};
            QCOMPARE(sidebarTreeGuideRecipe(theme).color, Tokens::color(u"sidebar-border", theme));

            const SidebarRepositoryRecipe repository = sidebarRepositoryRecipe(false, rest, theme);
            QCOMPARE(repository.name, Tokens::color(u"text-secondary", theme));
            QCOMPARE(repository.dot, Tokens::color(u"accent-text", theme));
            QCOMPARE(repository.background.alpha(), 0);
            QCOMPARE(sidebarRepositoryRecipe(false, hover, theme).background,
                     Tokens::color(u"surface-hover", theme));
            QCOMPARE(sidebarRepositoryRecipe(true, rest, theme).name,
                     Tokens::color(u"text-main", theme));
            QCOMPARE(sidebarRepositoryRecipe(false, focus, theme).focusRing,
                     Tokens::color(u"focus-border", theme));

            const SidebarTeamRecipe team = sidebarTeamRecipe(false, false, rest, theme);
            QCOMPARE(team.name, Tokens::color(u"text-secondary", theme));
            QCOMPARE(team.nameWeight, 500);
            const SidebarTeamRecipe waiting = sidebarTeamRecipe(false, true, rest, theme);
            QCOMPARE(waiting.name, Tokens::color(u"text-main", theme));
            QCOMPARE(waiting.nameWeight, 600);
            const ConversationStatusRecipe needsInput =
                conversationStatusRecipe(ConversationStatus::NeedsInput, theme);
            QCOMPARE(waiting.waitingIcon, needsInput.icon);
            QCOMPARE(waiting.waitingColor, needsInput.iconColor);
            const SidebarTeamRecipe selected = sidebarTeamRecipe(true, true, rest, theme);
            QCOMPARE(selected.highlight, Tokens::color(u"nav-selected", theme));
            QCOMPARE(selected.name, Tokens::color(u"accent-text", theme));
            QCOMPARE(selected.icon, Tokens::color(u"accent-text", theme));

            const SidebarSectionRecipe section = sidebarSectionRecipe(theme);
            QCOMPARE(section.icon, QStringLiteral("chats"));
            QCOMPARE(section.text, Tokens::color(u"text-secondary", theme));

            const SidebarRowRecipe row = sidebarRowRecipe(true, false, rest, theme);
            const ConversationRowRecipe twoLine = conversationRowRecipe(true, rest, theme);
            QCOMPARE(row.background, twoLine.background);
            QCOMPARE(row.title, twoLine.title);
            QCOMPARE(row.number, twoLine.number);
            QCOMPARE(row.titleWeight, 400);
            QCOMPARE(sidebarRowRecipe(false, true, rest, theme).titleWeight, 600);
            QCOMPARE(row.unreadDot, Tokens::color(u"accent-text", theme));
        }
        QVERIFY(sidebarTeamRecipe(true, false, {}, Theme::Light).highlight !=
                sidebarTeamRecipe(true, false, {}, Theme::Dark).highlight);
    }

    void wordsCarryNoTags() {
        QCOMPARE(sidebarRepositoryDescription(false, 3, 1),
                 QStringLiteral("Collapsed, 3 conversations, 1 waiting for you"));
        QCOMPARE(sidebarRepositoryDescription(true, 1, 0),
                 QStringLiteral("Expanded, 1 conversation"));
        QCOMPARE(sidebarTeamDescription(4, 1),
                 QStringLiteral("4 members, 1 decision waits for you"));
        QCOMPARE(sidebarTeamDescription(1, 2),
                 QStringLiteral("1 member, 2 decisions wait for you"));
        QCOMPARE(sidebarRowStateText(ConversationStatus::Completed, true, {}, {}),
                 QStringLiteral("Completed, unread"));
        QCOMPARE(sidebarRowStateText(ConversationStatus::NeedsInput, false, {},
                                     QStringLiteral("Allow git push?")),
                 QStringLiteral("Needs input · Allow git push?"));
        QCOMPARE(sidebarRowDescription(QStringLiteral("Developer"), QStringLiteral("Working"),
                                       QStringLiteral("team/3/t-2")),
                 QStringLiteral("Developer, Working, team/3/t-2"));
        QCOMPARE(sidebarRowDescription({}, QStringLiteral("Working"), {}),
                 QStringLiteral("Working"));
    }

    void widgetEdgesHoldAtEveryZoom_data() {
        QTest::addColumn<qreal>("zoom");
        QTest::newRow("100%") << 1.0;
        QTest::newRow("125%") << 1.25;
        QTest::newRow("150%") << 1.5;
    }

    void widgetEdgesHoldAtEveryZoom() {
        QFETCH(qreal, zoom);
        Appearance::instance().setZoom(zoom);
        WidgetTree tree;
        tree.root.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tree.root));
        QCoreApplication::processEvents();

        // One text edge per level, and one mark edge.
        const int topText = tree.left(tree.repository, tree.repository->nameRect());
        QCOMPARE(topText, px(44, zoom));
        QCOMPARE(tree.left(tree.section, tree.section->textRect()), topText);
        QCOMPARE(tree.left(tree.row, tree.row->titleRect()), topText);
        const int topMark = tree.left(tree.repository, tree.repository->markRect());
        QCOMPARE(tree.left(tree.section, tree.section->markRect()), topMark);
        QCOMPARE(tree.left(tree.row, tree.row->markRect()), topMark);

        const int teamText = tree.left(tree.team, tree.team->nameRect());
        QCOMPARE(teamText, px(62, zoom));
        const int teamMark = tree.left(tree.team, tree.team->markRect());
        const int teamHighlight = tree.left(tree.team, tree.team->highlightRect());
        for (SidebarRow* member : {tree.leader, tree.developer}) {
            QCOMPARE(tree.left(member, member->titleRect()), teamText);
            QCOMPARE(tree.left(member, member->markRect()), teamMark);
            QCOMPARE(tree.left(member, member->highlightRect()), teamHighlight);
        }

        // Every row is one line high.
        const int rowHeight = px(30, zoom);
        for (SidebarRow* row : {tree.row, tree.leader, tree.developer}) {
            QCOMPARE(row->height(), rowHeight);
            QCOMPARE(row->titleRect().height(), rowHeight);
            QFont title = row->font();
            title.setPixelSize(qRound(Appearance::scale(row->recipe().titlePixels, zoom)));
            // The title is one elided line in the row's full height.
            QVERIFY(QFontMetrics(title).height() <= rowHeight);
        }
        QCOMPARE(tree.team->height(), rowHeight);
        QCOMPARE(tree.repository->height(), rowHeight);
        QCOMPARE(tree.section->height(), px(28, zoom));

        // Guide lines under each chevron, as long as what they hold; highlights start right
        // of them, and the team's selection right of its chevron.
        const QRect topGuide = tree.repositoryGuide->guideRect();
        QCOMPARE(topGuide.x(), px(13, zoom));
        QCOMPARE(topGuide.width(), 1);
        QVERIFY(tree.left(tree.row, tree.row->highlightRect()) > topGuide.right());
        const QRect memberGuide = tree.members->guideRect();
        QCOMPARE(tree.members->mapTo(&tree.root, memberGuide.topLeft()).x(), px(31, zoom));
        QCOMPARE(memberGuide.height(), tree.leader->height() + tree.developer->height());
        QCOMPARE(tree.members->height(),
                 memberGuide.height() + px(Profile::current().sidebarTree.branchGap, zoom));
        QVERIFY(teamHighlight > tree.team->foldButton()->geometry().right());
        QVERIFY(teamHighlight > tree.members->mapTo(&tree.root, memberGuide.topLeft()).x());
        const QRect fold = tree.team->foldButton()->geometry();
        QVERIFY(qAbs(tree.team->mapTo(&tree.root, fold.center()).x() - px(31, zoom)) <= 1);
        QCOMPARE(topGuide.height(), tree.repositoryGuide->height());

        // Folding the team hides its members and their guide.
        tree.team->setExpanded(false);
        tree.members->setVisible(tree.team->isExpanded());
        QVERIFY(!tree.members->isVisible());
    }

    void widgetKeyboardAndAccessibility() {
        WidgetTree tree;
        tree.team->setMemberCount(4);
        tree.developer->setRole(QStringLiteral("Developer"));
        tree.developer->setWorktree(QStringLiteral("team/3/t-2"));
        tree.root.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tree.root));
        tree.root.activateWindow();

        // The repository header folds with Return and Space; folded with something
        // waiting, it shows the dot and says so.
        tree.repository->setWaitingCount(1);
        tree.repository->setFocus();
        QVERIFY(!tree.repository->isWaitingDotShown());
        QTest::keyClick(tree.repository, Qt::Key_Return);
        QVERIFY(!tree.repository->isExpanded());
        QVERIFY(tree.repository->isWaitingDotShown());
        QCOMPARE(tree.repository->accessibleName(), QStringLiteral("ketplus"));
        QCOMPARE(tree.repository->accessibleDescription(),
                 QStringLiteral("Collapsed, 3 conversations, 1 waiting for you"));
        QTest::keyClick(tree.repository, Qt::Key_Space);
        QVERIFY(tree.repository->isExpanded());
        QVERIFY(tree.repository->action()->isVisible()); // shown while focused
        QCOMPARE(tree.repository->focusPolicy(), Qt::StrongFocus);

        // The team branch: the chevron folds, the name activates; two stops, then its
        // actions, in that order.
        QSignalSpy activated(tree.team, &SidebarTeamBranch::activated);
        QAbstractButton* fold = tree.team->foldButton();
        QAbstractButton* name = tree.team->nameButton();
        QCOMPARE(fold->nextInFocusChain(), name);
        QCOMPARE(name->nextInFocusChain(), static_cast<QWidget*>(tree.team->addAction()));
        QCOMPARE(tree.team->addAction()->nextInFocusChain(),
                 static_cast<QWidget*>(tree.team->menuAction()));
        QCOMPARE(fold->accessibleName(), QStringLiteral("Fold Mobile Refactor"));
        fold->setFocus();
        QTest::keyClick(fold, Qt::Key_Return);
        QVERIFY(!tree.team->isExpanded());
        QCOMPARE(fold->accessibleName(), QStringLiteral("Unfold Mobile Refactor"));
        QTest::keyClick(fold, Qt::Key_Space);
        QVERIFY(tree.team->isExpanded());
        name->setFocus();
        QTest::keyClick(name, Qt::Key_Return);
        QTest::keyClick(name, Qt::Key_Enter);
        QTest::keyClick(name, Qt::Key_Space);
        QCOMPARE(activated.count(), 3);
        QCOMPARE(name->accessibleName(), QStringLiteral("Team Mobile Refactor"));
        QCOMPARE(name->accessibleDescription(),
                 QStringLiteral("4 members, 1 decision waits for you"));

        // A row activates with Return and Space and says its role, state and worktree.
        QSignalSpy clicked(tree.developer, &QAbstractButton::clicked);
        tree.developer->setFocus();
        QTest::keyClick(tree.developer, Qt::Key_Return);
        QVERIFY(tree.developer->isChecked());
        QTest::keyClick(tree.developer, Qt::Key_Space);
        QCOMPARE(clicked.count(), 2);
        QVERIFY(tree.developer->isUnreadDotShown());
        QCOMPARE(tree.developer->recipe().titleWeight, 600);
        QCOMPARE(tree.developer->accessibleName(),
                 QStringLiteral("Sidebar tree parts in ket-qt, #3651"));
        QCOMPARE(tree.developer->accessibleDescription(),
                 QStringLiteral("Developer, Completed, unread, team/3/t-2"));
        tree.developer->setUnread(false);
        tree.developer->setStatus(ConversationStatus::Working);
        QVERIFY(!tree.developer->isUnreadDotShown());
        QCOMPARE(tree.developer->recipe().titleWeight, 400);
        QCOMPARE(tree.developer->accessibleDescription(),
                 QStringLiteral("Developer, Working, team/3/t-2"));

        // The section label is a heading; its action is the stop.
        QCOMPARE(tree.section->accessibleName(), QStringLiteral("Conversations, 1"));
        QCOMPARE(tree.section->displayText(), QStringLiteral("Conversations (1)"));
        QCOMPARE(tree.section->focusPolicy(), Qt::NoFocus);
        QVERIFY(tree.section->action()->focusPolicy() & Qt::TabFocus);

        // Focus rings come from the recipe.
        tree.developer->setFocus();
        QCOMPARE(tree.developer->recipe().focusRing,
                 Tokens::color(u"focus-border", Appearance::instance().theme()));
    }

    void quickEdgesMatchWidgets_data() { widgetEdgesHoldAtEveryZoom_data(); }

    void quickEdgesMatchWidgets() {
        QFETCH(qreal, zoom);
        Appearance::instance().setZoom(zoom);
        std::unique_ptr<QObject> root = createTree();
        QVERIFY(root);
        auto* window = qobject_cast<QQuickWindow*>(root.get());
        QVERIFY(QTest::qWaitForWindowExposed(window));
        QQuickItem* content = window->contentItem();
        const auto left = [&](const char* part, const char* edge) {
            auto* item = root->property(part).value<QQuickItem*>();
            return qRound(item->mapToItem(content, QPointF(item->property(edge).toReal(), 0)).x());
        };
        const int topText = left("repository", "nameLeft");
        QCOMPARE(topText, px(44, zoom));
        QCOMPARE(left("section", "nameLeft"), topText);
        QCOMPARE(left("row", "nameLeft"), topText);
        const int topMark = left("repository", "markLeft");
        QCOMPARE(topMark, px(24, zoom));
        QCOMPARE(left("section", "markLeft"), topMark);
        QCOMPARE(left("row", "markLeft"), topMark);
        const int teamText = left("team", "nameLeft");
        QCOMPARE(teamText, px(62, zoom));
        QCOMPARE(left("leader", "nameLeft"), teamText);
        QCOMPARE(left("developer", "nameLeft"), teamText);
        QCOMPARE(left("leader", "markLeft"), left("team", "markLeft"));
        QCOMPARE(left("leader", "highlightLeft"), left("team", "highlightLeft"));
        QCOMPARE(left("members", "guideLeft"), px(31, zoom));
        QCOMPARE(left("repositoryGuide", "guideLeft"), px(13, zoom));
        QVERIFY(left("row", "highlightLeft") > left("repositoryGuide", "guideLeft"));

        const int rowHeight = px(30, zoom);
        for (const char* part : {"row", "leader", "developer", "team", "repository"})
            QCOMPARE(qRound(root->property(part).value<QQuickItem*>()->height()), rowHeight);
        auto* members = root->property("members").value<QQuickItem*>();
        auto* line = members->findChild<QQuickItem*>(QStringLiteral("sidebarTreeGuideLine"));
        QVERIFY(line);
        QCOMPARE(qRound(line->height()), 2 * rowHeight);
        for (auto* title : root->findChildren<QQuickItem*>(QStringLiteral("sidebarRowTitle")))
            QCOMPARE(title->property("lineCount").toInt(), 1);

        // The Widgets tree draws the same edges.
        WidgetTree tree;
        tree.root.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tree.root));
        QCOMPARE(tree.left(tree.row, tree.row->titleRect()), topText);
        QCOMPARE(tree.left(tree.leader, tree.leader->titleRect()), teamText);
        QCOMPARE(tree.left(tree.team, tree.team->nameRect()), teamText);
    }

    void quickKeyboardAndAccessibility() {
        std::unique_ptr<QObject> root = createTree();
        QVERIFY(root);
        auto* window = qobject_cast<QQuickWindow*>(root.get());
        QVERIFY(QTest::qWaitForWindowExposed(window));
        window->requestActivate();
        auto part = [&](const char* name) { return root->property(name).value<QQuickItem*>(); };

        QQuickItem* repository = part("repository");
        repository->forceActiveFocus(Qt::TabFocusReason);
        QTest::keyClick(window, Qt::Key_Return);
        QVERIFY(!repository->property("expanded").toBool());
        QVERIFY(repository->property("waitingDotShown").toBool());
        QCOMPARE(accessibleText(repository, QAccessible::Description),
                 QStringLiteral("Collapsed, 3 conversations, 1 waiting for you"));
        QTest::keyClick(window, Qt::Key_Space);
        QVERIFY(repository->property("expanded").toBool());

        QQuickItem* team = part("team");
        auto* fold = team->findChild<QQuickItem*>(QStringLiteral("sidebarTeamFold"));
        auto* name = team->findChild<QQuickItem*>(QStringLiteral("sidebarTeamName"));
        QVERIFY(fold && name);
        fold->forceActiveFocus(Qt::TabFocusReason);
        QTest::keyClick(window, Qt::Key_Return);
        QVERIFY(!team->property("expanded").toBool());
        QCOMPARE(accessibleText(fold, QAccessible::Name), QStringLiteral("Unfold Mobile Refactor"));
        QTest::keyClick(window, Qt::Key_Space);
        QVERIFY(team->property("expanded").toBool());
        QCOMPARE(fold->nextItemInFocusChain(), name);
        name->forceActiveFocus(Qt::TabFocusReason);
        QTest::keyClick(window, Qt::Key_Return);
        QTest::keyClick(window, Qt::Key_Enter);
        QTest::keyClick(window, Qt::Key_Space);
        QCOMPARE(root->property("opened").toInt(), 3);
        QCOMPARE(accessibleText(name, QAccessible::Name), QStringLiteral("Team Mobile Refactor"));
        QCOMPARE(accessibleText(name, QAccessible::Description),
                 QStringLiteral("2 members, 1 decision waits for you"));

        QQuickItem* developer = part("developer");
        developer->forceActiveFocus(Qt::TabFocusReason);
        QTest::keyClick(window, Qt::Key_Return);
        QVERIFY(developer->property("checked").toBool());
        QTest::keyClick(window, Qt::Key_Space);
        QVERIFY(!developer->property("checked").toBool());
        QCOMPARE(root->property("rowClicks").toInt(), 2);
        QCOMPARE(accessibleText(developer, QAccessible::Name),
                 QStringLiteral("Sidebar tree parts in ket-qt, #3651"));
        QCOMPARE(accessibleText(developer, QAccessible::Description),
                 QStringLiteral("Developer, Completed, unread, team/3/t-2"));
        QVERIFY(
            developer->findChild<QQuickItem*>(QStringLiteral("sidebarRowUnreadDot"))->isVisible());
        QCOMPARE(accessibleText(part("section"), QAccessible::Name),
                 QStringLiteral("Conversations, 1"));

        // Dark and light draw the recipe's colours.
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCoreApplication::processEvents();
        const QVariantMap dark = developer->property("recipe").toMap();
        QCOMPARE(dark.value(QStringLiteral("title")).value<QColor>(),
                 sidebarRowRecipe(false, true, {}, Theme::Dark).title);
        QCOMPARE(team->property("recipe").toMap().value(QStringLiteral("name")).value<QColor>(),
                 Tokens::color(u"text-main", Theme::Dark));
    }

  private:
    static QString accessibleText(QQuickItem* item, QAccessible::Text text) {
        QAccessibleInterface* face = QAccessible::queryAccessibleInterface(item);
        return face ? face->text(text) : QString();
    }

    std::unique_ptr<QObject> createTree() {
        QQmlComponent component(&engine_);
        component.setData(
            QByteArrayLiteral(
                "import QtQuick\nimport Ket\n"
                "Window { width: 280; height: 400; visible: true\n"
                "  property alias repository: repository; property alias section: section\n"
                "  property alias row: row; property alias team: team\n"
                "  property alias members: members; property alias leader: leader\n"
                "  property alias developer: developer\n"
                "  property alias repositoryGuide: repositoryGuide\n"
                "  property int opened: 0; property int rowClicks: 0\n"
                "  Column { width: parent.width\n"
                "    KetSidebarRepositoryHeader { id: repository; width: parent.width\n"
                "      name: \"ketplus\"; conversationCount: 3; waitingCount: 1 }\n"
                "    KetSidebarTreeGuide { id: repositoryGuide; width: parent.width\n"
                "      KetSidebarSectionLabel { id: section; text: \"Conversations\"; count: 1 }\n"
                "      KetSidebarRow { id: row; text: \"Fix the sidebar\"; number: \"3651\" }\n"
                "      KetSidebarTeamBranch { id: team; name: \"Mobile Refactor\"\n"
                "        memberCount: 2; decisionCount: 1; onActivated: opened++ }\n"
                "      KetSidebarTreeGuide { id: members; level: 1; visible: team.expanded\n"
                "        KetSidebarRow { id: leader; level: 1; text: \"Lead Mobile Refactor\"\n"
                "          number: \"9692\"; role: \"Leader\" }\n"
                "        KetSidebarRow { id: developer; level: 1\n"
                "          text: \"Sidebar tree parts in ket-qt\"; number: \"3651\"\n"
                "          status: KetTheme.ConversationStatus.Completed; unread: true\n"
                "          role: \"Developer\"; worktree: \"team/3/t-2\"\n"
                "          onClicked: rowClicks++ } } } } }\n"),
            QUrl());
        if (component.isError())
            qWarning().noquote() << component.errorString();
        return std::unique_ptr<QObject>(component.create());
    }

    QQmlEngine engine_;
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    SidebarTreeTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_sidebar_tree.moc"
