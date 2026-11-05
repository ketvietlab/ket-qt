// SPDX-License-Identifier: MPL-2.0
#include "KetChrome.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

QFont sized(const QWidget* widget, qreal pixels) {
    QFont font = widget->font();
    font.setPixelSize(qRound(scaled(pixels)));
    return font;
}

// Draws `icon` then `text` from the left, vertically centred; returns the width used.
qreal drawIconText(QPainter& painter, const QWidget* widget, const QString& icon,
                   const QString& text, qreal iconSize, qreal gap, const QFont& font,
                   const QColor& color, qreal width) {
    qreal x = 0;
    if (!icon.isEmpty()) {
        const qreal box = scaled(iconSize);
        painter.drawPixmap(QPointF(0, qRound((widget->height() - box) / 2)),
                           Icons::pixmap(icon, box, widget->devicePixelRatioF(), color));
        x += box + scaled(gap);
    }
    const QFontMetricsF metrics(font);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(QRectF(x, 0, width - x, widget->height()), Qt::AlignLeft | Qt::AlignVCenter,
                     metrics.elidedText(text, Qt::ElideRight, width - x));
    return x + metrics.horizontalAdvance(text);
}

// `.file-breadcrumb`'s path: "a / b" › "file".
class Trail final : public QWidget {
  public:
    explicit Trail(const Breadcrumb* owner)
        : QWidget(const_cast<Breadcrumb*>(owner)), owner_(owner) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    [[nodiscard]] QSize sizeHint() const override {
        const BreadcrumbRecipe r = breadcrumbRecipe(theme());
        const QStringList path = owner_->path();
        const QFontMetricsF metrics(sized(this, r.fontPixels));
        const QString folders = path.mid(0, path.size() - 1).join(QStringLiteral(" / "));
        const qreal width = metrics.horizontalAdvance(folders) + 2 * scaled(r.spacing) +
                            scaled(r.iconSize) +
                            metrics.horizontalAdvance(path.value(path.size() - 1));
        return {qCeil(width), qCeil(metrics.height())};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const BreadcrumbRecipe r = breadcrumbRecipe(theme());
        const QStringList path = owner_->path();
        if (path.isEmpty())
            return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const QFont font = sized(this, r.fontPixels);
        const QFontMetricsF metrics(font);
        painter.setFont(font);
        painter.setPen(r.text);
        qreal x = 0;
        if (path.size() > 1) {
            const QString folders = path.mid(0, path.size() - 1).join(QStringLiteral(" / "));
            const QString shown = metrics.elidedText(folders, Qt::ElideLeft, width() / 2.0);
            painter.drawText(QRectF(0, 0, width(), height()), Qt::AlignLeft | Qt::AlignVCenter,
                             shown);
            x = metrics.horizontalAdvance(shown) + scaled(r.spacing);
            const qreal box = scaled(r.iconSize);
            painter.drawPixmap(QPointF(qRound(x), qRound((height() - box) / 2)),
                               Icons::pixmap(u"chevron-right", box, devicePixelRatioF(), r.text));
            x += box + scaled(r.spacing);
        }
        painter.drawText(QRectF(x, 0, width() - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         metrics.elidedText(path.constLast(), Qt::ElideMiddle, width() - x));
    }

  private:
    const Breadcrumb* owner_;
};

class PaneTitle final : public QWidget {
  public:
    explicit PaneTitle(const PaneHeader* owner)
        : QWidget(const_cast<PaneHeader*>(owner)), owner_(owner) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    QString iconCode;

  protected:
    void paintEvent(QPaintEvent*) override {
        const PaneHeaderRecipe r = paneHeaderRecipe(theme());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        qreal x = 0;
        if (!iconCode.isEmpty()) {
            const qreal box = scaled(r.iconSize);
            painter.drawPixmap(QPointF(0, qRound((height() - box) / 2)),
                               Icons::pixmap(iconCode, box, devicePixelRatioF(), r.icon));
            x = box + scaled(r.spacing);
        }
        const QFont font = sized(this, r.fontPixels);
        painter.setFont(font);
        painter.setPen(r.text);
        painter.drawText(
            QRectF(x, 0, width() - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
            QFontMetricsF(font).elidedText(owner_->title(), Qt::ElideMiddle, width() - x));
    }

  private:
    const PaneHeader* owner_;
};

} // namespace

StatusItem::StatusItem(const QString& text, const QString& iconCode, QWidget* parent)
    : QWidget(parent), text_(text), iconCode_(iconCode) {
    setAccessibleName(text);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void StatusItem::setText(const QString& text) {
    text_ = text;
    setAccessibleName(text);
    updateGeometry();
    update();
}

void StatusItem::setIconCode(const QString& code) {
    iconCode_ = code;
    updateGeometry();
    update();
}

QFont StatusItem::textFont() const { return sized(this, statusBarRecipe(theme()).fontPixels); }

QSize StatusItem::sizeHint() const {
    const StatusBarRecipe r = statusBarRecipe(theme());
    const QFontMetricsF metrics(textFont());
    qreal width = metrics.horizontalAdvance(text_);
    if (!iconCode_.isEmpty())
        width += scaled(r.iconSize) + (text_.isEmpty() ? 0 : scaled(r.itemGap));
    return {qCeil(width), qCeil(metrics.height())};
}

void StatusItem::paintEvent(QPaintEvent*) {
    const StatusBarRecipe r = statusBarRecipe(theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawIconText(painter, this, iconCode_, text_, r.iconSize, r.itemGap, textFont(), r.text,
                 width());
}

StatusBar::StatusBar(QWidget* parent)
    : QWidget(parent), left_(new QHBoxLayout), right_(new QHBoxLayout) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* row = new QHBoxLayout(this);
    row->setSpacing(0);
    row->addLayout(left_);
    row->addStretch(1);
    row->addLayout(right_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, [this] { update(); });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &StatusBar::applyRecipe);
    applyRecipe();
}

StatusItem* StatusBar::addItem(const QString& text, const QString& iconCode, Side side) {
    auto* item = new StatusItem(text, iconCode, this);
    addWidget(item, side);
    return item;
}

void StatusBar::addWidget(QWidget* widget, Side side) {
    (side == Side::Left ? left_ : right_)->addWidget(widget, 0, Qt::AlignVCenter);
}

void StatusBar::applyRecipe() {
    const StatusBarRecipe r = statusBarRecipe(theme());
    const int padding = qRound(scaled(r.paddingInline));
    layout()->setContentsMargins(padding, 1, padding, 0); // 1: the top hairline
    left_->setSpacing(qRound(scaled(r.groupGap)));
    right_->setSpacing(qRound(scaled(r.groupGap)));
    setFixedHeight(qRound(scaled(r.height)));
    update();
}

void StatusBar::paintEvent(QPaintEvent*) {
    const StatusBarRecipe r = statusBarRecipe(theme());
    QPainter painter(this);
    painter.fillRect(rect(), r.background);
    painter.fillRect(QRect(0, 0, width(), 1), r.border);
}

Breadcrumb::Breadcrumb(QWidget* parent)
    : QWidget(parent), trail_(new Trail(this)), actions_(new QHBoxLayout) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* row = new QHBoxLayout(this);
    row->addWidget(trail_, 1);
    row->addLayout(actions_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, [this] {
        update();
        trail_->update();
    });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Breadcrumb::applyRecipe);
    applyRecipe();
}

void Breadcrumb::setPath(const QStringList& path) {
    path_ = path;
    setAccessibleName(path.join(QLatin1Char('/')));
    trail_->updateGeometry();
    trail_->update();
}

void Breadcrumb::applyRecipe() {
    const BreadcrumbRecipe r = breadcrumbRecipe(theme());
    const int padding = qRound(scaled(r.paddingInline));
    layout()->setContentsMargins(padding, 0, padding, 1); // 1: the bottom hairline
    layout()->setSpacing(qRound(scaled(r.spacing)));
    actions_->setSpacing(qRound(scaled(r.spacing)));
    setFixedHeight(qRound(scaled(r.height)));
    trail_->updateGeometry();
    update();
}

void Breadcrumb::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(QRect(0, height() - 1, width(), 1), breadcrumbRecipe(theme()).border);
}

PaneHeader::PaneHeader(const QString& title, const QString& iconCode, QWidget* parent)
    : QWidget(parent), title_(title), iconCode_(iconCode), label_(new PaneTitle(this)),
      actions_(new QHBoxLayout) {
    static_cast<PaneTitle*>(label_)->iconCode = iconCode;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAccessibleName(title);
    auto* row = new QHBoxLayout(this);
    row->addWidget(label_, 1);
    row->addLayout(actions_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, [this] {
        update();
        label_->update();
    });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &PaneHeader::applyRecipe);
    applyRecipe();
}

void PaneHeader::setTitle(const QString& title) {
    title_ = title;
    setAccessibleName(title);
    label_->update();
}

void PaneHeader::setIconCode(const QString& code) {
    iconCode_ = code;
    static_cast<PaneTitle*>(label_)->iconCode = code;
    label_->update();
}

void PaneHeader::applyRecipe() {
    const PaneHeaderRecipe r = paneHeaderRecipe(theme());
    layout()->setContentsMargins(qRound(scaled(r.paddingStart)), 0, qRound(scaled(r.paddingEnd)),
                                 1);
    layout()->setSpacing(qRound(scaled(r.spacing)));
    actions_->setSpacing(qRound(scaled(Tokens::px(u"space-1"))));
    setFixedHeight(qRound(scaled(r.height)));
    update();
}

void PaneHeader::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(QRect(0, height() - 1, width(), 1), paneHeaderRecipe(theme()).border);
}

} // namespace ket
