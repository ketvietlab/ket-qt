// SPDX-License-Identifier: MPL-2.0
// The image viewer in both layers, against the same recipes.
#include "KetAppearance.h"
#include "KetLightbox.h"
#include "KetLightboxButton.h"
#include "KetLightboxRecipes.h"
#include "KetLightboxThumb.h"
#include "KetPrint.h"
#include "KetTokens.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QFontMetricsF>
#include <QImage>
#include <QMenu>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

QPixmap picture(int width, int height) {
    QPixmap pixmap(width, height);
    pixmap.fill(QColor(0x36, 0x5f, 0xd6));
    return pixmap;
}

QList<LightboxItem> threeItems() {
    return {{picture(2000, 1000), QString(), QStringLiteral("Wide")},
            {picture(400, 300), QString(), QStringLiteral("Small")},
            {picture(1000, 2000), QString(), QStringLiteral("Tall")}};
}

// A popup lives under the window's overlay and its items have no QObject parent there:
// walk the item tree.
QQuickItem* findItem(QQuickItem* item, const QString& name) {
    if (!item)
        return nullptr;
    if (item->objectName() == name)
        return item;
    for (QQuickItem* child : item->childItems())
        if (QQuickItem* found = findItem(child, name))
            return found;
    return nullptr;
}

} // namespace

class LightboxTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    // A photo viewer: dark in both themes, with the design system's metrics.
    void recipesFollowTheDesignSystem() {
        const LightboxRecipe light = lightboxRecipe(Theme::Light);
        const LightboxRecipe dark = lightboxRecipe(Theme::Dark);
        QColor backdrop = Tokens::color(u"text-main", Theme::Light);
        backdrop.setAlphaF(0.92f);
        QCOMPARE(light.backdrop, backdrop);
        QCOMPARE(dark.backdrop, light.backdrop);
        QCOMPARE(dark.foreground, Tokens::color(u"panel-bg", Theme::Light));
        QCOMPARE(light.stagePaddingInline, 48.0);
        QCOMPARE(light.maxZoom, 4.0);
        QCOMPARE(light.zoomStep, 0.5);
        QCOMPARE(light.transitionMs, 160);

        const LightboxToolRecipe idle = lightboxToolRecipe({}, Theme::Light);
        const LightboxToolRecipe hovered =
            lightboxToolRecipe({true, false, true, false}, Theme::Dark);
        const LightboxToolRecipe disabled =
            lightboxToolRecipe({true, false, false, true}, Theme::Light);
        QCOMPARE(idle.size, 40.0);
        QCOMPARE(qRound(idle.background.alphaF() * 100), 12);
        QCOMPARE(qRound(hovered.background.alphaF() * 100), 24);
        QCOMPARE(disabled.opacity, 0.4);
        QCOMPARE(disabled.background, idle.background);
        QCOMPARE(disabled.focusRing.alpha(), 0);

        QCOMPARE(lightboxThumbRecipe(LightboxThumbSize::Small, false, Theme::Light).size, 40.0);
        QCOMPARE(lightboxThumbRecipe(LightboxThumbSize::Small, false, Theme::Light).radius,
                 Tokens::px(u"radius-sm"));
        QCOMPARE(lightboxThumbRecipe(LightboxThumbSize::Medium, false, Theme::Light).size, 88.0);
        QCOMPARE(lightboxThumbRecipe(LightboxThumbSize::Large, true, Theme::Dark).focusRing,
                 Tokens::color(u"focus-border", Theme::Dark));
    }

    void widgetsViewerZoomsNavigatesAndCloses() {
        QWidget window;
        window.resize(1000, 700);
        auto* lightbox = new Lightbox(&window);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        lightbox->setItems(threeItems());
        QSignalSpy closed(lightbox, &Lightbox::closed);
        lightbox->open(0);
        QVERIFY(lightbox->isOpen());
        QCOMPARE(lightbox->geometry(), window.rect());
        QVERIFY(lightbox->closeButton()->hasFocus());
        // The toolbar is 40 + 2 × 8; the stage pads 48 inline and 8 block.
        QCOMPARE(lightbox->stageRect(), QRect(0, 56, 1000, 644));
        QCOMPARE(lightbox->fitScale(), 904.0 / 2000.0);
        QVERIFY(lightbox->toolButton(LightboxTool::ZoomIn)->isVisible());
        QVERIFY(!lightbox->toolButton(LightboxTool::Rotate)->isVisible());
        QVERIFY(!lightbox->toolButton(LightboxTool::ZoomOut)->isEnabled());

        lightbox->zoomIn();
        QTRY_COMPARE(lightbox->zoom(), 1.5);
        QVERIFY(lightbox->toolButton(LightboxTool::ZoomOut)->isEnabled());
        for (int i = 0; i < 20; ++i)
            lightbox->zoomIn();
        QTRY_COMPARE(lightbox->zoom(), lightbox->maximumZoom());
        QVERIFY(!lightbox->toolButton(LightboxTool::ZoomIn)->isEnabled());
        lightbox->toggleActualSize();
        QTRY_COMPARE(lightbox->fitScale() * lightbox->zoom(), 1.0);
        QCOMPARE(lightbox->toolButton(LightboxTool::ZoomLevel)->text(), QStringLiteral("100%"));
        lightbox->fit();
        QTRY_COMPARE(lightbox->zoom(), 1.0);

        // A tap on the picture zooms to 2× there; a tap on the empty stage closes.
        const QPoint centre = lightbox->pictureRect().center().toPoint();
        QTest::mouseClick(lightbox, Qt::LeftButton, {}, centre);
        QTRY_COMPARE(lightbox->zoom(), 2.0);
        lightbox->fit();
        QTRY_COMPARE(lightbox->zoom(), 1.0);

        // Previous/next wrap, and a swipe at fit size moves to the neighbour.
        lightbox->previous();
        QCOMPARE(lightbox->currentIndex(), 2);
        lightbox->next();
        QCOMPARE(lightbox->currentIndex(), 0);
        QTest::mousePress(lightbox, Qt::LeftButton, {}, centre);
        QTest::mouseMove(lightbox, centre - QPoint(120, 0));
        QTest::mouseRelease(lightbox, Qt::LeftButton, {}, centre - QPoint(120, 0));
        QCOMPARE(lightbox->currentIndex(), 1);
        // The 400 × 300 picture fits at its own size.
        QCOMPARE(lightbox->fitScale(), 1.0);
        lightbox->setWraps(false);
        lightbox->next();
        QCOMPARE(lightbox->currentIndex(), 2);
        QVERIFY(!lightbox->nextButton()->isEnabled());
        lightbox->next();
        QCOMPARE(lightbox->currentIndex(), 2);

        lightbox->setTools(lightbox->tools() | LightboxTool::Rotate);
        lightbox->rotate();
        QCOMPARE(lightbox->rotation(), 90);
        QCOMPARE(lightbox->fitScale(), 904.0 / 2000.0); // the tall picture lies down

        const QPoint corner(lightbox->stageRect().left() + 4, lightbox->stageRect().top() + 4);
        QTest::mouseClick(lightbox, Qt::LeftButton, {}, corner);
        QVERIFY(!lightbox->isOpen());
        QCOMPARE(closed.count(), 1);

        lightbox->open(0);
        QTest::keyClick(lightbox->closeButton(), Qt::Key_Right);
        QCOMPARE(lightbox->currentIndex(), 1);
        QTest::keyClick(lightbox->closeButton(), Qt::Key_Escape);
        QVERIFY(!lightbox->isOpen());
        QCOMPARE(closed.count(), 2);
    }

    // A host engine draws the picture; the view tools only report.
    void widgetsViewerHostsAStage() {
        QWidget window;
        window.resize(1000, 700);
        auto* lightbox = new Lightbox(&window);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        lightbox->setItems({{QPixmap(), QStringLiteral("A caption"), QString()}});
        lightbox->setTools(LightboxTool::ZoomOut | LightboxTool::ZoomLevel | LightboxTool::ZoomIn |
                           LightboxTool::Copy);
        auto* stage = new QWidget;
        lightbox->setStage(stage);
        lightbox->open();
        QCOMPARE(stage->parentWidget(), lightbox);
        const QRect room = lightbox->stageRect().adjusted(48, 8, -48, -8);
        QCOMPARE(stage->geometry(), room);
        QVERIFY(lightbox->stageRect().bottom() < 700 - 1); // the caption takes the bottom
        QSignalSpy triggered(lightbox, &Lightbox::toolTriggered);
        lightbox->toolButton(LightboxTool::ZoomIn)->click();
        QCOMPARE(triggered.count(), 1);
        QCOMPARE(triggered.at(0).at(0).value<LightboxTool>(), LightboxTool::ZoomIn);
        QCOMPARE(lightbox->zoom(), 1.0);
        QCOMPARE(lightbox->toolButton(LightboxTool::ZoomLevel)->text(), QStringLiteral("—"));
        lightbox->setZoomLevel(0.5);
        QCOMPARE(lightbox->toolButton(LightboxTool::ZoomLevel)->text(), QStringLiteral("50%"));
        lightbox->setToolEnabled(LightboxTool::Copy, false);
        QVERIFY(!lightbox->toolButton(LightboxTool::Copy)->isEnabled());
        QVERIFY(!lightbox->previousButton()->isVisible()); // one image: no previous/next
    }

    void widgetsThumbAndButton() {
        LightboxThumb thumb(picture(300, 200), QStringLiteral("Photo"));
        QCOMPARE(thumb.sizeHint(), QSize(88, 88));
        thumb.setThumbSize(LightboxThumbSize::Small);
        QCOMPARE(thumb.sizeHint(), QSize(40, 40));
        QVERIFY(thumb.isEnabled());
        LightboxThumb empty;
        empty.setText(QStringLiteral("No images"));
        QVERIFY(!empty.isEnabled());

        LightboxButton button(QStringLiteral("zoom-in"), QStringLiteral("Zoom in"));
        QCOMPARE(button.sizeHint(), QSize(40, 40));
        button.setPreviewState(InteractionState{true, false, true, false});
        QCOMPARE(button.recipe().background,
                 lightboxToolRecipe({true, false, true, false}, Theme::Light).background);
        LightboxButton level(QString(), QStringLiteral("Zoom level"));
        QCOMPARE(level.sizeHint(), QSize(64, 40));
        Appearance::instance().setZoom(1.5);
        QCOMPARE(button.sizeHint(), QSize(60, 60));
    }

    // Print sends the whole picture as rotated, fitted to the page and never enlarged
    // beyond its size at 96 dpi. The dialog itself is the system's and is not opened here.
    void printSendsTheRotatedPicture() {
        const QRect page(100, 100, 4800, 6600); // 600 dpi, inside the margins
        QCOMPARE(printedImageRect(QSize(2000, 1000), page, 600), QRect(100, 2200, 4800, 2400));
        QCOMPARE(printedImageRect(QSize(400, 300), page, 600), QRect(1250, 2462, 2500, 1875));
        QVERIFY(printedImageRect(QSize(), page, 600).isNull());

        QWidget window;
        window.resize(1000, 700);
        auto* lightbox = new Lightbox(&window);
        lightbox->setItems(threeItems());
        lightbox->setTools(lightbox->tools() | LightboxTool::Rotate | LightboxTool::Print);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        lightbox->open(0);
        QVERIFY(lightbox->toolButton(LightboxTool::Print)->isVisible());
        QVERIFY(lightbox->toolButton(LightboxTool::Print)->isEnabled());
        // Print sits between Download's place and Close: right of Rotate here.
        QVERIFY(lightbox->toolButton(LightboxTool::Print)->x() >
                lightbox->toolButton(LightboxTool::Rotate)->x());
        QCOMPARE(lightbox->printableImage().size(), QSize(2000, 1000));
        lightbox->rotate();
        QCOMPARE(lightbox->printableImage().size(), QSize(1000, 2000));
        lightbox->setStage(new QWidget);
        QVERIFY(lightbox->printableImage().isNull()); // a host stage prints its own picture

        QTemporaryDir dir;
        const QString path = dir.filePath(QStringLiteral("wide.png"));
        QVERIFY(picture(300, 100).toImage().save(path));
        QCOMPARE(printableImage(QUrl::fromLocalFile(path), 90).size(), QSize(100, 300));
        QCOMPARE(printableImage(QUrl::fromLocalFile(path), 0).size(), QSize(300, 100));
        QVERIFY(printableImage(QUrl(QStringLiteral("https://example.com/a.png")), 0).isNull());
    }

    // The Quick twin fits, zooms and steps as ket::Lightbox does at the same size.
    // A narrow window keeps every tool reachable: those that do not fit, last first, move
    // into a More menu beside Close, and none crosses the counter or the window's edge.
    void narrowToolbarMovesToolsIntoMore() {
        QCOMPARE(lightboxShownToolCount({40, 72, 40}, 40, 8, 1000), 3);
        QCOMPARE(lightboxShownToolCount({40, 72, 40}, 40, 8, 40 + 3 * 8 + 152), 3);
        QCOMPARE(lightboxShownToolCount({40, 72, 40}, 40, 8, 40 + 3 * 8 + 151), 1);
        QCOMPARE(lightboxShownToolCount({40, 72, 40}, 40, 8, 88), 0);

        const LightboxTools all = LightboxTool::ZoomOut | LightboxTool::ZoomLevel |
                                  LightboxTool::ZoomIn | LightboxTool::Rotate | LightboxTool::Copy |
                                  LightboxTool::Download | LightboxTool::Print |
                                  LightboxTool::Fullscreen;
        Appearance::instance().setZoom(1.5);
        const LightboxRecipe r = lightboxRecipe(Theme::Light);
        const qreal padding = r.toolbarPaddingInline * 1.5;
        const qreal counterRight = [&] {
            QFont font;
            font.setPixelSize(qRound(r.counterPixels * 1.5));
            return padding + QFontMetricsF(font).horizontalAdvance(QStringLiteral("1 / 3"));
        }();

        for (const int width : {420, 600}) {
            QWidget window;
            window.resize(width, 700);
            auto* lightbox = new Lightbox(&window);
            lightbox->setTools(all);
            lightbox->setItems(threeItems());
            window.show();
            lightbox->open();
            QVERIFY(QTest::qWaitForWindowExposed(&window));
            QVERIFY(!lightbox->overflowTools().isEmpty());
            QVERIFY(lightbox->moreButton()->isVisible());
            QList<LightboxTool> shown;
            for (LightboxTool tool :
                 {LightboxTool::ZoomOut, LightboxTool::ZoomLevel, LightboxTool::ZoomIn,
                  LightboxTool::Rotate, LightboxTool::Copy, LightboxTool::Download,
                  LightboxTool::Print, LightboxTool::Fullscreen}) {
                QAbstractButton* button = lightbox->toolButton(tool);
                if (!button->isVisible())
                    continue;
                shown.append(tool);
                QVERIFY2(button->geometry().left() >= counterRight,
                         qPrintable(QStringLiteral("%1 px: tool at %2")
                                        .arg(width)
                                        .arg(button->geometry().left())));
            }
            QCOMPARE(shown.size() + lightbox->overflowTools().size(), 8);
            QVERIFY(lightbox->moreButton()->geometry().left() >= counterRight);
            QVERIFY(lightbox->closeButton()->geometry().right() <= width);
            // The menu lists the hidden tools and triggers them.
            QSignalSpy triggered(lightbox, &Lightbox::toolTriggered);
            lightbox->moreButton()->click();
            QTRY_VERIFY(lightbox->moreMenu()->isVisible());
            const QList<QAction*> actions = lightbox->moreMenu()->actions();
            QCOMPARE(actions.size(), lightbox->overflowTools().size());
            actions.last()->trigger();
            QCOMPARE(triggered.size(), 1);
            QCOMPARE(triggered.first().first().value<LightboxTool>(),
                     lightbox->overflowTools().last());
            lightbox->moreMenu()->close();
        }

        // An open menu follows the host's tool state and closes with the viewer.
        {
            QWidget window;
            window.resize(420, 700);
            auto* lightbox = new Lightbox(&window);
            lightbox->setStage(new QWidget);
            lightbox->setTools(all);
            lightbox->setItems(threeItems());
            window.show();
            lightbox->open();
            QVERIFY(QTest::qWaitForWindowExposed(&window));
            QVERIFY(!lightbox->overflowTools().isEmpty());
            const LightboxTool tool = lightbox->overflowTools().last();
            QSignalSpy triggered(lightbox, &Lightbox::toolTriggered);
            lightbox->moreButton()->click();
            QTRY_VERIFY(lightbox->moreMenu()->isVisible());
            QAction* action = lightbox->moreMenu()->actions().last();
            QVERIFY(action->isEnabled());
            lightbox->setToolEnabled(tool, false);
            QVERIFY(!action->isEnabled());
            action->setEnabled(true); // a stale action must still not reach a disabled tool
            action->trigger();
            QCOMPARE(triggered.size(), 0);
            lightbox->setToolEnabled(tool, true);
            QVERIFY(action->isEnabled());
            lightbox->close();
            QVERIFY(!lightbox->isVisible());
            QVERIFY(!lightbox->moreMenu()->isVisible());
        }

        QWidget wide;
        wide.resize(1600, 700);
        auto* roomy = new Lightbox(&wide);
        roomy->setTools(all);
        roomy->setItems(threeItems());
        wide.show();
        roomy->open();
        QVERIFY(roomy->overflowTools().isEmpty());
        QVERIFY(!roomy->moreButton()->isVisible());

        // The Qt Quick viewer makes the same choice.
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData(QByteArrayLiteral("import QtQuick\nimport Ket\n"
                                            "Window { width: 420; height: 700; visible: true\n"
                                            "  property alias box: box\n"
                                            "  property int triggered: 0\n"
                                            "  KetLightbox { id: box; tools: 0xff\n"
                                            "    onToolTriggered: tool => triggered = tool } }\n"),
                          QUrl());
        std::unique_ptr<QObject> root(component.create());
        QVERIFY2(root, qPrintable(component.errorString()));
        auto* window = qobject_cast<QQuickWindow*>(root.get());
        QVERIFY(QTest::qWaitForWindowExposed(window));
        QObject* box = root->property("box").value<QObject*>();
        QVariantList items;
        for (int i = 0; i < 3; ++i)
            items.append(QVariantMap{{QStringLiteral("source"), QString()}});
        box->setProperty("items", items);
        QMetaObject::invokeMethod(box, "openAt", Q_ARG(QVariant, 0));
        QTRY_VERIFY(box->property("opened").toBool());
        const QVariantList overflow = box->property("overflowTools").toList();
        QVERIFY(!overflow.isEmpty());
        QCOMPARE(box->property("shownToolCount").toInt() + overflow.size(), 8);
        auto* more = qvariant_cast<QQuickItem*>(box->property("moreButton"));
        QVERIFY(more && more->isVisible());
        for (int tool = 1; tool <= 0x80; tool <<= 1) {
            QQuickItem* button =
                findItem(window->contentItem(), QStringLiteral("lightboxTool%1").arg(tool));
            QVERIFY(button);
            if (button->isVisible())
                QVERIFY(button->mapToScene(QPointF()).x() >= counterRight);
        }
        QVERIFY(more->mapToScene(QPointF()).x() >= counterRight);
        QMetaObject::invokeMethod(more, "clicked");
        const int last = overflow.last().toMap().value(QStringLiteral("tool")).toInt();
        QQuickItem* item = nullptr;
        QTRY_VERIFY((item = findItem(window->contentItem(),
                                     QStringLiteral("lightboxMoreItem%1").arg(last))));
        QMetaObject::invokeMethod(item, "triggered");
        QCOMPARE(root->property("triggered").toInt(), last);
        Appearance::instance().setZoom(1.0);
    }

    void quickViewerMatchesWidgets() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        QStringList sources;
        for (const QSize size : {QSize(2000, 1000), QSize(400, 300), QSize(1000, 2000)}) {
            QImage image(size, QImage::Format_RGB32);
            image.fill(Qt::darkCyan);
            const QString path =
                dir.filePath(QStringLiteral("%1x%2.png").arg(size.width()).arg(size.height()));
            QVERIFY(image.save(path));
            sources.append(QUrl::fromLocalFile(path).toString());
        }

        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData(
            QByteArrayLiteral(
                "import QtQuick\nimport Ket\n"
                "Window { width: 1000; height: 700; visible: true\n"
                "  property alias box: box\n"
                "  property int copied: 0\n"
                "  KetLightbox { id: box\n"
                "    tools: KetTheme.LightboxTool.ZoomOut | KetTheme.LightboxTool.ZoomLevel"
                " | KetTheme.LightboxTool.ZoomIn | KetTheme.LightboxTool.Copy\n"
                "    onToolTriggered: tool => { if (tool === KetTheme.LightboxTool.Copy)"
                " copied++ } }\n"
                "  KetLightboxThumb { id: thumb; objectName: \"thumb\" }\n"
                "  KetLightboxButton { objectName: \"hover\"; code: \"zoom-in\";"
                " previewState: ({ hovered: true }) }\n"
                "}\n"),
            QUrl());
        std::unique_ptr<QObject> root(component.create());
        QVERIFY2(root, qPrintable(component.errorString()));
        auto* window = qobject_cast<QQuickWindow*>(root.get());
        QVERIFY(QTest::qWaitForWindowExposed(window));
        QObject* box = root->property("box").value<QObject*>();
        QVariantList items;
        for (const QString& source : sources)
            items.append(QVariantMap{{QStringLiteral("source"), source}});
        box->setProperty("items", items);
        QMetaObject::invokeMethod(box, "openAt", Q_ARG(QVariant, 0));
        QTRY_VERIFY(box->property("ready").toBool());
        QVERIFY(box->property("opened").toBool());

        QWidget widgetWindow;
        widgetWindow.resize(1000, 700);
        auto* lightbox = new Lightbox(&widgetWindow);
        lightbox->setItems(threeItems());
        lightbox->setGeometry(widgetWindow.rect());
        QCOMPARE(box->property("fitScale").toReal(), lightbox->fitScale());

        QMetaObject::invokeMethod(box, "zoomIn");
        QTRY_COMPARE(box->property("zoom").toReal(), 1.5);
        QMetaObject::invokeMethod(box, "toggleActualSize");
        QTRY_COMPARE(box->property("zoom").toReal() * box->property("fitScale").toReal(), 1.0);
        QMetaObject::invokeMethod(box, "previous");
        QCOMPARE(box->property("currentIndex").toInt(), 2);
        QCOMPARE(box->property("zoom").toReal(), 1.0); // a new picture fits
        box->setProperty("wraps", false);
        QMetaObject::invokeMethod(box, "next");
        QCOMPARE(box->property("currentIndex").toInt(), 2);

        auto* copy = findItem(window->contentItem(), QStringLiteral("lightboxTool16"));
        QVERIFY(copy);
        QTRY_VERIFY(copy->property("enabled").toBool());
        QMetaObject::invokeMethod(copy, "clicked");
        QCOMPARE(root->property("copied").toInt(), 1);

        auto* thumb = root->findChild<QQuickItem*>(QStringLiteral("thumb"));
        QCOMPARE(thumb->implicitWidth(), 88.0);
        QVERIFY(!thumb->isEnabled()); // no source: the empty tile
        auto* hover = root->findChild<QQuickItem*>(QStringLiteral("hover"));
        QCOMPARE(
            hover->property("recipe").toMap().value(QStringLiteral("background")).value<QColor>(),
            lightboxToolRecipe({true, false, true, false}, Theme::Light).background);

        QMetaObject::invokeMethod(box, "close");
        QTRY_VERIFY(!box->property("opened").toBool());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    LightboxTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_lightbox.moc"
