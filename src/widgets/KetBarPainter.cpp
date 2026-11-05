// SPDX-License-Identifier: MPL-2.0
#include "KetBarPainter.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetProfile.h"
#include "KetRecipes.h"

#include <QApplication>
#include <QDateTime>
#include <QPainter>
#include <QPointer>
#include <QStyleOption>
#include <QTimer>
#include <QWidget>
#include <QtMath>

namespace ket::detail {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

InteractionState stateOf(const QStyleOption* option) {
    return {bool(option->state & QStyle::State_MouseOver), false,
            bool(option->state & QStyle::State_Enabled),
            bool(option->state & QStyle::State_HasFocus)};
}

TabRecipe recipeOf(const QStyleOptionTab* option) {
    return tabRecipe(option->state & QStyle::State_Selected, stateOf(option), theme());
}

QFont tabFont(const QStyleOptionTab*, const QWidget* widget, const TabRecipe& r) {
    QFont font = widget ? widget->font() : QApplication::font();
    font.setPixelSize(qRound(scaled(r.fontPixels)));
    return font;
}

// Kit icons are black masks; tint them like the text, as buttons do.
QPixmap tinted(const QIcon& icon, qreal box, qreal dpr, const QColor& color) {
    QPixmap pixmap = icon.pixmap(QSizeF(box, box).toSize(), dpr);
    QPainter tint(&pixmap);
    tint.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tint.fillRect(QRectF(QPointF(), QSizeF(pixmap.size()) / pixmap.devicePixelRatio()), color);
    return pixmap;
}

} // namespace

bool isTopTab(const QStyleOption* option) {
    if (const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option))
        return tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::TriangularNorth;
    if (const auto* base = qstyleoption_cast<const QStyleOptionTabBarBase*>(option))
        return base->shape == QTabBar::RoundedNorth || base->shape == QTabBar::TriangularNorth;
    return false;
}

void drawTabBarBase(const QStyleOption* option, QPainter* painter) {
    const TabBarRecipe r = tabBarRecipe(theme());
    painter->save();
    painter->fillRect(option->rect, r.background);
    painter->setPen(QPen(r.border, 1));
    const qreal y = option->rect.bottom() + 0.5;
    painter->drawLine(QPointF(option->rect.left(), y), QPointF(option->rect.right() + 1, y));
    painter->restore();
}

void drawTab(const QStyleOptionTab* option, QPainter* painter, const QWidget*) {
    const TabRecipe r = recipeOf(option);
    const TabBarRecipe bar = tabBarRecipe(theme());
    const QRectF rect(option->rect);
    painter->save();
    painter->setOpacity(r.opacity);
    // The strip's own background and bottom edge, then the tab's.
    painter->fillRect(rect, bar.background);
    painter->fillRect(rect, r.background);
    // QRectF edges are x + width: the hairlines sit half a pixel inside them.
    painter->setPen(QPen(bar.border, 1));
    const qreal right = rect.right() - 0.5;
    painter->drawLine(QPointF(right, rect.top()), QPointF(right, rect.bottom()));
    const qreal indicator = scaled(r.indicatorHeight);
    if (r.indicator.alpha())
        painter->fillRect(QRectF(rect.left(), rect.bottom() - indicator, rect.width(), indicator),
                          r.indicator);
    else
        painter->drawLine(QPointF(rect.left(), rect.bottom() - 0.5),
                          QPointF(rect.right(), rect.bottom() - 0.5));
    if (r.focusRing.alpha()) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(r.focusRing, 2));
        painter->drawRect(rect.adjusted(1, 1, -1, -1 - indicator));
    }
    painter->restore();
}

void drawTabLabel(const QStyleOptionTab* option, QPainter* painter, const QWidget* widget) {
    const TabRecipe r = recipeOf(option);
    QRectF rect(option->rect);
    rect.adjust(scaled(r.paddingInline), 0, -scaled(r.paddingInline), 0);
    if (!option->leftButtonSize.isEmpty())
        rect.setLeft(rect.left() + option->leftButtonSize.width() + scaled(r.spacing));
    if (!option->rightButtonSize.isEmpty())
        rect.setRight(rect.right() - option->rightButtonSize.width() - scaled(r.spacing));
    painter->save();
    painter->setOpacity(r.opacity);
    if (!option->icon.isNull()) {
        const qreal box = scaled(r.iconSize);
        const qreal dpr =
            widget ? widget->devicePixelRatioF() : painter->device()->devicePixelRatioF();
        painter->drawPixmap(QPointF(qRound(rect.left()), qRound(rect.center().y() - box / 2)),
                            tinted(option->icon, box, dpr, r.text));
        rect.setLeft(rect.left() + box + scaled(r.spacing));
    }
    const QFont font = tabFont(option, widget, r);
    painter->setFont(font);
    painter->setPen(r.text);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                      QFontMetricsF(font).elidedText(option->text, Qt::ElideRight, rect.width(),
                                                     Qt::TextShowMnemonic));
    painter->restore();
}

QSize tabSize(const QStyleOptionTab* option, const QSize&, const QWidget* widget) {
    const TabRecipe r = recipeOf(option);
    qreal width = 2 * scaled(r.paddingInline) +
                  QFontMetricsF(tabFont(option, widget, r))
                      .horizontalAdvance(QString(option->text).remove(QLatin1Char('&')));
    if (!option->icon.isNull())
        width += scaled(r.iconSize) + scaled(r.spacing);
    if (!option->leftButtonSize.isEmpty())
        width += option->leftButtonSize.width() + scaled(r.spacing);
    if (!option->rightButtonSize.isEmpty())
        width += option->rightButtonSize.width() + scaled(r.spacing);
    return {qCeil(width), qRound(scaled(r.height))};
}

void drawTabClose(const QStyleOption* option, QPainter* painter, const QWidget* widget) {
    const bool hovered = option->state & QStyle::State_MouseOver;
    const TabRecipe r =
        tabRecipe(option->state & QStyle::State_Selected,
                  {hovered, false, bool(option->state & QStyle::State_Enabled), false}, theme());
    const qreal box = scaled(r.iconSize);
    const QRectF rect(option->rect);
    const qreal dpr = widget ? widget->devicePixelRatioF() : painter->device()->devicePixelRatioF();
    painter->save();
    if (hovered) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(Tokens::color(u"surface-hover", theme()));
        const qreal radius = scaled(Tokens::px(u"radius-sm"));
        painter->drawRoundedRect(rect, radius, radius);
    }
    painter->drawPixmap(
        QPointF(qRound(rect.center().x() - box / 2), qRound(rect.center().y() - box / 2)),
        Icons::pixmap(u"close", box, dpr, r.text));
    painter->restore();
}

int progressHeight() { return qRound(scaled(progressRecipe(theme()).height)); }

void drawProgressBar(const QStyleOptionProgressBar* option, QPainter* painter,
                     const QWidget* widget) {
    const ProgressRecipe r = progressRecipe(theme());
    const qreal height = scaled(r.height);
    const QRectF track(option->rect.left(), option->rect.center().y() - height / 2 + 0.5,
                       option->rect.width(), height);
    const qreal radius = height / 2;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    if (!(option->state & QStyle::State_Enabled))
        painter->setOpacity(Profile::current().disabledOpacity);
    painter->setBrush(r.track);
    painter->drawRoundedRect(track, radius, radius);
    painter->setClipRect(track);
    painter->setBrush(r.fill);
    const bool busy = option->minimum == option->maximum;
    const bool reversed = (option->direction == Qt::RightToLeft) != option->invertedAppearance;
    if (busy) {
        // A segment sweeps across; repaint while it is shown.
        const qreal segment = track.width() * r.busyFraction;
        const qreal phase = qreal(QDateTime::currentMSecsSinceEpoch() % qMax(1, r.busyDurationMs)) /
                            qMax(1, r.busyDurationMs);
        const qreal x = track.left() - segment + (track.width() + segment) * phase;
        painter->drawRoundedRect(QRectF(x, track.top(), segment, height), radius, radius);
        if (widget)
            QTimer::singleShot(16, const_cast<QWidget*>(widget),
                               [target = QPointer<QWidget>(const_cast<QWidget*>(widget))] {
                                   if (target && target->isVisible())
                                       target->update();
                               });
    } else if (option->progress > option->minimum) {
        const qreal fraction =
            qreal(option->progress - option->minimum) / qreal(option->maximum - option->minimum);
        const qreal width = qBound<qreal>(0, track.width() * fraction, track.width());
        const QRectF fill = reversed ? QRectF(track.right() - width, track.top(), width, height)
                                     : QRectF(track.left(), track.top(), width, height);
        painter->drawRoundedRect(fill, radius, radius);
    }
    painter->restore();
}

} // namespace ket::detail
