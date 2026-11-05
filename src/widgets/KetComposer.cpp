// SPDX-License-Identifier: MPL-2.0
#include "KetComposer.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"
#include "KetMarkdown.h"
#include "KetProfile.h"
#include "KetShadow.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QTextBlock>
#include <QTextEdit>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

// `.send-button.stop`: a filled square button with the stop glyph in the on-accent colour.
class StopButton final : public QAbstractButton {
  public:
    explicit StopButton(Composer* composer) : QAbstractButton(composer), composer_(composer) {
        setAccessibleName(Composer::tr("Stop turn"));
        setToolTip(accessibleName());
        setCursor(Qt::PointingHandCursor);
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const ComposerRecipe r = composer_->recipe();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(isEnabled() ? 1.0 : Profile::current().disabledOpacity);
        const qreal radius = scaled(Tokens::px(u"control-radius"));
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.stopBackground);
        painter.drawRoundedRect(QRectF(rect()), radius, radius);
        const qreal box = scaled(r.stopIconSize);
        // The mock fills Lucide `square`: an 18/24 square with 2/24 corners.
        const qreal inset = box * 3 / 24;
        const QRectF square = QRectF((width() - box) / 2, (height() - box) / 2, box, box)
                                  .adjusted(inset, inset, -inset, -inset);
        painter.setBrush(r.stopIcon);
        painter.drawRoundedRect(square, box * 2 / 24, box * 2 / 24);
    }

  private:
    Composer* composer_;
};

// QTextEdit with public viewport margins: the textarea padding sits outside the text area,
// so the caret and placeholder start where the mock's do.
class PaddedEdit final : public QTextEdit {
  public:
    explicit PaddedEdit(QWidget* parent) : QTextEdit(parent) {}
    void setPadding(int inline_, int block) { setViewportMargins(inline_, block, inline_, block); }
};

} // namespace

Composer::Composer(QWidget* parent)
    : QWidget(parent), editor_(new PaddedEdit(this)),
      send_(new Button({}, ButtonVariant::Secondary, this)), stop_(new StopButton(this)),
      leading_(new QHBoxLayout), trailing_(new QHBoxLayout), input_(new QHBoxLayout),
      chips_(new QWidget(this)), chipRow_(new QHBoxLayout(chips_)), toolbar_(new QWidget(this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editor_->setAcceptRichText(false);
    editor_->setFrameShape(QFrame::NoFrame);
    editor_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    editor_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor_->viewport()->setAutoFillBackground(false);
    editor_->setAutoFillBackground(false);
    editor_->setPlaceholderText(tr("Ask for follow-up changes"));
    editor_->installEventFilter(this);
    send_->setIcon(Icons::icon(u"arrow-up"));
    send_->setAccessibleName(tr("Send request"));
    send_->setToolTip(send_->accessibleName());
    send_->setEnabled(false);
    stop_->hide();

    input_->setContentsMargins(0, 0, 0, 0);
    input_->addWidget(editor_, 1);
    input_->addWidget(stop_, 0, Qt::AlignBottom);
    input_->addWidget(send_, 0, Qt::AlignBottom);
    chipRow_->addStretch(1);
    chips_->hide();

    auto* tools = new QHBoxLayout(toolbar_);
    tools->setContentsMargins(0, 0, 0, 0);
    tools->addLayout(leading_);
    tools->addStretch(1);
    tools->addLayout(trailing_);

    auto* column = new QVBoxLayout(this);
    column->setSpacing(0);
    column->addWidget(chips_);
    column->addLayout(input_);
    column->addWidget(toolbar_);

    connect(editor_, &QTextEdit::textChanged, this, [this] {
        send_->setEnabled(!text().trimmed().isEmpty());
        fitEditor();
        updateMention();
    });
    connect(editor_, &QTextEdit::cursorPositionChanged, this, &Composer::updateMention);
    connect(send_, &QAbstractButton::clicked, this, &Composer::submit);
    connect(stop_, &QAbstractButton::clicked, this, &Composer::stopRequested);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &Composer::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Composer::applyRecipe);
    applyRecipe();
}

AttachmentChip* Composer::addAttachment(const QString& name, const QString& iconCode) {
    auto* chip = new AttachmentChip(name, iconCode, chips_);
    chipRow_->insertWidget(chipRow_->count() - 1, chip);
    attachments_.append(chip);
    connect(chip, &AttachmentChip::removeRequested, this,
            [this, chip] { removeAttachment(int(attachments_.indexOf(chip))); });
    chips_->show();
    return chip;
}

QStringList Composer::attachments() const {
    QStringList names;
    for (const AttachmentChip* chip : attachments_)
        names.append(chip->name());
    return names;
}

void Composer::removeAttachment(int index) {
    if (index < 0 || index >= attachments_.size())
        return;
    AttachmentChip* chip = attachments_.takeAt(index);
    const QString name = chip->name();
    const bool focused = chip->hasFocus();
    chip->deleteLater();
    chip->hide();
    chips_->setVisible(!attachments_.isEmpty());
    if (focused)
        editor_->setFocus();
    emit attachmentRemoved(index, name);
}

void Composer::clearAttachments() {
    while (!attachments_.isEmpty())
        removeAttachment(int(attachments_.size()) - 1);
}

QString Composer::text() const { return editor_->toPlainText(); }

void Composer::setText(const QString& text) { editor_->setPlainText(text); }

void Composer::setPlaceholderText(const QString& text) { editor_->setPlaceholderText(text); }

void Composer::setBusy(bool busy) {
    busy_ = busy;
    stop_->setVisible(busy);
    send_->setAccessibleName(busy ? tr("Send guidance") : tr("Send request"));
    send_->setToolTip(send_->accessibleName());
}

void Composer::submit() {
    const QString message = text();
    if (message.trimmed().isEmpty())
        return;
    editor_->clear();
    emit submitted(message);
}

ComposerRecipe Composer::recipe() const {
    return composerRecipe(focused_, Appearance::instance().theme());
}

void Composer::applyRecipe() {
    const ComposerRecipe r = recipe();
    const int ring = qCeil(scaled(r.ringWidth));
    const int padding = qRound(scaled(r.padding));
    // The ring sits outside the 1 px border; the padding inside it.
    auto* column = static_cast<QVBoxLayout*>(layout());
    column->setContentsMargins(ring + 1 + padding, ring + 1 + padding, ring + 1 + padding,
                               ring + 1 + padding);
    const int send = qRound(scaled(r.sendSize));
    send_->setFixedSize(send, send);
    stop_->setFixedSize(send, send);
    input_->setSpacing(qRound(scaled(Tokens::px(u"space-1"))));
    // The send button sits centred on the input's first line box.
    input_->setContentsMargins(0, 0, 0, qMax(0, qRound((scaled(r.minInputHeight) - send) / 2)));
    const AttachmentChipRecipe chip = attachmentChipRecipe(false, Appearance::instance().theme());
    chipRow_->setSpacing(qRound(scaled(chip.rowGap)));
    chipRow_->setContentsMargins(0, 0, 0, qRound(scaled(chip.rowPaddingTop)));
    toolbar_->setFixedHeight(qRound(scaled(r.toolbarHeight)));
    leading_->setSpacing(qRound(scaled(r.toolbarGap)));
    trailing_->setSpacing(qRound(scaled(r.toolbarGap)));

    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.textPixels)));
    editor_->setFont(font);
    editor_->document()->setDocumentMargin(0);
    const int inline_ = qRound(scaled(r.textPaddingInline));
    const int block = qRound(scaled(r.textPaddingBlock));
    static_cast<PaddedEdit*>(editor_)->setPadding(inline_, block);
    QPalette palette = editor_->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, r.text);
    palette.setColor(QPalette::PlaceholderText, r.placeholder);
    editor_->setPalette(palette);
    fitEditor();
    update();
}

void Composer::fitEditor() {
    const ComposerRecipe r = recipe();
    // CSS line-height on every block, then grow to the content up to maxLines.
    QTextCursor cursor(editor_->document());
    cursor.select(QTextCursor::Document);
    // CSS line boxes are font size × line height (Qt's proportional height scales the font's
    // line spacing instead), whole pixels so the input lands on the mock's 48 px.
    const qreal lineBox = qFloor(editor_->font().pixelSize() * r.lineHeight);
    QTextBlockFormat format;
    format.setLineHeight(lineBox, QTextBlockFormat::FixedHeight);
    const QSignalBlocker blocker(editor_);
    cursor.mergeBlockFormat(format);
    const qreal padding = 2 * qRound(scaled(r.textPaddingBlock));
    const qreal content = editor_->document()->size().height();
    const qreal maxHeight = r.maxLines * lineBox + padding;
    const int height =
        qRound(qBound(scaled(r.minInputHeight), qMax(content, lineBox) + padding, maxHeight));
    editor_->setFixedHeight(height);
}

void Composer::paintEvent(QPaintEvent*) {
    const ComposerRecipe r = recipe();
    const qreal ring = scaled(r.ringWidth);
    const qreal radius = scaled(r.radius);
    const QRectF card = QRectF(rect()).adjusted(ring, ring, -ring, -ring);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (r.ring.alpha()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.ring);
        painter.drawRoundedRect(QRectF(rect()), radius + ring, radius + ring);
    } else {
        detail::paintShadow(painter, card, radius, detail::scaled(r.shadow));
    }
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(card.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

bool Composer::isMentionOpen() const { return picker_ && picker_->isVisible(); }

void Composer::updateMention() {
    const QTextCursor cursor = editor_->textCursor();
    const std::optional<AgentMention> mention =
        cursor.hasSelection() ? std::nullopt : agentMention(text(), cursor.position());
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

void Composer::setMentionItems(const QList<MentionItem>& items) {
    if (!picker_) {
        picker_ = new MentionPicker(window());
        connect(picker_, &MentionPicker::activated, this, &Composer::applyMention);
    }
    picker_->setItems(items);
    picker_->setHighlightedIndex(0);
    placePicker();
    picker_->show();
    picker_->raise();
}

void Composer::closeMentions() {
    if (picker_)
        picker_->hide();
}

void Composer::applyMention(const MentionItem& item) {
    const std::optional<AgentMention> mention =
        agentMention(text(), editor_->textCursor().position());
    if (!mention)
        return closeMentions();
    QTextCursor cursor = editor_->textCursor();
    cursor.setPosition(int(mention->start));
    cursor.setPosition(int(mention->end), QTextCursor::KeepAnchor);
    cursor.insertText(agentMentionText(item.number));
    editor_->setTextCursor(cursor);
    mentionQuery_.reset();
    closeMentions();
    editor_->setFocus();
}

void Composer::placePicker() {
    if (!picker_)
        return;
    // Over the composer's top edge, overlapping it (`bottom: calc(100% - 4px)`).
    const MentionPickerRecipe r = mentionPickerRecipe(Appearance::instance().theme());
    const qreal ring = scaled(recipe().ringWidth);
    const QRect card = rect().adjusted(qRound(ring), qRound(ring), -qRound(ring), -qRound(ring));
    const QMargins m = picker_->shadowMargins();
    const int width = qMin(qRound(scaled(r.maxWidth)), card.width());
    const int height = picker_->sizeHint().height();
    const QPoint topLeft = mapTo(window(), card.topLeft());
    picker_->setGeometry(topLeft.x() - m.left(),
                         topLeft.y() + qRound(scaled(r.overlap)) - height + m.bottom(),
                         width + m.left() + m.right(), height);
}

void Composer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    placePicker();
}

void Composer::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    placePicker();
}

bool Composer::handleMentionKey(QKeyEvent* key) {
    if (!isMentionOpen())
        return false;
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
        // With no options the key keeps its usual meaning, as in the mock.
        if (picker_->items().isEmpty() || picker_->highlightedIndex() < 0)
            return false;
        applyMention(picker_->items().at(picker_->highlightedIndex()));
        return true;
    default:
        return false;
    }
}

bool Composer::eventFilter(QObject* watched, QEvent* event) {
    if (watched == editor_) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            focused_ = event->type() == QEvent::FocusIn;
            applyRecipe();
        } else if (event->type() == QEvent::KeyPress) {
            auto* key = static_cast<QKeyEvent*>(event);
            if (handleMentionKey(key))
                return true;
            const bool enter = key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter;
            if (enter && !(key->modifiers() & Qt::ShiftModifier)) {
                submit();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

} // namespace ket
