// SPDX-License-Identifier: MPL-2.0
// The Qt Quick controls and chrome (batch 8) and editor surfaces (batch 9). Any QML warning
// fails the test that caused it.
#include "KetAppearance.h"
#include "KetRecipes.h"

#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>
#include <algorithm>
#include <functional>

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

} // namespace

class QuickChromeTest final : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase() { previousHandler = qInstallMessageHandler(countQmlWarnings); }

    void cleanup() {
        const int warnings = std::exchange(qmlWarnings, 0);
        QCOMPARE(warnings, 0);
    }

    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void controlsLoadAndBehave() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Window { width: 800; height: 600; visible: true\n"
            "  property alias picker: picker; property alias avatar: avatar\n"
            "  property alias tabs: tabs; property alias header: header\n"
            "  property alias tree: tree; property int actions: 0\n"
            "  KetStepSlider { id: picker; width: 400; label: \"Reasoning effort\"\n"
            "    steps: [\"low\", \"medium\", \"high\"]; currentIndex: 1 }\n"
            "  KetAvatar { id: avatar; y: 100; name: \"Kieu Duy\" }\n"
            "  KetRouteTabs { id: tabs; y: 150; width: 500; tabs: [\"General\", \"Providers\"] }\n"
            "  KetGroupHeader { id: header; y: 230; width: 280; name: \"ketplus\"\n"
            "    onActionTriggered: actions++ }\n"
            "  KetTreeItem { id: tree; y: 260; width: 280; text: \"develop\"; iconName: "
            "\"branch\"\n"
            "    activity: \"working\"; activityText: \"Working\"; count: \"3\" }\n"
            "  KetStatusBar { y: 400; width: 600\n"
            "    trailing: [KetStatusItem { text: \"UTF-8\" }]\n"
            "    KetStatusItem { text: \"Local\"; iconName: \"success\" } }\n"
            "  KetBreadcrumb { y: 440; width: 600; path: [\"a\", \"b.qml\"] }\n"
            "  KetPaneHeader { y: 480; width: 600; title: \"b.qml\"; iconName: \"file-code\" }\n"
            "  KetSlider { y: 530; width: 200; from: 0; to: 10; value: 5 }\n"
            "}\n"));
        QVERIFY(root);
        auto* picker = root->property("picker").value<QQuickItem*>();
        QCOMPARE(picker->property("currentStep").toString(), QStringLiteral("medium"));
        auto* avatar = root->property("avatar").value<QQuickItem*>();
        QCOMPARE(avatar->property("initials").toString(), QStringLiteral("KD"));
        QCOMPARE(avatar->implicitWidth(), 29.0);
        auto* tabs = root->property("tabs").value<QQuickItem*>();
        QCOMPARE(tabs->property("currentIndex").toInt(), 0);
        auto* header = root->property("header").value<QQuickItem*>();
        QCOMPARE(header->implicitHeight(), 24.0);
        QVERIFY(header->property("expanded").toBool());
        auto* tree = root->property("tree").value<QQuickItem*>();
        QVERIFY(tree->property("hasActivity").toBool());
        QVERIFY(tree->implicitHeight() > 2 * 9 + 13);
    }

    void surfacesLoadAndBehave() {
        std::unique_ptr<QObject> root = create(QByteArrayLiteral(
            "import QtQuick\nimport Ket\n"
            "Window { width: 900; height: 700; visible: true\n"
            "  property alias spotlight: spotlight; property alias diff: diff\n"
            "  property alias bar: bar; property alias card: card; property alias file: file\n"
            "  property alias changes: changes; property alias swatch: swatch\n"
            "  property string chosen\n"
            "  KetCommandPalette { id: spotlight\n"
            "    groups: [{ title: \"Files\", items: [{ id: \"a\", title: \"A.qml\" },\n"
            "                                        { id: \"b\", title: \"B.qml\" }] }]\n"
            "    onActivated: item => chosen = item.id }\n"
            "  KetFileRow { id: file; width: 280; text: \"src\"; folder: true; depth: 1 }\n"
            "  KetChangedFiles { id: changes; y: 40; width: 400\n"
            "    files: [{ name: \"a\", added: 2, removed: 1 }, { name: \"b\", added: 3 }] }\n"
            "  KetDiffView { id: diff; y: 200; width: 400 }\n"
            "  KetTaskBar { id: bar; y: 400; width: 500\n"
            "    tasks: [{ title: \"npm run dev\" }, { title: \"ctest\", status: "
            "KetTheme.TaskStatus.Failed }] }\n"
            "  KetUpdateCard { id: card; y: 500; width: 400; title: \"1.8.0\"; progress: 40 }\n"
            "  KetUpdatePill { y: 600; width: 240; text: \"Update\"; detail: \"1.8.0\" }\n"
            "  KetThemeCard { x: 500; width: 220; text: \"Dark\"; previewDark: true }\n"
            "  KetAccentSwatch { id: swatch; x: 500; y: 300 }\n"
            "}\n"));
        QVERIFY(root);
        QObject* spotlight = root->property("spotlight").value<QObject*>();
        QMetaObject::invokeMethod(spotlight, "open");
        QTRY_VERIFY(spotlight->property("opened").toBool());
        QCOMPARE(spotlight->property("itemCount").toInt(), 2);
        QMetaObject::invokeMethod(spotlight, "setHighlightedIndex", Q_ARG(QVariant, -1));
        QCOMPARE(spotlight->property("highlightedIndex").toInt(), 1); // wraps
        QMetaObject::invokeMethod(spotlight, "activate", Q_ARG(QVariant, 1));
        QCOMPARE(root->property("chosen").toString(), QStringLiteral("b"));
        QTRY_VERIFY(!spotlight->property("visible").toBool());

        auto* file = root->property("file").value<QQuickItem*>();
        QCOMPARE(file->implicitHeight(), 28.0);
        QVERIFY(file->property("checkable").toBool());
        auto* changes = root->property("changes").value<QQuickItem*>();
        QCOMPARE(changes->property("addedTotal").toInt(), 5);
        QCOMPARE(changes->property("removedTotal").toInt(), 1);
        QVERIFY(changes->property("countsKnown").toBool());
        // Repeater rows are visual children, not QObject children.
        const auto countWidths = [changes] {
            QList<qreal> widths;
            std::function<void(QQuickItem*)> visit = [&](QQuickItem* item) {
                if (item->objectName() == QLatin1String("changedFileCounts"))
                    widths.append(item->implicitWidth());
                for (auto* child : item->childItems())
                    visit(child);
            };
            visit(changes);
            return widths;
        };
        auto* totals = changes->findChild<QQuickItem*>("changedFilesTotals");
        QVERIFY(totals && totals->implicitWidth() > 0);
        QCOMPARE(countWidths().size(), 2);
        // Unknown counts are not shown as numbers, neither on the row nor as totals.
        changes->setProperty("files", QVariantList{QVariantMap{{"name", "a"}, {"added", 2}},
                                                   QVariantMap{{"name", "b"}, {"known", false}}});
        QVERIFY(!changes->property("countsKnown").toBool());
        QTRY_COMPARE(totals->implicitWidth(), 0.0);
        QTRY_VERIFY(countWidths().contains(0.0));
        // Nor are totals over a list that is not complete.
        changes->setProperty("files", QVariantList{QVariantMap{{"name", "a"}, {"added", 2}}});
        QTRY_VERIFY(totals->implicitWidth() > 0);
        changes->setProperty("complete", false);
        QTRY_COMPARE(totals->implicitWidth(), 0.0);
        changes->setProperty("complete", true);
        changes->setProperty("files", QVariantList{QVariantMap{{"name", "a"}, {"added", 2}},
                                                   QVariantMap{{"name", "b"}, {"known", false}}});
        QTRY_VERIFY(countWidths().contains(0.0));
        const auto widths = countWidths();
        QCOMPARE(
            std::count_if(widths.cbegin(), widths.cend(), [](qreal width) { return width > 0; }),
            1);

        auto* diff = root->property("diff").value<QQuickItem*>();
        QVariant lines;
        QMetaObject::invokeMethod(
            diff, "parseUnified", Q_RETURN_ARG(QVariant, lines),
            Q_ARG(QVariant, QStringLiteral("@@ -3 +3,2 @@\n a\n-b\n+c\n+d\n")));
        const QVariantList parsed = lines.toList();
        QCOMPARE(parsed.size(), 5);
        QCOMPARE(parsed.at(2).toMap().value(QStringLiteral("kind")).toString(),
                 QStringLiteral("remove"));
        QCOMPARE(parsed.at(4).toMap().value(QStringLiteral("number")).toInt(), 5);
        diff->setProperty("lines", lines);
        QTRY_VERIFY(diff->implicitHeight() > 4 * 25);

        auto* bar = root->property("bar").value<QQuickItem*>();
        QTRY_VERIFY(bar->implicitHeight() > 2 * 34);
        bar->setProperty("expanded", false);
        QTRY_COMPARE(bar->implicitHeight(), 24.0);
        auto* card = root->property("card").value<QQuickItem*>();
        QVERIFY(card->implicitHeight() > 0);
        QCOMPARE(root->property("swatch").value<QQuickItem*>()->implicitWidth(), 31.0);
    }

  private:
    std::unique_ptr<QObject> create(const QByteArray& source) {
        QQmlComponent component(&engine_);
        component.setData(source, QUrl()); // a local URL loads imports synchronously
        if (component.isError())
            qWarning().noquote() << component.errorString();
        return std::unique_ptr<QObject>(component.create());
    }

    QQmlEngine engine_;
};

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    QuickChromeTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_quick_chrome.moc"
