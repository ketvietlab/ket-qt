// SPDX-License-Identifier: MPL-2.0
#include "KetIcon.h"

#include "KetAppearance.h"
#include "KetTokens.h"

#include <QPainter>

namespace ket {

Icon::Icon(const QString& code, qreal size, QWidget* parent)
    : QWidget(parent), code_(code), size_(size) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(code);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Icon::setCode(const QString& code) {
    code_ = code;
    update();
}

void Icon::setIconSize(qreal size) {
    size_ = size;
    updateGeometry();
    update();
}

void Icon::setWeight(IconWeight weight) {
    weight_ = weight;
    update();
}

QColor Icon::color() const {
    return color_.value_or(Tokens::color(u"text-main", Appearance::instance().theme()));
}

void Icon::setColor(std::optional<QColor> color) {
    color_ = color;
    update();
}

QSize Icon::sizeHint() const {
    const int box = qRound(Appearance::instance().scaled(size_));
    return {box, box};
}

void Icon::paintEvent(QPaintEvent*) {
    const qreal box = Appearance::instance().scaled(size_);
    QPainter painter(this);
    painter.drawPixmap(QPointF((width() - box) / 2.0, (height() - box) / 2.0),
                       Icons::pixmap(code_, box, devicePixelRatioF(), color(), weight_));
}

} // namespace ket
