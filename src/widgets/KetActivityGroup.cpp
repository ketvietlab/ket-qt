// SPDX-License-Identifier: MPL-2.0
#include "KetActivityGroup.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

// `summary`: the label, then the chevron right after it.
class Summary final : public QAbstractButton {
  public:
    explicit Summary(QWidget* parent) : QAbstractButton(parent) {
        setCheckable(true);
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
        connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
    }

    std::optional<InteractionState> preview;

    [[nodiscard]] ActivityGroupRecipe recipe() const {
        const InteractionState state =
            preview.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
        return activityGroupRecipe(state, Appearance::instance().theme());
    }

    [[nodiscard]] QFont textFont() const {
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(recipe().fontPixels)));
        return font;
    }

    [[nodiscard]] QSize sizeHint() const override {
        const ActivityGroupRecipe r = recipe();
        const qreal width = QFontMetricsF(textFont()).horizontalAdvance(text()) +
                            scaled(r.spacing) + scaled(r.chevronSize);
        return {qCeil(width), qRound(scaled(r.height))};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const ActivityGroupRecipe r = recipe();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        const qreal radius = scaled(r.radius);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(QRectF(rect()), radius, radius);
        if (r.focusRing.alpha()) {
            painter.setPen(QPen(r.focusRing, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
        }
        const QFont font = textFont();
        const QFontMetricsF metrics(font);
        const qreal chevron = scaled(r.chevronSize);
        const qreal gap = scaled(r.spacing);
        const qreal available = qMax<qreal>(0, width() - chevron - gap);
        const QString label = metrics.elidedText(text(), Qt::ElideRight, available);
        const qreal textWidth = metrics.horizontalAdvance(label);
        painter.setFont(font);
        painter.setPen(r.text);
        painter.drawText(QRectF(0, 0, textWidth + 1, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         label);
        painter.drawPixmap(QPointF(qRound(textWidth + gap), qRound((height() - chevron) / 2)),
                           Icons::pixmap(isChecked() ? u"chevron-up" : u"chevron-down", chevron,
                                         devicePixelRatioF(), r.text));
    }
    void enterEvent(QEnterEvent* event) override {
        update();
        QAbstractButton::enterEvent(event);
    }
    void leaveEvent(QEvent* event) override {
        update();
        QAbstractButton::leaveEvent(event);
    }
};

// `.activity-children`: the rows behind a hairline.
class Children final : public QWidget {
  public:
    using QWidget::QWidget;

  protected:
    void paintEvent(QPaintEvent*) override {
        const ActivityGroupRecipe r = activityGroupRecipe({}, Appearance::instance().theme());
        const int top = qRound(scaled(r.childrenSpacingTop));
        const int bottom = qRound(scaled(r.childrenSpacingBottom));
        QPainter painter(this);
        painter.fillRect(QRect(qRound(scaled(r.childrenIndent)), top, 1, height() - top - bottom),
                         r.rule);
    }
};

} // namespace

ActivityGroup::ActivityGroup(const QString& text, QWidget* parent)
    : QWidget(parent), summary_(new Summary(this)), children_(new Children(this)),
      rows_(new QVBoxLayout) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    summary_->setText(text);
    auto* inner = new QVBoxLayout(children_);
    inner->addLayout(rows_);
    rows_->setContentsMargins(0, 0, 0, 0);
    children_->hide();
    auto* column = new QVBoxLayout(this);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(0);
    column->addWidget(summary_);
    column->addWidget(children_);
    connect(summary_, &QAbstractButton::toggled, this, [this](bool expanded) {
        children_->setVisible(expanded);
        emit toggled(expanded);
    });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &ActivityGroup::applyRecipe);
    applyRecipe();
}

QString ActivityGroup::text() const { return summary_->text(); }

void ActivityGroup::setText(const QString& text) {
    summary_->setText(text);
    summary_->updateGeometry();
}

void ActivityGroup::addRow(QWidget* row) { rows_->addWidget(row); }

bool ActivityGroup::isExpanded() const { return summary_->isChecked(); }

void ActivityGroup::setExpanded(bool expanded) { summary_->setChecked(expanded); }

QAbstractButton* ActivityGroup::summary() const { return summary_; }

void ActivityGroup::setPreviewState(std::optional<InteractionState> state) {
    static_cast<Summary*>(summary_)->preview = state;
    summary_->update();
}

void ActivityGroup::applyRecipe() {
    // `.activity-children`: margin 4 0 12 5 and padding 2 0 2 14 around the rows, with the
    // hairline on the padding box's leading edge.
    const ActivityGroupRecipe r = activityGroupRecipe({}, Appearance::instance().theme());
    const int inset = qRound(scaled(Tokens::px(u"space-0-5")));
    children_->layout()->setContentsMargins(qRound(scaled(r.childrenIndent)) + 1 +
                                                qRound(scaled(r.childrenPadding)),
                                            qRound(scaled(r.childrenSpacingTop)) + inset, 0,
                                            qRound(scaled(r.childrenSpacingBottom)) + inset);
    rows_->setSpacing(qRound(scaled(r.childrenGap)));
    children_->update();
}

} // namespace ket
