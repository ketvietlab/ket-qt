// SPDX-License-Identifier: MPL-2.0
#include "KetAttachmentChip.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

AttachmentChip::AttachmentChip(const QString& name, const QString& iconCode, QWidget* parent)
    : QWidget(parent), name_(name), iconCode_(iconCode) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    setAccessibleName(name);
    setAccessibleDescription(tr("Press Delete to remove"));
    setToolTip(name);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void AttachmentChip::setName(const QString& name) {
    name_ = name;
    setAccessibleName(name);
    setToolTip(name);
    updateGeometry();
    update();
}

void AttachmentChip::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

AttachmentChipRecipe AttachmentChip::recipe() const {
    return attachmentChipRecipe(removeHovered_, Appearance::instance().theme());
}

QFont AttachmentChip::textFont() const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(recipe().fontPixels)));
    return font;
}

QSize AttachmentChip::sizeHint() const {
    const AttachmentChipRecipe r = recipe();
    const qreal icon = scaled(r.iconSize);
    const qreal width = 2 * scaled(r.paddingInline) + 2 * (icon + scaled(r.spacing)) +
                        QFontMetricsF(textFont()).horizontalAdvance(name_);
    return {qMin(qCeil(width), qRound(scaled(r.maxWidth))), qRound(scaled(r.height))};
}

QRect AttachmentChip::removeRect() const {
    const AttachmentChipRecipe r = recipe();
    const int icon = qRound(scaled(r.iconSize));
    const int right = width() - qRound(scaled(r.paddingInline));
    return {right - icon, (height() - icon) / 2, icon, icon};
}

void AttachmentChip::paintEvent(QPaintEvent*) {
    const AttachmentChipRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()), radius, radius);
    if (hasFocus()) {
        painter.setPen(QPen(Tokens::color(u"focus-border", Appearance::instance().theme()), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
    }
    const qreal icon = scaled(r.iconSize);
    const qreal gap = scaled(r.spacing);
    qreal x = scaled(r.paddingInline);
    painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                       Icons::pixmap(iconCode_, icon, dpr, r.text));
    x += icon + gap;
    const QRect remove = removeRect();
    painter.drawPixmap(remove.topLeft(), Icons::pixmap(u"close", icon, dpr, r.remove));
    const qreal available = remove.left() - gap - x;
    const QFont font = textFont();
    painter.setFont(font);
    painter.setPen(r.text);
    painter.drawText(QRectF(x, 0, available, height()), Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetricsF(font).elidedText(name_, Qt::ElideMiddle, available));
}

void AttachmentChip::mouseMoveEvent(QMouseEvent* event) {
    const bool over = removeRect().adjusted(-2, -2, 2, 2).contains(event->position().toPoint());
    if (over != removeHovered_) {
        removeHovered_ = over;
        setCursor(over ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void AttachmentChip::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton &&
        removeRect().adjusted(-2, -2, 2, 2).contains(event->position().toPoint())) {
        emit removeRequested();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void AttachmentChip::leaveEvent(QEvent* event) {
    removeHovered_ = false;
    update();
    QWidget::leaveEvent(event);
}

void AttachmentChip::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        emit removeRequested();
        return;
    }
    QWidget::keyPressEvent(event);
}

} // namespace ket
