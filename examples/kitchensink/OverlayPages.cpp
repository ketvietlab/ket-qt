// SPDX-License-Identifier: MPL-2.0
#include "OverlayPages.h"

#include "KetAppearance.h"
#include "KetBanner.h"
#include "KetButton.h"
#include "KetDialog.h"
#include "KetFieldRow.h"
#include "KetIcons.h"
#include "KetStyle.h"
#include "KetTextField.h"
#include "KetToast.h"
#include "KitchenSinkCommon.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QQuickWidget>
#include <QStyleOption>
#include <QToolTip>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

QWidget* toolkits(QWidget* widgets, QWidget* quick, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-2")));
    for (const auto& [title, content] :
         {std::pair{"Qt Widgets", widgets}, std::pair{"Qt Quick", quick}}) {
        column->addWidget(new TokenLabel(QString::fromLatin1(title),
                                         QStringLiteral("text-secondary"),
                                         QStringLiteral("text-sm"), 600, host));
        column->addWidget(content, 0, Qt::AlignLeft | Qt::AlignTop);
        column->addSpacing(qRound(Tokens::px(u"space-3")));
    }
    return host;
}

QWidget* quick(const QString& variant, QWidget* parent) {
    return quickView(QStringLiteral("OverlaySamples"), parent, {{QStringLiteral("mode"), variant}});
}

QWidget* banners(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-3")));
    column->addWidget(new Banner(BannerTone::Info, QStringLiteral("Update ready"),
                                 QStringLiteral("KetPlus 2.4 installs when you restart."), host));
    column->addWidget(new Banner(BannerTone::Warning, QStringLiteral("Router unavailable"),
                                 QStringLiteral("Requests use the direct connection until it "
                                                "returns."),
                                 host));
    auto* error = new Banner(BannerTone::Danger, QStringLiteral("Sync failed"),
                             QStringLiteral("Check your connection and try again."), host);
    error->setAction(QStringLiteral("Retry"), QStringLiteral("refresh"));
    column->addWidget(error);
    host->setFixedWidth(520);
    return host;
}

QMenu* sampleMenu(QWidget* parent) {
    auto* menu = new QMenu(parent);
    menu->addAction(Icons::icon(u"folder-open"), QStringLiteral("Open in Finder"));
    QAction* rename = menu->addAction(Icons::icon(u"edit"), QStringLiteral("Rename"));
    rename->setShortcut(QKeySequence(Qt::Key_F2));
    QAction* pin = menu->addAction(QStringLiteral("Pin to sidebar"));
    pin->setCheckable(true);
    pin->setChecked(true);
    Style::setDescription(pin, QStringLiteral("Keep it above recent conversations."));
    menu->addAction(QStringLiteral("Archive"))->setEnabled(false);
    menu->addSeparator();
    Style::setDanger(menu->addAction(Icons::icon(u"trash"), QStringLiteral("Delete")));
    return menu;
}

// A menu drawn in place, with the second command highlighted, for the static preview.
QWidget* menuPreview(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* layout = new QHBoxLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    QMenu* menu = sampleMenu(host);
    menu->setWindowFlags(Qt::Widget);
    menu->setActiveAction(menu->actions().at(1));
    layout->addWidget(menu, 0, Qt::AlignLeft | Qt::AlignTop);
    menu->ensurePolished();
    menu->setFixedSize(menu->sizeHint());
    menu->show();
    return host;
}

// The tooltip panel drawn by ket::Style, in place.
class TooltipPreview final : public QWidget {
  public:
    TooltipPreview(QString text, QWidget* parent) : QWidget(parent), text_(std::move(text)) {
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
    }

    QSize sizeHint() const override {
        const QFontMetrics metrics(QToolTip::font());
        const TooltipRecipe r = tooltipRecipe(Appearance::instance().theme());
        return {metrics.horizontalAdvance(text_) + 2 * qRound(r.paddingInline),
                metrics.height() + 2 * qRound(r.paddingBlock)};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        QStyleOption option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_PanelTipLabel, &option, &painter, this);
        painter.setFont(QToolTip::font());
        painter.setPen(QToolTip::palette().color(QPalette::ToolTipText));
        painter.drawText(rect(), Qt::AlignCenter, text_);
    }

  private:
    QString text_;
};

Dialog* renameDialog(QWidget* anchor) {
    auto* dialog = new Dialog(QStringLiteral("Rename conversation"), anchor);
    auto* field = new TextField;
    field->setText(QStringLiteral("Release checklist"));
    dialog->setBody(new FieldRow(QStringLiteral("Name"), field,
                                 QStringLiteral("Shown in the sidebar and the window title.")));
    QObject::connect(dialog->addButton(QStringLiteral("Cancel")), &QAbstractButton::clicked, dialog,
                     &Dialog::reject);
    QObject::connect(dialog->addButton(QStringLiteral("Rename"), ButtonVariant::Primary),
                     &QAbstractButton::clicked, dialog, &Dialog::accept);
    return dialog;
}

QWidget* staticPreviews(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* row = new QHBoxLayout;
    row->setSpacing(qRound(Tokens::px(u"space-6")));
    row->addWidget(menuPreview(host), 0, Qt::AlignTop);
    row->addWidget(new TooltipPreview(QStringLiteral("Copy link"), host), 0, Qt::AlignTop);
    row->addStretch();
    column->addLayout(row);

    // Each stage starts parentless, so the overlay attaches to it as its window.
    auto* dialogStage = new QWidget;
    dialogStage->setFixedSize(620, 400);
    Dialog* dialog = renameDialog(dialogStage);
    dialogStage->setParent(host);
    dialog->open();
    column->addWidget(dialogStage);

    auto* toastStage = new QWidget;
    toastStage->setFixedSize(420, 96);
    auto* toast = new Toast(QStringLiteral("Settings saved"), toastStage);
    toastStage->setParent(host);
    toast->popup(0);
    column->addWidget(toastStage);
    return host;
}

QWidget* liveControls(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    auto* toast = new Button(QStringLiteral("Show toast"), ButtonVariant::Secondary, host);
    QObject::connect(toast, &QAbstractButton::clicked, toast,
                     [toast] { Toast::showMessage(toast, QStringLiteral("Settings saved")); });
    auto* dialog = new Button(QStringLiteral("Open dialog"), ButtonVariant::Secondary, host);
    QObject::connect(dialog, &QAbstractButton::clicked, dialog, [dialog] {
        Dialog* modal = renameDialog(dialog);
        QObject::connect(modal, &Dialog::finished, modal, &QObject::deleteLater);
        modal->open();
    });
    auto* menu = new Button(QStringLiteral("Context menu"), ButtonVariant::Secondary, host);
    QMenu* commands = sampleMenu(menu);
    QObject::connect(menu, &QAbstractButton::clicked, menu, [menu, commands] {
        commands->popup(menu->mapToGlobal(QPoint(0, menu->height() + 4)));
    });
    auto* tip = new Button(QStringLiteral("Hover for tooltip"), ButtonVariant::Ghost, host);
    tip->setToolTip(QStringLiteral("Tooltips use the accent background."));
    for (QWidget* widget : {static_cast<QWidget*>(toast), static_cast<QWidget*>(dialog),
                            static_cast<QWidget*>(menu), static_cast<QWidget*>(tip)})
        row->addWidget(widget);
    row->addStretch();
    return host;
}

} // namespace

QWidget* overlayPage() {
    PageFrame frame =
        makePage(QStringLiteral("Feedback and overlays"),
                 QStringLiteral(
                     "Banners, toasts, dialogs, context menus and tooltips, from "
                     "ket::bannerRecipe, toastRecipe, dialogRecipe, menuRecipe and "
                     "tooltipRecipe. Dialogs and toasts draw inside their window, as in the "
                     "KetPlus mock; ket::Style draws every QMenu, the select popup and QToolTip."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Banners"), frame.page));
    frame.body->addWidget(
        toolkits(banners(frame.page), quick(QStringLiteral("banners"), frame.page), frame.page));
    frame.body->addWidget(
        sectionTitle(QStringLiteral("Menu, tooltip, dialog and toast"), frame.page));
    frame.body->addWidget(toolkits(staticPreviews(frame.page),
                                   quick(QStringLiteral("previews"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Live"), frame.page));
    frame.body->addWidget(
        toolkits(liveControls(frame.page), quick(QStringLiteral("live"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
