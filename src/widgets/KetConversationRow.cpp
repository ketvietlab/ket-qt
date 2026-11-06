// SPDX-License-Identifier: MPL-2.0
#include "KetConversationRow.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetProfile.h"

#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

} // namespace

ConversationRow::ConversationRow(const QString& title, QWidget* parent) : QAbstractButton(parent) {
    setText(title);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    frame_.setInterval(16);
    connect(&frame_, &QTimer::timeout, this, qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
    clock_.start();
}

void ConversationRow::setNumber(const QString& number) {
    number_ = number;
    update();
}

void ConversationRow::setTags(const QList<ConversationTag>& tags) {
    tags_ = tags;
    update();
}

void ConversationRow::setStatus(ConversationStatus status) {
    status_ = status;
    updateSpinner();
    update();
}

void ConversationRow::setStatusLabel(const QString& label) {
    statusLabel_ = label;
    update();
}

QString ConversationRow::statusLabel() const {
    return statusLabel_.isEmpty() ? conversationStatusLabel(status_) : statusLabel_;
}

void ConversationRow::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

ConversationRowRecipe ConversationRow::recipe() const {
    const InteractionState state = preview_.value_or(InteractionState{
        hovered_, isDown(), isEnabled(), hasFocus() && focusPolicy() != Qt::NoFocus});
    return conversationRowRecipe(isChecked(), state, theme());
}

QFont ConversationRow::font(qreal pixels, int weight, bool mono) const {
    QFont result = QAbstractButton::font();
    if (mono)
        if (const QString family = Profile::current().monoFamily(); !family.isEmpty())
            result.setFamilies({family});
    result.setPixelSize(qRound(scaled(pixels)));
    result.setWeight(QFont::Weight(weight));
    return result;
}

QSize ConversationRow::sizeHint() const {
    const ConversationRowRecipe r = recipe();
    return {qRound(scaled(r.paddingInline) * 2 +
                   QFontMetricsF(font(r.titlePixels)).horizontalAdvance(text()) +
                   scaled(r.spacing) + scaled(r.indicatorSize)),
            qRound(scaled(r.height))};
}

QSize ConversationRow::minimumSizeHint() const {
    const ConversationRowRecipe r = recipe();
    return {qRound(scaled(r.paddingInline) * 4 + scaled(r.indicatorSize)),
            qRound(scaled(r.height))};
}

QList<ConversationRow::Chip> ConversationRow::layoutChips(qreal left, qreal right) const {
    const ConversationRowRecipe r = recipe();
    const TagRecipe more = tagRecipe(QColor(), true, false, theme());
    const QFontMetricsF metrics(font(more.fontPixels, more.fontWeight));
    const qreal gap = scaled(r.tagGap);
    const qreal height = scaled(more.height);
    const auto chipWidth = [&](const QString& text, const TagRecipe& tag) {
        return qMin(metrics.horizontalAdvance(text) + 2 * scaled(tag.paddingInline),
                    scaled(r.tagMaxWidth));
    };
    // Whole chips only, keeping room for +N while any tag is left out.
    QList<Chip> chips;
    qreal x = left;
    for (int i = 0; i < tags_.size(); ++i) {
        const TagRecipe tag = tagRecipe(tags_.at(i).color, false, false, theme());
        const qreal width = chipWidth(tags_.at(i).label, tag);
        const int remaining = int(tags_.size()) - i - 1;
        const qreal reserve =
            remaining ? gap + chipWidth(QStringLiteral("+%1").arg(remaining), more) : 0;
        if (x + width + reserve > right) {
            const QString label = QStringLiteral("+%1").arg(tags_.size() - i);
            chips.append({label, QRectF(x, 0, chipWidth(label, more), height), more, true});
            return chips;
        }
        chips.append({tags_.at(i).label, QRectF(x, 0, width, height), tag, false});
        x += width + gap;
    }
    return chips;
}

qreal ConversationRow::tagsLeft(const ConversationRowRecipe& r) const {
    qreal x = scaled(r.paddingInline);
    if (!number_.isEmpty())
        x += QFontMetricsF(font(r.numberPixels, 400, true)).horizontalAdvance(number_) +
             scaled(r.metaGap);
    return x;
}

qreal ConversationRow::tagsRight(const ConversationRowRecipe& r) const {
    return width() - scaled(r.paddingInline) - scaled(r.indicatorSize) - scaled(r.spacing);
}

int ConversationRow::visibleTagCount() const {
    const ConversationRowRecipe r = recipe();
    int count = 0;
    for (const Chip& chip : layoutChips(tagsLeft(r), tagsRight(r)))
        count += chip.overflow ? 0 : 1;
    return count;
}

void ConversationRow::paintEvent(QPaintEvent*) {
    const ConversationRowRecipe r = recipe();
    const ConversationStatusRecipe s = conversationStatusRecipe(status_, theme());
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()), radius, radius);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius - 1, radius - 1);
    }

    const qreal left = scaled(r.paddingInline);
    const qreal top = scaled(r.paddingBlock);
    const qreal indicator = scaled(r.indicatorSize);
    const qreal indicatorLeft = width() - left - indicator;
    const qreal right = tagsRight(r);

    // The status icon; a working icon turns once per spin period.
    const QPixmap icon = Icons::pixmap(s.icon, indicator, dpr, s.iconColor);
    const QPointF iconOrigin(qRound(indicatorLeft), qRound(top + scaled(r.indicatorTop)));
    if (s.spinning && s.spinMs > 0) {
        painter.save();
        painter.translate(iconOrigin + QPointF(indicator / 2, indicator / 2));
        painter.rotate(360.0 * qreal(clock_.elapsed() % s.spinMs) / s.spinMs);
        painter.drawPixmap(QPointF(-indicator / 2, -indicator / 2), icon);
        painter.restore();
    } else {
        painter.drawPixmap(iconOrigin, icon);
    }

    // Line 1: the title.
    const QFont titleFont = font(r.titlePixels, r.titleWeight);
    const qreal titleBox = scaled(r.titleLineHeight);
    painter.setFont(titleFont);
    painter.setPen(r.title);
    painter.drawText(QRectF(left, top, right - left, titleBox), Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetricsF(titleFont).elidedText(text(), Qt::ElideRight, right - left));

    // Line 2: the number, then tags or the status label.
    const qreal metaTop = top + titleBox + scaled(r.lineGap);
    const qreal metaBox = scaled(r.metaHeight);
    qreal x = left;
    if (!number_.isEmpty()) {
        const QFont numberFont = font(r.numberPixels, 400, true);
        const qreal width = QFontMetricsF(numberFont).horizontalAdvance(number_);
        painter.setFont(numberFont);
        painter.setPen(r.number);
        painter.drawText(QRectF(x, metaTop, width, metaBox), Qt::AlignLeft | Qt::AlignVCenter,
                         number_);
        x += width + scaled(r.metaGap);
    }
    if (!tags_.isEmpty()) {
        const TagRecipe more = tagRecipe(QColor(), true, false, theme());
        const QFont tagFont = font(more.fontPixels, more.fontWeight);
        painter.setFont(tagFont);
        for (Chip chip : layoutChips(x, right)) {
            chip.rect.moveTop(metaTop + (metaBox - chip.rect.height()) / 2);
            const qreal chipRadius = scaled(chip.recipe.radius);
            painter.setPen(Qt::NoPen);
            painter.setBrush(chip.recipe.background);
            painter.drawRoundedRect(chip.rect, chipRadius, chipRadius);
            painter.setPen(chip.recipe.text);
            const QRectF label = chip.rect.adjusted(scaled(chip.recipe.paddingInline), 0,
                                                    -scaled(chip.recipe.paddingInline), 0);
            painter.drawText(
                label, Qt::AlignCenter,
                QFontMetricsF(tagFont).elidedText(chip.text, Qt::ElideRight, label.width()));
        }
    } else {
        const QFont metaFont = font(r.metaPixels, r.metaWeight);
        painter.setFont(metaFont);
        painter.setPen(s.labelColor);
        painter.drawText(
            QRectF(x, metaTop, right - x, metaBox), Qt::AlignLeft | Qt::AlignVCenter,
            QFontMetricsF(metaFont).elidedText(statusLabel(), Qt::ElideRight, right - x));
    }
}

void ConversationRow::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ConversationRow::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void ConversationRow::showEvent(QShowEvent* event) {
    updateSpinner();
    QAbstractButton::showEvent(event);
}

void ConversationRow::hideEvent(QHideEvent* event) {
    frame_.stop();
    QAbstractButton::hideEvent(event);
}

void ConversationRow::updateSpinner() {
    if (isVisible() && conversationStatusRecipe(status_, theme()).spinning)
        frame_.start();
    else
        frame_.stop();
}

} // namespace ket
