// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QFont>
#include <QPainter>
#include <QString>
#include <QTextLayout>
#include <QtMath>

namespace ket::detail {

// Plain wrapped text with a CSS line height (a multiple of the font size), which QLabel
// lacks. Each line box is `lineHeight × font size` tall (whole pixels, rounded down, as the
// Quick layer's Text.FixedHeight boxes are) with the glyphs centred in it.
class Paragraph final {
  public:
    Paragraph(const QString& text, const QFont& font, qreal lineHeight)
        : layout_(text, font), lineHeight_(lineHeight) {
        QTextOption option;
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        layout_.setTextOption(option);
    }

    // Lays the text out at `width`; returns the natural width used (<= width).
    qreal layout(qreal width) {
        const QFontMetricsF metrics(layout_.font());
        const qreal box = lineBox();
        const qreal lead = (box - metrics.height()) / 2;
        qreal used = 0;
        qreal y = 0;
        layout_.beginLayout();
        for (QTextLine line = layout_.createLine(); line.isValid(); line = layout_.createLine()) {
            line.setLineWidth(width);
            line.setPosition(QPointF(0, y + lead));
            used = qMax(used, line.naturalTextWidth());
            y += box;
        }
        layout_.endLayout();
        height_ = y;
        return used;
    }

    [[nodiscard]] qreal height() const { return height_; }
    [[nodiscard]] qreal lineBox() const {
        const QFont& font = layout_.font();
        const qreal size = font.pixelSize() > 0 ? font.pixelSize() : font.pointSizeF();
        return qFloor(size * lineHeight_);
    }
    void setFormats(const QList<QTextLayout::FormatRange>& formats) { layout_.setFormats(formats); }
    void draw(QPainter& painter, const QPointF& origin) const { layout_.draw(&painter, origin); }

  private:
    QTextLayout layout_;
    qreal lineHeight_;
    qreal height_ = 0;
};

} // namespace ket::detail
