// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetIcons.h"

#include <QPainter>

namespace ket {

// A turning icon (a working conversation's status mark) drawn from frames turned once and
// kept, so an AnimationClock step blits one untransformed pixmap instead of rotating the
// icon: a rotated blit takes the raster engine's transformed path, which waits on Qt's thread
// pool on every step.
//
// A turn has at least as many frames as the clock has steps in `spinMs`, so the angle steps
// no further than it did when it followed the clock. Frames are kept per icon, box, device
// pixel ratio, colour, weight and period in a cache of at most `cacheLimit` turns; a theme,
// zoom or screen change asks for another key. Not installed; GUI thread only.
class SpinnerFrames final {
  public:
    static constexpr int cacheLimit = 16;

    [[nodiscard]] static int frameCount(int spinMs);
    // The frame showing `elapsed` milliseconds of a `spinMs` turn: the nearest one.
    [[nodiscard]] static int frameIndex(qint64 elapsed, int spinMs);
    // Degrees frame `index` of a `spinMs` turn is turned by.
    [[nodiscard]] static qreal frameAngle(int index, int spinMs);
    // Logical pixels a frame reaches past each side of the icon's box, for the corners of a
    // turned square; whole device pixels, so a frame lands on the pixel grid.
    [[nodiscard]] static qreal margin(qreal box, qreal devicePixelRatio);

    // Frame `index`: the icon `Icons::pixmap(code, box, devicePixelRatio, color, weight)`
    // turned about its centre, in a square `box + 2 * margin` logical pixels wide.
    [[nodiscard]] static QPixmap frame(QStringView code, qreal box, qreal devicePixelRatio,
                                       const QColor& color, IconWeight weight, int spinMs,
                                       int index);

    // Draws the icon whose box has its top left at `topLeft`, turned for `elapsed`.
    static void paint(QPainter& painter, const QPointF& topLeft, QStringView code, qreal box,
                      qreal devicePixelRatio, const QColor& color, int spinMs, qint64 elapsed,
                      IconWeight weight = IconWeight::Light);

    // For tests: turns built since start, and turns held by the cache.
    [[nodiscard]] static int buildCountForTest();
    [[nodiscard]] static int cacheSizeForTest();
};

} // namespace ket
