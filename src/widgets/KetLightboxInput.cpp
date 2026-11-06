// SPDX-License-Identifier: MPL-2.0
// ket::Lightbox input: keys, focus, the pointer on the stage, the wheel and pinch.
#include "KetLightbox.h"

#include "KetAppearance.h"

#include <QAbstractButton>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QNativeGestureEvent>
#include <QWheelEvent>

#include <cmath>

namespace ket {
namespace {

constexpr qreal kEpsilon = 1e-3;

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

bool Lightbox::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible())
        place();
    // The tools take Space and Return; the viewer's keys work while one has focus.
    if (event->type() == QEvent::KeyPress && watched != parentWidget()) {
        auto* key = static_cast<QKeyEvent*>(event);
        if (key->key() != Qt::Key_Space && key->key() != Qt::Key_Return &&
            key->key() != Qt::Key_Enter && key->key() != Qt::Key_Tab &&
            key->key() != Qt::Key_Backtab) {
            keyPressEvent(key);
            return key->isAccepted();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Lightbox::keyPressEvent(QKeyEvent* event) {
    event->accept();
    switch (event->key()) {
    case Qt::Key_Escape:
        close();
        return;
    case Qt::Key_Left:
        previous();
        return;
    case Qt::Key_Right:
        next();
        return;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        zoomIn();
        return;
    case Qt::Key_Minus:
        zoomOut();
        return;
    case Qt::Key_0:
        fit();
        return;
    case Qt::Key_R:
        if (tools_.testFlag(LightboxTool::Rotate)) {
            rotate();
            return;
        }
        break;
    default:
        break;
    }
    event->ignore();
}

bool Lightbox::focusNextPrevChild(bool next) {
    QList<QWidget*> stops;
    if (stage_ && stage_->focusPolicy() & Qt::TabFocus)
        stops.append(stage_);
    for (QWidget* widget : {static_cast<QWidget*>(previous_), static_cast<QWidget*>(next_)})
        stops.append(widget);
    for (LightboxTool tool : kLightboxToolOrder)
        stops.append(buttons_.value(int(tool)));
    stops.append(close_);
    stops.removeIf([](QWidget* widget) { return !widget->isVisible() || !widget->isEnabled(); });
    if (stops.isEmpty())
        return false;
    QWidget* focused = QApplication::focusWidget();
    qsizetype at = stops.indexOf(focused);
    at = at < 0 ? 0 : (at + (next ? 1 : -1) + stops.size()) % stops.size();
    stops.at(at)->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    return true;
}

void Lightbox::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !stageRect().contains(event->position().toPoint())) {
        event->accept();
        return;
    }
    animation_.stop();
    pressing_ = true;
    moved_ = false;
    pressAt_ = event->position();
    panAtPress_ = view_.pan;
}

void Lightbox::mouseMoveEvent(QMouseEvent* event) {
    if (!pressing_)
        return;
    const QPointF delta = event->position() - pressAt_;
    if (std::abs(delta.x()) + std::abs(delta.y()) > 4)
        moved_ = true;
    if (builtIn() && view_.zoom > 1 + kEpsilon)
        setView(clamped(View{view_.zoom, panAtPress_ + delta}), false);
}

void Lightbox::mouseReleaseEvent(QMouseEvent* event) {
    if (!pressing_)
        return;
    pressing_ = false;
    const QPointF delta = event->position() - pressAt_;
    const LightboxRecipe r = recipe();
    const bool fitted = view_.zoom <= 1 + kEpsilon;
    if (!moved_) {
        // A tap on the picture toggles fit and 2×; a tap on the empty stage closes.
        if (builtIn() && !orientedSize().isEmpty() && pictureRect().contains(event->position()))
            fitted ? setView(zoomedAt(r.tapZoom, event->position()), true) : fit();
        else
            close();
    } else if (builtIn() && fitted && std::abs(delta.x()) > scaled(r.swipeDistance) &&
               std::abs(delta.x()) > std::abs(delta.y())) {
        step(delta.x() < 0 ? 1 : -1);
    }
}

// A trackpad scroll pans; a mouse wheel, or a scroll with Ctrl, zooms about the pointer.
void Lightbox::wheelEvent(QWheelEvent* event) {
    event->accept();
    if (!builtIn() || orientedSize().isEmpty())
        return;
    animation_.stop();
    if (!event->pixelDelta().isNull() && !(event->modifiers() & Qt::ControlModifier)) {
        setView(clamped(View{view_.zoom, view_.pan + event->pixelDelta()}), false);
        return;
    }
    const qreal notches = event->angleDelta().isNull() ? event->pixelDelta().y() / 40.0
                                                       : event->angleDelta().y() / 120.0;
    if (notches != 0)
        setView(zoomedAt(view_.zoom * std::pow(recipe().wheelStep, notches), event->position()),
                false);
}

bool Lightbox::event(QEvent* event) {
    if (event->type() == QEvent::NativeGesture && builtIn() && !orientedSize().isEmpty()) {
        auto* gesture = static_cast<QNativeGestureEvent*>(event);
        if (gesture->gestureType() == Qt::ZoomNativeGesture) {
            animation_.stop();
            setView(zoomedAt(view_.zoom * std::exp(gesture->value()), gesture->position()), false);
            return true;
        }
        if (gesture->gestureType() == Qt::SmartZoomNativeGesture) {
            view_.zoom > 1 + kEpsilon
                ? fit()
                : setView(zoomedAt(recipe().tapZoom, gesture->position()), true);
            return true;
        }
    }
    return QWidget::event(event);
}

} // namespace ket
