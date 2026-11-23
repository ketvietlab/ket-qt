// SPDX-License-Identifier: MPL-2.0
#include "KetIcons.h"

#include <QFile>
#include <QGuiApplication>
#include <QPainter>
#include <QSvgRenderer>
#include <QTest>

using namespace ket;

namespace {

// Icons::pixmap() as it drew before it had a cache: the master rendered black at whole
// device pixels, then tinted through its alpha.
QImage uncachedPixmap(QStringView code, qreal box, qreal dpr, const QColor& color,
                      IconWeight weight) {
    const int side = qMax(1, qRound(box * dpr));
    QImage image(side, side, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    image.setDevicePixelRatio(dpr);
    QSvgRenderer renderer(Icons::resourcePath(code, box, weight));
    if (renderer.isValid()) {
        const qreal size = Icons::drawSize(box);
        const qreal origin = Icons::offset(box, dpr);
        QPainter painter(&image);
        renderer.render(&painter, QRectF(origin, origin, size, size));
    }
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(QRectF(0, 0, box, box), color);
    painter.end();
    return QPixmap::fromImage(image).toImage();
}

} // namespace

class IconsTest final : public QObject {
    Q_OBJECT
  private slots:
    void everyCodeHasEveryMasterAndWeight() {
        const QStringList codes = Icons::codes();
        QCOMPARE(codes.size(), 95);
        QVERIFY(codes.contains(QStringLiteral("users"))); // teams (ketvietlab/ketplus#469)
        // Touch the resources once; the qrc is registered on first use.
        (void)Icons::pixmap(codes.first(), 16, 1, Qt::black);
        for (const QString& code : codes) {
            for (const qreal box : {14.0, 16.0, 20.0, 24.0}) {
                for (const IconWeight weight : {IconWeight::Light, IconWeight::Regular})
                    QVERIFY2(QFile::exists(Icons::resourcePath(code, box, weight)),
                             qPrintable(Icons::resourcePath(code, box, weight)));
            }
        }
    }

    void lucideNamesMapToKetCodes() {
        QCOMPARE(Icons::codeForLucide(u"x"), QStringLiteral("close"));
        QCOMPARE(Icons::codeForLucide(u"monitor"), QStringLiteral("display"));
        QCOMPARE(Icons::codeForLucide(u"folder-open"), QStringLiteral("folder-open"));
        QVERIFY(Icons::codeForLucide(u"not-an-icon").isEmpty());
        QVERIFY(Icons::contains(u"close"));
        QVERIFY(!Icons::contains(u"x"));
    }

    void boxesPickTheDrawnMaster() {
        QCOMPARE(Icons::masterSize(14), 14);
        QCOMPARE(Icons::masterSize(15), 14);
        QCOMPARE(Icons::masterSize(13), 14);
        QCOMPARE(Icons::masterSize(17), 16);
        QCOMPARE(Icons::masterSize(18), 16);
        QCOMPARE(Icons::masterSize(19), 20);
        QCOMPARE(Icons::masterSize(22), 20);
        QCOMPARE(Icons::masterSize(23), 24);
        QCOMPARE(Icons::drawSize(17), 16.0); // never stretched
        QCOMPARE(Icons::drawSize(14), 14.0); // the compact master at its own size
        QCOMPARE(Icons::drawSize(12), 12.0); // smaller boxes scale the 14 px master down
        QVERIFY(Icons::resourcePath(u"close", 14).endsWith(QStringLiteral("/light/14/close.svg")));
        QCOMPARE(Icons::offset(17, 1), 0.0);
        QCOMPARE(Icons::offset(17, 2), 0.5); // one device pixel at 2x
        QCOMPARE(Icons::offset(16, 2), 0.0);
        QVERIFY(Icons::resourcePath(u"close", 17, IconWeight::Regular)
                    .endsWith(QStringLiteral("/regular/16/close.svg")));
    }

    void pixmapsAreTintedMasks() {
        const QColor tint(0x1746ff);
        const QPixmap pixmap = Icons::pixmap(u"close", 17, 2, tint);
        QCOMPARE(pixmap.size(), QSize(34, 34));
        QCOMPARE(pixmap.devicePixelRatio(), 2.0);
        const QImage image = pixmap.toImage();
        int opaque = 0;
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                const QColor pixel = image.pixelColor(x, y);
                if (pixel.alpha() == 255) {
                    ++opaque;
                    QCOMPARE(pixel.rgb(), tint.rgb());
                }
            }
        }
        QVERIFY(opaque > 0);
        // The 1 px column the 17 px box adds at 2x stays empty on the left.
        for (int y = 0; y < image.height(); ++y)
            QCOMPARE(image.pixelColor(0, y).alpha(), 0);
    }

    // Every key draws what an uncached render draws; asking again renders nothing.
    void cachedPixmapsMatchAnUncachedRender() {
        Icons::clearPixmapCacheForTest();
        struct Key {
            const char16_t* code;
            qreal box;
            qreal dpr;
            QColor color;
            IconWeight weight;
        };
        // Neighbours differ in one argument each, so a mixed-up key draws the wrong pixmap.
        const QList<Key> keys = {
            {u"loading", 16, 1, QColor(0x1746ff), IconWeight::Light},
            {u"loading", 16, 2, QColor(0x1746ff), IconWeight::Light},
            {u"loading", 16, 1.5, QColor(0x1746ff), IconWeight::Light},
            {u"loading", 17, 2, QColor(0x1746ff), IconWeight::Light},
            {u"loading", 20, 2, QColor(0x1746ff), IconWeight::Light},
            {u"loading", 16, 2, QColor(0x1746ff), IconWeight::Regular},
            {u"loading", 16, 2, QColor(0xd92d20), IconWeight::Regular},
            {u"loading", 16, 2, QColor(23, 70, 255, 128), IconWeight::Regular},
            {u"close", 16, 2, QColor(23, 70, 255, 128), IconWeight::Regular},
            {u"close", 14, 1, QColor(0x101828), IconWeight::Light},
        };
        for (const Key& key : keys) {
            const QImage expected =
                uncachedPixmap(key.code, key.box, key.dpr, key.color, key.weight);
            const QPixmap first = Icons::pixmap(key.code, key.box, key.dpr, key.color, key.weight);
            QCOMPARE(first.toImage(), expected);
            QCOMPARE(first.devicePixelRatio(), key.dpr);
        }
        const int renders = Icons::renderCountForTest();
        for (const Key& key : keys) {
            const QImage expected =
                uncachedPixmap(key.code, key.box, key.dpr, key.color, key.weight);
            QCOMPARE(Icons::pixmap(key.code, key.box, key.dpr, key.color, key.weight).toImage(),
                     expected);
        }
        QCOMPARE(Icons::renderCountForTest(), renders);
    }

    void pixmapCacheHasACeiling() {
        Icons::clearPixmapCacheForTest();
        const int keys = Icons::pixmapCacheLimit + 64;
        for (int i = 0; i < keys; ++i) {
            const QColor color(i % 256, i / 256, 128);
            const QImage image = Icons::pixmap(u"close", 16, 1, color).toImage();
            QCOMPARE(image, uncachedPixmap(u"close", 16, 1, color, IconWeight::Light));
            QVERIFY(Icons::pixmapCacheSizeForTest() <= Icons::pixmapCacheLimit);
        }
        QCOMPARE(Icons::pixmapCacheSizeForTest(), Icons::pixmapCacheLimit);
        // The newest entry is kept; an evicted one renders again, still the same pixels.
        const int renders = Icons::renderCountForTest();
        const QColor newest((keys - 1) % 256, (keys - 1) / 256, 128);
        (void)Icons::pixmap(u"close", 16, 1, newest);
        QCOMPARE(Icons::renderCountForTest(), renders);
        QCOMPARE(Icons::pixmap(u"close", 16, 1, QColor(0, 0, 128)).toImage(),
                 uncachedPixmap(u"close", 16, 1, QColor(0, 0, 128), IconWeight::Light));
        QCOMPARE(Icons::renderCountForTest(), renders + 1);
    }

    void qiconUsesTheSameRules() {
        const QIcon icon = Icons::icon(u"settings");
        QVERIFY(!icon.isNull());
        const QPixmap pixmap = icon.pixmap(QSize(20, 20), 2.0);
        QCOMPARE(pixmap.size(), QSize(40, 40));
        QVERIFY(Icons::icon(u"no-such-code").isNull());
    }
};

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    IconsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_icons.moc"
