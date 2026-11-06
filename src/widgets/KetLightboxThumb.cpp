// SPDX-License-Identifier: MPL-2.0
#include "KetLightboxThumb.h"

#include "KetAppearance.h"

#include <QPainter>
#include <QPainterPath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

LightboxThumb::LightboxThumb(const QPixmap& image, const QString& description, QWidget* parent)
    : QAbstractButton(parent), image_(image) {
    setAccessibleName(description);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
    setImage(image);
}

void LightboxThumb::setImage(const QPixmap& image) {
    image_ = image;
    // `cursor: zoom-in` has no Qt shape; the pointing hand says it opens.
    setCursor(image_.isNull() ? Qt::ArrowCursor : Qt::PointingHandCursor);
    setEnabled(!image_.isNull());
    update();
}

void LightboxThumb::setThumbSize(LightboxThumbSize size) {
    size_ = size;
    updateGeometry();
    update();
}

void LightboxThumb::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

LightboxThumbRecipe LightboxThumb::recipe() const {
    const bool focused = preview_ ? preview_->focused : hasFocus();
    return lightboxThumbRecipe(size_, focused, Appearance::instance().theme());
}

QSize LightboxThumb::sizeHint() const {
    const int side = qRound(scaled(recipe().size));
    return {side, side};
}

void LightboxThumb::paintEvent(QPaintEvent*) {
    const LightboxThumbRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    const qreal radius = scaled(r.radius);
    const QRectF frame = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath shape;
    shape.addRoundedRect(frame, radius, radius);
    painter.fillPath(shape, r.background);
    if (image_.isNull()) {
        QFont label = font();
        label.setPixelSize(qRound(scaled(r.textPixels)));
        painter.setFont(label);
        painter.setPen(r.text);
        painter.drawText(rect().adjusted(4, 4, -4, -4), Qt::AlignCenter | Qt::TextWordWrap, text());
    } else {
        // `object-fit: cover`: the middle square of the picture.
        const QSizeF source = image_.size();
        const qreal side = qMin(source.width(), source.height());
        const QRectF crop((source.width() - side) / 2, (source.height() - side) / 2, side, side);
        painter.save();
        painter.setClipPath(shape);
        painter.drawPixmap(QRectF(rect()), image_, crop);
        painter.restore();
    }
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(shape);
    if (r.focusRing.alpha()) {
        // Inside the bounds, where Qt Widgets can paint the outline.
        const qreal width = scaled(r.focusWidth);
        const qreal inset = width / 2;
        painter.setPen(QPen(r.focusRing, width));
        painter.drawRoundedRect(QRectF(rect()).adjusted(inset, inset, -inset, -inset),
                                qMax<qreal>(0, radius - inset), qMax<qreal>(0, radius - inset));
    }
}

} // namespace ket
