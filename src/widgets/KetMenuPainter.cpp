// SPDX-License-Identifier: MPL-2.0
#include "KetMenuPainter.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetRecipes.h"
#include "KetStyle.h"

#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include <QtMath>

namespace ket::detail {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

// The QAction an option describes: by position while painting, by text while sizing
// (QMenu sizes items with the whole menu's rect in the option).
const QAction* actionFor(const QStyleOptionMenuItem* option, const QWidget* widget, bool painting) {
    const auto* menu = qobject_cast<const QMenu*>(widget);
    if (!menu)
        return nullptr;
    if (painting)
        if (const QAction* action = menu->actionAt(option->rect.center()))
            return action;
    const QString text = option->text.section(QLatin1Char('\t'), 0, 0);
    for (const QAction* action : menu->actions())
        if (action->text() == text)
            return action;
    return nullptr;
}

QString descriptionOf(const QAction* action) {
    return action ? action->property(Style::kDescriptionProperty).toString() : QString();
}

bool isDanger(const QAction* action) {
    return action && action->property(Style::kDangerProperty).toBool();
}

QFont itemFont(const QStyleOptionMenuItem* option, qreal pixels) {
    QFont font = option->font;
    font.setPixelSize(qRound(scaled(pixels)));
    return font;
}

// A translucent popup window starts from cleared pixels; a panel drawn inside another
// widget (a preview) paints over its parent instead.
void clearWindow(const QStyleOption* option, QPainter* painter, const QWidget* widget) {
    if (!widget || !widget->isWindow())
        return;
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(option->rect, Qt::transparent);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
}

} // namespace

int menuPanelPadding() { return qRound(scaled(menuRecipe(theme()).padding)) - 1; }

void drawMenuPanel(const QStyleOption* option, QPainter* painter, const QWidget* widget) {
    const MenuRecipe r = menuRecipe(theme());
    const qreal radius = scaled(r.radius);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    clearWindow(option, painter, widget);
    painter->setPen(QPen(r.border, 1));
    painter->setBrush(r.background);
    painter->drawRoundedRect(QRectF(option->rect).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter->restore();
}

void drawTooltipPanel(const QStyleOption* option, QPainter* painter, const QWidget* widget) {
    const TooltipRecipe r = tooltipRecipe(theme());
    const qreal radius = scaled(r.radius);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    clearWindow(option, painter, widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(r.background);
    painter->drawRoundedRect(QRectF(option->rect), radius, radius);
    painter->restore();
}

QSize menuItemSize(const QStyleOptionMenuItem* option, const QWidget* widget) {
    const MenuRecipe menu = menuRecipe(theme());
    if (option->menuItemType == QStyleOptionMenuItem::Separator)
        return {1, 1 + 2 * qRound(scaled(menu.separatorMargin))};

    const QAction* action = actionFor(option, widget, false);
    const MenuItemRecipe r = menuItemRecipe(false, true, false, theme());
    const QFontMetricsF titleMetrics(itemFont(option, r.fontPixels));
    const QString title = option->text.section(QLatin1Char('\t'), 0, 0);
    const QString shortcut = option->text.section(QLatin1Char('\t'), 1);
    const QString description = descriptionOf(action);

    qreal width = scaled(r.paddingStart) + scaled(r.paddingEnd) +
                  titleMetrics.horizontalAdvance(QString(title).remove(QLatin1Char('&')));
    if (option->menuHasCheckableItems)
        width += scaled(r.checkSize) + scaled(r.spacing);
    if (option->maxIconWidth > 0 && !option->icon.isNull())
        width += scaled(r.iconSize) + scaled(r.spacing);
    if (!shortcut.isEmpty())
        width += 4 * scaled(r.spacing) + titleMetrics.horizontalAdvance(shortcut);
    if (option->menuItemType == QStyleOptionMenuItem::SubMenu)
        width += scaled(r.iconSize) + scaled(r.spacing);

    qreal height = scaled(r.height);
    if (!description.isEmpty()) {
        const QFontMetricsF descriptionMetrics(itemFont(option, r.descriptionPixels));
        width =
            qMax(width,
                 scaled(r.paddingStart) + scaled(r.paddingEnd) +
                     (option->menuHasCheckableItems ? scaled(r.checkSize) + scaled(r.spacing) : 0) +
                     descriptionMetrics.horizontalAdvance(description));
        height =
            qMax(height, titleMetrics.height() + scaled(r.descriptionGap) +
                             descriptionMetrics.lineSpacing() * 1.5 + 2 * scaled(r.paddingStart));
    }
    width = qBound(scaled(menu.minWidth) - 2 * scaled(menu.padding), width,
                   scaled(menu.maxWidth) - 2 * scaled(menu.padding));
    return {qCeil(width), qCeil(height)};
}

void drawMenuItem(const QStyleOptionMenuItem* option, QPainter* painter, const QWidget* widget) {
    const QRectF rect(option->rect);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (option->menuItemType == QStyleOptionMenuItem::Separator) {
        const qreal y = qFloor(rect.center().y()) + 0.5;
        painter->setPen(QPen(menuRecipe(theme()).separator, 1));
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right() + 1, y));
        painter->restore();
        return;
    }
    if (option->menuItemType == QStyleOptionMenuItem::EmptyArea) {
        painter->restore();
        return;
    }

    const QAction* action = actionFor(option, widget, true);
    const bool enabled = option->state & QStyle::State_Enabled;
    const bool highlighted = option->state & QStyle::State_Selected;
    const MenuItemRecipe r = menuItemRecipe(highlighted, enabled, isDanger(action), theme());
    const qreal dpr = painter->device()->devicePixelRatioF();
    painter->setOpacity(r.opacity);

    if (r.background.alpha() > 0) {
        const qreal radius = scaled(r.radius);
        painter->setPen(Qt::NoPen);
        painter->setBrush(r.background);
        painter->drawRoundedRect(rect, radius, radius);
    }

    const qreal spacing = scaled(r.spacing);
    qreal x = rect.left() + scaled(r.paddingStart);
    const qreal right = rect.right() + 1 - scaled(r.paddingEnd);
    const QString description = descriptionOf(action);
    const QFont titleFont = itemFont(option, r.fontPixels);
    const QFontMetricsF titleMetrics(titleFont);
    // With a description the title sits on the first line; the check aligns with it.
    const qreal titleCenter = description.isEmpty()
                                  ? rect.center().y()
                                  : rect.top() + scaled(r.paddingStart) + titleMetrics.height() / 2;

    const auto drawIcon = [&](QStringView code, qreal box, qreal left) {
        painter->drawPixmap(QPointF(qRound(left), qRound(titleCenter - box / 2)),
                            Icons::pixmap(code, box, dpr, r.text));
    };

    if (option->menuHasCheckableItems) {
        const qreal check = scaled(r.checkSize);
        if (option->checked)
            drawIcon(u"check", scaled(r.iconSize), x + (check - scaled(r.iconSize)) / 2);
        x += check + spacing;
    }
    if (option->maxIconWidth > 0 && !option->icon.isNull()) {
        const qreal box = scaled(r.iconSize);
        const QPixmap pixmap = option->icon.pixmap(QSizeF(box, box).toSize(), dpr);
        // Kit icons are black masks; tint them like the text, as buttons do.
        QPixmap tinted(pixmap.size());
        tinted.setDevicePixelRatio(pixmap.devicePixelRatio());
        tinted.fill(Qt::transparent);
        QPainter tint(&tinted);
        tint.drawPixmap(0, 0, pixmap);
        tint.setCompositionMode(QPainter::CompositionMode_SourceIn);
        tint.fillRect(tinted.rect(), r.text);
        tint.end();
        painter->drawPixmap(QPointF(qRound(x), qRound(titleCenter - box / 2)), tinted);
        x += box + spacing;
    }

    qreal textRight = right;
    if (option->menuItemType == QStyleOptionMenuItem::SubMenu) {
        const qreal box = scaled(r.iconSize);
        drawIcon(u"chevron-right", box, right - box);
        textRight -= box + spacing;
    }

    const QString title = option->text.section(QLatin1Char('\t'), 0, 0);
    const QString shortcut = option->text.section(QLatin1Char('\t'), 1);
    painter->setFont(titleFont);
    if (!shortcut.isEmpty()) {
        painter->setPen(menuItemRecipe(highlighted, enabled, false, theme()).description);
        const qreal shortcutWidth = titleMetrics.horizontalAdvance(shortcut);
        painter->drawText(QRectF(textRight - shortcutWidth, titleCenter - titleMetrics.height() / 2,
                                 shortcutWidth, titleMetrics.height()),
                          Qt::AlignRight | Qt::AlignVCenter, shortcut);
        textRight -= shortcutWidth + 4 * spacing;
    }
    painter->setPen(r.text);
    const QRectF titleRect(x, titleCenter - titleMetrics.height() / 2, textRight - x,
                           titleMetrics.height());
    painter->drawText(
        titleRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
        titleMetrics.elidedText(title, Qt::ElideRight, titleRect.width(), Qt::TextShowMnemonic));
    if (!description.isEmpty()) {
        painter->setFont(itemFont(option, r.descriptionPixels));
        painter->setPen(r.description);
        painter->drawText(QRectF(x, titleRect.bottom() + scaled(r.descriptionGap), right - x,
                                 rect.bottom() - titleRect.bottom()),
                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, description);
    }
    painter->restore();
}

} // namespace ket::detail
