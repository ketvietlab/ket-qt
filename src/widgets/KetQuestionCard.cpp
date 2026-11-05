// SPDX-License-Identifier: MPL-2.0
#include "KetQuestionCard.h"

#include "KetAppearance.h"
#include "KetIcon.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

void styleLabel(QLabel* label, qreal pixels, int weight, const QColor& color) {
    QFont font = label->font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    label->setFont(font);
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, color);
    label->setPalette(palette);
}

} // namespace

QuestionCard::QuestionCard(const QString& question, QWidget* parent)
    : QWidget(parent), icon_(new Icon(QStringLiteral("help"), 14, this)),
      label_(new QLabel(tr("Your input is needed"), this)), step_(new QLabel(this)),
      question_(new QLabel(question, this)), body_(new QVBoxLayout), footer_(new QHBoxLayout) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    question_->setWordWrap(true);
    question_->setTextFormat(Qt::PlainText);
    step_->hide();
    auto* header = new QHBoxLayout;
    header->setContentsMargins(0, 0, 0, 0);
    header->addWidget(icon_);
    header->addWidget(label_);
    header->addStretch(1);
    header->addWidget(step_);
    footer_->setContentsMargins(0, 0, 0, 0);
    auto* column = new QVBoxLayout(this);
    column->addLayout(header);
    column->addWidget(question_);
    column->addLayout(body_);
    column->addLayout(footer_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &QuestionCard::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QuestionCard::applyRecipe);
    applyRecipe();
}

QString QuestionCard::question() const { return question_->text(); }

void QuestionCard::setQuestion(const QString& question) { question_->setText(question); }

QString QuestionCard::label() const { return label_->text(); }

void QuestionCard::setLabel(const QString& label) { label_->setText(label); }

QString QuestionCard::step() const { return step_->text(); }

void QuestionCard::setStep(const QString& step) {
    step_->setText(step);
    step_->setVisible(!step.isEmpty());
}

QuestionCardRecipe QuestionCard::recipe() const {
    return questionCardRecipe(Appearance::instance().theme());
}

void QuestionCard::applyRecipe() {
    const QuestionCardRecipe r = recipe();
    const int padding = qRound(scaled(r.padding)) + 1;
    auto* column = static_cast<QVBoxLayout*>(layout());
    column->setContentsMargins(padding, padding, padding, padding);
    column->setSpacing(0);
    static_cast<QHBoxLayout*>(column->itemAt(0)->layout())
        ->setSpacing(qRound(scaled(Tokens::px(u"space-1-5"))));
    const int headingSpacing = qRound(scaled(r.headingSpacing));
    question_->setContentsMargins(0, headingSpacing, 0, headingSpacing);
    body_->setSpacing(qRound(scaled(r.controlGap)));
    body_->setContentsMargins(0, 0, 0, qRound(scaled(r.controlGap)));
    footer_->setSpacing(qRound(scaled(r.spacing)));
    icon_->setColor(r.label);
    icon_->setIconSize(r.iconSize); // ket::Icon scales with the zoom itself
    styleLabel(label_, r.labelPixels, r.labelWeight, r.label);
    styleLabel(step_, r.labelPixels, QFont::Normal, r.step);
    styleLabel(question_, r.headingPixels, r.headingWeight, r.heading);
    update();
}

void QuestionCard::paintEvent(QPaintEvent*) {
    const QuestionCardRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

} // namespace ket
