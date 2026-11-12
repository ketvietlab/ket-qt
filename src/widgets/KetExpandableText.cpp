// SPDX-License-Identifier: MPL-2.0
#include "KetExpandableText.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetMentionTokens.h"

#include <QAbstractTextDocumentLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

} // namespace

ExpandableText::ExpandableText(QWidget* parent)
    : QWidget(parent), document_(new QTextDocument(this)),
      toggle_(new Button(tr("Show more"), ButtonVariant::Ghost, this)) {
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setMouseTracking(true);
    document_->setDocumentMargin(0);
    toggle_->setControlSize(ControlSize::Small);
    toggle_->hide();
    connect(toggle_, &QAbstractButton::clicked, this, [this] { setExpanded(!expanded_); });
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &ExpandableText::rebuild);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &ExpandableText::rebuild);
    rebuild();
}

void ExpandableText::setText(const QString& text) {
    if (text_ == text)
        return;
    text_ = text;
    focused_ = -1;
    rebuild();
}

int ExpandableText::foldedLines() const {
    return foldedLines_ > 0 ? foldedLines_ : recipe().foldedLines;
}

void ExpandableText::setFoldedLines(int lines) {
    foldedLines_ = qMax(0, lines);
    rebuild();
}

void ExpandableText::setExpanded(bool expanded) {
    if (expanded_ == expanded)
        return;
    expanded_ = expanded;
    rebuild();
    emit expandedChanged(expanded_);
}

void ExpandableText::setUnknownMentions(const QStringList& numbers) {
    unknown_ = numbers;
    rebuild();
}

void ExpandableText::setMentionTitle(const QString& number, const QString& title) {
    titles_.insert(number, title);
    syncAccessibility();
}

void ExpandableText::setFocusedMention(int index) {
    index = index < 0 || spans_.isEmpty() ? -1 : qMin(index, int(spans_.size()) - 1);
    if (focused_ == index)
        return;
    focused_ = index;
    // A mention behind the fold opens the text, so the focus stays visible.
    if (focused_ >= 0 && !expanded_ && lineOf(int(spans_.at(focused_).start)) >= foldedLines()) {
        setExpanded(true);
        return;
    }
    rebuild();
}

ExpandableTextRecipe ExpandableText::recipe() const {
    return expandableTextRecipe(Appearance::instance().theme());
}

qreal ExpandableText::lineBox() const {
    const ExpandableTextRecipe r = recipe();
    return qFloor(qRound(scaled(r.fontPixels)) * r.lineHeight);
}

void ExpandableText::rebuild() {
    const ExpandableTextRecipe r = recipe();
    spans_ = findAgentMentions(text_);
    if (focused_ >= spans_.size())
        focused_ = -1;
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.fontPixels)));
    document_->setDefaultFont(font);
    document_->setHtml(mentionLinkedHtml(text_, r, unknown_, hasFocus() ? focused_ : -1));
    QTextCursor cursor(document_);
    cursor.select(QTextCursor::Document);
    QTextBlockFormat format;
    format.setLineHeight(lineBox(), QTextBlockFormat::FixedHeight);
    cursor.mergeBlockFormat(format);
    setFocusPolicy(spans_.isEmpty() ? Qt::NoFocus : Qt::StrongFocus);
    toggle_->setText(expanded_ ? tr("Show less") : tr("Show more"));
    layoutDocument(width());
    toggle_->setVisible(isFoldable());
    const int top = textHeight(width()) + qRound(scaled(r.toggleGap));
    toggle_->setGeometry(0, top, toggle_->sizeHint().width(), toggle_->sizeHint().height());
    syncAccessibility();
    updateGeometry();
    update();
}

void ExpandableText::layoutDocument(int width) const {
    if (!qFuzzyCompare(document_->textWidth(), qreal(qMax(1, width))))
        document_->setTextWidth(qMax(1, width));
    (void)document_->size(); // lays the blocks out, so their lines can be counted
}

int ExpandableText::lineCount() const {
    layoutDocument(width());
    int lines = 0;
    for (QTextBlock block = document_->begin(); block.isValid(); block = block.next())
        lines += block.layout() ? block.layout()->lineCount() : 0;
    return lines;
}

int ExpandableText::visibleLineCount() const {
    return expanded_ ? lineCount() : qMin(lineCount(), foldedLines());
}

bool ExpandableText::isFoldable() const { return lineCount() > foldedLines(); }

int ExpandableText::lineOf(int position) const {
    layoutDocument(width());
    int before = 0;
    for (QTextBlock block = document_->begin(); block.isValid(); block = block.next()) {
        QTextLayout* layout = block.layout();
        if (!layout)
            continue;
        if (block.contains(position))
            return before + layout->lineForTextPosition(position - block.position()).lineNumber();
        before += layout->lineCount();
    }
    return before;
}

int ExpandableText::textHeight(int width) const {
    layoutDocument(width);
    int lines = 0;
    for (QTextBlock block = document_->begin(); block.isValid(); block = block.next())
        lines += block.layout() ? block.layout()->lineCount() : 0;
    const int shown = expanded_ ? lines : qMin(lines, foldedLines());
    return qRound(qMax(1, shown) * lineBox());
}

int ExpandableText::heightForWidth(int width) const {
    int height = textHeight(width);
    layoutDocument(width);
    int lines = 0;
    for (QTextBlock block = document_->begin(); block.isValid(); block = block.next())
        lines += block.layout() ? block.layout()->lineCount() : 0;
    if (lines > foldedLines())
        height += qRound(scaled(recipe().toggleGap)) + toggle_->sizeHint().height();
    layoutDocument(this->width());
    return height;
}

QSize ExpandableText::sizeHint() const {
    const int width = qMax(this->width(), qRound(scaled(320)));
    return {width, heightForWidth(width)};
}

QSize ExpandableText::minimumSizeHint() const { return {0, qRound(lineBox())}; }

void ExpandableText::syncAccessibility() {
    setAccessibleName(text_);
    if (hasFocus() && focused_ >= 0) {
        const QString& number = spans_.at(focused_).number;
        setAccessibleDescription(
            spokenMention(number, titles_.value(number), unknown_.contains(number)));
    } else {
        setAccessibleDescription(spokenMentions(spans_, titles_, unknown_));
    }
    toggle_->setAccessibleName(toggle_->text());
}

void ExpandableText::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    const QRect clip(0, 0, width(), textHeight(width()));
    painter.setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext context;
    context.clip = clip;
    context.palette = palette();
    context.palette.setColor(QPalette::Text, recipe().text);
    document_->documentLayout()->draw(&painter, context);
}

void ExpandableText::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    rebuild();
}

void ExpandableText::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        if (!spans_.isEmpty())
            setFocusedMention(focused_ <= 0 ? int(spans_.size()) - 1 : focused_ - 1);
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
        if (!spans_.isEmpty())
            setFocusedMention(focused_ + 1 >= spans_.size() ? 0 : focused_ + 1);
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (focused_ >= 0) {
            emit mentionActivated(spans_.at(focused_).number);
            return;
        }
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void ExpandableText::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    if (focused_ < 0 && !spans_.isEmpty())
        setFocusedMention(0);
    else
        rebuild();
}

void ExpandableText::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    rebuild();
}

QString ExpandableText::linkAt(const QPoint& point) const {
    if (point.y() >= textHeight(width()))
        return {};
    return mentionLinkNumber(document_->documentLayout()->anchorAt(point));
}

void ExpandableText::mouseMoveEvent(QMouseEvent* event) {
    setCursor(linkAt(event->position().toPoint()).isEmpty() ? Qt::ArrowCursor
                                                            : Qt::PointingHandCursor);
    QWidget::mouseMoveEvent(event);
}

void ExpandableText::mouseReleaseEvent(QMouseEvent* event) {
    const QString number = linkAt(event->position().toPoint());
    if (event->button() == Qt::LeftButton && !number.isEmpty()) {
        emit mentionActivated(number);
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

} // namespace ket
