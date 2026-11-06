// SPDX-License-Identifier: MPL-2.0
#include "KetButton.h"

#include "KetAppearance.h"

#include <QEnterEvent>
#include <QPainter>
#include <QPixmap>
#include <QtMath>

namespace ket {
namespace {

QFont recipeFont(const QFont& base, const ButtonRecipe& recipe) {
    QFont font = base;
    font.setPixelSize(qRound(Appearance::instance().scaled(recipe.fontPixels)));
    font.setWeight(QFont::Weight(recipe.fontWeight));
    return font;
}

// The icon's alpha filled with `color`; icons are monochrome glyphs (Lucide in KetPlus).
QPixmap tinted(const QIcon& icon, int size, qreal dpr, const QColor& color) {
    QPixmap pixmap = icon.pixmap(QSize(size, size), dpr);
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    return pixmap;
}

} // namespace

Button::Button(const QString& text, ButtonVariant variant, QWidget* parent)
    : QAbstractButton(parent), variant_(variant) {
    setText(text);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Button::setVariant(ButtonVariant variant) {
    if (variant_ == variant)
        return;
    variant_ = variant;
    update();
}

void Button::setControlSize(ControlSize size) {
    if (size_ == size)
        return;
    size_ = size;
    updateGeometry();
    update();
}

void Button::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

InteractionState Button::interactionState() const {
    return preview_.value_or(InteractionState{hovered_, isDown(), isEnabled(),
                                              hasFocus() && focusPolicy() != Qt::NoFocus});
}

ButtonRecipe Button::recipe() const {
    return buttonRecipe(variant_, size_, interactionState(), Appearance::instance().theme());
}

QSize Button::sizeHint() const {
    const ButtonRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QFontMetricsF metrics(recipeFont(font(), r));
    qreal width = 2 * appearance.scaled(r.paddingInline) + qCeil(metrics.horizontalAdvance(text()));
    if (!icon().isNull())
        width +=
            appearance.scaled(r.iconSize) + (text().isEmpty() ? 0 : appearance.scaled(r.spacing));
    const qreal height = appearance.scaled(r.height);
    return {qMax(qRound(width), qRound(height)), qRound(height)};
}

void Button::paintEvent(QPaintEvent*) {
    const ButtonRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const QRectF box = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal radius = appearance.scaled(r.radius);
    painter.setPen(r.border.alpha() ? QPen(r.border, 1) : Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(box, radius, radius);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
    }

    const QFont textFont = recipeFont(font(), r);
    const QFontMetrics metrics(textFont);
    const qreal iconSize = icon().isNull() ? 0 : appearance.scaled(r.iconSize);
    const qreal gap = iconSize > 0 && !text().isEmpty() ? appearance.scaled(r.spacing) : 0;
    const qreal contentWidth = iconSize + gap + metrics.horizontalAdvance(text());
    qreal x = (width() - contentWidth) / 2.0;
    if (iconSize > 0) {
        const int side = qRound(iconSize);
        painter.drawPixmap(QRect(qRound(x), qRound((height() - iconSize) / 2.0), side, side),
                           tinted(icon(), side, devicePixelRatioF(), r.text));
        x += iconSize + gap;
    }
    painter.setFont(textFont);
    painter.setPen(r.text);
    painter.drawText(QRectF(x, 0, width() - x, height()), Qt::AlignLeft | Qt::AlignVCenter, text());
}

IconButton::IconButton(const QIcon& icon, const QString& label, QWidget* parent)
    : Button({}, ButtonVariant::Ghost, parent) {
    setIcon(icon);
    setLabel(label);
}

void IconButton::setLabel(const QString& label) {
    setAccessibleName(label);
    setToolTip(label);
}

void IconButton::setGlyphSize(qreal size) {
    glyphSize_ = size;
    update();
}

ButtonRecipe IconButton::recipe() const {
    ButtonRecipe r =
        iconButtonRecipe(controlSize(), interactionState(), Appearance::instance().theme());
    if (glyphSize_ > 0)
        r.iconSize = glyphSize_;
    return r;
}

QSize IconButton::sizeHint() const {
    const int side = qRound(Appearance::instance().scaled(recipe().height));
    return {side, side};
}

void Button::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void Button::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

} // namespace ket
