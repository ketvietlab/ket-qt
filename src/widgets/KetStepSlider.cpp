// SPDX-License-Identifier: MPL-2.0
#include "KetStepSlider.h"

#include "KetAppearance.h"

#include <QBoxLayout>
#include <QLabel>
#include <QSlider>

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

StepSlider::StepSlider(const QString& label, QWidget* parent)
    : QWidget(parent), label_(new QLabel(label, this)), value_(new QLabel(this)),
      slider_(new QSlider(Qt::Horizontal, this)), first_(new QLabel(this)),
      last_(new QLabel(this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider_->setPageStep(1);
    slider_->setAccessibleName(label);
    auto* header = new QHBoxLayout;
    header->setContentsMargins(0, 0, 0, 0);
    header->addWidget(label_);
    header->addStretch(1);
    header->addWidget(value_);
    auto* ends = new QHBoxLayout;
    ends->setContentsMargins(0, 0, 0, 0);
    ends->addWidget(first_);
    ends->addStretch(1);
    ends->addWidget(last_);
    auto* column = new QVBoxLayout(this);
    column->setContentsMargins(0, 0, 0, 0);
    column->addLayout(header);
    column->addWidget(slider_);
    column->addLayout(ends);
    connect(slider_, &QSlider::valueChanged, this, [this](int index) {
        updateLabels();
        emit currentIndexChanged(index);
    });
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &StepSlider::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &StepSlider::applyRecipe);
    applyRecipe();
}

QString StepSlider::label() const { return label_->text(); }

void StepSlider::setLabel(const QString& label) {
    label_->setText(label);
    slider_->setAccessibleName(label);
}

void StepSlider::setSteps(const QStringList& steps) {
    steps_ = steps;
    const QSignalBlocker block(slider_);
    slider_->setRange(0, qMax(0, int(steps.size()) - 1));
    slider_->setEnabled(steps.size() > 1);
    first_->setText(steps.value(0));
    last_->setText(steps.size() > 1 ? steps.constLast() : QString());
    updateLabels();
}

int StepSlider::currentIndex() const { return steps_.isEmpty() ? -1 : slider_->value(); }

void StepSlider::setCurrentIndex(int index) { slider_->setValue(index); }

QString StepSlider::currentStep() const { return steps_.value(currentIndex()); }

void StepSlider::updateLabels() {
    value_->setText(currentStep());
    slider_->setAccessibleDescription(currentStep());
}

void StepSlider::applyRecipe() {
    const SliderRecipe r = sliderRecipe({}, Appearance::instance().theme());
    auto* column = static_cast<QVBoxLayout*>(layout());
    column->setSpacing(0);
    column->itemAt(0)->layout()->setContentsMargins(0, 0, 0, qRound(scaled(r.headerGap)));
    column->itemAt(2)->layout()->setContentsMargins(0, qRound(scaled(r.labelsGap)), 0, 0);
    styleLabel(label_, r.labelPixels, r.labelWeight, r.label);
    styleLabel(value_, r.labelPixels, QFont::Normal, r.value);
    styleLabel(first_, r.rangePixels, QFont::Normal, r.rangeLabel);
    styleLabel(last_, r.rangePixels, QFont::Normal, r.rangeLabel);
    slider_->setFixedHeight(qRound(scaled(r.height)));
}

} // namespace ket
