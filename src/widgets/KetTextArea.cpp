// SPDX-License-Identifier: MPL-2.0
#include "KetTextArea.h"

#include "KetAppearance.h"
#include "KetMarkdown.h"
#include "KetMentionTokens.h"

#include <QAbstractTextDocumentLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextEdit>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

// Cmd/Ctrl+Return belongs to the window that hosts the field, such as its main action.
bool isHostKey(const QKeyEvent* key) {
    return (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) &&
           (key->modifiers() & (Qt::ControlModifier | Qt::MetaModifier));
}

class FieldEdit final : public QTextEdit {
  public:
    explicit FieldEdit(QWidget* parent) : QTextEdit(parent) {}
    void setPadding(int inline_, int block) { setViewportMargins(inline_, block, inline_, block); }
    // QTextEdit clips its placeholder to the first line; this one wraps on the field's line
    // boxes and starts where the caret does.
    void setHint(const QString& hint, qreal lineBox) {
        hint_ = hint;
        lineBox_ = lineBox;
        viewport()->update();
    }
    [[nodiscard]] QString hint() const { return hint_; }

  protected:
    bool event(QEvent* event) override {
        if (event->type() == QEvent::ShortcutOverride &&
            isHostKey(static_cast<QKeyEvent*>(event))) {
            event->ignore(); // let a window shortcut take it
            return true;
        }
        return QTextEdit::event(event);
    }
    void keyPressEvent(QKeyEvent* event) override {
        if (isHostKey(event)) {
            event->ignore(); // propagates to the parent widgets
            return;
        }
        QTextEdit::keyPressEvent(event);
    }
    void paintEvent(QPaintEvent* event) override {
        QTextEdit::paintEvent(event);
        if (hint_.isEmpty() || !document()->isEmpty())
            return;
        QTextDocument hint;
        hint.setDocumentMargin(0);
        hint.setDefaultFont(font());
        hint.setPlainText(hint_);
        hint.setTextWidth(viewport()->width());
        QTextCursor cursor(&hint);
        cursor.select(QTextCursor::Document);
        QTextBlockFormat format;
        format.setLineHeight(lineBox_, QTextBlockFormat::FixedHeight);
        cursor.mergeBlockFormat(format);
        QPainter painter(viewport());
        painter.setClipRect(viewport()->rect());
        QAbstractTextDocumentLayout::PaintContext context;
        context.palette = palette();
        context.palette.setColor(QPalette::Text, palette().color(QPalette::PlaceholderText));
        hint.documentLayout()->draw(&painter, context);
    }

  private:
    QString hint_;
    qreal lineBox_ = 0;
};

} // namespace

TextArea::TextArea(QWidget* parent)
    : QWidget(parent), editor_(new FieldEdit(this)),
      tokens_(new MentionTokens(editor_->document())) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_Hover);
    setFocusProxy(editor_);
    editor_->setAcceptRichText(false);
    editor_->setTabChangesFocus(true);
    editor_->setFrameShape(QFrame::NoFrame);
    editor_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    editor_->setAttribute(Qt::WA_MacShowFocusRect, false);
    editor_->viewport()->setAutoFillBackground(false);
    editor_->setAutoFillBackground(false);
    editor_->installEventFilter(this);
    connect(editor_, &QTextEdit::textChanged, this, [this] {
        fitEditor();
        syncAccessibility();
        update(); // the count and the invalid border
        emit textChanged();
    });
    connect(editor_, &QTextEdit::cursorPositionChanged, this, [this] {
        QTextCursor cursor = editor_->textCursor();
        const int position = cursor.position();
        const int snapped = tokens_->snap(position, lastPosition_);
        if (snapped != position) {
            if (cursor.hasSelection()) {
                const int anchor = cursor.anchor();
                cursor.setPosition(anchor);
                cursor.setPosition(snapped, QTextCursor::KeepAnchor);
            } else {
                cursor.setPosition(snapped);
            }
            editor_->setTextCursor(cursor); // emits again with the snapped position
            return;
        }
        lastPosition_ = position;
        updateMention();
    });
    connect(tokens_, &MentionTokens::spansChanged, this, &TextArea::syncAccessibility);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &TextArea::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &TextArea::applyRecipe);
    applyRecipe();
}

QString TextArea::text() const { return editor_->toPlainText(); }

void TextArea::setText(const QString& text) {
    editor_->setPlainText(text);
    tokens_->reset();
    lastPosition_ = editor_->textCursor().position();
}

QString TextArea::placeholderText() const { return static_cast<FieldEdit*>(editor_)->hint(); }

void TextArea::setPlaceholderText(const QString& text) {
    static_cast<FieldEdit*>(editor_)->setHint(text, lineBox());
    syncAccessibility();
}

int TextArea::minimumLines() const { return minLines_ > 0 ? minLines_ : recipe().minLines; }

void TextArea::setMinimumLines(int lines) {
    minLines_ = qMax(0, lines);
    fitEditor();
}

int TextArea::maximumLines() const {
    return qMax(minimumLines(), maxLines_ > 0 ? maxLines_ : recipe().maxLines);
}

void TextArea::setMaximumLines(int lines) {
    maxLines_ = qMax(0, lines);
    fitEditor();
}

void TextArea::setMaxLength(int length) {
    maxLength_ = qMax(0, length);
    fitEditor();
    syncAccessibility();
    update();
}

bool TextArea::isOverLimit() const { return maxLength_ > 0 && text().size() > maxLength_; }

void TextArea::setInvalid(bool invalid) {
    invalid_ = invalid;
    update();
}

void TextArea::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    applyRecipe();
}

TextAreaRecipe TextArea::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{hovered_, false, isEnabled(), editor_->hasFocus()});
    return textAreaRecipe(state, invalid_, isOverLimit(), Appearance::instance().theme());
}

qreal TextArea::lineBox() const {
    // CSS line boxes, whole pixels, as in ket::Composer.
    return qFloor(editor_->font().pixelSize() * recipe().lineHeight);
}

int TextArea::countHeight() const {
    if (maxLength_ <= 0)
        return 0;
    const TextAreaRecipe r = recipe();
    return qRound(scaled(r.countGap)) + qCeil(scaled(r.countPixels) * r.lineHeight);
}

int TextArea::visibleLines() const {
    const qreal box = lineBox();
    const int content = box > 0 ? qCeil(editor_->document()->size().height() / box - 0.01) : 1;
    return qBound(minimumLines(), content, maximumLines());
}

void TextArea::applyRecipe() {
    const TextAreaRecipe r = recipe();
    tokens_->setTheme(Appearance::instance().theme());
    QFont font = editor_->font();
    font.setPixelSize(qRound(scaled(r.field.fontPixels)));
    editor_->setFont(font);
    editor_->document()->setDocumentMargin(0);
    static_cast<FieldEdit*>(editor_)->setPadding(qRound(scaled(r.field.paddingInline)),
                                                 qRound(scaled(r.paddingBlock)));
    QPalette palette = editor_->palette();
    const auto faded = [&r](QColor color) {
        color.setAlphaF(float(color.alphaF() * r.field.opacity));
        return color;
    };
    for (const QPalette::ColorGroup group :
         {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        palette.setColor(group, QPalette::Base, Qt::transparent);
        palette.setColor(group, QPalette::Text, faded(r.field.text));
        palette.setColor(group, QPalette::PlaceholderText, faded(r.field.placeholder));
        palette.setColor(group, QPalette::Highlight,
                         Tokens::color(u"accent-muted", Appearance::instance().theme()));
        palette.setColor(group, QPalette::HighlightedText, faded(r.field.text));
    }
    editor_->setPalette(palette);
    fitEditor();
    update();
}

void TextArea::fitEditor() {
    const TextAreaRecipe r = recipe();
    QTextCursor cursor(editor_->document());
    cursor.select(QTextCursor::Document);
    QTextBlockFormat format;
    format.setLineHeight(lineBox(), QTextBlockFormat::FixedHeight);
    {
        const QSignalBlocker blocker(editor_);
        cursor.mergeBlockFormat(format);
    }
    static_cast<FieldEdit*>(editor_)->setHint(placeholderText(), lineBox());
    const int frame = qRound(visibleLines() * lineBox() + 2 * qRound(scaled(r.paddingBlock)));
    editor_->setGeometry(0, 0, width(), frame);
    setFixedHeight(frame + countHeight());
    placePicker();
}

void TextArea::syncAccessibility() {
    if (editor_->accessibleName().isEmpty() || editor_->accessibleName() == placeholderText())
        editor_->setAccessibleName(accessibleName().isEmpty() ? placeholderText()
                                                              : accessibleName());
    QStringList parts;
    if (maxLength_ > 0)
        parts.append(characterCountDescription(text().size(), maxLength_));
    const QString mentions = tokens_->spokenMentions();
    if (!mentions.isEmpty())
        parts.append(mentions);
    editor_->setAccessibleDescription(parts.join(QStringLiteral(". ")));
}

QSize TextArea::sizeHint() const {
    const qreal height = scaled(recipe().field.height);
    return {qRound(10 * height), this->height()};
}

QSize TextArea::minimumSizeHint() const {
    const qreal height = scaled(recipe().field.height);
    return {qRound(3 * height), this->height()};
}

void TextArea::paintEvent(QPaintEvent*) {
    const TextAreaRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.field.opacity);
    const qreal inset = r.field.borderWidth / 2.0;
    const qreal radius = scaled(r.field.radius);
    const QRectF frame(0, 0, width(), editor_->height());
    painter.setPen(QPen(r.field.border, r.field.borderWidth));
    painter.setBrush(r.field.background);
    painter.drawRoundedRect(frame.adjusted(inset, inset, -inset, -inset), radius, radius);
    if (maxLength_ <= 0)
        return;
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.countPixels)));
    painter.setFont(font);
    painter.setPen(r.count);
    const QRectF count(0, frame.bottom() + qRound(scaled(r.countGap)), width(),
                       height() - frame.height() - qRound(scaled(r.countGap)));
    painter.drawText(count, Qt::AlignRight | Qt::AlignVCenter,
                     characterCountText(text().size(), maxLength_));
}

void TextArea::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    fitEditor();
}

void TextArea::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    placePicker();
}

void TextArea::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange)
        applyRecipe();
}

void TextArea::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    hovered_ = true;
    update();
}

void TextArea::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    hovered_ = false;
    update();
}

bool TextArea::isMentionOpen() const { return picker_ && picker_->isVisible(); }

void TextArea::setUnknownMentions(const QStringList& numbers) {
    tokens_->setUnknown(numbers);
    syncAccessibility();
}

void TextArea::updateMention() {
    const QTextCursor cursor = editor_->textCursor();
    std::optional<AgentMention> mention =
        cursor.hasSelection() ? std::nullopt : agentMention(text(), cursor.position());
    // A drawn token is not a query: the caret only rests at its end.
    if (mention && tokens_->deletion(int(mention->start) + 1, true))
        mention.reset();
    if (!mention) {
        if (mentionQuery_) {
            mentionQuery_.reset();
            closeMentions();
            emit mentionClosed();
        }
        return;
    }
    if (mentionQuery_ == mention->query)
        return;
    mentionQuery_ = mention->query;
    emit mentionQueryChanged(mention->query);
}

void TextArea::setMentionItems(const QList<MentionItem>& items) {
    if (!picker_) {
        picker_ = new MentionPicker(window());
        connect(picker_, &MentionPicker::activated, this, &TextArea::applyMention);
    }
    picker_->setItems(items);
    picker_->setHighlightedIndex(0);
    placePicker();
    picker_->show();
    picker_->raise();
}

void TextArea::closeMentions() {
    if (picker_)
        picker_->hide();
}

void TextArea::applyMention(const MentionItem& item) {
    const std::optional<AgentMention> mention =
        agentMention(text(), editor_->textCursor().position());
    mentionQuery_.reset();
    closeMentions();
    if (!mention)
        return;
    const int position =
        tokens_->insert(int(mention->start), int(mention->end), item.number, item.title);
    QTextCursor cursor = editor_->textCursor();
    cursor.setPosition(position);
    lastPosition_ = position;
    editor_->setTextCursor(cursor);
    editor_->setFocus();
}

void TextArea::placePicker() {
    if (!picker_ || !window())
        return;
    // Below the field, overlapping its bottom edge; above it when the window has no room.
    const MentionPickerRecipe r = mentionPickerRecipe(Appearance::instance().theme());
    const QMargins m = picker_->shadowMargins();
    const int width = qMin(qRound(scaled(r.maxWidth)), this->width());
    const int height = picker_->sizeHint().height();
    const int overlap = qRound(scaled(r.overlap));
    const QPoint top = mapTo(window(), QPoint(0, 0));
    int y = top.y() + editor_->height() - overlap - m.top();
    if (y + height > window()->height())
        y = top.y() + overlap - height + m.bottom();
    picker_->setGeometry(top.x() - m.left(), y, width + m.left() + m.right(), height);
}

bool TextArea::handleKey(QKeyEvent* key) {
    if (isMentionOpen()) {
        switch (key->key()) {
        case Qt::Key_Escape:
            mentionQuery_.reset();
            closeMentions();
            emit mentionClosed();
            return true;
        case Qt::Key_Down:
        case Qt::Key_Up:
            picker_->moveHighlight(key->key() == Qt::Key_Down ? 1 : -1);
            return !picker_->items().isEmpty();
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Tab:
            if (isHostKey(key) || picker_->items().isEmpty() || picker_->highlightedIndex() < 0)
                break;
            applyMention(picker_->items().at(picker_->highlightedIndex()));
            return true;
        default:
            break;
        }
    }
    const bool backward = key->key() == Qt::Key_Backspace;
    if ((backward || key->key() == Qt::Key_Delete) && key->modifiers() == Qt::NoModifier) {
        QTextCursor cursor = editor_->textCursor();
        if (cursor.hasSelection())
            return false;
        const auto range = tokens_->deletion(cursor.position(), backward);
        if (!range)
            return false;
        cursor.setPosition(range->first);
        cursor.setPosition(range->second, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        lastPosition_ = range->first;
        editor_->setTextCursor(cursor);
        return true;
    }
    return false;
}

bool TextArea::eventFilter(QObject* watched, QEvent* event) {
    if (watched == editor_) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            applyRecipe();
            if (event->type() == QEvent::FocusOut && isMentionOpen()) {
                mentionQuery_.reset();
                closeMentions();
                emit mentionClosed();
            }
        } else if (event->type() == QEvent::KeyPress) {
            if (handleKey(static_cast<QKeyEvent*>(event)))
                return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

} // namespace ket
