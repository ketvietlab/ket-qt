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
