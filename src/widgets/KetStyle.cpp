// SPDX-License-Identifier: MPL-2.0
#include "KetStyle.h"

#include "KetAppearance.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>

namespace ket {

Style::Style() : QProxyStyle(QStyleFactory::create(QStringLiteral("Fusion"))) {}

void Style::install(QApplication& application) {
    application.setStyle(new Style);
    application.setFont(interfaceFont());
    const auto apply = [&application](Theme theme) { application.setPalette(paletteFor(theme)); };
    apply(Appearance::instance().theme());
    QObject::connect(&Appearance::instance(), &Appearance::themeChanged, &application, apply);
}

QPalette Style::paletteFor(Theme theme) {
    const auto c = [theme](QStringView name) { return Tokens::color(name, theme); };
    QPalette palette;
    palette.setColor(QPalette::Window, c(u"page-bg"));
    palette.setColor(QPalette::WindowText, c(u"text-main"));
    palette.setColor(QPalette::Base, c(u"input-bg"));
    palette.setColor(QPalette::AlternateBase, c(u"panel-bg-subtle"));
    palette.setColor(QPalette::Text, c(u"input-text"));
    palette.setColor(QPalette::PlaceholderText, c(u"input-placeholder"));
    palette.setColor(QPalette::Button, c(u"interactive-bg"));
    palette.setColor(QPalette::ButtonText, c(u"text-main"));
    palette.setColor(QPalette::BrightText, c(u"on-accent"));
    palette.setColor(QPalette::Highlight, c(u"accent"));
    palette.setColor(QPalette::HighlightedText, c(u"on-accent"));
    palette.setColor(QPalette::Accent, c(u"accent"));
    palette.setColor(QPalette::Link, c(u"accent-text"));
    palette.setColor(QPalette::LinkVisited, c(u"accent-text"));
    palette.setColor(QPalette::ToolTipBase, c(u"panel-bg"));
    palette.setColor(QPalette::ToolTipText, c(u"text-main"));
    palette.setColor(QPalette::Light, c(u"panel-bg"));
    palette.setColor(QPalette::Midlight, c(u"border-default"));
    palette.setColor(QPalette::Mid, c(u"border-strong"));
    palette.setColor(QPalette::Dark, c(u"border-strong"));
    palette.setColor(QPalette::Shadow, c(u"border-strong"));
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText})
        palette.setColor(QPalette::Disabled, role, c(u"text-disabled"));
    palette.setColor(QPalette::Disabled, QPalette::Button, c(u"interactive-disabled"));
    return palette;
}

QFont Style::interfaceFont() {
    QFont font = QApplication::font();
    if (const QString family = Tokens::availableFamily(u"font-sans"); !family.isEmpty())
        font.setFamilies({family});
    font.setPixelSize(qRound(Tokens::px(u"text-base")));
    return font;
}

void Style::polish(QPalette& palette) { palette = paletteFor(Appearance::instance().theme()); }

int Style::pixelMetric(PixelMetric metric, const QStyleOption* option,
                       const QWidget* widget) const {
    switch (metric) {
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        return qRound(Tokens::px(u"space-2"));
    case PM_LayoutLeftMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutTopMargin:
    case PM_LayoutBottomMargin:
        return qRound(Tokens::px(u"space-4"));
    case PM_DefaultFrameWidth:
        return 1;
    case PM_SmallIconSize:
    case PM_ButtonIconSize:
        return 16;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                          const QWidget* widget) const {
    if (element == PE_FrameFocusRect && qobject_cast<const QAbstractItemView*>(widget)) {
        // Item views mark the current cell with their selection, not a ring around it.
        return;
    }
    if (element == PE_FrameFocusRect) {
        // A 2px design-system focus ring instead of the platform's dotted rectangle.
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        QPen pen(Tokens::color(u"focus-border", Appearance::instance().theme()), 2);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        const qreal radius = Tokens::px(u"control-radius");
        painter->drawRoundedRect(QRectF(option->rect).adjusted(1, 1, -1, -1), radius, radius);
        painter->restore();
        return;
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

} // namespace ket
