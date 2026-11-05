// SPDX-License-Identifier: MPL-2.0
#include "KetSegmentedControl.h"

#include "KetAppearance.h"
#include "KetShadow.h"

#include <QAccessible>
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>

namespace ket {

SegmentedControl::SegmentedControl(const QStringList& options, QWidget* parent)
    : QWidget(parent), options_(options) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(tr("Options"));
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void SegmentedControl::setOptions(const QStringList& options) {
    options_ = options;
    current_ = qBound(0, current_, qMax(0, int(options_.size()) - 1));
    updateGeometry();
    update();
}

void SegmentedControl::setCurrentIndex(int index) {
    if (index < 0 || index >= options_.size() || index == current_)
        return;
    current_ = index;
    update();
    emit currentChanged(index);
}

void SegmentedControl::setExpanding(bool expanding) {
    expanding_ = expanding;
    setSizePolicy(expanding ? QSizePolicy::Expanding : QSizePolicy::Fixed, QSizePolicy::Fixed);
    update();
}

void SegmentedControl::setPreviewHovered(std::optional<int> index) {
    previewHovered_ = index;
    update();
}

QFont SegmentedControl::segmentFont() const {
    QFont result = font();
    const SegmentRecipe r = segmentRecipe(false, {}, Appearance::instance().theme());
    result.setPixelSize(qRound(Appearance::instance().scaled(r.fontPixels)));
    return result;
}

qreal SegmentedControl::naturalWidth(int index) const {
    const SegmentRecipe r = segmentRecipe(false, {}, Appearance::instance().theme());
    return qCeil(QFontMetricsF(segmentFont()).horizontalAdvance(options_.at(index)) +
                 2 * Appearance::instance().scaled(r.paddingInline));
}

QSize SegmentedControl::sizeHint() const {
    const Appearance& a = Appearance::instance();
    const SegmentedRecipe track = segmentedRecipe(a.theme());
    const SegmentRecipe r = segmentRecipe(false, {}, a.theme());
    qreal width = 2 * a.scaled(track.padding);
    for (int i = 0; i < options_.size(); ++i)
        width += naturalWidth(i) + (i ? a.scaled(track.spacing) : 0);
    const qreal height = 2 * a.scaled(track.padding) + 2 * a.scaled(r.paddingBlock) +
                         QFontMetricsF(segmentFont()).height();
    return {qCeil(width), qCeil(height)};
}

QRect SegmentedControl::segmentRect(int index) const {
    const Appearance& a = Appearance::instance();
    const SegmentedRecipe track = segmentedRecipe(a.theme());
    const qreal padding = a.scaled(track.padding);
    const qreal spacing = a.scaled(track.spacing);
    const qreal inner = width() - 2 * padding - spacing * qMax(0, int(options_.size()) - 1);
    qreal x = padding;
    for (int i = 0; i < index; ++i)
        x += (expanding_ ? inner / options_.size() : naturalWidth(i)) + spacing;
    const qreal w = expanding_ ? inner / options_.size() : naturalWidth(index);
    return QRectF(x, padding, w, height() - 2 * padding).toAlignedRect();
}

int SegmentedControl::segmentAt(const QPoint& position) const {
    for (int i = 0; i < options_.size(); ++i)
        if (segmentRect(i).contains(position))
            return i;
    return -1;
}

void SegmentedControl::paintEvent(QPaintEvent*) {
    const Appearance& a = Appearance::instance();
    const SegmentedRecipe track = segmentedRecipe(a.theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (!isEnabled())
        painter.setOpacity(segmentRecipe(false, {false, false, false, false}, a.theme()).opacity);
    const qreal trackRadius = a.scaled(track.radius);
    painter.setPen(QPen(track.border, 1));
    painter.setBrush(track.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), trackRadius,
                            trackRadius);

    const int hovered = previewHovered_.value_or(hovered_);
    painter.setFont(segmentFont());
    for (int i = 0; i < options_.size(); ++i) {
        const InteractionState state{i == hovered, false, isEnabled(), hasFocus() && i == current_};
        const SegmentRecipe r = segmentRecipe(i == current_, state, a.theme());
        const QRectF segment(segmentRect(i));
        const qreal radius = a.scaled(r.radius);
        if (r.background.alpha()) {
            detail::paintShadow(painter, segment, radius, detail::scaled(r.shadow));
            painter.setPen(Qt::NoPen);
            painter.setBrush(r.background);
            painter.drawRoundedRect(segment, radius, radius);
        }
        if (r.focusRing.alpha()) {
            painter.setPen(QPen(r.focusRing, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(segment.adjusted(1, 1, -1, -1), radius, radius);
        }
        painter.setPen(r.text);
        painter.drawText(segment, Qt::AlignCenter, options_.at(i));
    }
}

void SegmentedControl::mouseMoveEvent(QMouseEvent* event) {
    const int index = segmentAt(event->position().toPoint());
    if (index != hovered_) {
        hovered_ = index;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void SegmentedControl::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (const int index = segmentAt(event->position().toPoint()); index >= 0)
            setCurrentIndex(index);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void SegmentedControl::leaveEvent(QEvent* event) {
    hovered_ = -1;
    update();
    QWidget::leaveEvent(event);
}

void SegmentedControl::keyPressEvent(QKeyEvent* event) {
    const int last = int(options_.size()) - 1;
    const bool rtl = layoutDirection() == Qt::RightToLeft;
    switch (event->key()) {
    case Qt::Key_Left:
        setCurrentIndex(qMax(0, current_ + (rtl ? 1 : -1)));
        break;
    case Qt::Key_Right:
        setCurrentIndex(qMin(last, current_ + (rtl ? -1 : 1)));
        break;
    case Qt::Key_Home:
        setCurrentIndex(0);
        break;
    case Qt::Key_End:
        setCurrentIndex(last);
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    event->accept();
}

} // namespace ket
