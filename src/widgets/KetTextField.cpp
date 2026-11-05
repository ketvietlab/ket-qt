// SPDX-License-Identifier: MPL-2.0
#include "KetTextField.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QEvent>
#include <QPainter>

namespace ket {

TextField::TextField(QWidget* parent) : QLineEdit(parent) {
    setFrame(false);
    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &TextField::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, [this] {
        applyRecipe();
        updateGeometry();
    });
    applyRecipe();
}

void TextField::setLeadingIcon(const QString& code) {
    leadingIcon_ = code;
    applyRecipe();
}

void TextField::setInvalid(bool invalid) {
    invalid_ = invalid;
    update();
}

void TextField::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    applyRecipe();
}

FieldRecipe TextField::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{underMouse(), false, isEnabled(), hasFocus()});
    return fieldRecipe(state, invalid_, Appearance::instance().theme());
}

void TextField::applyRecipe() {
    const FieldRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    QFont f = font();
    f.setPixelSize(qRound(appearance.scaled(r.fontPixels)));
    setFont(f);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::transparent);
    // QLineEdit draws the text itself, so the disabled fade goes into the colours.
    const auto faded = [&r](QColor color) {
        color.setAlphaF(float(color.alphaF() * r.opacity));
        return color;
    };
    for (const QPalette::ColorGroup group :
         {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        p.setColor(group, QPalette::Text, faded(r.text));
        p.setColor(group, QPalette::PlaceholderText, faded(r.placeholder));
    }
    setPalette(p);
    const int padding = qRound(appearance.scaled(r.paddingInline));
    const int icon = leadingIcon_.isEmpty() ? 0 : qRound(appearance.scaled(r.iconSize + r.spacing));
    // QLineEdit adds a 2 px inner margin of its own on each side.
    setTextMargins(padding - 2 + icon, 0, padding - 2, 0);
    update();
}

QSize TextField::sizeHint() const {
    const int height = qRound(Appearance::instance().scaled(recipe().height));
    return {qMax(QLineEdit::sizeHint().width(), 10 * height), height};
}

QSize TextField::minimumSizeHint() const {
    const int height = qRound(Appearance::instance().scaled(recipe().height));
    return {3 * height, height};
}

void TextField::paintEvent(QPaintEvent* event) {
    const FieldRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        const qreal inset = r.borderWidth / 2.0;
        const qreal radius = appearance.scaled(r.radius);
        painter.setPen(QPen(r.border, r.borderWidth));
        painter.setBrush(r.background);
        painter.drawRoundedRect(QRectF(rect()).adjusted(inset, inset, -inset, -inset), radius,
                                radius);
        if (!leadingIcon_.isEmpty()) {
            const qreal box = appearance.scaled(r.iconSize);
            painter.drawPixmap(QPointF(appearance.scaled(r.paddingInline), (height() - box) / 2.0),
                               Icons::pixmap(leadingIcon_, box, devicePixelRatioF(), r.icon));
        }
    }
    QLineEdit::paintEvent(event);
}

void TextField::changeEvent(QEvent* event) {
    QLineEdit::changeEvent(event);
    if (event->type() == QEvent::EnabledChange)
        applyRecipe();
}

} // namespace ket
