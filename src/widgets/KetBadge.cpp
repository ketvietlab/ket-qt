// SPDX-License-Identifier: MPL-2.0
#include "KetBadge.h"

#include "KetAppearance.h"

#include <QPainter>
#include <QtMath>

namespace ket {

Badge::Badge(const QString& text, BadgeTone tone, QWidget* parent)
    : QWidget(parent), text_(text), tone_(tone) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Badge::setText(const QString& text) {
    if (text_ == text)
        return;
    text_ = text;
    updateGeometry();
    update();
}

void Badge::setTone(BadgeTone tone) {
    if (tone_ == tone)
        return;
    tone_ = tone;
    update();
}

BadgeRecipe Badge::recipe() const { return badgeRecipe(tone_, Appearance::instance().theme()); }

QFont Badge::badgeFont() const {
    const BadgeRecipe r = recipe();
    QFont result = font();
    result.setPixelSize(qRound(Appearance::instance().scaled(r.fontPixels)));
    result.setWeight(QFont::Weight(r.fontWeight));
    return result;
}

QSize Badge::sizeHint() const {
    const BadgeRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const int width = qCeil(QFontMetricsF(badgeFont()).horizontalAdvance(text_)) +
                      2 * qRound(appearance.scaled(r.paddingInline));
    return {width, qRound(appearance.scaled(r.height))};
}

void Badge::paintEvent(QPaintEvent*) {
    const BadgeRecipe r = recipe();
    const qreal radius = Appearance::instance().scaled(r.radius);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter.setFont(badgeFont());
    painter.setPen(r.text);
    painter.drawText(rect(), Qt::AlignCenter, text_);
}

} // namespace ket
