// SPDX-License-Identifier: MPL-2.0
#include "KetAppearance.h"

#include <QGuiApplication>
#include <QStyleHints>
#include <QtMath>

namespace ket {

Appearance& Appearance::instance() {
    static Appearance* appearance = new Appearance(qGuiApp);
    return *appearance;
}

Appearance::Appearance(QObject* parent) : QObject(parent) {
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this,
            [this] { update(); });
    update();
}

void Appearance::setMode(Mode mode) {
    if (mode_ == mode)
        return;
    mode_ = mode;
    update();
}

void Appearance::setZoom(qreal zoom) {
    zoom = qBound(0.5, zoom, 3.0);
    if (qFuzzyCompare(zoom_, zoom))
        return;
    zoom_ = zoom;
    emit zoomChanged(zoom_);
}

qreal Appearance::scale(qreal logicalPixels, qreal zoom) {
    return logicalPixels <= 1.0 ? logicalPixels : qRound(logicalPixels * zoom);
}

void Appearance::update() {
    Theme next = Theme::Light;
    if (mode_ == Mode::Dark)
        next = Theme::Dark;
    else if (mode_ == Mode::System &&
             QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
        next = Theme::Dark;
    if (next == theme_)
        return;
    theme_ = next;
    emit themeChanged(theme_);
}

} // namespace ket
