// SPDX-License-Identifier: MPL-2.0
#include "KetToast.h"
#include "KetNativeOverlay.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetShadow.h"

#include <QAccessible>
#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QTextLayout>
#include <QtMath>

namespace ket {
namespace {

constexpr char kCurrentToast[] = "_ket_toast";

} // namespace

// Window-owned reservation survives individual toast lifetimes. Watching application
// geometry events also catches an ancestor moving without the region itself moving.
class ToastPlacement final : public QObject {
  public:
    explicit ToastPlacement(QWidget* window) : QObject(window), window_(window) {
        qApp->installEventFilter(this);
    }
    QPointer<QWidget> region;
    void refresh() {
        const auto toasts = window_->findChildren<Toast*>(QString(), Qt::FindDirectChildrenOnly);
        for (auto* toast : toasts)
            toast->place();
    }

  protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
        case QEvent::Show:
        case QEvent::Hide:
        case QEvent::ParentChange:
            if (auto* widget = qobject_cast<QWidget*>(watched);
                region && (widget == region || (widget && widget->isAncestorOf(region))))
                refresh();
            break;
        default:
            break;
        }
        return false;
    }

  private:
    QWidget* window_;
};

void Toast::setBottomAvoidance(QWidget* window, QWidget* region) {
    if (!window)
        return;
    window = window->window();
    if (region && (region == window || region->window() != window))
        return;
    auto* placement = static_cast<ToastPlacement*>(window->findChild<QObject*>(
        QStringLiteral("_ket_toast_placement"), Qt::FindDirectChildrenOnly));
    if (!placement) {
        placement = new ToastPlacement(window);
        placement->setObjectName(QStringLiteral("_ket_toast_placement"));
    }
    if (placement->region)
        disconnect(placement->region, &QObject::destroyed, placement, nullptr);
    placement->region = region;
    if (region)
        connect(region, &QObject::destroyed, placement, [placement] {
            QTimer::singleShot(0, placement, [placement] { placement->refresh(); });
        });
    placement->refresh();
}

Toast::Toast(const QString& text, QWidget* anchor, const ToastTone tone)
    : QWidget(anchor->window()), text_(text), tone_(tone) {
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

Toast* Toast::showMessage(QWidget* anchor, const QString& text, const ToastTone tone) {
    QWidget* window = anchor->window();
    if (auto* previous = window->property(kCurrentToast).value<QObject*>())
        previous->deleteLater();
    auto* toast = new Toast(text, anchor, tone);
    window->setProperty(kCurrentToast, QVariant::fromValue<QObject*>(toast));
    // The property holds a plain pointer: forget it once the toast is gone (it expired or its
    // window deleted it), or the next message would delete it a second time.
    connect(toast, &QObject::destroyed, window, [window, toast] {
        if (window->property(kCurrentToast).value<QObject*>() == toast)
            window->setProperty(kCurrentToast, QVariant());
    });
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
    // Above a window container the toast is a native child, drawing the window beneath its
    // shadow from a snapshot.
    underlay_ = {};
    if (detail::hasNativeChildren(parentWidget(), this)) {
        const QPixmap window = detail::windowSnapshot(parentWidget());
        const qreal dpr = window.devicePixelRatio();
        underlay_ =
            window.copy(QRectF(QRectF(geometry()).topLeft() * dpr, QSizeF(size()) * dpr).toRect());
        underlay_.setDevicePixelRatio(dpr);
        detail::makeNativeOverlay(this);
    }
    show();
    raise();
    QAccessibleEvent event(this, QAccessible::Alert);
    QAccessible::updateAccessibility(&event);
    if (durationMs > 0)
        timer_.start(durationMs);
    else
        timer_.stop();
}

ToastRecipe Toast::recipe() const { return toastRecipe(Appearance::instance().theme(), tone_); }

QFont Toast::toastFont() const {
    QFont result = font();
    result.setPixelSize(qRound(Appearance::instance().scaled(recipe().fontPixels)));
    return result;
}

QMargins Toast::margins() const {
    const QMarginsF m = detail::shadowMargins(detail::scaled(recipe().shadow));
    return {qCeil(m.left()), qCeil(m.top()), qCeil(m.right()), qCeil(m.bottom())};
}

namespace {

// Lays the message out in lines no wider than width. Words break at spaces, and a run without
// one (a URL, a path) breaks anywhere, so the text never runs past the card.
QSizeF layoutText(QTextLayout& layout, const qreal width) {
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);
    layout.beginLayout();
    qreal height = 0;
    qreal widest = 0;
    for (QTextLine line = layout.createLine(); line.isValid(); line = layout.createLine()) {
        line.setLineWidth(width);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widest = qMax(widest, line.naturalTextWidth());
    }
    layout.endLayout();
    return {widest, height};
}

} // namespace

QRectF Toast::textBox() const {
    const ToastRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const qreal chrome = 2 * appearance.scaled(r.paddingInline) + appearance.scaled(r.iconSize) +
                         appearance.scaled(r.spacing);
    qreal card = appearance.scaled(r.maxWidth);
    if (const QWidget* window = parentWidget())
        card = qMin(card, window->width() - 2 * appearance.scaled(r.sideMargin));
    QTextLayout layout(text_, toastFont());
    return {QPointF(0, 0), layoutText(layout, qMax<qreal>(1, card - chrome))};
}

QSize Toast::sizeHint() const {
    const ToastRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const QRectF text = textBox();
    const qreal width = 2 * appearance.scaled(r.paddingInline) + appearance.scaled(r.iconSize) +
                        appearance.scaled(r.spacing) + text.width();
    const qreal height =
        2 * appearance.scaled(r.paddingBlock) + qMax(text.height(), appearance.scaled(r.iconSize));
    const QMargins m = margins();
    return {qCeil(width) + m.left() + m.right(), qCeil(height) + m.top() + m.bottom()};
}

void Toast::place() {
    QWidget* window = parentWidget();
    const QSize size = sizeHint();
    const int bottom = qRound(Appearance::instance().scaled(recipe().bottomOffset));
    int bottomEdge = window->height() - bottom + margins().bottom();
    auto* placement = static_cast<ToastPlacement*>(window->findChild<QObject*>(
        QStringLiteral("_ket_toast_placement"), Qt::FindDirectChildrenOnly));
    if (placement && placement->region && placement->region->window() == window &&
        placement->region->isVisibleTo(window))
        bottomEdge = qMin(bottomEdge, placement->region->mapTo(window, QPoint()).y() - bottom);
    setGeometry((window->width() - size.width()) / 2, bottomEdge - size.height(), size.width(),
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
    if (!underlay_.isNull())
        painter.drawPixmap(rect(), underlay_);
    painter.setRenderHint(QPainter::Antialiasing);
    detail::paintShadow(painter, card, radius, detail::scaled(r.shadow));
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(card.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

    const qreal iconBox = appearance.scaled(r.iconSize);
    const qreal x = card.left() + appearance.scaled(r.paddingInline);
    const qreal dpr = devicePixelRatioF();
    // The icon sits on the first line, so a wrapped message reads from its symbol.
    const QFontMetricsF metrics(toastFont());
    const qreal textTop = card.top() + appearance.scaled(r.paddingBlock);
    const qreal firstLine = qMax(metrics.height(), iconBox);
    painter.drawPixmap(QPointF(qRound(x), qRound(textTop + (firstLine - iconBox) / 2)),
                       Icons::pixmap(r.iconName, iconBox, dpr, r.icon));
    painter.setFont(toastFont());
    painter.setPen(r.text);
    const qreal textLeft = x + iconBox + appearance.scaled(r.spacing);
    QTextLayout layout(text_, toastFont());
    const QSizeF text = layoutText(layout, card.right() - textLeft);
    layout.draw(&painter, QPointF(textLeft, card.top() + (card.height() - text.height()) / 2));
}

} // namespace ket
