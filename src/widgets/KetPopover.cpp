// SPDX-License-Identifier: MPL-2.0
#include "KetPopover.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"
#include "KetShadow.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

constexpr char kRoleProperty[] = "_ket_popover_role";

// `.settings-divider`: a hairline with the recipe's spacing above and below.
class Divider final : public QWidget {
  public:
    using QWidget::QWidget;

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(QRect(0, height() / 2, width(), 1),
                         popoverRecipe(Appearance::instance().theme()).divider);
    }
};

} // namespace

Popover::Popover(const QString& title, QWidget* parent)
    : QWidget(parent), title_(new QLabel(title, this)),
      close_(new IconButton(Icons::icon(u"close"), tr("Close"), this)), body_(new QVBoxLayout),
      footer_(new QHBoxLayout), footerHost_(new QWidget(this)) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setAccessibleName(title);
    close_->setControlSize(ControlSize::Small);
    connect(close_, &QAbstractButton::clicked, this, &Popover::dismiss);

    auto* header = new QHBoxLayout;
    header->setContentsMargins(0, 0, 0, 0);
    header->addWidget(title_, 1);
    header->addWidget(close_, 0, Qt::AlignTop);
    auto* footerRow = new QHBoxLayout(footerHost_);
    footerRow->setContentsMargins(0, 0, 0, 0);
    footerRow->addStretch(1);
    footerRow->addLayout(footer_);
    footerHost_->hide();

    auto* column = new QVBoxLayout(this);
    column->addLayout(header);
    column->addLayout(body_);
    column->addWidget(footerHost_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &Popover::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Popover::applyRecipe);
    applyRecipe();
}

QString Popover::title() const { return title_->text(); }

void Popover::setTitle(const QString& title) {
    title_->setText(title);
    setAccessibleName(title);
}

QLabel* Popover::addHeading(const QString& text) {
    auto* label = new QLabel(text, this);
    label->setProperty(kRoleProperty, QStringLiteral("heading"));
    body_->addWidget(label);
    applyRecipe();
    return label;
}

QLabel* Popover::addNote(const QString& text) {
    auto* label = new QLabel(text, this);
    label->setWordWrap(true);
    label->setProperty(kRoleProperty, QStringLiteral("note"));
    body_->addWidget(label);
    applyRecipe();
    return label;
}

void Popover::addFooterWidget(QWidget* widget) {
    footer_->addWidget(widget);
    footerHost_->show();
    updateGeometry();
}

void Popover::addDivider() {
    auto* line = new Divider(this);
    line->setProperty(kRoleProperty, QStringLiteral("divider"));
    body_->addWidget(line);
    applyRecipe();
}

PopoverRecipe Popover::recipe() const { return popoverRecipe(Appearance::instance().theme()); }

QMargins Popover::shadowMargins() const {
    const QMarginsF m = detail::shadowMargins(detail::scaled(recipe().shadow));
    return {qCeil(m.left()), qCeil(m.top()), qCeil(m.right()), qCeil(m.bottom())};
}

QSize Popover::sizeHint() const {
    const QMargins m = shadowMargins();
    const int width = qRound(scaled(recipe().width)) + m.left() + m.right();
    return {width, layout()->hasHeightForWidth() ? layout()->heightForWidth(width)
                                                 : layout()->sizeHint().height()};
}

void Popover::applyRecipe() {
    const PopoverRecipe r = recipe();
    const QMargins shadow = shadowMargins();
    const int padding = qRound(scaled(r.padding)) + 1;
    auto* column = static_cast<QVBoxLayout*>(layout());
    column->setContentsMargins(shadow.left() + padding, shadow.top() + padding,
                               shadow.right() + padding, shadow.bottom() + padding);
    column->setSpacing(0);
    static_cast<QHBoxLayout*>(column->itemAt(0)->layout())
        ->setContentsMargins(0, 0, 0, qRound(scaled(r.headerGap)));
    footerHost_->setContentsMargins(0, qRound(scaled(r.footerGap)), 0, 0);
    footerHost_->setVisible(footer_->count() > 0);
    footer_->setSpacing(qRound(scaled(Tokens::px(u"space-2"))));

    const auto style = [this](QLabel* label, qreal pixels, int weight, const QColor& color) {
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(pixels)));
        font.setWeight(QFont::Weight(weight));
        label->setFont(font);
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, color);
        label->setPalette(palette);
    };
    style(title_, r.titlePixels, r.titleWeight, r.title);
    for (QObject* child : children()) {
        auto* widget = qobject_cast<QWidget*>(child);
        if (!widget)
            continue;
        const QString role = widget->property(kRoleProperty).toString();
        if (role == u"heading") {
            style(static_cast<QLabel*>(widget), r.titlePixels, r.titleWeight, r.title);
        } else if (role == u"note") {
            auto* label = static_cast<QLabel*>(widget);
            style(label, r.notePixels, QFont::Normal, r.muted);
            label->setContentsMargins(0, 0, 0, 0);
        } else if (role == u"divider") {
            widget->setFixedHeight(2 * qRound(scaled(r.dividerSpacing)) + 1);
            widget->update();
        }
    }
    updateGeometry();
    if (isVisible())
        place();
    update();
}

void Popover::openAbove(QWidget* anchor) {
    if (window_)
        window_->removeEventFilter(this);
    anchor_ = anchor;
    window_ = anchor ? anchor->window() : nullptr;
    if (!window_)
        return;
    setParent(window_);
    window_->installEventFilter(this);
    anchor->installEventFilter(this);
    applyRecipe();
    place();
    show();
    raise();
    setFocus(Qt::PopupFocusReason);
}

void Popover::place() {
    if (!anchor_ || !window_)
        return;
    const QSize size = sizeHint();
    resize(size);
    const QMargins shadow = shadowMargins();
    const QPoint topRight = anchor_->mapTo(window_.data(), QPoint(anchor_->width(), 0));
    const int offset = qRound(scaled(recipe().offset));
    int x = topRight.x() - size.width() + shadow.right();
    int y = topRight.y() - offset - size.height() + shadow.bottom();
    x = qBound(-shadow.left(), x, qMax(-shadow.left(), window_->width() - size.width()));
    y = qMax(-shadow.top(), y);
    move(x, y);
}

void Popover::dismiss() {
    if (!isVisible())
        return;
    hide();
}

void Popover::hideEvent(QHideEvent* event) {
    QWidget::hideEvent(event);
    if (window_)
        window_->removeEventFilter(this);
    if (anchor_)
        anchor_->removeEventFilter(this);
    emit closed();
}

void Popover::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        dismiss();
        if (anchor_)
            anchor_->setFocus(Qt::PopupFocusReason);
        return;
    }
    QWidget::keyPressEvent(event);
}

bool Popover::eventFilter(QObject* watched, QEvent* event) {
    if (watched == window_ && event->type() == QEvent::MouseButtonPress) {
        const QPoint at = static_cast<QMouseEvent*>(event)->position().toPoint();
        const QRect surface = geometry().marginsRemoved(shadowMargins());
        const bool onAnchor =
            anchor_ &&
            QRect(anchor_->mapTo(window_.data(), QPoint()), anchor_->size()).contains(at);
        if (!surface.contains(at) && !onAnchor)
            dismiss();
    } else if (watched == window_ && event->type() == QEvent::Resize) {
        place();
    } else if (watched == anchor_ &&
               (event->type() == QEvent::Move || event->type() == QEvent::Hide)) {
        if (event->type() == QEvent::Hide)
            dismiss();
        else
            place();
    }
    return QWidget::eventFilter(watched, event);
}

void Popover::paintEvent(QPaintEvent*) {
    const PopoverRecipe r = recipe();
    const QRectF surface = QRectF(rect()).marginsRemoved(QMarginsF(shadowMargins()));
    const qreal radius = scaled(r.radius);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    detail::paintShadow(painter, surface, radius, detail::scaled(r.shadow));
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(surface.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

} // namespace ket
