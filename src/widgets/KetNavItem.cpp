// SPDX-License-Identifier: MPL-2.0
#include "KetNavItem.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QFocusEvent>
#include <QPainter>
#include <QtMath>

namespace ket {

NavItem::NavItem(const QString& text, const QString& iconCode, QWidget* parent)
    : QAbstractButton(parent), iconCode_(iconCode) {
    setText(text);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void NavItem::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

void NavItem::setCount(std::optional<int> count) {
    count_ = count;
    updateGeometry();
    update();
}

void NavItem::setShortcutText(const QString& text) {
    shortcutText_ = text;
    updateGeometry();
    update();
}

void NavItem::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

NavItemRecipe NavItem::recipe() const {
    const InteractionState state = preview_.value_or(
        InteractionState{hovered_, isDown(), isEnabled(),
                         visualFocus_ && hasFocus() && focusPolicy() != Qt::NoFocus});
    return navItemRecipe(isChecked(), state, Appearance::instance().theme());
}

QFont NavItem::labelFont(const NavItemRecipe& r) const {
    QFont result = font();
    result.setPixelSize(qRound(Appearance::instance().scaled(r.fontPixels)));
    result.setWeight(QFont::Weight(r.fontWeight));
    return result;
}

QFont NavItem::trailingFont(const NavItemRecipe& r) const {
    QFont result = font();
    result.setPixelSize(
        qRound(Appearance::instance().scaled(count_ ? r.countFontPixels : r.kbdFontPixels)));
    return result;
}

QString NavItem::trailingText() const { return count_ ? QString::number(*count_) : shortcutText_; }

QSize NavItem::sizeHint() const {
    const NavItemRecipe r = navItemRecipe(true, {}, Appearance::instance().theme());
    const Appearance& a = Appearance::instance();
    qreal width =
        2 * a.scaled(r.paddingInline) + QFontMetricsF(labelFont(r)).horizontalAdvance(text());
    if (!iconCode_.isEmpty())
        width += a.scaled(r.iconSize) + a.scaled(r.spacing);
    if (const QString trailing = trailingText(); !trailing.isEmpty()) {
        width += a.scaled(r.spacing) + QFontMetricsF(trailingFont(r)).horizontalAdvance(trailing);
        if (count_)
            width += 2 * a.scaled(r.countPadding);
    }
    return {qCeil(width), qRound(a.scaled(r.height))};
}

QSize NavItem::minimumSizeHint() const {
    const NavItemRecipe r = recipe();
    const Appearance& a = Appearance::instance();
    return {qRound(2 * a.scaled(r.paddingInline) + a.scaled(r.iconSize)), sizeHint().height()};
}

void NavItem::paintEvent(QPaintEvent*) {
    const NavItemRecipe r = recipe();
    const Appearance& a = Appearance::instance();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = a.scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()), radius, radius);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius - 1, radius - 1);
    }

    qreal x = a.scaled(r.paddingInline);
    qreal right = width() - a.scaled(r.paddingInline);
    if (!iconCode_.isEmpty()) {
        const qreal box = a.scaled(r.iconSize);
        painter.drawPixmap(QPointF(qRound(x), qRound((height() - box) / 2)),
                           Icons::pixmap(iconCode_, box, devicePixelRatioF(), r.text));
        x += box + a.scaled(r.spacing);
    }
    if (const QString trailing = trailingText(); !trailing.isEmpty()) {
        const QFont font = trailingFont(r);
        const qreal textWidth = QFontMetricsF(font).horizontalAdvance(trailing);
        painter.setFont(font);
        if (count_) {
            const qreal w = textWidth + 2 * a.scaled(r.countPadding);
            const qreal h = a.scaled(r.countHeight);
            const QRectF pill(right - w, (height() - h) / 2, w, h);
            const qreal pillRadius = a.scaled(r.countRadius);
            painter.setPen(QPen(r.countBorder, 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(pill.adjusted(0.5, 0.5, -0.5, -0.5), pillRadius, pillRadius);
            painter.setPen(r.countText);
            painter.drawText(pill, Qt::AlignCenter, trailing);
            right -= w + a.scaled(r.spacing);
        } else {
            painter.setPen(r.kbdText);
            painter.drawText(QRectF(right - textWidth, 0, textWidth, height()),
                             Qt::AlignRight | Qt::AlignVCenter, trailing);
            right -= textWidth + a.scaled(r.spacing);
        }
    }
    const QFont font = labelFont(r);
    painter.setFont(font);
    painter.setPen(r.text);
    const QRectF label(x, 0, qMax<qreal>(0, right - x), height());
    painter.drawText(label, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetricsF(font).elidedText(text(), Qt::ElideRight, label.width()));
}

void NavItem::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void NavItem::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void NavItem::focusInEvent(QFocusEvent* event) {
    switch (event->reason()) {
    case Qt::TabFocusReason:
    case Qt::BacktabFocusReason:
    case Qt::ShortcutFocusReason:
        visualFocus_ = true;
        break;
    case Qt::ActiveWindowFocusReason:
    case Qt::PopupFocusReason:
        break; // focus returns to the row; keep how it was shown
    default:
        visualFocus_ = false;
        break;
    }
    QAbstractButton::focusInEvent(event);
}

void NavItem::focusOutEvent(QFocusEvent* event) {
    if (event->reason() != Qt::ActiveWindowFocusReason && event->reason() != Qt::PopupFocusReason)
        visualFocus_ = false;
    QAbstractButton::focusOutEvent(event);
}

Kbd::Kbd(const QString& text, QWidget* parent) : QWidget(parent), text_(text) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Kbd::setText(const QString& text) {
    text_ = text;
    updateGeometry();
    update();
}

QFont Kbd::kbdFont() const {
    QFont result = font();
    result.setPixelSize(qRound(
        Appearance::instance().scaled(kbdRecipe(Appearance::instance().theme()).fontPixels)));
    return result;
}

QSize Kbd::sizeHint() const {
    const QFontMetricsF metrics(kbdFont());
    return {qCeil(metrics.horizontalAdvance(text_)), qCeil(metrics.height())};
}

void Kbd::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setFont(kbdFont());
    painter.setPen(kbdRecipe(Appearance::instance().theme()).text);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, text_);
}

} // namespace ket
