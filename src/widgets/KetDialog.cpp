// SPDX-License-Identifier: MPL-2.0
#include "KetDialog.h"
#include "KetNativeOverlay.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"
#include "KetShadow.h"

#include <QAccessible>
#include <QApplication>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>

namespace ket {
namespace {

// The card paints its own surface, header divider and footer band; the dialog paints
// the backdrop and the card's shadow underneath.
class DialogCard final : public QWidget {
  public:
    DialogCard(Dialog* dialog, QWidget* header, QWidget* footer)
        : QWidget(dialog), dialog_(dialog), header_(header), footer_(footer) {}

  protected:
    void paintEvent(QPaintEvent*) override {
        const DialogRecipe r = dialog_->recipe();
        const qreal radius = Appearance::instance().scaled(r.radius);
        const QRectF bounds = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath shape;
        shape.addRoundedRect(bounds, radius, radius);
        painter.fillPath(shape, r.background);
        painter.save();
        painter.setClipPath(shape);
        painter.setPen(QPen(r.divider, 1));
        const qreal headerBottom = header_->geometry().bottom() + 0.5;
        painter.drawLine(QPointF(0, headerBottom), QPointF(width(), headerBottom));
        if (footer_->isVisible()) {
            const qreal footerTop = footer_->geometry().top() + 0.5;
            painter.fillRect(QRectF(0, footerTop, width(), height() - footerTop), r.footer);
            painter.drawLine(QPointF(0, footerTop), QPointF(width(), footerTop));
        }
        painter.restore();
        painter.setPen(QPen(r.border, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(shape);
    }

  private:
    Dialog* dialog_;
    QWidget* header_;
    QWidget* footer_;
};

bool isTabStop(const QWidget* widget) {
    return widget->isVisibleTo(widget->window()) && widget->isEnabled() &&
           (widget->focusPolicy() & Qt::TabFocus);
}

} // namespace

Dialog::Dialog(const QString& title, QWidget* anchor)
    : QWidget(anchor->window()), header_(new QWidget), title_(new QLabel(title)),
      close_(new IconButton(Icons::icon(u"close"), tr("Close"))), bodyLayout_(new QVBoxLayout),
      footer_(new QWidget), footerLayout_(new QHBoxLayout(footer_)) {
    card_ = new DialogCard(this, header_, footer_);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::NoFocus);
    hide();

    title_->setTextFormat(Qt::PlainText);
    title_->setWordWrap(true);
    auto* headerLayout = new QHBoxLayout(header_);
    headerLayout->addWidget(title_, 1);
    headerLayout->addWidget(close_, 0, Qt::AlignVCenter);
    connect(close_, &QAbstractButton::clicked, this, &Dialog::reject);

    footerLayout_->addStretch(1);
    footer_->hide();

    auto* cardLayout = new QVBoxLayout(card_);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(header_);
    cardLayout->addLayout(bodyLayout_, 1);
    cardLayout->addWidget(footer_);

    card_->setAccessibleName(title);
    parentWidget()->installEventFilter(this);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &Dialog::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Dialog::applyRecipe);
    applyRecipe();
}

QString Dialog::title() const { return title_->text(); }

void Dialog::setTitle(const QString& title) {
    title_->setText(title);
    card_->setAccessibleName(title);
}

void Dialog::setBody(QWidget* body) {
    if (body_)
        body_->deleteLater();
    body_ = body;
    if (body_)
        bodyLayout_->addWidget(body_);
    place();
}

Button* Dialog::addButton(const QString& text, ButtonVariant variant) {
    auto* button = new Button(text, variant, footer_);
    footerLayout_->addWidget(button);
    footer_->show();
    place();
    return button;
}

DialogRecipe Dialog::recipe() const { return dialogRecipe(Appearance::instance().theme()); }

void Dialog::applyRecipe() {
    const DialogRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const int inline_ = qRound(appearance.scaled(r.paddingInline));
    const int header = qRound(appearance.scaled(r.headerPaddingBlock));
    const int body = qRound(appearance.scaled(r.bodyPadding));
    const int footer = qRound(appearance.scaled(r.footerPaddingBlock));
    // The dividers are 1 px inside the header's bottom and the footer's top edge.
    header_->layout()->setContentsMargins(inline_, header, inline_, header + 1);
    header_->layout()->setSpacing(qRound(appearance.scaled(r.footerGap)));
    bodyLayout_->setContentsMargins(body, body, body, body);
    footerLayout_->setContentsMargins(inline_, footer + 1, inline_, footer);
    footerLayout_->setSpacing(qRound(appearance.scaled(r.footerGap)));

    QFont titleFont = font();
    titleFont.setPixelSize(qRound(appearance.scaled(r.titlePixels)));
    titleFont.setWeight(QFont::Weight(r.titleWeight));
    title_->setFont(titleFont);
    QPalette palette = title_->palette();
    palette.setColor(QPalette::WindowText, r.title);
    title_->setPalette(palette);
    place();
    update();
}

void Dialog::setCardWidth(const qreal pixels) {
    cardWidth_ = qMax<qreal>(0, pixels);
    if (isVisible())
        place();
}

void Dialog::place() {
    QWidget* window = parentWidget();
    setGeometry(window->rect());
    const DialogRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const int margin = qRound(appearance.scaled(r.margin));
    const qreal pixels = cardWidth_ > 0 ? cardWidth_ : r.width;
    const int width = qMin(qRound(appearance.scaled(pixels)), window->width() - 2 * margin);
    const int wanted = card_->layout()->hasHeightForWidth()
                           ? card_->layout()->totalHeightForWidth(width)
                           : card_->sizeHint().height();
    const int height = qMin(wanted, window->height() - 2 * margin);
    card_->setGeometry((window->width() - width) / 2, (window->height() - height) / 2, width,
                       height);
}

void Dialog::open() {
    restoreFocus_ = QApplication::focusWidget();
    // Above a window container (the Agent Workspace's QQuickView) the dialog is a native child
    // too; it draws what the window showed as it opened beneath its translucent backdrop.
    underlay_ = {};
    if (detail::hasNativeChildren(parentWidget(), this)) {
        underlay_ = detail::windowSnapshot(parentWidget());
        detail::makeNativeOverlay(this);
    }
    place();
    show();
    raise();
    // The first tab stop in the body, else the first footer button, else the close button.
    QWidget* first = nullptr;
    const QList<QWidget*> candidates = card_->findChildren<QWidget*>();
    for (QWidget* widget : candidates) {
        if (widget != close_ && isTabStop(widget) && !footer_->isAncestorOf(widget)) {
            first = widget;
            break;
        }
    }
    if (!first) {
        const QList<Button*> buttons = footer_->findChildren<Button*>();
        first = buttons.isEmpty() ? static_cast<QWidget*>(close_) : buttons.last();
    }
    first->setFocus(Qt::PopupFocusReason);
    QAccessibleEvent event(card_, QAccessible::DialogStart);
    QAccessible::updateAccessibility(&event);
}

void Dialog::accept() { finish(true); }

void Dialog::reject() { finish(false); }

void Dialog::finish(bool accepted) {
    if (!isVisible())
        return;
    QAccessibleEvent event(card_, QAccessible::DialogEnd);
    QAccessible::updateAccessibility(&event);
    hide();
    if (restoreFocus_)
        restoreFocus_->setFocus(Qt::PopupFocusReason);
    if (accepted)
        emit this->accepted();
    else
        emit rejected();
    emit finished(accepted);
}

void Dialog::paintEvent(QPaintEvent*) {
    const DialogRecipe r = recipe();
    QPainter painter(this);
    if (!underlay_.isNull() && underlay_.deviceIndependentSize().toSize() == size())
        painter.drawPixmap(rect(), underlay_);
    painter.fillRect(rect(), r.backdrop);
    detail::paintShadow(painter, QRectF(card_->geometry()), Appearance::instance().scaled(r.radius),
                        detail::scaled(r.shadow));
}

void Dialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Dialog::mousePressEvent(QMouseEvent* event) {
    if (!card_->geometry().contains(event->position().toPoint()))
        reject();
    event->accept();
}

bool Dialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible())
        place();
    return QWidget::eventFilter(watched, event);
}

// Keeps Tab and Shift+Tab inside the card while the dialog is open.
bool Dialog::focusNextPrevChild(bool next) {
    QList<QWidget*> stops;
    for (QWidget* widget = card_->nextInFocusChain(); widget != card_;
         widget = widget->nextInFocusChain()) {
        if (card_->isAncestorOf(widget) && isTabStop(widget))
            stops.append(widget);
        if (stops.size() > 512)
            break;
    }
    if (stops.isEmpty())
        return true;
    const qsizetype current = stops.indexOf(QApplication::focusWidget());
    const qsizetype count = stops.size();
    const qsizetype target = current < 0 ? 0 : (current + (next ? 1 : count - 1)) % count;
    stops.at(target)->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    return true;
}

} // namespace ket
