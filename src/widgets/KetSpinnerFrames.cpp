// SPDX-License-Identifier: MPL-2.0
#include "KetSpinnerFrames.h"

#include "KetAnimationClock.h"

#include <QCache>
#include <QCoreApplication>
#include <QList>
#include <QtMath>

namespace ket {
namespace {

using Turn = QList<QPixmap>;

int builds = 0;

// Emptied when the application goes, so no pixmap outlives the GUI.
QCache<QString, Turn>& turnCache() {
    static QCache<QString, Turn> cache = [] {
        qAddPostRoutine([] { turnCache().clear(); });
        return QCache<QString, Turn>(SpinnerFrames::cacheLimit);
    }();
    return cache;
}

QString turnKey(QStringView code, qreal box, qreal dpr, const QColor& color, IconWeight weight,
                int spinMs) {
    return QStringLiteral("%1|%2|%3|%4|%5|%6")
        .arg(code)
        .arg(box, 0, 'g', 17)
        .arg(dpr, 0, 'g', 17)
        .arg(quint64(color.rgba64()), 16, 16, QLatin1Char('0'))
        .arg(int(weight))
        .arg(spinMs);
}

Turn buildTurn(QStringView code, qreal box, qreal dpr, const QColor& color, IconWeight weight,
               int spinMs) {
    ++builds;
    const QPixmap icon = Icons::pixmap(code, box, dpr, color, weight);
    const qreal margin = SpinnerFrames::margin(box, dpr);
    const qreal side = box + 2 * margin;
    const int pixels = qMax(1, qRound(side * dpr));
    const int count = SpinnerFrames::frameCount(spinMs);
    Turn turn;
    turn.reserve(count);
    for (int index = 0; index < count; ++index) {
        QImage image(pixels, pixels, QImage::Format_ARGB32_Premultiplied);
        image.setDevicePixelRatio(dpr);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        if (index == 0) {
            painter.drawPixmap(QPointF(margin, margin), icon);
        } else {
            painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter.translate(side / 2, side / 2);
            painter.rotate(SpinnerFrames::frameAngle(index, spinMs));
            painter.drawPixmap(QPointF(-box / 2, -box / 2), icon);
        }
        painter.end();
        turn.append(QPixmap::fromImage(image));
    }
    return turn;
}

} // namespace

int SpinnerFrames::frameCount(int spinMs) {
    if (spinMs <= 0)
        return 1;
    return qMax(1, (spinMs + AnimationClock::intervalMs - 1) / AnimationClock::intervalMs);
}

int SpinnerFrames::frameIndex(qint64 elapsed, int spinMs) {
    if (spinMs <= 0)
        return 0;
    const int count = frameCount(spinMs);
    const qint64 phase = ((elapsed % spinMs) + spinMs) % spinMs;
    return int(qRound64(qreal(phase) * count / spinMs) % count);
}

qreal SpinnerFrames::frameAngle(int index, int spinMs) {
    return 360.0 * index / frameCount(spinMs);
}

qreal SpinnerFrames::margin(qreal box, qreal devicePixelRatio) {
    // Half the growth of a square's diagonal over its side, plus one pixel of antialiasing.
    return (qCeil(box * devicePixelRatio * (M_SQRT2 - 1) / 2) + 1) / devicePixelRatio;
}

QPixmap SpinnerFrames::frame(QStringView code, qreal box, qreal devicePixelRatio,
                             const QColor& color, IconWeight weight, int spinMs, int index) {
    const QString key = turnKey(code, box, devicePixelRatio, color, weight, spinMs);
    if (const Turn* cached = turnCache().object(key))
        return cached->value(qBound(0, index, int(cached->size()) - 1));
    auto* turn = new Turn(buildTurn(code, box, devicePixelRatio, color, weight, spinMs));
    const QPixmap result = turn->value(qBound(0, index, int(turn->size()) - 1));
    turnCache().insert(key, turn);
    return result;
}

void SpinnerFrames::paint(QPainter& painter, const QPointF& topLeft, QStringView code, qreal box,
                          qreal devicePixelRatio, const QColor& color, int spinMs, qint64 elapsed,
                          IconWeight weight) {
    const qreal reach = margin(box, devicePixelRatio);
    painter.drawPixmap(
        topLeft - QPointF(reach, reach),
        frame(code, box, devicePixelRatio, color, weight, spinMs, frameIndex(elapsed, spinMs)));
}

int SpinnerFrames::buildCountForTest() { return builds; }

int SpinnerFrames::cacheSizeForTest() { return int(turnCache().size()); }

} // namespace ket
