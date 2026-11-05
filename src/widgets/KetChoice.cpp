// SPDX-License-Identifier: MPL-2.0
#include "KetChoice.h"

#include "KetAppearance.h"
#include "KetToggle.h"

#include <QPainter>
#include <QTextOption>
#include <QtMath>

namespace ket {

Choice::Choice(const QString& title, const QString& description, ToggleKind kind, QWidget* parent)
    : QAbstractButton(parent), description_(description), kind_(kind) {
    setText(title);
    setCheckable(true);
    setAutoExclusive(kind == ToggleKind::Radio);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Choice::setDescription(const QString& description) {
    description_ = description;
    updateGeometry();
    update();
}

void Choice::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

InteractionState Choice::state() const {
    return preview_.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
}

ChoiceRecipe Choice::recipe() const {
    return choiceRecipe(isChecked(), state(), Appearance::instance().theme());
}

ToggleRecipe Choice::toggle() const {
    // The card shows focus; the indicator inside does not repeat it.
    InteractionState inner = state();
    inner.focused = false;
    return toggleRecipe(kind_, isChecked(), inner, Appearance::instance().theme());
}

QFont Choice::titleFont() const {
    const ChoiceRecipe r = recipe();
    QFont f = font();
    f.setPixelSize(qRound(Appearance::instance().scaled(r.titlePixels)));
    f.setWeight(QFont::Weight(r.titleWeight));
    return f;
}

QFont Choice::descriptionFont() const {
    QFont f = font();
    f.setPixelSize(qRound(Appearance::instance().scaled(recipe().descriptionPixels)));
    return f;
}

qreal Choice::textLeft() const {
    const Appearance& appearance = Appearance::instance();
    const ChoiceRecipe r = recipe();
    return appearance.scaled(r.padding) + appearance.scaled(toggle().size) +
           appearance.scaled(r.spacing);
}

int Choice::heightForWidth(int width) const {
    const Appearance& appearance = Appearance::instance();
    const ChoiceRecipe r = recipe();
    const qreal padding = appearance.scaled(r.padding);
    const int textWidth = qMax(1, int(width - textLeft() - padding));
    const QFontMetrics title(titleFont());
    qreal height = title.boundingRect(0, 0, textWidth, 10000, Qt::TextWordWrap, text()).height();
    if (!description_.isEmpty())
        height += QFontMetrics(descriptionFont())
                      .boundingRect(0, 0, textWidth, 10000, Qt::TextWordWrap, description_)
                      .height();
    height = qMax(height, appearance.scaled(toggle().size));
    return qCeil(height + 2 * padding);
}

QSize Choice::sizeHint() const {
    const int width = qCeil(textLeft() + Appearance::instance().scaled(recipe().padding) +
                            qMax(QFontMetrics(titleFont()).horizontalAdvance(text()),
                                 QFontMetrics(descriptionFont()).horizontalAdvance(description_)));
    return {width, heightForWidth(width)};
}

QSize Choice::minimumSizeHint() const {
    const int width = qCeil(textLeft() + Appearance::instance().scaled(recipe().padding)) + 40;
    return {width, heightForWidth(width)};
}

void Choice::paintEvent(QPaintEvent*) {
    const ChoiceRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = appearance.scaled(r.radius);
    painter.setPen(QPen(r.focusRing.alpha() ? r.focusRing : r.border, r.focusRing.alpha() ? 2 : 1));
    painter.setBrush(r.background);
    const qreal inset = r.focusRing.alpha() ? 1 : 0.5;
    painter.drawRoundedRect(QRectF(rect()).adjusted(inset, inset, -inset, -inset), radius, radius);

    const qreal padding = appearance.scaled(r.padding);
    const qreal box = appearance.scaled(toggle().size);
    Toggle::paintIndicator(painter, QRectF(padding, (height() - box) / 2.0, box, box), kind_,
                           toggle(), devicePixelRatioF());

    const qreal left = textLeft();
    const qreal textWidth = width() - left - padding;
    const QFontMetrics titleMetrics(titleFont());
    const QRect titleBounds =
        titleMetrics.boundingRect(0, 0, int(textWidth), 10000, Qt::TextWordWrap, text());
    qreal descriptionHeight = 0;
    if (!description_.isEmpty())
        descriptionHeight =
            QFontMetrics(descriptionFont())
                .boundingRect(0, 0, int(textWidth), 10000, Qt::TextWordWrap, description_)
                .height();
    qreal y = (height() - titleBounds.height() - descriptionHeight) / 2.0;
    painter.setFont(titleFont());
    painter.setPen(r.title);
    painter.drawText(QRectF(left, y, textWidth, titleBounds.height()), Qt::TextWordWrap, text());
    if (!description_.isEmpty()) {
        y += titleBounds.height();
        painter.setFont(descriptionFont());
        painter.setPen(r.description);
        painter.drawText(QRectF(left, y, textWidth, descriptionHeight), Qt::TextWordWrap,
                         description_);
    }
}

} // namespace ket
