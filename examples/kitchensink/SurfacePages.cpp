// SPDX-License-Identifier: MPL-2.0
#include "SurfacePages.h"

#include "KetAppearance.h"
#include "KetBadge.h"
#include "KetButton.h"
#include "KetChangedFiles.h"
#include "KetCommandPalette.h"
#include "KetDiffView.h"
#include "KetFileRow.h"
#include "KetTaskBar.h"
#include "KetThemeCard.h"
#include "KetUpdateCard.h"
#include "KitchenSinkCommon.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QQuickWidget>

namespace ket::kitchensink {
namespace {

// The diff both toolkits show.
const char* const kSampleDiff = R"(diff --git a/src/NavigationRail.qml b/src/NavigationRail.qml
--- a/src/NavigationRail.qml
+++ b/src/NavigationRail.qml
@@ -12,6 +12,7 @@ Item {
     id: rail
     property bool compact: false
-    width: compact ? 48 : 220
+    width: compact ? KetTheme.px(48) : KetTheme.px(220)
+    clip: true
 
     Column {
         spacing: 4
@@ -40,3 +41,3 @@ Item {
-        color: "#f4f4f5"
+        color: KetTheme.t.sidebarBg
     }
)";

QWidget* toolkits(QWidget* widgets, QWidget* quick, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-2")));
    for (const auto& [title, content] :
         {std::pair{"Qt Widgets", widgets}, std::pair{"Qt Quick", quick}}) {
        column->addWidget(new TokenLabel(QString::fromLatin1(title),
                                         QStringLiteral("text-secondary"),
                                         QStringLiteral("text-sm"), 600, host));
        column->addWidget(content, 0, Qt::AlignLeft | Qt::AlignTop);
        column->addSpacing(qRound(Tokens::px(u"space-3")));
    }
    return host;
}

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("SurfaceSamples"), parent, {{QStringLiteral("mode"), mode}});
}

QWidget* palette(QWidget* parent) {
    // The stage starts parentless, so the overlay attaches to it as its window.
    auto* stage = new QWidget;
    const qreal zoom = Appearance::instance().zoom();
    stage->setFixedSize(qRound(720 * zoom), qRound(460 * zoom));
    auto* spotlight = new CommandPalette(stage);
    spotlight->setGroups({
        {QStringLiteral("Files"),
         {{QStringLiteral("rail"), QStringLiteral("NavigationRail.qml"),
           QStringLiteral("src/components/NavigationRail.qml"), QStringLiteral("file-code")},
          {QStringLiteral("readme"), QStringLiteral("README.md"), QStringLiteral("README.md"),
           QStringLiteral("file")}}},
        {QStringLiteral("Commands"),
         {{QStringLiteral("theme"), QStringLiteral("Toggle dark theme"),
           QStringLiteral("Appearance"), QStringLiteral("moon")},
          {QStringLiteral("terminal"), QStringLiteral("New terminal"),
           QStringLiteral("Workspace · ⌃`"), QStringLiteral("terminal")}}},
        {QStringLiteral("Conversations"),
         {{QStringLiteral("chat"), QStringLiteral("Refine the sidebar spacing"),
           QStringLiteral("develop · 2 hours ago"), QStringLiteral("bot")}}},
    });
    stage->setParent(parent);
    spotlight->open();
    return stage;
}

QWidget* explorer(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(0);
    const struct {
        const char* name;
        FileRow::Kind kind;
        int depth;
        const char* status;
        bool open;
        bool active;
    } rows[] = {
        {"src", FileRow::Kind::Folder, 0, "", true, false},
        {"components", FileRow::Kind::Folder, 1, "", true, false},
        {"NavigationRail.qml", FileRow::Kind::File, 2, "M", false, true},
        {"Composer.qml", FileRow::Kind::File, 2, "A", false, false},
        {"assets", FileRow::Kind::Folder, 1, "", false, false},
        {"README.md", FileRow::Kind::File, 0, "", false, false},
    };
    for (const auto& spec : rows) {
        auto* row = new FileRow(QString::fromLatin1(spec.name), spec.kind, spec.depth, host);
        row->setChecked(spec.open);
        row->setStatus(QString::fromLatin1(spec.status));
        row->setActive(spec.active);
        column->addWidget(row);
    }
    host->setFixedWidth(280);
    return host;
}

QWidget* changes(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* files = new ChangedFiles(host);
    files->addFile(QStringLiteral("NavigationRail.qml"), QStringLiteral("src/components"), 2, 2);
    files->addFile(QStringLiteral("Composer.qml"), QStringLiteral("src/components"), 48, 0);
    files->addFile(QStringLiteral("README.md"), {}, 3, 1);
    files->footer()->addWidget(
        new Button(QStringLiteral("Review changes"), ButtonVariant::Secondary, files));
    files->setNote(QStringLiteral("Not committed"));
    column->addWidget(files);
    auto* diff = new DiffView(host);
    diff->setLines(DiffView::parseUnified(QString::fromUtf8(kSampleDiff)));
    column->addWidget(diff);
    host->setFixedWidth(560);
    return host;
}

QWidget* tasks(QWidget* parent) {
    auto* bar = new TaskBar(parent);
    TaskRow* server = bar->addTask(QStringLiteral("npm run dev"));
    server->setMeta(QStringLiteral("localhost:5173 · 2m 14s"));
    server->setCurrent(true);
    QObject::connect(server, &TaskRow::stopRequested, server,
                     [server] { server->setStatus(TaskStatus::Stopped); });
    TaskRow* agent = bar->addTask(QStringLiteral("Review the diff"), QStringLiteral("bot"));
    agent->setMeta(QStringLiteral("Completed in 48s"));
    agent->setStatus(TaskStatus::Completed);
    TaskRow* tests = bar->addTask(QStringLiteral("ctest --test-dir build"));
    tests->setMeta(QStringLiteral("2 tests failed"));
    tests->setStatus(TaskStatus::Failed);
    bar->setFixedWidth(560);
    return bar;
}

QWidget* updates(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-3")));
    auto* downloading = new UpdateCard(QStringLiteral("KetPlus 1.8.0"), host);
    downloading->addBadge(new Badge(QStringLiteral("Stable"), BadgeTone::Accent));
    downloading->setDetail(QStringLiteral("Downloading · 42 MB of 96 MB"));
    downloading->setProgress(44);
    column->addWidget(downloading);
    auto* ready = new UpdateCard(QStringLiteral("KetPlus 1.8.0"), host);
    ready->addBadge(new Badge(QStringLiteral("Ready"), BadgeTone::Success));
    ready->setReady(true);
    ready->setDetail(QStringLiteral("Restart to finish installing."));
    ready->setNotes({QStringLiteral("Command palette searches conversations"),
                     QStringLiteral("Background tasks keep running after a restart")});
    ready->addAction(new Button(QStringLiteral("Restart now"), ButtonVariant::Primary));
    ready->addAction(new Button(QStringLiteral("Later"), ButtonVariant::Ghost));
    column->addWidget(ready);
    auto* pill =
        new UpdatePill(QStringLiteral("Update available"), QStringLiteral("KetPlus 1.8.0"), host);
    pill->setFixedWidth(240);
    column->addWidget(pill);
    host->setFixedWidth(520);
    return host;
}

QWidget* appearance(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* cards = new QHBoxLayout;
    cards->setSpacing(qRound(Tokens::px(u"space-3")));
    auto* group = new QButtonGroup(host);
    for (const auto& [title, theme] :
         {std::pair{"Light", Theme::Light}, std::pair{"Dark", Theme::Dark}}) {
        auto* card = new ThemeCard(QString::fromLatin1(title), theme, host);
        card->setChecked(theme == Theme::Light);
        group->addButton(card);
        cards->addWidget(card);
    }
    column->addLayout(cards);
    auto* accent = new QHBoxLayout;
    accent->setSpacing(qRound(Tokens::px(u"space-2-5")));
    accent->addWidget(new AccentSwatch(host));
    accent->addWidget(new TokenLabel(QStringLiteral("Indigo"), QStringLiteral("text-main"),
                                     QStringLiteral("text-md"), 600, host));
    accent->addStretch();
    column->addLayout(accent);
    host->setFixedWidth(520);
    return host;
}

} // namespace

QWidget* surfacePage() {
    PageFrame frame = makePage(
        QStringLiteral("Surfaces"),
        QStringLiteral("The command palette, file explorer rows, changed files and diff lines, "
                       "background tasks, update cards and the theme picker, from "
                       "ket::commandPaletteRecipe, fileRowRecipe, changedFilesRecipe, "
                       "diffRecipe, taskRowRecipe, updateCardRecipe and themeCardRecipe."));
    const auto section = [&](const char* title, QWidget* widgets, const char* mode) {
        frame.body->addWidget(sectionTitle(QString::fromUtf8(title), frame.page));
        frame.body->addWidget(
            toolkits(widgets, quick(QString::fromLatin1(mode), frame.page), frame.page));
    };
    section("Command palette · live", palette(frame.page), "palette");
    section("File explorer · live", explorer(frame.page), "explorer");
    section("Changed files and diff", changes(frame.page), "changes");
    section("Background tasks · live", tasks(frame.page), "tasks");
    section("Updates", updates(frame.page), "updates");
    section("Theme and accent · live", appearance(frame.page), "appearance");
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
