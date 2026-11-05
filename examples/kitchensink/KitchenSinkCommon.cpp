// SPDX-License-Identifier: MPL-2.0
#include "KitchenSinkCommon.h"

#include "KetAppearance.h"

#include <QQuickItem>
#include <QQuickWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QtMath>

namespace ket::kitchensink {

TokenLabel::TokenLabel(const QString& text, QString colorToken, QString sizeToken, int weight,
                       QWidget* parent, bool wrap)
    : QLabel(text, parent), colorToken_(std::move(colorToken)), sizeToken_(std::move(sizeToken)),
      weight_(weight) {
    setWordWrap(wrap);
    setTextInteractionFlags(Qt::TextSelectableByMouse);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &TokenLabel::refresh);
    refresh();
}

void TokenLabel::refresh() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText,
                     Tokens::color(colorToken_, Appearance::instance().theme()));
    setPalette(palette);
    QFont font = this->font();
    font.setPixelSize(qRound(Tokens::px(sizeToken_)));
    font.setWeight(QFont::Weight(weight_));
    setFont(font);
}

PageFrame makePage(const QString& title, const QString& description) {
    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* content = new QWidget(scroll);
    auto* body = new QVBoxLayout(content);
    const int padding = qRound(Tokens::px(u"space-8"));
    body->setContentsMargins(padding, padding, padding, padding);
    body->setSpacing(qRound(Tokens::px(u"space-4")));
    body->addWidget(new TokenLabel(title, QStringLiteral("text-main"),
                                   QStringLiteral("page-title-size"), 600, content));
    body->addWidget(new TokenLabel(description, QStringLiteral("text-secondary"),
                                   QStringLiteral("text-md"), 400, content));
    scroll->setWidget(content);
    return {scroll, body};
}

QWidget* sectionTitle(const QString& text, QWidget* parent) {
    auto* label =
        new TokenLabel(text, QStringLiteral("text-main"), QStringLiteral("text-lg"), 600, parent);
    label->setContentsMargins(0, qRound(Tokens::px(u"space-4")), 0, 0);
    return label;
}

QQuickWidget* quickView(const QString& type, QWidget* parent, const QVariantMap& properties) {
    auto* view = new QQuickWidget(parent);
    // The widget owns the size and the root follows it, so a content size change (zoom)
    // resizes the widget and its render target together instead of stretching the texture.
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setClearColor(Qt::transparent);
    view->setAttribute(Qt::WA_AlwaysStackOnTop);
    view->setInitialProperties(properties);
    view->loadFromModule(QStringLiteral("KetKitchenSink"), type);
    if (QQuickItem* root = view->rootObject()) {
        const auto fit = [view, root] {
            view->setFixedSize(qCeil(root->implicitWidth()), qCeil(root->implicitHeight()));
        };
        // Queued: the implicit size changes while the scene polishes, and resizing the widget
        // there would start a frame inside the current one.
        QObject::connect(root, &QQuickItem::implicitWidthChanged, view, fit, Qt::QueuedConnection);
        QObject::connect(root, &QQuickItem::implicitHeightChanged, view, fit, Qt::QueuedConnection);
        fit();
    }
    return view;
}

} // namespace ket::kitchensink
