// SPDX-License-Identifier: MPL-2.0
#include "KetToast.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetShadow.h"

#include <QAccessible>
#include <QEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

constexpr char kCurrentToast[] = "_ket_toast";

} // namespace

Toast::Toast(const QString& text, QWidget* anchor) : QWidget(anchor->window()), text_(text) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
    setAccessibleName(text);
    hide();
    parentWidget()->installEventFilter(this);
    timer_.setSingleShot(true);
    connect(&timer_, &QTimer::timeout, this, &QWidget::hide);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Toast::place);
}

Toast* Toast::showMessage(QWidget* anchor, const QString& text) {
    QWidget* window = anchor->window();
    if (auto* previous = window->property(kCurrentToast).value<QObject*>())
        previous->deleteLater();
    auto* toast = new Toast(text, anchor);
    window->setProperty(kCurrentToast, QVariant::fromValue<QObject*>(toast));
    connect(&toast->timer_, &QTimer::timeout, toast, &QObject::deleteLater);
    toast->popup(toast->recipe().durationMs);
    return toast;
}

void Toast::setText(const QString& text) {
    if (text_ == text)
        return;
    text_ = text;
    setAccessibleName(text);
    place();
    update();
}

void Toast::popup(int durationMs) {
    place();
    show();
    raise();
    QAccessibleEvent event(this, QAccessible::Alert);
    QAccessible::updateAccessibility(&event);
    if (durationMs > 0)
        timer_.start(durationMs);
    else
        timer_.stop();
}

ToastRecipe Toast::recipe() const { return toastRecipe(Appearance::instance().theme()); }

QFont Toast::toastFont() const {
    QFont result = font();
    result.setPixelSize(qRound(Appearance::instance().scaled(recipe().fontPixels)));
    return result;
}

QMargins Toast::margins() const {
    const QMarginsF m = detail::shadowMargins(detail::scaled(recipe().shadow));
    return {qCeil(m.left()), qCeil(m.top()), qCeil(m.right()), qCeil(m.bottom())};
}

QSize Toast::sizeHint() const {
    const ToastRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QFontMetricsF metrics(toastFont());
    const qreal width = 2 * appearance.scaled(r.paddingInline) + appearance.scaled(r.iconSize) +
                        appearance.scaled(r.spacing) + metrics.horizontalAdvance(text_);
    const qreal height = 2 * appearance.scaled(r.paddingBlock) +
                         qMax(metrics.height(), appearance.scaled(r.iconSize));
    const QMargins m = margins();
    return {qCeil(width) + m.left() + m.right(), qCeil(height) + m.top() + m.bottom()};
}

void Toast::place() {
    QWidget* window = parentWidget();
    const QSize size = sizeHint();
    const int bottom = qRound(Appearance::instance().scaled(recipe().bottomOffset));
    setGeometry((window->width() - size.width()) / 2,
                window->height() - bottom - size.height() + margins().bottom(), size.width(),
                size.height());
}

bool Toast::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parentWidget() && event->type() == QEvent::Resize)
        place();
    return QWidget::eventFilter(watched, event);
}

void Toast::paintEvent(QPaintEvent*) {
    const ToastRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QRectF card = QRectF(rect()).marginsRemoved(QMarginsF(margins()));
    const qreal radius = appearance.scaled(r.radius);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    detail::paintShadow(painter, card, radius, detail::scaled(r.shadow));
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(card.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

    const qreal iconBox = appearance.scaled(r.iconSize);
    const qreal x = card.left() + appearance.scaled(r.paddingInline);
    const qreal dpr = devicePixelRatioF();
    painter.drawPixmap(QPointF(qRound(x), qRound(card.center().y() - iconBox / 2)),
                       Icons::pixmap(u"success", iconBox, dpr, r.icon));
    painter.setFont(toastFont());
    painter.setPen(r.text);
    const qreal textLeft = x + iconBox + appearance.scaled(r.spacing);
    painter.drawText(QRectF(textLeft, card.top(), card.right() - textLeft, card.height()),
                     Qt::AlignLeft | Qt::AlignVCenter, text_);
}

} // namespace ket
