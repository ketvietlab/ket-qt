// SPDX-License-Identifier: MPL-2.0
#include "KetChangedFiles.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

ChangedFilesRecipe recipe() { return changedFilesRecipe(Appearance::instance().theme()); }

QFont sized(const QWidget* widget, qreal pixels, int weight = QFont::Normal) {
    QFont font = widget->font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

// Paints "+added −removed" right-aligned at `right`; returns the left edge.
qreal paintCounts(QPainter& painter, const QWidget* widget, int added, int removed, qreal right,
                  qreal height, const ChangedFilesRecipe& r) {
    const QFont font = sized(widget, r.textPixels);
    const QFontMetricsF metrics(font);
    painter.setFont(font);
    const QString minus = QStringLiteral("−%1").arg(removed);
    const QString plus = QStringLiteral("+%1").arg(added);
    const qreal minusWidth = metrics.horizontalAdvance(minus);
    const qreal plusWidth = metrics.horizontalAdvance(plus);
    const qreal gap = scaled(Tokens::px(u"space-1-5"));
    painter.setPen(r.removed);
    painter.drawText(QRectF(right - minusWidth, 0, minusWidth, height),
                     Qt::AlignRight | Qt::AlignVCenter, minus);
    right -= minusWidth + gap;
    painter.setPen(r.added);
    painter.drawText(QRectF(right - plusWidth, 0, plusWidth, height),
                     Qt::AlignRight | Qt::AlignVCenter, plus);
    return right - plusWidth;
}

} // namespace

// `.changed-heading`: an icon, the title and the totals.
class ChangedHeading final : public QWidget {
  public:
    explicit ChangedHeading(ChangedFiles* owner) : QWidget(owner), owner_(owner) {}

    [[nodiscard]] QSize sizeHint() const override {
        const ChangedFilesRecipe r = recipe();
        return {0, qCeil(2 * scaled(r.paddingBlock) +
                         QFontMetricsF(sized(this, r.titlePixels, r.titleWeight)).height())};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const ChangedFilesRecipe r = recipe();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        // Only the top corners follow the card's radius (inside its 1 px border).
        const qreal radius = qMax<qreal>(0, scaled(r.radius) - 1);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.headingBackground);
        painter.setClipRect(rect());
        painter.drawRoundedRect(QRectF(0, 0, width(), height() + radius), radius, radius);
        painter.setClipping(false);
        qreal x = scaled(r.paddingInline);
        const qreal icon = scaled(r.iconSize);
        painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                           Icons::pixmap(u"edit", icon, devicePixelRatioF(), r.icon));
        x += icon + scaled(r.headingGap);
        int added = 0;
        int removed = 0;
        for (const ChangedFileRow* row : owner_->rows_) {
            added += row->added();
            removed += row->removed();
        }
        const qreal right = paintCounts(painter, this, added, removed,
                                        width() - scaled(r.paddingInline), height(), r) -
                            scaled(r.headingGap);
        const QFont title = sized(this, r.titlePixels, r.titleWeight);
        painter.setFont(title);
        painter.setPen(r.heading);
        painter.drawText(
            QRectF(x, 0, right - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
            QFontMetricsF(title).elidedText(owner_->title(), Qt::ElideRight, right - x));
    }

  private:
    ChangedFiles* owner_;
};

ChangedFileRow::ChangedFileRow(const QString& name, const QString& path, int added, int removed,
                               QWidget* parent)
    : QAbstractButton(parent), path_(path), added_(added), removed_(removed) {
    setText(name);
    setAccessibleDescription(tr("%1, %2 added, %3 removed").arg(path).arg(added).arg(removed));
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize ChangedFileRow::sizeHint() const {
    const ChangedFilesRecipe r = recipe();
    const qreal name = qFloor(scaled(r.titlePixels) * r.lineHeight);
    const qreal path = path_.isEmpty() ? 0 : qFloor(scaled(r.pathPixels) * r.lineHeight);
    return {0, qCeil(2 * scaled(r.rowPaddingBlock) + name + path) + 1}; // 1: the top hairline
}

void ChangedFileRow::paintEvent(QPaintEvent*) {
    const ChangedFilesRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (underMouse() || isDown())
        painter.fillRect(rect(), r.rowHover);
    painter.fillRect(QRect(0, 0, width(), 1), r.border);
    if (hasFocus()) {
        painter.setPen(QPen(Tokens::color(u"focus-border", Appearance::instance().theme()), 2));
        painter.drawRect(QRectF(rect()).adjusted(1, 2, -1, -1));
    }
    qreal x = scaled(r.paddingInline);
    const qreal icon = scaled(r.iconSize);
    painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                       Icons::pixmap(u"file-code", icon, dpr, r.icon));
    x += icon + scaled(r.rowGap);
    qreal right = width() - scaled(r.paddingInline);
    const qreal arrow = scaled(r.arrowSize);
    painter.drawPixmap(QPointF(qRound(right - arrow), qRound((height() - arrow) / 2)),
                       Icons::pixmap(u"arrow-up-right", arrow, dpr, r.icon));
    right -= arrow + scaled(r.rowGap);
    right = paintCounts(painter, this, added_, removed_, right, height(), r) - scaled(r.rowGap);
    const qreal nameBox = qFloor(scaled(r.titlePixels) * r.lineHeight);
    const qreal pathBox = path_.isEmpty() ? 0 : qFloor(scaled(r.pathPixels) * r.lineHeight);
    qreal y = (height() + 1 - nameBox - pathBox) / 2;
    const QFont name = sized(this, r.titlePixels, r.nameWeight);
    painter.setFont(name);
    painter.setPen(r.name);
    painter.drawText(QRectF(x, y, right - x, nameBox), Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetricsF(name).elidedText(text(), Qt::ElideMiddle, right - x));
    if (!path_.isEmpty()) {
        const QFont small = sized(this, r.pathPixels);
        painter.setFont(small);
        painter.setPen(r.path);
        painter.drawText(QRectF(x, y + nameBox, right - x, pathBox),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetricsF(small).elidedText(path_, Qt::ElideLeft, right - x));
    }
}

void ChangedFileRow::enterEvent(QEnterEvent* event) {
    update();
    QAbstractButton::enterEvent(event);
}

void ChangedFileRow::leaveEvent(QEvent* event) {
    update();
    QAbstractButton::leaveEvent(event);
}

ChangedFiles::ChangedFiles(QWidget* parent)
    : QWidget(parent), heading_(new ChangedHeading(this)), rowsLayout_(new QVBoxLayout),
      footerHost_(new QWidget(this)), footer_(new QHBoxLayout) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rowsLayout_->setSpacing(0);
    rowsLayout_->setContentsMargins(0, 0, 0, 0);
    auto* footerRow = new QHBoxLayout(footerHost_);
    footerRow->addLayout(footer_);
    footerRow->addStretch(1);
    auto* column = new QVBoxLayout(this);
    column->setContentsMargins(1, 1, 1, 1);
    column->setSpacing(0);
    column->addWidget(heading_);
    column->addLayout(rowsLayout_);
    column->addWidget(footerHost_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &ChangedFiles::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &ChangedFiles::applyRecipe);
    applyRecipe();
}

ChangedFileRow* ChangedFiles::addFile(const QString& name, const QString& path, int added,
                                      int removed) {
    auto* row = new ChangedFileRow(name, path, added, removed, this);
    const int index = int(rows_.size());
    rows_.append(row);
    rowsLayout_->addWidget(row);
    connect(row, &QAbstractButton::clicked, this, [this, index] { emit fileActivated(index); });
    heading_->update();
    return row;
}

QString ChangedFiles::title() const {
    if (!title_.isEmpty())
        return title_;
    const int count = int(rows_.size());
    return count == 1 ? tr("1 file changed") : tr("%1 files changed").arg(count);
}

void ChangedFiles::setTitle(const QString& title) {
    title_ = title;
    heading_->update();
}

void ChangedFiles::setNote(const QString& note) {
    note_ = note;
    footerHost_->update();
    update();
}

void ChangedFiles::applyRecipe() {
    const ChangedFilesRecipe r = recipe();
    const int inline_ = qRound(scaled(r.paddingInline));
    const int block = qRound(scaled(r.footerPaddingBlock));
    footerHost_->layout()->setContentsMargins(inline_, block + 1, inline_, block);
    footer_->setSpacing(qRound(scaled(r.headingGap)));
    heading_->updateGeometry();
    for (ChangedFileRow* row : std::as_const(rows_))
        row->updateGeometry();
    update();
}

void ChangedFiles::paintEvent(QPaintEvent*) {
    const ChangedFilesRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    const QRectF box = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(box, radius, radius);
    const QRect footer = footerHost_->geometry();
    painter.fillRect(QRect(footer.left(), footer.top(), footer.width(), 1), r.border);
    if (!note_.isEmpty()) {
        const QFont small = sized(this, r.pathPixels);
        painter.setFont(small);
        painter.setPen(r.footer);
        painter.drawText(QRectF(footer).adjusted(0, 0, -scaled(r.paddingInline), 0),
                         Qt::AlignRight | Qt::AlignVCenter, note_);
    }
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(box, radius, radius);
}

} // namespace ket
