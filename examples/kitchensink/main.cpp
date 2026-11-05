// SPDX-License-Identifier: MPL-2.0
// KetQt kitchensink: every foundation and component of the kit, with the Qt Widgets and
// Qt Quick versions side by side. Command-line options make review evidence repeatable:
//   --page=<id> --dark --zoom=1.25 --screenshot=<file>   render one page and exit
//   --smoke                                               open every page; fail on QML errors
#include "KetAppearance.h"
#include "KetStyle.h"
#include "KitchenSinkWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QQuickItem>
#include <QQuickWidget>
#include <QScrollArea>
#include <QTimer>
#include <QtQml/qqmlextensionplugin.h>

#include <cstdio>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

int failures = 0;

void countQmlProblems(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    const bool qml =
        QByteArrayView(context.category ? context.category : "").startsWith("qt.qml") ||
        message.contains(QStringLiteral(".qml:"));
    if (type != QtDebugMsg && type != QtInfoMsg && qml)
        ++failures;
    std::fprintf(stderr, "%s\n", qPrintable(message));
}

} // namespace

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("KetQt kitchensink"));
    QCommandLineParser parser;
    parser.addHelpOption();
    const QCommandLineOption page(
        QStringLiteral("page"),
        QStringLiteral("Open <id>: %1.").arg(kitchensink::KitchenSinkWindow::pageIds().join(u", ")),
        QStringLiteral("id"));
    const QCommandLineOption dark(QStringLiteral("dark"), QStringLiteral("Start in dark mode."));
    const QCommandLineOption zoom(QStringLiteral("zoom"),
                                  QStringLiteral("Interface zoom, e.g. 1.25."),
                                  QStringLiteral("factor"), QStringLiteral("1"));
    const QCommandLineOption screenshot(QStringLiteral("screenshot"),
                                        QStringLiteral("Save the window to <file> and exit."),
                                        QStringLiteral("file"));
    const QCommandLineOption smoke(
        QStringLiteral("smoke"),
        QStringLiteral("Open every page and exit non-zero on QML errors."));
    const QCommandLineOption size(
        QStringLiteral("size"), QStringLiteral("Window size, e.g. 1280x2400 for a full-page shot."),
        QStringLiteral("WxH"), QStringLiteral("1280x860"));
    const QCommandLineOption full(
        QStringLiteral("full"), QStringLiteral("With --screenshot, save the whole scrolled page."));
    parser.addOptions({page, dark, zoom, size, screenshot, full, smoke});
    parser.process(application);

    if (parser.isSet(smoke))
        qInstallMessageHandler(countQmlProblems);
    if (parser.isSet(dark))
        Appearance::instance().setMode(Appearance::Mode::Dark);
    Appearance::instance().setZoom(parser.value(zoom).toDouble());
    Style::install(application);

    kitchensink::KitchenSinkWindow window;
    const QStringList dimensions = parser.value(size).split(u'x');
    window.resize(dimensions.value(0).toInt() > 0 ? dimensions.value(0).toInt() : 1280,
                  dimensions.value(1).toInt() > 0 ? dimensions.value(1).toInt() : 860);
    if (parser.isSet(page) && !window.showPage(parser.value(page))) {
        std::fprintf(stderr, "Unknown page %s\n", qPrintable(parser.value(page)));
        return 2;
    }
    window.show();

    if (parser.isSet(smoke)) {
        // A Qt Quick view whose root is not exactly its size draws a stretched texture.
        const auto mismatched = [&window] {
            QList<QQuickWidget*> result;
            for (QQuickWidget* view : window.findChildren<QQuickWidget*>()) {
                const QQuickItem* root = view->rootObject();
                if (root && view->isVisibleTo(&window) && QSizeF(view->size()) != root->size())
                    result.append(view);
            }
            return result;
        };
        for (const qreal factor : {1.0, 1.25}) {
            Appearance::instance().setZoom(factor);
            for (const QString& id : kitchensink::KitchenSinkWindow::pageIds()) {
                window.showPage(id);
                // Views resize through queued calls; let them settle before checking.
                for (int pass = 0; pass < 50 && (pass < 2 || !mismatched().isEmpty()); ++pass)
                    QApplication::processEvents();
                for (const QQuickWidget* view : mismatched()) {
                    std::fprintf(stderr, "%s: view %dx%d, root %gx%g\n", qPrintable(id),
                                 view->width(), view->height(), view->rootObject()->width(),
                                 view->rootObject()->height());
                    ++failures;
                }
            }
        }
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QApplication::processEvents();
        return failures == 0 ? 0 : 1;
    }
    if (const QString file = parser.value(screenshot); !file.isEmpty()) {
        const bool wholePage = parser.isSet(full);
        QTimer::singleShot(400, &window, [&window, file, wholePage] {
            QWidget* target = &window;
            if (wholePage) {
                for (QScrollArea* area : window.findChildren<QScrollArea*>()) {
                    if (area->isVisible() && area->widget() && area->widget()->isVisible())
                        target = area->widget();
                }
            }
            if (!target->grab().save(file))
                QApplication::exit(1);
            else
                QApplication::quit();
        });
    }
    return QApplication::exec();
}
