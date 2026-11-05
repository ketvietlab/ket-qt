// SPDX-License-Identifier: MPL-2.0
#include "KetDiffView.h"

#include "KetAppearance.h"
#include "KetProfile.h"

#include <QFontDatabase>
#include <QPainter>
#include <QRegularExpression>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

DiffRecipe recipe() { return diffRecipe(Appearance::instance().theme()); }

} // namespace

DiffView::DiffView(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAccessibleName(tr("Diff"));
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void DiffView::setLines(const QList<DiffLine>& lines) {
    lines_ = lines;
    updateGeometry();
    update();
}

QList<DiffLine> DiffView::parseUnified(const QString& diff) {
    static const QRegularExpression hunk(QStringLiteral(R"(^@@ -(\d+)(?:,\d+)? \+(\d+))"));
    QList<DiffLine> lines;
    int oldLine = 0;
    int newLine = 0;
    bool inHunk = false;
    for (const QString& line : diff.split(QLatin1Char('\n'))) {
        if (const QRegularExpressionMatch match = hunk.match(line); match.hasMatch()) {
            oldLine = match.captured(1).toInt();
            newLine = match.captured(2).toInt();
            inHunk = true;
            lines.append({DiffLine::Kind::Hunk, 0, line});
            continue;
        }
        if (!inHunk || line.startsWith(QLatin1String("\\ ")))
            continue; // file headers, "\ No newline at end of file"
        if (line.startsWith(QLatin1Char('+')))
            lines.append({DiffLine::Kind::Add, newLine++, line.mid(1)});
        else if (line.startsWith(QLatin1Char('-')))
            lines.append({DiffLine::Kind::Remove, oldLine++, line.mid(1)});
        else if (line.startsWith(QLatin1Char(' ')) || line.isEmpty()) {
            lines.append({DiffLine::Kind::Context, newLine++, line.mid(1)});
            ++oldLine;
        } else {
            inHunk = false; // the next file's header
        }
    }
    while (!lines.isEmpty() && lines.constLast().kind == DiffLine::Kind::Context &&
           lines.constLast().text.isEmpty())
        lines.removeLast();
    return lines;
}

QFont DiffView::monoFont() const {
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    if (const QString family = Profile::current().monoFamily(); !family.isEmpty())
        font.setFamily(family);
    font.setPixelSize(qRound(scaled(recipe().fontPixels)));
    return font;
}

qreal DiffView::rowHeight(const DiffLine& line) const {
    const DiffRecipe r = recipe();
    if (line.kind == DiffLine::Kind::Hunk)
        return 2 * scaled(r.hunkPaddingBlock) + QFontMetricsF(monoFont()).height();
    return scaled(r.lineHeight);
}

QSize DiffView::sizeHint() const {
    const DiffRecipe r = recipe();
    const QFontMetricsF metrics(monoFont());
    qreal height = 0;
    qreal width = 0;
    bool inLines = false;
    for (const DiffLine& line : lines_) {
        const bool hunk = line.kind == DiffLine::Kind::Hunk;
        if ((hunk && inLines) || (!hunk && !inLines))
            height += scaled(r.paddingBlock);
        inLines = !hunk;
        height += rowHeight(line);
        const qreal lead = hunk ? 2 * scaled(r.hunkPaddingInline)
                                : scaled(r.numberWidth) + scaled(r.signWidth) + scaled(r.codeGap) +
                                      scaled(r.paddingEnd);
        width = qMax(width, lead + metrics.horizontalAdvance(line.text));
    }
    if (inLines)
        height += scaled(r.paddingBlock);
    return {qCeil(width), qCeil(height)};
}

void DiffView::paintEvent(QPaintEvent*) {
    const DiffRecipe r = recipe();
    QPainter painter(this);
    painter.fillRect(rect(), r.background);
    const QFont font = monoFont();
    const QFontMetricsF metrics(font);
    painter.setFont(font);
    qreal y = 0;
    bool inLines = false;
    for (const DiffLine& line : lines_) {
        const bool hunk = line.kind == DiffLine::Kind::Hunk;
        // `.diff-lines` pads each run of lines above and below.
        if ((hunk && inLines) || (!hunk && !inLines))
            y += scaled(r.paddingBlock);
        inLines = !hunk;
        const qreal height = rowHeight(line);
        const QRectF row(0, y, width(), height);
        if (hunk) {
            painter.fillRect(row, r.hunkBackground);
            painter.setPen(r.hunkText);
            painter.drawText(row.adjusted(scaled(r.hunkPaddingInline), 0, 0, 0),
                             Qt::AlignLeft | Qt::AlignVCenter, line.text);
            y += height;
            continue;
        }
        const bool add = line.kind == DiffLine::Kind::Add;
        const bool remove = line.kind == DiffLine::Kind::Remove;
        if (add || remove)
            painter.fillRect(row, add ? r.addBackground : r.removeBackground);
        const qreal numberWidth = scaled(r.numberWidth);
        if (line.number > 0) {
            painter.setPen(r.number);
            painter.drawText(QRectF(0, y, numberWidth - scaled(r.numberGap), height),
                             Qt::AlignRight | Qt::AlignVCenter, QString::number(line.number));
        }
        const qreal signWidth = scaled(r.signWidth);
        painter.setPen(add ? r.addSign : remove ? r.removeSign : r.sign);
        painter.drawText(QRectF(numberWidth, y, signWidth, height), Qt::AlignCenter,
                         add      ? QStringLiteral("+")
                         : remove ? QStringLiteral("−")
                                  : QString());
        painter.setPen(r.text);
        const qreal x = numberWidth + signWidth + scaled(r.codeGap);
        painter.drawText(QRectF(x, y, width() - x, height), Qt::AlignLeft | Qt::AlignVCenter,
                         line.text);
        y += height;
    }
}

} // namespace ket
