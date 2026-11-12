// SPDX-License-Identifier: MPL-2.0
#include "KetAnimationClock.h"

#include <QWindow>

namespace ket {

AnimationClock& AnimationClock::instance() {
    static AnimationClock clock;
    return clock;
}

AnimationClock::AnimationClock() {
    elapsed_.start();
    timer_.setInterval(intervalMs);
    connect(&timer_, &QTimer::timeout, this, &AnimationClock::step);
}

void AnimationClock::subscribe(QWidget* widget, std::function<void()> step) {
    if (widget == nullptr || !step)
        return;
    if (!steps_.contains(widget))
        connect(widget, &QObject::destroyed, this, [this, widget] { unsubscribe(widget); });
    steps_.insert(widget, std::move(step));
    if (!timer_.isActive())
        timer_.start();
}

void AnimationClock::unsubscribe(QWidget* widget) {
    if (steps_.remove(widget) > 0)
        disconnect(widget, &QObject::destroyed, this, nullptr);
    if (steps_.isEmpty())
        timer_.stop();
}

bool AnimationClock::canBeSeen(const QWidget* widget) {
    if (widget == nullptr || !widget->isVisible())
        return false;
    const QWidget* top = widget->window();
    if (top->isMinimized())
        return false;
    const QWindow* handle = top->windowHandle();
    return handle == nullptr || handle->isExposed();
}

void AnimationClock::step() {
    // A step may subscribe or unsubscribe widgets; walk a copy.
    const auto steps = steps_;
    for (auto it = steps.cbegin(); it != steps.cend(); ++it)
        if (canBeSeen(it.key()))
            it.value()();
}

} // namespace ket
