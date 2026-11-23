// SPDX-License-Identifier: MPL-2.0
// A working conversation's mark turns from frames drawn once: a clock step blits one frame
// with no rotation and renders no SVG, the turn steps no coarser than the clock, and a theme
// or zoom change draws the new recipe at once.
#include "KetAnimationClock.h"
#include "KetAppearance.h"
#include "KetConversationRecipes.h"
#include "KetConversationRow.h"
#include "KetIcons.h"
#include "KetProfile.h"
#include "KetSidebarTree.h"
#include "KetSpinnerFrames.h"

#include <QApplication>
#include <QPaintEngine>
#include <QPainter>
#include <QTest>
#include <QtMath>

using namespace ket;

namespace {

struct Blit {
    QImage image;
    bool turned = false;
};

// Records every pixmap and image a paint draws, with whether its transform rotates.
class RecordingEngine final : public QPaintEngine {
  public:
    RecordingEngine() : QPaintEngine(QPaintEngine::AllFeatures) {}
    bool begin(QPaintDevice*) override { return true; }
    bool end() override { return true; }
    void updateState(const QPaintEngineState&) override {}
    void drawPixmap(const QRectF&, const QPixmap& pixmap, const QRectF&) override {
        record(pixmap.toImage());
    }
    void drawImage(const QRectF&, const QImage& image, const QRectF&,
                   Qt::ImageConversionFlags) override {
        record(image);
    }
    void drawPath(const QPainterPath&) override {}
    void drawPolygon(const QPointF*, int, PolygonDrawMode) override {}
    void drawTextItem(const QPointF&, const QTextItem&) override {}
    Type type() const override { return QPaintEngine::User; }

    QList<Blit> blits;

  private:
    void record(const QImage& image) {
        const QTransform t = painter()->combinedTransform();
        blits.append({image, !qFuzzyIsNull(t.m12()) || !qFuzzyIsNull(t.m21())});
    }
};

class RecordingDevice final : public QPaintDevice {
  public:
    explicit RecordingDevice(QSize size) : size_(size) {}
    QPaintEngine* paintEngine() const override { return &engine; }
    mutable RecordingEngine engine;

  protected:
    int metric(PaintDeviceMetric metric) const override {
        switch (metric) {
        case PdmWidth:
            return size_.width();
        case PdmHeight:
            return size_.height();
        case PdmDepth:
            return 32;
        case PdmDpiX:
        case PdmDpiY:
        case PdmPhysicalDpiX:
        case PdmPhysicalDpiY:
            return 96;
        case PdmDevicePixelRatio:
            return 1;
        case PdmDevicePixelRatioScaled:
            return int(devicePixelRatioFScale());
        default:
            return QPaintDevice::metric(metric);
        }
    }

  private:
    QSize size_;
};

QList<Blit> paintOnce(QWidget& widget) {
    RecordingDevice device(widget.size());
    widget.render(&device);
    return device.engine.blits;
}

// Whether the most opaque pixel of a tinted frame is `color`, the frame being a turned,
// antialiased drawing whose thin strokes may stay short of full alpha.
bool drawnIn(const QImage& image, const QColor& color) {
    QColor best(Qt::transparent);
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            if (image.pixelColor(x, y).alpha() > best.alpha())
                best = image.pixelColor(x, y);
    return best.alpha() > 128 && qAbs(best.red() - color.red()) <= 3 &&
           qAbs(best.green() - color.green()) <= 3 && qAbs(best.blue() - color.blue()) <= 3;
}

QImage argb(const QImage& image) {
    return image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

} // namespace

class SpinnerFramesTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Profile::setCurrent(Profile::ketPlus());
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void cleanup() { Appearance::instance().setZoom(1.0); }

    void turnsStepNoCoarserThanTheClock_data() {
        QTest::addColumn<int>("spinMs");
        QTest::newRow("ketplus") << Profile::ketPlus().conversationSpinMs;
        QTest::newRow("design system") << Profile::designSystem().conversationSpinMs;
        QTest::newRow("uneven") << 1234;
        QTest::newRow("one step") << AnimationClock::intervalMs;
    }

    void turnsStepNoCoarserThanTheClock() {
        QFETCH(int, spinMs);
        const int frames = SpinnerFrames::frameCount(spinMs);
        const int clockSteps = qCeil(qreal(spinMs) / AnimationClock::intervalMs);
        QVERIFY2(frames >= clockSteps, qPrintable(QString::number(frames)));
        const qreal halfStep = 180.0 / frames;
        for (qint64 elapsed = 0; elapsed < 3 * spinMs; elapsed += 7) {
            const qreal continuous = 360.0 * qreal(elapsed % spinMs) / spinMs;
            const qreal shown =
                SpinnerFrames::frameAngle(SpinnerFrames::frameIndex(elapsed, spinMs), spinMs);
            qreal off = qAbs(shown - continuous);
            off = qMin(off, 360.0 - off);
            QVERIFY2(
                off <= halfStep + 1e-9,
                qPrintable(
                    QStringLiteral("%1 ms: %2 vs %3").arg(elapsed).arg(shown).arg(continuous)));
        }
    }

    void frameZeroIsTheUnturnedIcon_data() {
        QTest::addColumn<qreal>("box");
        QTest::addColumn<qreal>("dpr");
        QTest::newRow("16 @1x") << 16.0 << 1.0;
        QTest::newRow("16 @2x") << 16.0 << 2.0;
        QTest::newRow("17 @2x") << 17.0 << 2.0;
        QTest::newRow("20 @1.5x") << 20.0 << 1.5;
    }

    void frameZeroIsTheUnturnedIcon() {
        QFETCH(qreal, box);
        QFETCH(qreal, dpr);
        const QColor color(0x1746ff);
        const int spinMs = Profile::ketPlus().conversationSpinMs;
        const QImage icon = argb(Icons::pixmap(u"loading", box, dpr, color).toImage());
        const QImage zero =
            argb(SpinnerFrames::frame(u"loading", box, dpr, color, IconWeight::Light, spinMs, 0)
                     .toImage());
        const int margin = qRound(SpinnerFrames::margin(box, dpr) * dpr);
        QCOMPARE(zero.devicePixelRatio(), dpr);
        QCOMPARE(zero.width(), qRound((box + 2 * SpinnerFrames::margin(box, dpr)) * dpr));
        QCOMPARE(zero.copy(margin, margin, icon.width(), icon.height()), icon);
        for (int y = 0; y < zero.height(); ++y)
            for (int x = 0; x < zero.width(); ++x)
                if (!QRect(margin, margin, icon.width(), icon.height()).contains(x, y))
                    QCOMPARE(zero.pixelColor(x, y).alpha(), 0);
        // A quarter turn holds the icon turned, inside the frame.
        const QImage quarter =
            argb(SpinnerFrames::frame(u"loading", box, dpr, color, IconWeight::Light, spinMs,
                                      SpinnerFrames::frameCount(spinMs) / 4)
                     .toImage());
        QCOMPARE(quarter.size(), zero.size());
        QVERIFY(quarter != zero);
        QVERIFY(drawnIn(quarter, color));
    }

    void sidebarStepBlitsAPreparedFrame() {
        SidebarRow row(QStringLiteral("Fix the sidebar"));
        row.setStatus(ConversationStatus::Working);
        row.resize(280, row.height());
        row.show();
        QVERIFY(QTest::qWaitForWindowExposed(&row));
        verifyStepsBlitFrames(row, row.markRect().width());
    }

    void conversationRowStepBlitsAPreparedFrame() {
        ConversationRow row(QStringLiteral("Fix the sidebar"));
        row.setStatus(ConversationStatus::Working);
        row.resize(320, row.sizeHint().height());
        row.show();
        QVERIFY(QTest::qWaitForWindowExposed(&row));
        verifyStepsBlitFrames(row, Appearance::instance().scaled(row.recipe().indicatorSize));
    }

    // Light, dark and zoom draw the recipe they switch to on the next paint.
    void themeAndZoomRedrawAtOnce() {
        SidebarRow row(QStringLiteral("Fix the sidebar"));
        row.setStatus(ConversationStatus::Working);
        row.resize(280, row.height());
        row.show();
        QVERIFY(QTest::qWaitForWindowExposed(&row));
        struct Step {
            Appearance::Mode mode;
            qreal zoom;
        };
        for (const Step step :
             {Step{Appearance::Mode::Light, 1.0}, Step{Appearance::Mode::Dark, 1.0},
              Step{Appearance::Mode::Dark, 1.5}, Step{Appearance::Mode::Light, 1.25},
              Step{Appearance::Mode::System, 1.0}}) {
            Appearance::instance().setMode(step.mode);
            Appearance::instance().setZoom(step.zoom);
            QCoreApplication::processEvents();
            const ConversationStatusRecipe s = conversationStatusRecipe(
                ConversationStatus::Working, Appearance::instance().theme());
            const qreal box = row.markRect().width();
            const qreal dpr = row.devicePixelRatioF();
            const int side = qRound((box + 2 * SpinnerFrames::margin(box, dpr)) * dpr);
            const QList<Blit> blits = paintOnce(row);
            bool found = false;
            for (const Blit& blit : blits) {
                if (blit.image.width() != side || blit.image.height() != side)
                    continue;
                found = true;
                QVERIFY(drawnIn(blit.image, s.iconColor));
            }
            QVERIFY2(found, qPrintable(QStringLiteral("zoom %1").arg(step.zoom)));
        }
    }

    void turnCacheHasACeiling() {
        const int spinMs = Profile::ketPlus().conversationSpinMs;
        for (int i = 0; i < SpinnerFrames::cacheLimit + 8; ++i) {
            const QColor color(i, 0, 255 - i);
            const QImage frame =
                SpinnerFrames::frame(u"loading", 16, 1, color, IconWeight::Light, spinMs, 0)
                    .toImage();
            QVERIFY(drawnIn(frame, color));
            QVERIFY(SpinnerFrames::cacheSizeForTest() <= SpinnerFrames::cacheLimit);
        }
        QCOMPARE(SpinnerFrames::cacheSizeForTest(), SpinnerFrames::cacheLimit);
    }

  private:
    // After the first paint built the turn, later clock steps render no SVG, build nothing and
    // blit one frame without a rotating transform.
    static void verifyStepsBlitFrames(QWidget& row, qreal box) {
        (void)paintOnce(row);
        const int renders = Icons::renderCountForTest();
        const int builds = SpinnerFrames::buildCountForTest();
        const qreal dpr = row.devicePixelRatioF();
        const int side = qRound((box + 2 * SpinnerFrames::margin(box, dpr)) * dpr);
        for (int step = 0; step < 4; ++step) {
            AnimationClock::instance().stepForTest();
            const QList<Blit> blits = paintOnce(row);
            int frames = 0;
            for (const Blit& blit : blits) {
                QVERIFY(!blit.turned);
                if (blit.image.width() == side && blit.image.height() == side)
                    ++frames;
            }
            QCOMPARE(frames, 1);
            QTest::qWait(AnimationClock::intervalMs);
        }
        QCOMPARE(Icons::renderCountForTest(), renders);
        QCOMPARE(SpinnerFrames::buildCountForTest(), builds);
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    SpinnerFramesTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_spinner_frames.moc"
