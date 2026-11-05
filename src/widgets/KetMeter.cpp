// SPDX-License-Identifier: MPL-2.0
#include "KetMeter.h"

#include "KetAppearance.h"
#include "KetParagraph.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

Meter::Meter(const QString& label, QWidget* parent) : QWidget(parent), label_(label) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setAccessibleName(label);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Meter::changed);
}

void Meter::setLabel(const QString& label) {
    label_ = label;
    setAccessibleName(label);
    changed();
}

void Meter::setValueText(const QString& value) {
    value_ = value;
    setAccessibleDescription(value);
    changed();
}

void Meter::setFraction(qreal fraction) {
    fraction_ = fraction < 0 ? -1 : qBound<qreal>(0, fraction, 1);
    changed();
}

void Meter::setCaption(const QString& caption) {
    caption_ = caption;
    changed();
}

void Meter::setCompact(bool compact) {
    compact_ = compact;
    changed();
}

void Meter::changed() {
    updateGeometry();
    update();
}

MeterRecipe Meter::recipe() const {
    return meterRecipe(compact_, qMax<qreal>(0, fraction_), Appearance::instance().theme());
}

QFont Meter::labelFont(const MeterRecipe& r) const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.labelPixels)));
    return font;
}

QFont Meter::captionFont(const MeterRecipe& r) const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.captionPixels)));
    return font;
}

int Meter::heightForWidth(int width) const {
    const MeterRecipe r = recipe();
    qreal height = QFontMetricsF(labelFont(r)).height();
    if (fraction_ >= 0)
        height += scaled(r.trackSpacingTop) + scaled(r.trackHeight) + scaled(r.trackSpacingBottom);
    if (!caption_.isEmpty()) {
        detail::Paragraph caption(caption_, captionFont(r), r.captionLineHeight);
        caption.layout(width);
        height += caption.height();
    }
    return qCeil(height);
}

QSize Meter::sizeHint() const {
    const int width = qRound(scaled(Tokens::px(u"space-16")) * 4);
    return {width, heightForWidth(width)};
}

void Meter::paintEvent(QPaintEvent*) {
    const MeterRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const QFont font = labelFont(r);
    const QFontMetricsF metrics(font);
    const qreal line = metrics.height();
    QFont strong = font;
    strong.setWeight(QFont::Weight(r.valueWeight));
    const qreal valueWidth = QFontMetricsF(strong).horizontalAdvance(value_);
    painter.setFont(strong);
    painter.setPen(r.value);
    painter.drawText(QRectF(width() - valueWidth, 0, valueWidth, line),
                     Qt::AlignRight | Qt::AlignVCenter, value_);
    const qreal labelWidth = qMax<qreal>(0, width() - valueWidth - scaled(r.spacing));
    painter.setFont(font);
    painter.setPen(r.label);
    painter.drawText(QRectF(0, 0, labelWidth, line), Qt::AlignLeft | Qt::AlignVCenter,
                     metrics.elidedText(label_, Qt::ElideRight, labelWidth));
    qreal y = line;
    if (fraction_ >= 0) {
        y += scaled(r.trackSpacingTop);
        const qreal height = scaled(r.trackHeight);
        const QRectF track(0, y, width(), height);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.track);
        painter.drawRoundedRect(track, height / 2, height / 2);
        if (fraction_ > 0) {
            painter.save();
            QPainterPath clip;
            clip.addRoundedRect(track, height / 2, height / 2);
            painter.setClipPath(clip);
            painter.fillRect(QRectF(0, y, width() * fraction_, height), r.fill);
            painter.restore();
        }
        y += height + scaled(r.trackSpacingBottom);
    }
    if (!caption_.isEmpty()) {
        detail::Paragraph caption(caption_, captionFont(r), r.captionLineHeight);
        caption.layout(width());
        painter.setPen(r.caption);
        caption.draw(painter, QPointF(0, y));
    }
}

} // namespace ket
