// SPDX-License-Identifier: MPL-2.0
#include "KetMentionPicker.h"

#include "KetAppearance.h"
#include "KetShadow.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

} // namespace

MentionPicker::MentionPicker(QWidget* parent)
    : QWidget(parent), title_(tr("Conversations")), emptyText_(tr("No conversation matches")) {
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_NoSystemBackground);
    setAccessibleName(title_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void MentionPicker::setItems(const QList<MentionItem>& items) {
    items_ = items.mid(0, mentionPickerRecipe(theme()).maxItems);
    highlighted_ = items_.isEmpty() ? -1 : qBound(0, highlighted_, int(items_.size()) - 1);
    updateGeometry();
    update();
}

void MentionPicker::setTitle(const QString& title) {
    title_ = title;
    setAccessibleName(title);
    update();
}

void MentionPicker::setEmptyText(const QString& text) {
    emptyText_ = text;
    update();
}

void MentionPicker::setHighlightedIndex(int index) {
    if (items_.isEmpty())
        return;
    highlighted_ = qBound(0, index, int(items_.size()) - 1);
    update();
}

void MentionPicker::moveHighlight(int step) {
    if (items_.isEmpty())
        return;
    const int count = int(items_.size());
    highlighted_ = ((qMax(0, highlighted_) + step) % count + count) % count;
    update();
}

QFont MentionPicker::sized(qreal pixels, int weight, bool mono) const {
    QFont font = this->font();
    if (mono)
        if (const QString family = Tokens::availableFamily(u"font-mono"); !family.isEmpty())
            font.setFamilies({family});
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

QMargins MentionPicker::shadowMargins() const {
    const QMarginsF m = detail::shadowMargins(detail::scaled(mentionPickerRecipe(theme()).shadow));
    return {qCeil(m.left()), qCeil(m.top()), qCeil(m.right()), qCeil(m.bottom())};
}

qreal MentionPicker::titleHeight() const {
    const MentionPickerRecipe r = mentionPickerRecipe(theme());
    return QFontMetricsF(sized(r.titlePixels, r.titleWeight)).height() + scaled(r.titlePaddingTop) +
           scaled(r.titlePaddingBottom);
}

QSize MentionPicker::sizeHint() const {
    const MentionPickerRecipe r = mentionPickerRecipe(theme());
    const MentionOptionRecipe o = mentionOptionRecipe(false, theme());
    const qreal padding = scaled(r.padding);
    const qreal rows = items_.isEmpty() ? QFontMetricsF(sized(r.emptyPixels)).height() +
                                              2 * scaled(o.paddingInline)
                                        : items_.size() * scaled(o.height);
    const QMargins m = shadowMargins();
    return {qRound(scaled(r.maxWidth)) + m.left() + m.right(),
            qCeil(2 * padding + titleHeight() + rows) + m.top() + m.bottom()};
}

QRect MentionPicker::optionRect(int index) const {
    const MentionPickerRecipe r = mentionPickerRecipe(theme());
    const MentionOptionRecipe o = mentionOptionRecipe(false, theme());
    const QRectF surface = QRectF(rect()).marginsRemoved(QMarginsF(shadowMargins()));
    const qreal padding = scaled(r.padding);
    const qreal height = scaled(o.height);
    return QRectF(surface.left() + padding,
                  surface.top() + padding + titleHeight() + index * height,
                  surface.width() - 2 * padding, height)
        .toAlignedRect();
}

void MentionPicker::paintEvent(QPaintEvent*) {
    const MentionPickerRecipe r = mentionPickerRecipe(theme());
    const QRectF surface = QRectF(rect()).marginsRemoved(QMarginsF(shadowMargins()));
    const qreal radius = scaled(r.radius);
    const qreal padding = scaled(r.padding);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    detail::paintShadow(painter, surface, radius, detail::scaled(r.shadow));
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(surface.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

    const QFont titleFont = sized(r.titlePixels, r.titleWeight);
    painter.setFont(titleFont);
    painter.setPen(r.title);
    painter.drawText(QRectF(surface.left() + padding + scaled(r.titlePaddingInline),
                            surface.top() + padding + scaled(r.titlePaddingTop),
                            surface.width() - 2 * padding, QFontMetricsF(titleFont).height()),
                     Qt::AlignLeft | Qt::AlignVCenter, title_);

    if (items_.isEmpty()) {
        const MentionOptionRecipe o = mentionOptionRecipe(false, theme());
        const QFont font = sized(r.emptyPixels);
        painter.setFont(font);
        painter.setPen(r.empty);
        const qreal top = surface.top() + padding + titleHeight();
        painter.drawText(QRectF(surface.left() + padding + scaled(o.paddingInline), top,
                                surface.width() - 2 * padding,
                                QFontMetricsF(font).height() + 2 * scaled(o.paddingInline)),
                         Qt::AlignLeft | Qt::AlignVCenter, emptyText_);
        return;
    }

    for (int i = 0; i < items_.size(); ++i) {
        const MentionOptionRecipe o = mentionOptionRecipe(i == highlighted_, theme());
        const QRectF option(optionRect(i));
        const qreal optionRadius = scaled(o.radius);
        painter.setPen(Qt::NoPen);
        painter.setBrush(o.background);
        painter.drawRoundedRect(option, optionRadius, optionRadius);

        // Grid: number, title (elides), place.
        const qreal gap = scaled(o.spacing);
        qreal x = option.left() + scaled(o.paddingInline);
        qreal right = option.right() - scaled(o.paddingInline);
        const QFont number = sized(o.numberPixels, 400, true);
        const QString numberText = QStringLiteral("@agent/") + items_.at(i).number;
        const qreal numberWidth = QFontMetricsF(number).horizontalAdvance(numberText);
        painter.setFont(number);
        painter.setPen(o.number);
        painter.drawText(QRectF(x, option.top(), numberWidth, option.height()),
                         Qt::AlignLeft | Qt::AlignVCenter, numberText);
        x += numberWidth + gap;
        const QFont place = sized(o.placePixels);
        const qreal placeWidth = QFontMetricsF(place).horizontalAdvance(items_.at(i).place);
        painter.setFont(place);
        painter.setPen(o.place);
        painter.drawText(QRectF(right - placeWidth, option.top(), placeWidth, option.height()),
                         Qt::AlignRight | Qt::AlignVCenter, items_.at(i).place);
        right -= placeWidth + gap;
        const QFont title = sized(o.fontPixels);
        painter.setFont(title);
        painter.setPen(o.title);
        painter.drawText(QRectF(x, option.top(), qMax<qreal>(0, right - x), option.height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetricsF(title).elidedText(items_.at(i).title, Qt::ElideRight,
                                                         qMax<qreal>(0, right - x)));
    }
}

void MentionPicker::mouseMoveEvent(QMouseEvent* event) {
    for (int i = 0; i < items_.size(); ++i)
        if (optionRect(i).contains(event->position().toPoint()) && i != highlighted_) {
            highlighted_ = i;
            update();
        }
    QWidget::mouseMoveEvent(event);
}

void MentionPicker::mousePressEvent(QMouseEvent* event) {
    for (int i = 0; i < items_.size(); ++i)
        if (optionRect(i).contains(event->position().toPoint())) {
            emit activated(items_.at(i));
            event->accept();
            return;
        }
    event->accept();
}

UsageRing::UsageRing(QWidget* parent) : QAbstractButton(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setValue(0);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void UsageRing::setValue(qreal used) {
    value_ = qBound<qreal>(0, used, 1);
    const QString label = tr("Context %1% used").arg(qRound(value_ * 100));
    setAccessibleName(label);
    setToolTip(label);
    update();
}

void UsageRing::setPreviewHovered(bool hovered) {
    previewHovered_ = hovered;
    update();
}

QSize UsageRing::sizeHint() const {
    const int size = qRound(scaled(usageRingRecipe(value_, false, theme()).size));
    return {size, size};
}

void UsageRing::paintEvent(QPaintEvent*) {
    const UsageRingRecipe r = usageRingRecipe(value_, hovered_ || previewHovered_, theme());
    const qreal size = scaled(r.size);
    const qreal stroke = scaled(r.stroke);
    const QRectF ring = QRectF((width() - size) / 2, (height() - size) / 2, size, size)
                            .adjusted(stroke / 2, stroke / 2, -stroke / 2, -stroke / 2);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(r.track, stroke));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(ring);
    if (value_ > 0) {
        QPen arc(r.arc, stroke);
        arc.setCapStyle(Qt::FlatCap);
        painter.setPen(arc);
        // Clockwise from the top.
        painter.drawArc(ring, 90 * 16, -qRound(value_ * 360 * 16));
    }
    if (hasFocus()) {
        painter.setPen(QPen(Tokens::color(u"focus-border", theme()), 2));
        painter.drawEllipse(QRectF(rect()).adjusted(1, 1, -1, -1));
    }
}

void UsageRing::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void UsageRing::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

} // namespace ket
