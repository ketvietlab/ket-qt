// SPDX-License-Identifier: MPL-2.0
#include "KetSuggestion.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

Suggestion::Suggestion(const QString& title, const QString& caption, const QString& iconCode,
                       QWidget* parent)
    : QAbstractButton(parent), caption_(caption), iconCode_(iconCode) {
    setText(title);
    setAccessibleDescription(caption);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void Suggestion::setCaption(const QString& caption) {
    caption_ = caption;
    setAccessibleDescription(caption);
    updateGeometry();
    update();
}

void Suggestion::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

void Suggestion::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

SuggestionRecipe Suggestion::recipe() const {
    const InteractionState state = preview_.value_or(InteractionState{
        hovered_, isDown(), isEnabled(), hasFocus() && focusPolicy() != Qt::NoFocus});
    return suggestionRecipe(state, Appearance::instance().theme());
}

QFont Suggestion::titleFont(const SuggestionRecipe& r) const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.titlePixels)));
    font.setWeight(QFont::Weight(r.titleWeight));
    return font;
}

QFont Suggestion::captionFont(const SuggestionRecipe& r) const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.captionPixels)));
    return font;
}

QSize Suggestion::sizeHint() const {
    const SuggestionRecipe r = recipe();
    const QFontMetricsF title(titleFont(r));
    const QFontMetricsF caption(captionFont(r));
    qreal textHeight = title.height();
    qreal textWidth = title.horizontalAdvance(text());
    if (!caption_.isEmpty()) {
        textHeight += scaled(r.captionGap) + caption.height();
        textWidth = qMax(textWidth, caption.horizontalAdvance(caption_));
    }
    const qreal width = 2 * scaled(r.paddingInline) + scaled(r.iconSize) + scaled(r.arrowSize) +
                        2 * scaled(r.spacing) + textWidth;
    const qreal height = 2 * scaled(r.paddingBlock) + qMax(textHeight, scaled(r.iconSize)) + 2;
    return {qCeil(width), qCeil(height)};
}

QSize Suggestion::minimumSizeHint() const {
    return {qRound(scaled(Tokens::px(u"space-16")) * 3), sizeHint().height()};
}

void Suggestion::paintEvent(QPaintEvent*) {
    const SuggestionRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    const QRectF box = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(box, radius, radius);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
    }
    const qreal gap = scaled(r.spacing);
    qreal x = scaled(r.paddingInline) + 1;
    const qreal icon = scaled(r.iconSize);
    if (!iconCode_.isEmpty()) {
        painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                           Icons::pixmap(iconCode_, icon, dpr, r.icon));
        x += icon + gap;
    }
    const qreal arrow = scaled(r.arrowSize);
    const qreal right = width() - scaled(r.paddingInline) - 1;
    painter.drawPixmap(QPointF(qRound(right - arrow), qRound((height() - arrow) / 2)),
                       Icons::pixmap(u"arrow-right", arrow, dpr, r.arrow));
    const qreal available = right - arrow - gap - x;
    const QFont title = titleFont(r);
    const QFont caption = captionFont(r);
    const QFontMetricsF titleMetrics(title);
    const QFontMetricsF captionMetrics(caption);
    qreal textHeight = titleMetrics.height();
    if (!caption_.isEmpty())
        textHeight += scaled(r.captionGap) + captionMetrics.height();
    qreal y = (height() - textHeight) / 2;
    painter.setFont(title);
    painter.setPen(r.title);
    painter.drawText(QRectF(x, y, available, titleMetrics.height()), Qt::AlignLeft,
                     titleMetrics.elidedText(text(), Qt::ElideRight, available));
    if (!caption_.isEmpty()) {
        y += titleMetrics.height() + scaled(r.captionGap);
        painter.setFont(caption);
        painter.setPen(r.caption);
        painter.drawText(QRectF(x, y, available, captionMetrics.height()), Qt::AlignLeft,
                         captionMetrics.elidedText(caption_, Qt::ElideRight, available));
    }
}

void Suggestion::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void Suggestion::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

} // namespace ket
