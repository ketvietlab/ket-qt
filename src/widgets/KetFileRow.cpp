// SPDX-License-Identifier: MPL-2.0
#include "KetFileRow.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

FileRow::FileRow(const QString& name, Kind kind, int depth, QWidget* parent)
    : QAbstractButton(parent), kind_(kind), depth_(qMax(0, depth)) {
    setText(name);
    setCheckable(kind == Kind::Folder);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void FileRow::setDepth(int depth) {
    depth_ = qMax(0, depth);
    updateGeometry();
    update();
}

void FileRow::setIconCode(const QString& code) {
    iconCode_ = code;
    update();
}

void FileRow::setStatus(const QString& status) {
    status_ = status;
    setAccessibleDescription(status);
    update();
}

void FileRow::setActive(bool active) {
    active_ = active;
    update();
}

void FileRow::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

FileRowRecipe FileRow::recipe() const {
    const InteractionState state = preview_.value_or(InteractionState{
        hovered_, isDown(), isEnabled(), hasFocus() && focusPolicy() != Qt::NoFocus});
    return fileRowRecipe(active_, state, Appearance::instance().theme());
}

qreal FileRow::indent() const {
    const FileRowRecipe r = recipe();
    // `.indent-N`: 12 at the root, then 37, 49, 61…
    return depth_ == 0 ? scaled(r.paddingInline)
                       : scaled(r.indentStart) + depth_ * scaled(r.indentStep);
}

QFont FileRow::textFont() const {
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(recipe().fontPixels)));
    return font;
}

QSize FileRow::sizeHint() const {
    const FileRowRecipe r = recipe();
    qreal width = indent() + scaled(r.iconSize) + scaled(r.spacing) +
                  QFontMetricsF(textFont()).horizontalAdvance(text()) + scaled(r.paddingInline);
    if (kind_ == Kind::Folder)
        width += scaled(r.chevronSize) + scaled(r.folderSpacing);
    return {qCeil(width), qRound(scaled(kind_ == Kind::Folder ? r.folderHeight : r.height))};
}

void FileRow::paintEvent(QPaintEvent*) {
    const FileRowRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    painter.fillRect(rect(), r.background);
    if (r.focusRing.alpha()) {
        painter.setPen(QPen(r.focusRing, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRectF(rect()).adjusted(1, 1, -1, -1));
    }
    qreal x = indent();
    const qreal gap = scaled(kind_ == Kind::Folder ? r.folderSpacing : r.spacing);
    if (kind_ == Kind::Folder) {
        const qreal chevron = scaled(r.chevronSize);
        painter.drawPixmap(
            QPointF(qRound(x), qRound((height() - chevron) / 2)),
            Icons::pixmap(isChecked() ? u"chevron-down" : u"chevron-right", chevron, dpr, r.text));
        x += chevron + gap;
    }
    const qreal icon = scaled(r.iconSize);
    const QString code = !iconCode_.isEmpty()  ? iconCode_
                         : kind_ == Kind::File ? QStringLiteral("file")
                         : isChecked()         ? QStringLiteral("folder-open")
                                               : QStringLiteral("folder");
    painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                       Icons::pixmap(code, icon, dpr, r.text));
    x += icon + gap;
    qreal right = width() - scaled(r.paddingInline);
    if (!status_.isEmpty()) {
        QFont small = this->font();
        small.setPixelSize(qRound(scaled(r.statusPixels)));
        const qreal statusWidth = QFontMetricsF(small).horizontalAdvance(status_);
        painter.setFont(small);
        painter.setPen(r.status);
        painter.drawText(QRectF(right - statusWidth, 0, statusWidth, height()),
                         Qt::AlignRight | Qt::AlignVCenter, status_);
        right -= statusWidth + gap;
    }
    const QFont font = textFont();
    painter.setFont(font);
    painter.setPen(r.text);
    painter.drawText(QRectF(x, 0, right - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetricsF(font).elidedText(text(), Qt::ElideMiddle, right - x));
}

void FileRow::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    update();
    QAbstractButton::enterEvent(event);
}

void FileRow::leaveEvent(QEvent* event) {
    hovered_ = false;
    update();
    QAbstractButton::leaveEvent(event);
}

} // namespace ket
