// SPDX-License-Identifier: MPL-2.0
#include "KetStepSlider.h"

#include "KetAppearance.h"

#include <QBoxLayout>
#include <QFocusEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
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

// The notched track. It stays a QSlider so keys, the value and accessibility (a slider named
// after the label) come from Qt; it only draws the recipe and snaps the pointer to a step.
class NotchedSlider final : public QSlider {
  public:
    explicit NotchedSlider(QWidget* parent) : QSlider(Qt::Horizontal, parent) {
        setAttribute(Qt::WA_Hover);
        setFocusPolicy(Qt::StrongFocus);
        setSingleStep(1);
        setPageStep(1);
        setCursor(Qt::PointingHandCursor);
    }

    void setPreviewState(std::optional<InteractionState> state) {
        preview_ = state;
        update();
    }

    [[nodiscard]] SliderRecipe recipe() const {
        const InteractionState state = preview_.value_or(InteractionState{
            underMouse(), isSliderDown(), isEnabled(), visualFocus_ && hasFocus()});
        return sliderRecipe(state, Appearance::instance().theme());
    }

    [[nodiscard]] QSize sizeHint() const override {
        return {QSlider::sizeHint().width(), qRound(scaled(recipe().notchHeight))};
    }
    [[nodiscard]] QSize minimumSizeHint() const override {
        const SliderRecipe r = recipe();
        return {qRound(scaled(r.notchInset * 2)), qRound(scaled(r.notchHeight))};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const SliderRecipe r = recipe();
        const int count = maximum() - minimum() + 1;
        const qreal width = this->width();
        const qreal height = this->height();
        const qreal inset = scaled(r.notchInset);
        const qreal trackHeight = scaled(r.notchTrackHeight);
        const qreal dot = scaled(r.notchDotSize);
        const qreal thumb = scaled(r.notchThumbSize);
        const qreal ring = scaled(r.notchRingWidth);
        const qreal middle = height / 2;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.notchPill);
        painter.drawRoundedRect(QRectF(rect()), height / 2, height / 2);
        // The track reaches half its height past the end steps, so their dots sit in its caps.
        const qreal start = inset - trackHeight / 2;
        const QRectF track(start, middle - trackHeight / 2, qMax<qreal>(0, width - start * 2),
                           trackHeight);
        painter.setBrush(r.track);
        painter.drawRoundedRect(track, trackHeight / 2, trackHeight / 2);
        const int chosen = value() - minimum();
        const qreal chosenCentre = stepNotchCentre(width, inset, count, chosen);
        QRectF filled = track;
        filled.setRight(qMin(track.right(), chosenCentre + trackHeight / 2));
        painter.setBrush(r.fill);
        painter.drawRoundedRect(filled, trackHeight / 2, trackHeight / 2);
        for (int index = 0; index < count; ++index) {
            const QPointF centre(stepNotchCentre(width, inset, count, index), middle);
            if (index == chosen) {
                painter.setPen(QPen(r.handleBorder, ring));
                painter.setBrush(r.handle);
                const qreal radius = (thumb - ring) / 2;
                painter.drawEllipse(centre, radius, radius);
                painter.setPen(Qt::NoPen);
            }
            painter.setBrush(index == chosen ? r.fill : r.notch);
            painter.drawEllipse(centre, dot / 2, dot / 2);
        }
        if (r.focusRing.alpha()) {
            const qreal pen = scaled(r.focusRingWidth);
            const QRectF outline = QRectF(rect()).adjusted(pen / 2, pen / 2, -pen / 2, -pen / 2);
            painter.setPen(QPen(r.focusRing, pen));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(outline, outline.height() / 2, outline.height() / 2);
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() != Qt::LeftButton || maximum() <= minimum()) {
            event->ignore();
            return;
        }
        setSliderDown(true);
        snapTo(event->position().x());
        event->accept();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (isSliderDown())
            snapTo(event->position().x());
        event->accept();
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (isSliderDown() && event->button() == Qt::LeftButton) {
            snapTo(event->position().x());
            setSliderDown(false);
        }
        event->accept();
    }

    void focusInEvent(QFocusEvent* event) override {
        switch (event->reason()) {
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
        case Qt::ShortcutFocusReason:
            visualFocus_ = true;
            break;
        case Qt::ActiveWindowFocusReason:
        case Qt::PopupFocusReason:
            break; // focus returns to the track; keep how it was shown
        default:
            visualFocus_ = false;
            break;
        }
        QSlider::focusInEvent(event);
    }

    void focusOutEvent(QFocusEvent* event) override {
        if (event->reason() != Qt::ActiveWindowFocusReason &&
            event->reason() != Qt::PopupFocusReason)
            visualFocus_ = false;
        QSlider::focusOutEvent(event);
    }

  private:
    void snapTo(qreal x) {
        const SliderRecipe r = recipe();
        const int count = maximum() - minimum() + 1;
        setValue(minimum() + stepNotchNearest(x, width(), scaled(r.notchInset), count));
    }

    std::optional<InteractionState> preview_;
    bool visualFocus_ = false;
};

} // namespace

StepSlider::StepSlider(const QString& label, QWidget* parent)
    : QWidget(parent), label_(new QLabel(label, this)), value_(new QLabel(this)),
      slider_(new NotchedSlider(this)), first_(new QLabel(this)), last_(new QLabel(this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider_->setAccessibleName(label);
    slider_->setEnabled(false);
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
    slider_->update();
}

int StepSlider::currentIndex() const { return steps_.isEmpty() ? -1 : slider_->value(); }

void StepSlider::setCurrentIndex(int index) { slider_->setValue(index); }

QString StepSlider::currentStep() const { return steps_.value(currentIndex()); }

void StepSlider::setPreviewState(std::optional<InteractionState> state) {
    static_cast<NotchedSlider*>(slider_)->setPreviewState(state);
}

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
    slider_->setFixedHeight(qRound(scaled(r.notchHeight)));
    slider_->update();
}

} // namespace ket
