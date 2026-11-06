// SPDX-License-Identifier: MPL-2.0
#include "LightboxPages.h"

#include "KetAppearance.h"
#include "KetLightbox.h"
#include "KetLightboxButton.h"
#include "KetLightboxThumb.h"
#include "KitchenSinkCommon.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

constexpr LightboxTools kAllTools =
    LightboxTool::ZoomOut | LightboxTool::ZoomLevel | LightboxTool::ZoomIn | LightboxTool::Rotate |
    LightboxTool::Copy | LightboxTool::Download | LightboxTool::Fullscreen;

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

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("LightboxSamples"), parent, {{QStringLiteral("mode"), mode}});
}

QList<LightboxItem> screenshots() {
    QList<LightboxItem> items;
    for (const char* name : {"sidebar", "editor", "settings", "dialog", "terminal"}) {
        const QString file = QString::fromLatin1(name);
        items.append({QPixmap(QStringLiteral(":/kitchensink/images/screenshot-%1.png").arg(file)),
                      QStringLiteral("%1.png").arg(file), QString()});
    }
    return items;
}

QWidget* thumbs(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-2")));
    const QPixmap image(QStringLiteral(":/kitchensink/images/screenshot-editor.png"));
    for (LightboxThumbSize size :
         {LightboxThumbSize::Small, LightboxThumbSize::Medium, LightboxThumbSize::Large}) {
        auto* thumb = new LightboxThumb(image, QStringLiteral("editor.png"), host);
        thumb->setThumbSize(size);
        row->addWidget(thumb, 0, Qt::AlignTop);
    }
    auto* focused = new LightboxThumb(image, QStringLiteral("editor.png"), host);
    focused->setPreviewState(InteractionState{false, false, true, true});
    row->addWidget(focused, 0, Qt::AlignTop);
    auto* empty = new LightboxThumb({}, {}, host);
    empty->setText(QStringLiteral("No images"));
    row->addWidget(empty, 0, Qt::AlignTop);
    row->addStretch();
    return host;
}

// The round buttons sit on the backdrop they are drawn for.
class Backdrop final : public QWidget {
  public:
    using QWidget::QWidget;

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const qreal radius = Tokens::px(u"radius-md");
        painter.setPen(Qt::NoPen);
        painter.setBrush(lightboxRecipe(Appearance::instance().theme()).backdrop);
        painter.drawRoundedRect(rect(), radius, radius);
    }
};

QWidget* buttons(QWidget* parent) {
    auto* host = new Backdrop(parent);
    auto* row = new QHBoxLayout(host);
    const int padding = qRound(Tokens::px(u"space-3"));
    row->setContentsMargins(padding, padding, padding, padding);
    row->setSpacing(qRound(Tokens::px(u"space-2")));
    const std::pair<const char*, InteractionState> states[] = {
        {"Default", {}},
        {"Hover", {true, false, true, false}},
        {"Focus", {false, false, true, true}},
        {"Disabled", {false, false, false, false}},
    };
    for (const auto& [label, state] : states) {
        auto* button =
            new LightboxButton(QStringLiteral("zoom-in"), QString::fromLatin1(label), host);
        button->setPreviewState(state);
        button->setEnabled(state.enabled);
        row->addWidget(button);
    }
    auto* rotate = new LightboxButton(QStringLiteral("restore"), QStringLiteral("Rotate"), host);
    rotate->setFlipped(true);
    row->addWidget(rotate);
    auto* level = new LightboxButton(QString(), QStringLiteral("Fit or actual size"), host);
    level->setText(QStringLiteral("100%"));
    row->addWidget(level);
    return host;
}

QWidget* viewer(QWidget* parent) {
    // The stage starts parentless, so the viewer covers it as its window.
    auto* stage = new QWidget;
    stage->setFixedSize(720, 460);
    auto* lightbox = new Lightbox(stage);
    lightbox->setTools(kAllTools);
    lightbox->setItems(screenshots(), 1);
    stage->setParent(parent);
    lightbox->open();
    return stage;
}

QWidget* live(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-2")));
    const QList<LightboxItem> items = screenshots();
    for (int index = 0; index < items.size(); ++index) {
        auto* thumb = new LightboxThumb(items.at(index).image, items.at(index).caption, host);
        // Made on the first click, once the page is in its window, as the live dialog is.
        QObject::connect(thumb, &QAbstractButton::clicked, thumb, [thumb, items, index] {
            auto* lightbox = new Lightbox(thumb);
            lightbox->setTools(kAllTools);
            lightbox->setItems(items, index);
            QObject::connect(lightbox, &Lightbox::closed, lightbox, &QObject::deleteLater);
            lightbox->open();
        });
        row->addWidget(thumb);
    }
    row->addStretch();
    return host;
}

} // namespace

QWidget* lightboxPage() {
    PageFrame frame = makePage(
        QStringLiteral("Image viewer"),
        QStringLiteral("The design system's lightbox: thumbnails that open a full-window viewer "
                       "with zoom, pan, pinch, swipe, previous/next and a caption. Hosts add "
                       "rotate, copy, download and full screen, or put their own image engine "
                       "on the stage. From lightboxRecipe, lightboxToolRecipe and "
                       "lightboxThumbRecipe."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Thumbnails"), frame.page));
    frame.body->addWidget(
        toolkits(thumbs(frame.page), quick(QStringLiteral("thumbs"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Buttons"), frame.page));
    frame.body->addWidget(
        toolkits(buttons(frame.page), quick(QStringLiteral("buttons"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Viewer"), frame.page));
    frame.body->addWidget(
        toolkits(viewer(frame.page), quick(QStringLiteral("viewer"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Live"), frame.page));
    frame.body->addWidget(
        toolkits(live(frame.page), quick(QStringLiteral("live"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
