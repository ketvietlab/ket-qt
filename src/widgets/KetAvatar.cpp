// SPDX-License-Identifier: MPL-2.0
#include "KetAvatar.h"

#include "KetAppearance.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {

Avatar::Avatar(const QString& name, QWidget* parent) : QWidget(parent), name_(name) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(name);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Avatar::setName(const QString& name) {
    name_ = name;
    setAccessibleName(name);
    update();
}

QString Avatar::initials() const { return initialsOf(name_); }

QString Avatar::initialsOf(const QString& name) {
    const QStringList words = name.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    QString initials;
    for (const QString& word : words) {
        if (initials.size() == 2)
            break;
        initials.append(word.at(0).toUpper());
    }
    if (words.size() == 1 && words.first().size() > 1)
        initials.append(words.first().at(1).toUpper());
    return initials;
}

void Avatar::setImage(const QPixmap& image) {
    image_ = image;
    update();
}

void Avatar::setDiameter(qreal diameter) {
    diameter_ = diameter;
    updateGeometry();
    update();
}

QSize Avatar::sizeHint() const {
    const qreal size = diameter_ > 0 ? diameter_ : avatarRecipe(Theme::Light).size;
    const int side = qRound(Appearance::instance().scaled(size));
    return {side, side};
}

void Avatar::paintEvent(QPaintEvent*) {
    const AvatarRecipe r = avatarRecipe(Appearance::instance().theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    const qreal side = qMin(width(), height());
    const QRectF circle((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawEllipse(circle);
    if (!image_.isNull()) {
        painter.save();
        QPainterPath clip;
        clip.addEllipse(circle);
        painter.setClipPath(clip);
        const QSizeF source = image_.deviceIndependentSize();
        const qreal scale = qMax(side / source.width(), side / source.height());
        const QSizeF drawn = source * scale;
        painter.drawPixmap(
            QRectF(circle.center() - QPointF(drawn.width(), drawn.height()) / 2, drawn), image_,
            QRectF(QPointF(), image_.size()));
        painter.restore();
    } else {
        QFont font = this->font();
        // The recipe's size is for the default diameter; other diameters scale it.
        const qreal ratio = side / Appearance::instance().scaled(r.size);
        font.setPixelSize(qMax(1, qRound(Appearance::instance().scaled(r.fontPixels) * ratio)));
        font.setWeight(QFont::Weight(r.fontWeight));
        painter.setFont(font);
        painter.setPen(r.text);
        painter.drawText(circle, Qt::AlignCenter, initials());
    }
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(circle.adjusted(0.5, 0.5, -0.5, -0.5));
}

} // namespace ket
