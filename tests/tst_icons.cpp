// SPDX-License-Identifier: MPL-2.0
#include "KetIcons.h"

#include <QFile>
#include <QGuiApplication>
#include <QTest>

using namespace ket;

class IconsTest final : public QObject {
    Q_OBJECT
  private slots:
    void everyCodeHasEveryMasterAndWeight() {
        const QStringList codes = Icons::codes();
        QCOMPARE(codes.size(), 93);
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
