// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QElapsedTimer>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QWidget>

#include <functional>

namespace ket {

// The one clock every looping Widgets animation steps on, as KetTheme.animationClock is for
// Qt Quick: 20 steps a second, the rate the design system settled on for spinners and
// activity bars. Each widget used to run its own 60 Hz timer and repaint itself whole, so a
// sidebar with a few working conversations kept the GUI thread busy.
//
// The timer runs only while a widget subscribes. A step skips widgets that cannot be seen
// (hidden, or in a window that is minimized or not exposed), and calls the rest one after
// another so their repaints are flushed together.
class AnimationClock final : public QObject {
    Q_OBJECT
  public:
    static constexpr int intervalMs = 50;

    static AnimationClock& instance();

    // Milliseconds since the clock was created: one phase for every animation, so spinners
    // turn together.
    [[nodiscard]] qint64 elapsed() const { return elapsed_.elapsed(); }

    // Calls `step` on every tick while `widget` is subscribed and can be seen. Subscribing
    // again replaces the step; a destroyed widget is dropped.
    void subscribe(QWidget* widget, std::function<void()> step);
    void unsubscribe(QWidget* widget);

    [[nodiscard]] bool isRunning() const { return timer_.isActive(); }
    [[nodiscard]] qsizetype subscriberCount() const { return steps_.size(); }
    // Whether `widget` is drawn on screen now; a step for one that is not is skipped.
    [[nodiscard]] static bool canBeSeen(const QWidget* widget);

    // Runs one step at once, for tests.
    void stepForTest() { step(); }

  private:
    AnimationClock();
    void step();

    QTimer timer_;
    QElapsedTimer elapsed_;
    QHash<QWidget*, std::function<void()>> steps_;
};

} // namespace ket
