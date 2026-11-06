// SPDX-License-Identifier: MPL-2.0
#include "KetStyle.h"

#include "KetAppearance.h"
#include "KetBarPainter.h"
#include "KetMenuPainter.h"
#include "KetRecipes.h"
#include "KetSliderPainter.h"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>
#include <QToolTip>

namespace ket {

Style::Style() : QProxyStyle(QStyleFactory::create(QStringLiteral("Fusion"))) {}

void Style::install(QApplication& application) {
    application.setStyle(new Style);
    application.setFont(interfaceFont());
    const auto apply = [&application] {
        const Theme theme = Appearance::instance().theme();
        const QPalette palette = paletteFor(theme);
        application.setPalette(palette);
        // Tooltips keep their own palette and font, set apart from the application's.
        QToolTip::setPalette(palette);
        QFont tooltipFont = interfaceFont();
        tooltipFont.setPixelSize(
            qRound(Appearance::instance().scaled(tooltipRecipe(theme).fontPixels)));
        QToolTip::setFont(tooltipFont);
    };
    apply();
    QObject::connect(&Appearance::instance(), &Appearance::themeChanged, &application, apply);
    QObject::connect(&Appearance::instance(), &Appearance::zoomChanged, &application, apply);
}

void Style::setDanger(QAction* action, bool danger) {
    action->setProperty(kDangerProperty, danger);
}

void Style::setWarning(QAction* action, bool warning) {
    action->setProperty(kWarningProperty, warning);
}

void Style::setDescription(QAction* action, const QString& description) {
    action->setProperty(kDescriptionProperty, description);
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
    const TooltipRecipe tooltip = tooltipRecipe(theme);
    palette.setColor(QPalette::ToolTipBase, tooltip.background);
    palette.setColor(QPalette::ToolTipText, tooltip.text);
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

namespace {

// Popups that draw their own rounded panel need a transparent window behind it.
bool isRoundedPopup(const QWidget* widget) {
    return qobject_cast<const QMenu*>(widget) || widget->inherits("QTipLabel") ||
           widget->inherits("QComboBoxPrivateContainer");
}

} // namespace

void Style::polish(QWidget* widget) {
    QProxyStyle::polish(widget);
    if (widget->inherits("QSlider"))
        widget->setAttribute(Qt::WA_Hover);
    if (isRoundedPopup(widget)) {
        widget->setAttribute(Qt::WA_TranslucentBackground);
        widget->setAttribute(Qt::WA_NoSystemBackground);
    }
    if (widget->inherits("QTipLabel")) {
        // QTipLabel's margin covers the block padding; the extra inline padding goes here.
        const TooltipRecipe r = tooltipRecipe(Appearance::instance().theme());
        const int extra = qRound(Appearance::instance().scaled(r.paddingInline - r.paddingBlock));
        widget->setContentsMargins(extra, 0, extra, 0);
    }
    if (widget->inherits("QComboBoxPrivateContainer")) {
        const int padding = detail::menuPanelPadding() + 1;
        widget->setContentsMargins(padding, padding, padding, padding);
        for (auto* view : widget->findChildren<QAbstractItemView*>()) {
            view->setFrameShape(QFrame::NoFrame);
            view->viewport()->setAutoFillBackground(false);
            view->setAutoFillBackground(false);
            QPalette palette = view->palette();
            palette.setColor(QPalette::Base, Qt::transparent);
            view->setPalette(palette);
        }
    }
}

void Style::unpolish(QWidget* widget) {
    if (isRoundedPopup(widget)) {
        widget->setAttribute(Qt::WA_TranslucentBackground, false);
        widget->setAttribute(Qt::WA_NoSystemBackground, false);
    }
    QProxyStyle::unpolish(widget);
}

int Style::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget,
                     QStyleHintReturn* returnData) const {
    switch (hint) {
    case SH_ComboBox_Popup:
        return 1; // the select popup is a menu, as in the mock
    case SH_ComboBox_PopupFrameStyle:
        return QFrame::NoFrame;
    case SH_Menu_Mask:
        return 0; // rounded corners come from the translucent window instead
    case SH_ToolTip_WakeUpDelay:
        return tooltipRecipe(Appearance::instance().theme()).delayMs;
    case SH_ToolTipLabel_Opacity:
        return 255;
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

QSize Style::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size,
                              const QWidget* widget) const {
    if (type == CT_TabBarTab && detail::isTopTab(option))
        return detail::tabSize(qstyleoption_cast<const QStyleOptionTab*>(option), size, widget);
    if (type == CT_ProgressBar)
        if (const auto* bar = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
            bar && (bar->state & State_Horizontal))
            return {size.width(), detail::progressHeight()};
    if (type == CT_MenuItem)
        if (const auto* item = qstyleoption_cast<const QStyleOptionMenuItem*>(option))
            return detail::menuItemSize(item, widget);
    return QProxyStyle::sizeFromContents(type, option, size, widget);
}

void Style::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                        const QWidget* widget) const {
    if (element == CE_TabBarTabShape && detail::isTopTab(option)) {
        detail::drawTab(qstyleoption_cast<const QStyleOptionTab*>(option), painter, widget);
        return;
    }
    if (element == CE_TabBarTabLabel && detail::isTopTab(option)) {
        detail::drawTabLabel(qstyleoption_cast<const QStyleOptionTab*>(option), painter, widget);
        return;
    }
    if (element == CE_ProgressBar)
        if (const auto* bar = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
            bar && (bar->state & State_Horizontal)) {
            detail::drawProgressBar(bar, painter, widget);
            return;
        }
    if (element == CE_MenuItem || element == CE_MenuEmptyArea) {
        if (const auto* item = qstyleoption_cast<const QStyleOptionMenuItem*>(option)) {
            if (element == CE_MenuItem)
                detail::drawMenuItem(item, painter, widget);
            return;
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                               QPainter* painter, const QWidget* widget) const {
    if (control == CC_Slider && detail::isHorizontalSlider(option)) {
        detail::drawSlider(qstyleoption_cast<const QStyleOptionSlider*>(option), painter, widget);
        return;
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

QRect Style::subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                            SubControl subControl, const QWidget* widget) const {
    if (control == CC_Slider && detail::isHorizontalSlider(option))
        return detail::sliderSubControlRect(qstyleoption_cast<const QStyleOptionSlider*>(option),
                                            subControl);
    return QProxyStyle::subControlRect(control, option, subControl, widget);
}

QStyle::SubControl Style::hitTestComplexControl(ComplexControl control,
                                                const QStyleOptionComplex* option,
                                                const QPoint& position,
                                                const QWidget* widget) const {
    if (control == CC_Slider && detail::isHorizontalSlider(option)) {
        const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
        if (detail::sliderSubControlRect(slider, SC_SliderHandle).contains(position))
            return SC_SliderHandle;
        if (detail::sliderSubControlRect(slider, SC_SliderGroove).contains(position))
            return SC_SliderGroove;
        return SC_None;
    }
    return QProxyStyle::hitTestComplexControl(control, option, position, widget);
}

int Style::pixelMetric(PixelMetric metric, const QStyleOption* option,
                       const QWidget* widget) const {
    switch (metric) {
    case PM_SliderThickness:
    case PM_SliderControlThickness:
        if (const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
            !slider || slider->orientation == Qt::Horizontal)
            return detail::sliderThickness();
        break;
    case PM_SliderLength:
        if (const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
            !slider || slider->orientation == Qt::Horizontal)
            return detail::sliderHandleLength();
        break;
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
    case PM_TabBarBaseOverlap:
    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
    case PM_TabBarTabOverlap:
        return 0;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        return qRound(Appearance::instance().scaled(Tokens::px(u"space-5")));
    case PM_MenuPanelWidth:
        return 1;
    case PM_MenuHMargin:
    case PM_MenuVMargin:
        return detail::menuPanelPadding();
    case PM_MenuDesktopFrameWidth:
        return 0;
    case PM_ToolTipLabelFrameWidth: {
        // QTipLabel adds one pixel to this margin on every side.
        const TooltipRecipe r = tooltipRecipe(Appearance::instance().theme());
        return qRound(Appearance::instance().scaled(r.paddingBlock)) - 1;
    }
    default:
        break;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                          const QWidget* widget) const {
    if (element == PE_PanelMenu) {
        detail::drawMenuPanel(option, painter, widget);
        return;
    }
    if (element == PE_FrameMenu)
        return; // drawn with the panel
    if (element == PE_FrameTabBarBase && detail::isTopTab(option)) {
        detail::drawTabBarBase(option, painter);
        return;
    }
    if (element == PE_IndicatorTabClose) {
        detail::drawTabClose(option, painter, widget);
        return;
    }
    if (element == PE_FrameTabWidget) {
        // The pane is the panel surface; the tab strip draws the only edge.
        painter->fillRect(option->rect, Tokens::color(u"panel-bg", Appearance::instance().theme()));
        return;
    }
    if (element == PE_PanelTipLabel) {
        detail::drawTooltipPanel(option, painter, widget);
        return;
    }
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
