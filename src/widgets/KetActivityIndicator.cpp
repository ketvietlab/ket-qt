// SPDX-License-Identifier: MPL-2.0
#include "KetActivityIndicator.h"

#include "KetAnimationClock.h"
#include "KetAppearance.h"

#include <QPainter>
#include <QtMath>

namespace ket {

ActivityIndicator::ActivityIndicator(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(tr("Working"));
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

ActivityIndicator::~ActivityIndicator() { AnimationClock::instance().unsubscribe(this); }

void ActivityIndicator::updateClock() {
    if (isVisible() && !preview_)
        AnimationClock::instance().subscribe(this, [this] { update(); });
    else
        AnimationClock::instance().unsubscribe(this);
}

void ActivityIndicator::setColor(std::optional<QColor> color) {
    color_ = color;
    update();
}

void ActivityIndicator::setPreviewTime(std::optional<qint64> elapsedMs) {
    preview_ = elapsedMs;
    updateClock();
    update();
}

ActivityRecipe ActivityIndicator::recipe() const {
    return activityRecipe(Appearance::instance().theme(), color_.value_or(QColor()));
}

QSize ActivityIndicator::sizeHint() const {
    const ActivityRecipe r = recipe();
    const Appearance& a = Appearance::instance();
    const qreal width = r.bars * a.scaled(r.barWidth) + (r.bars - 1) * a.scaled(r.spacing);
    return {qCeil(width), qRound(a.scaled(r.barHeight))};
}

void ActivityIndicator::paintEvent(QPaintEvent*) {
    const ActivityRecipe r = recipe();
    const Appearance& a = Appearance::instance();
    const qint64 elapsed = preview_.value_or(AnimationClock::instance().elapsed());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    const qreal barWidth = a.scaled(r.barWidth);
    const qreal radius = barWidth / 2;
    qreal x = (width() - sizeHint().width()) / 2.0;
    for (int bar = 0; bar < r.bars; ++bar) {
        const auto [height, opacity] = activityBarFrame(r, bar, elapsed);
        const qreal h = a.scaled(height);
        QColor color = r.color;
        color.setAlphaF(float(color.alphaF() * opacity));
        painter.setBrush(color);
        painter.drawRoundedRect(QRectF(x, (this->height() - h) / 2, barWidth, h), radius, radius);
        x += barWidth + a.scaled(r.spacing);
    }
}

void ActivityIndicator::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateClock();
}

void ActivityIndicator::hideEvent(QHideEvent* event) {
    AnimationClock::instance().unsubscribe(this);
    QWidget::hideEvent(event);
}

} // namespace ket
