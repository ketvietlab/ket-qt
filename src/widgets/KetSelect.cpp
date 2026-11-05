// SPDX-License-Identifier: MPL-2.0
#include "KetSelect.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>

namespace ket {

Select::Select(QWidget* parent) : QComboBox(parent) {
    setAttribute(Qt::WA_Hover);
    setFocusPolicy(Qt::StrongFocus);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Select::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

FieldRecipe Select::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{underMouse(), false, isEnabled(), hasFocus()});
    return fieldRecipe(state, false, Appearance::instance().theme());
}

QFont Select::fieldFont() const {
    QFont f = font();
    f.setPixelSize(qRound(Appearance::instance().scaled(recipe().fontPixels)));
    return f;
}

QSize Select::sizeHint() const {
    const FieldRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QFontMetrics metrics(fieldFont());
    int text = 0;
    for (int i = 0; i < count(); ++i)
        text = qMax(text, metrics.horizontalAdvance(itemText(i)));
    const qreal width =
        2 * appearance.scaled(r.paddingInline) + text + appearance.scaled(r.spacing + r.iconSize);
    return {qCeil(width), qRound(appearance.scaled(r.height))};
}

QSize Select::minimumSizeHint() const {
    const int height = qRound(Appearance::instance().scaled(recipe().height));
    return {3 * height, height};
}

void Select::paintEvent(QPaintEvent*) {
    const FieldRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal inset = r.borderWidth / 2.0;
    const qreal radius = appearance.scaled(r.radius);
    painter.setPen(QPen(r.border, r.borderWidth));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(inset, inset, -inset, -inset), radius, radius);

    const qreal padding = appearance.scaled(r.paddingInline);
    const qreal box = appearance.scaled(r.iconSize);
    painter.drawPixmap(QPointF(width() - padding - box, (height() - box) / 2.0),
                       Icons::pixmap(u"chevron-down", box, devicePixelRatioF(), r.icon));
    const QRectF textRect(padding, 0, width() - 2 * padding - box - appearance.scaled(r.spacing),
                          height());
    painter.setFont(fieldFont());
    painter.setPen(r.text);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(painter.font())
                         .elidedText(currentText(), Qt::ElideRight, int(textRect.width())));
}

} // namespace ket
