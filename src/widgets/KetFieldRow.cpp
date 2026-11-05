// SPDX-License-Identifier: MPL-2.0
#include "KetFieldRow.h"

#include "KetAppearance.h"
#include "KetTextField.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>

namespace ket {

FieldRow::FieldRow(const QString& label, QWidget* control, const QString& help, QWidget* parent)
    : QWidget(parent), label_(new QLabel(label, this)), control_(control), note_(new QLabel(this)),
      help_(help) {
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    row->addWidget(label_, 0, Qt::AlignTop);
    auto* column = new QVBoxLayout;
    column->setContentsMargins(0, 0, 0, 0);
    control_->setParent(this);
    // Wrapped help text must not squeeze the control below its own height.
    control_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    control_->setMinimumHeight(control_->sizeHint().height());
    column->addWidget(control_);
    note_->setWordWrap(true);
    column->addWidget(note_);
    row->addLayout(column, 1);
    label_->setBuddy(control_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &FieldRow::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &FieldRow::applyRecipe);
    applyRecipe();
}

// Wrapped help text needs the real width. A parent that places the row at its size hint
// (an aligned layout item) would give it a single-line height, so the note's minimum
// height follows the width the row actually has.
void FieldRow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (event->oldSize().width() != event->size().width())
        fitNote();
}

void FieldRow::fitNote() {
    if (!note_->isVisible() && note_->isHidden()) {
        note_->setMinimumHeight(0);
        return;
    }
    const auto* row = static_cast<QHBoxLayout*>(layout());
    const int noteWidth = width() - label_->width() - row->spacing();
    if (noteWidth > 0)
        note_->setMinimumHeight(note_->heightForWidth(noteWidth));
}

void FieldRow::setHelp(const QString& help) {
    help_ = help;
    applyRecipe();
}

void FieldRow::setError(const QString& error) {
    error_ = error;
    if (auto* field = qobject_cast<TextField*>(control_))
        field->setInvalid(!error.isEmpty());
    applyRecipe();
}

void FieldRow::applyRecipe() {
    const FieldRowRecipe r = fieldRowRecipe(Appearance::instance().theme());
    const Appearance& appearance = Appearance::instance();
    const auto styled = [](QLabel* label, const QColor& color, qreal pixels) {
        QFont f = label->font();
        f.setPixelSize(qRound(pixels));
        label->setFont(f);
        QPalette p = label->palette();
        p.setColor(QPalette::WindowText, color);
        label->setPalette(p);
    };
    styled(label_, r.label, appearance.scaled(r.labelPixels));
    label_->setFixedWidth(qRound(appearance.scaled(r.labelWidth)));
    label_->setContentsMargins(0, qRound(appearance.scaled(r.labelTop)), 0, 0);
    const bool error = !error_.isEmpty();
    styled(note_, error ? r.error : r.help, appearance.scaled(r.helpPixels));
    note_->setText(error ? error_ : help_);
    note_->setVisible(!note_->text().isEmpty());
    fitNote();
    control_->setMinimumHeight(control_->sizeHint().height());
    auto* row = static_cast<QHBoxLayout*>(layout());
    row->setSpacing(qRound(appearance.scaled(r.gap)));
    static_cast<QVBoxLayout*>(row->itemAt(1)->layout())
        ->setSpacing(qRound(appearance.scaled(r.helpGap)));
}

} // namespace ket
