// SPDX-License-Identifier: MPL-2.0
#include "KetLightbox.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetLightboxButton.h"
#include "KetPrint.h"

#include <QAbstractButton>
#include <QAccessible>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QTextLayout>

#include <cmath>

namespace ket {
namespace {

constexpr qreal kEpsilon = 1e-3;

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

QString toolCode(LightboxTool tool) {
    switch (tool) {
    case LightboxTool::ZoomOut:
        return QStringLiteral("zoom-out");
    case LightboxTool::ZoomLevel:
        return {};
    case LightboxTool::ZoomIn:
        return QStringLiteral("zoom-in");
    case LightboxTool::Rotate:
        return QStringLiteral("restore"); // mirrored: Két Symbol draws it anticlockwise
    case LightboxTool::Copy:
        return QStringLiteral("copy");
    case LightboxTool::Download:
        return QStringLiteral("download");
    case LightboxTool::Fullscreen:
        return QStringLiteral("fullscreen");
    case LightboxTool::Print:
        return QStringLiteral("printer");
    }
    return {};
}

QString toolLabel(LightboxTool tool) {
    switch (tool) {
    case LightboxTool::ZoomOut:
        return Lightbox::tr("Zoom out");
    case LightboxTool::ZoomLevel:
        return Lightbox::tr("Fit or actual size");
    case LightboxTool::ZoomIn:
        return Lightbox::tr("Zoom in");
    case LightboxTool::Rotate:
        return Lightbox::tr("Rotate");
    case LightboxTool::Copy:
        return Lightbox::tr("Copy image");
    case LightboxTool::Download:
        return Lightbox::tr("Download");
    case LightboxTool::Fullscreen:
        return Lightbox::tr("Full screen");
    case LightboxTool::Print:
        return Lightbox::tr("Print…");
    }
    return {};
}

QFont textFont(const QFont& base, qreal pixels, bool tabular) {
    QFont font = base;
    font.setPixelSize(qRound(scaled(pixels)));
    if (tabular)
        font.setFeature(QFont::Tag("tnum"), 1);
    return font;
}

int wrappedLines(const QString& text, const QFont& font, qreal width) {
    QTextLayout layout(text, font);
    QTextOption option(Qt::AlignHCenter);
    option.setWrapMode(QTextOption::WordWrap);
    layout.setTextOption(option);
    layout.beginLayout();
    int lines = 0;
    for (QTextLine line = layout.createLine(); line.isValid(); line = layout.createLine()) {
        line.setLineWidth(qMax<qreal>(1, width));
        ++lines;
    }
    layout.endLayout();
    return qMax(1, lines);
}

} // namespace

Lightbox::Lightbox(QWidget* anchor) : QWidget(anchor->window()) {
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);
    hide();

    auto add = [this](const QString& code, const QString& label) {
        auto* button = new LightboxButton(code, label, this);
        button->installEventFilter(this);
        return button;
    };
    for (LightboxTool tool : kLightboxToolOrder) {
        auto* button = add(toolCode(tool), toolLabel(tool));
        button->setFlipped(tool == LightboxTool::Rotate);
        connect(button, &QAbstractButton::clicked, this, [this, tool] { trigger(tool); });
        buttons_.insert(int(tool), button);
    }
    close_ = add(QStringLiteral("close"), tr("Close"));
    more_ = add(QStringLiteral("more"), tr("More tools"));
    moreMenu_ = new QMenu(this);
    connect(more_, &QAbstractButton::clicked, this, [this] {
        moreMenu_->clear();
        for (LightboxTool tool : std::as_const(overflow_)) {
            QAction* action =
                moreMenu_->addAction(toolLabel(tool), this, [this, tool] { trigger(tool); });
            action->setEnabled(buttons_.value(int(tool))->isEnabled());
        }
        moreMenu_->popup(more_->mapToGlobal(QPoint(0, more_->height())));
    });
    connect(close_, &QAbstractButton::clicked, this, &QWidget::close);
    previous_ = add(QStringLiteral("chevron-left"), tr("Previous image"));
    connect(previous_, &QAbstractButton::clicked, this, &Lightbox::previous);
    next_ = add(QStringLiteral("chevron-right"), tr("Next image"));
    connect(next_, &QAbstractButton::clicked, this, &Lightbox::next);

    QEasingCurve easeOut(QEasingCurve::BezierSpline); // CSS `ease-out`
    easeOut.addCubicBezierSegment(QPointF(0, 0), QPointF(0.58, 1), QPointF(1, 1));
    animation_.setEasingCurve(easeOut);
    animation_.setStartValue(0.0);
    animation_.setEndValue(1.0);
    connect(&animation_, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        const qreal t = value.toReal();
        view_.zoom = from_.zoom + (to_.zoom - from_.zoom) * t;
        view_.pan = from_.pan + (to_.pan - from_.pan) * t;
        refresh();
        emit viewChanged();
    });

    parentWidget()->installEventFilter(this);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &Lightbox::refresh);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Lightbox::refresh);
    refresh();
}

LightboxRecipe Lightbox::recipe() const { return lightboxRecipe(Appearance::instance().theme()); }

void Lightbox::setItems(const QList<LightboxItem>& items, int index) {
    items_ = items;
    index_ = items_.isEmpty() ? 0 : qBound(0, index, count() - 1);
    resetView();
    refresh();
    emit currentChanged(index_);
}

void Lightbox::setCurrentIndex(int index) {
    if (items_.isEmpty())
        return;
    index = qBound(0, index, count() - 1);
    if (index == index_)
        return;
    index_ = index;
    resetView();
    refresh();
    emit currentChanged(index_);
}

void Lightbox::setTools(LightboxTools tools) {
    tools_ = tools;
    refresh();
}

void Lightbox::setWraps(bool wraps) {
    wraps_ = wraps;
    refresh();
}

void Lightbox::setStage(QWidget* stage) {
    if (stage_)
        stage_->deleteLater();
    stage_ = stage;
    if (stage_) {
        stage_->setParent(this);
        stage_->setAutoFillBackground(false);
        // The host draws on the backdrop: a Base or Window fill must not hide it.
        QPalette palette = stage_->palette();
        palette.setColor(QPalette::Base, Qt::transparent);
        palette.setColor(QPalette::Window, Qt::transparent);
        stage_->setPalette(palette);
        stage_->show();
        stage_->lower();
    }
    hostZoomLevel_ = -1;
    hostEnabled_.clear();
    resetView();
    refresh();
}

void Lightbox::setZoomLevel(qreal scale) {
    hostZoomLevel_ = scale;
    refresh();
}

void Lightbox::setToolEnabled(LightboxTool tool, bool enabled) {
    hostEnabled_.insert(int(tool), enabled);
    refresh();
}

QAbstractButton* Lightbox::toolButton(LightboxTool tool) const { return buttons_.value(int(tool)); }

const LightboxItem* Lightbox::current() const {
    return items_.isEmpty() ? nullptr : &items_.at(index_);
}

QSizeF Lightbox::orientedSize() const {
    const LightboxItem* item = current();
    if (!item || item->image.isNull())
        return {};
    const QSizeF size = item->image.size();
    return turns_ % 2 ? size.transposed() : size;
}

QRect Lightbox::stageRect() const {
    const LightboxRecipe r = recipe();
    const qreal tool = lightboxToolRecipe({}, Appearance::instance().theme()).size;
    const int toolbar = qRound(scaled(tool + 2 * r.toolbarPaddingBlock));
    int caption = 0;
    const LightboxItem* item = current();
    if (item && !item->caption.isEmpty()) {
        const qreal width = this->width() - 2 * scaled(r.captionPaddingInline);
        const int lines =
            wrappedLines(item->caption, textFont(font(), r.captionPixels, false), width);
        caption = qRound(scaled(r.captionPaddingTop) + lines * scaled(r.captionLineHeight) +
                         scaled(r.captionPaddingBottom));
    }
    return QRect(0, toolbar, width(), qMax(0, height() - toolbar - caption));
}

QPointF Lightbox::stageCentre() const { return QRectF(stageRect()).center(); }

qreal Lightbox::fitScale() const {
    const QSizeF size = orientedSize();
    if (size.isEmpty())
        return 1;
    const LightboxRecipe r = recipe();
    const qreal inline_ = scaled(width() <= scaled(r.narrowWidth) ? r.narrowStagePaddingInline
                                                                  : r.stagePaddingInline);
    const QSizeF room =
        QRectF(stageRect())
            .adjusted(inline_, scaled(r.stagePaddingBlock), -inline_, -scaled(r.stagePaddingBlock))
            .size();
    const qreal fit = qMin(room.width() / size.width(), room.height() / size.height());
    return fit > 0 ? qMin<qreal>(1, fit) : 1;
}

// The design system's 4× of the fitted picture, and always far enough to reach 100 %.
qreal Lightbox::maximumZoom() const { return qMax(recipe().maxZoom, 1 / fitScale()); }

QRectF Lightbox::pictureRect() const {
    const QSizeF size = orientedSize() * fitScale() * view_.zoom;
    const QPointF centre = stageCentre() + view_.pan;
    return {centre.x() - size.width() / 2, centre.y() - size.height() / 2, size.width(),
            size.height()};
}

Lightbox::View Lightbox::clamped(View view) const {
    view.zoom = qBound<qreal>(1, view.zoom, maximumZoom());
    // The picture's edges stop at the stage's: nothing to pan across along a short axis.
    const QSizeF size = orientedSize() * fitScale() * view.zoom;
    const QSizeF stage = stageRect().size();
    const qreal x = qMax<qreal>(0, (size.width() - stage.width()) / 2);
    const qreal y = qMax<qreal>(0, (size.height() - stage.height()) / 2);
    view.pan = QPointF(qBound(-x, view.pan.x(), x), qBound(-y, view.pan.y(), y));
    return view;
}

Lightbox::View Lightbox::target() const {
    return animation_.state() == QAbstractAnimation::Running ? to_ : view_;
}

Lightbox::View Lightbox::zoomedAt(qreal zoom, QPointF anchor, const View& from) const {
    zoom = qBound<qreal>(1, zoom, maximumZoom());
    // The image point under the anchor stays under it.
    const QPointF centre = stageCentre();
    const QPointF point = (anchor - centre - from.pan) / from.zoom;
    View view{zoom, anchor - centre - point * zoom};
    if (zoom <= 1 + kEpsilon)
        view = View{};
    return clamped(view);
}

void Lightbox::resetView() {
    animation_.stop();
    view_ = View{};
    turns_ = 0;
}

void Lightbox::setView(View view, bool animated) {
    animation_.stop();
    const int duration = recipe().transitionMs;
    if (animated && duration > 0 && isVisible()) {
        from_ = view_;
        to_ = view;
        animation_.setDuration(duration);
        animation_.start();
        return;
    }
    view_ = view;
    refresh();
    emit viewChanged();
}

void Lightbox::open(int index) {
    if (items_.isEmpty())
        return;
    if (index >= 0)
        setCurrentIndex(index);
    restoreFocus_ = QApplication::focusWidget();
    resetView();
    place();
    show();
    raise();
    close_->setFocus(Qt::PopupFocusReason);
    QAccessibleEvent event(this, QAccessible::DialogStart);
    QAccessible::updateAccessibility(&event);
    emit opened();
}

void Lightbox::closeEvent(QCloseEvent* event) {
    QWidget::closeEvent(event);
    animation_.stop();
    pressing_ = false;
    QAccessibleEvent end(this, QAccessible::DialogEnd);
    QAccessible::updateAccessibility(&end);
    if (restoreFocus_)
        restoreFocus_->setFocus(Qt::PopupFocusReason);
    restoreFocus_ = nullptr;
    emit closed();
}

void Lightbox::step(int delta) {
    const int total = count();
    if (total < 2)
        return;
    int target = index_ + delta;
    if (wraps_)
        target = (target % total + total) % total;
    else if (target < 0 || target >= total)
        return;
    setCurrentIndex(target);
}

void Lightbox::next() { step(1); }

void Lightbox::previous() { step(-1); }

void Lightbox::trigger(LightboxTool tool) {
    emit toolTriggered(tool);
    if (!builtIn())
        return;
    switch (tool) {
    case LightboxTool::ZoomOut:
        zoomOut();
        break;
    case LightboxTool::ZoomLevel:
        toggleActualSize();
        break;
    case LightboxTool::ZoomIn:
        zoomIn();
        break;
    case LightboxTool::Rotate:
        rotate();
        break;
    case LightboxTool::Print:
        print();
        break;
    default:
        break; // Copy, Download and Fullscreen belong to the host
    }
}

// Toolbar and keyboard steps zoom about the picture's centre, as the design system's do.
void Lightbox::zoomIn() {
    const View from = target();
    if (builtIn())
        setView(zoomedAt(from.zoom + recipe().zoomStep, stageCentre() + from.pan, from), true);
}

void Lightbox::zoomOut() {
    const View from = target();
    if (builtIn())
        setView(zoomedAt(from.zoom - recipe().zoomStep, stageCentre() + from.pan, from), true);
}

void Lightbox::fit() {
    if (builtIn())
        setView(View{}, true);
}

void Lightbox::toggleActualSize() {
    if (!builtIn())
        return;
    const qreal actual = 1 / fitScale();
    const View from = target();
    if (std::abs(from.zoom - actual) < kEpsilon)
        fit();
    else
        setView(zoomedAt(actual, stageCentre() + from.pan, from), true);
}

QImage Lightbox::printableImage() const {
    const LightboxItem* item = current();
    if (!builtIn() || !item || item->image.isNull())
        return {};
    return item->image.toImage().transformed(QTransform().rotate(rotation()));
}

void Lightbox::print() {
    const LightboxItem* item = current();
    const QImage image = printableImage();
    if (!image.isNull())
        printImage(image, item->description.isEmpty() ? item->caption : item->description,
                   window());
}

void Lightbox::rotate() {
    if (!builtIn() || orientedSize().isEmpty())
        return;
    animation_.stop();
    turns_ = (turns_ + 1) % 4;
    view_ = View{};
    refresh();
    emit viewChanged();
}

void Lightbox::refresh() {
    const LightboxItem* item = current();
    const bool picture = builtIn() && item && !item->image.isNull();
    for (LightboxTool tool : kLightboxToolOrder) {
        QAbstractButton* button = buttons_.value(int(tool));
        button->setVisible(tools_.testFlag(tool));
        bool enabled = hostEnabled_.value(int(tool), true);
        if (builtIn()) {
            switch (tool) {
            case LightboxTool::ZoomOut:
                enabled = picture && view_.zoom > 1 + kEpsilon;
                break;
            case LightboxTool::ZoomIn:
                enabled = picture && view_.zoom < maximumZoom() - kEpsilon;
                break;
            case LightboxTool::Fullscreen:
                enabled = true;
                break;
            default:
                enabled = picture;
                break;
            }
        }
        button->setEnabled(enabled);
    }
    const qreal level = builtIn() ? (picture ? fitScale() * view_.zoom : -1) : hostZoomLevel_;
    buttons_.value(int(LightboxTool::ZoomLevel))
        ->setText(level < 0 ? QStringLiteral("—") : QStringLiteral("%1%").arg(qRound(level * 100)));
    const bool many = count() > 1;
    previous_->setVisible(many);
    next_->setVisible(many);
    previous_->setEnabled(wraps_ || index_ > 0);
    next_->setEnabled(wraps_ || index_ < count() - 1);
    if (item)
        setAccessibleName(item->description.isEmpty() ? item->caption : item->description);
    place();
    update();
}

void Lightbox::place() {
    QWidget* window = parentWidget();
    if (geometry() != window->rect())
        setGeometry(window->rect());
    const LightboxRecipe r = recipe();
    const int paddingInline = qRound(scaled(r.toolbarPaddingInline));
    const int paddingBlock = qRound(scaled(r.toolbarPaddingBlock));
    const int gap = qRound(scaled(r.toolbarGap));
    // The requested tools that fit show; the rest, last first, move into the More menu.
    QList<LightboxTool> requested;
    QList<qreal> widths;
    for (LightboxTool tool : kLightboxToolOrder) {
        if (tools_.testFlag(tool)) {
            requested.append(tool);
            widths.append(buttons_.value(int(tool))->sizeHint().width());
        }
    }
    qreal available = width() - 2 * paddingInline;
    if (count() > 1) {
        const QFontMetricsF counter(textFont(font(), r.counterPixels, true));
        available -= counter.horizontalAdvance(tr("%1 / %2").arg(index_ + 1).arg(count())) + gap;
    }
    const int shown = lightboxShownToolCount(widths, close_->sizeHint().width(), gap, available);
    overflow_ = requested.mid(shown);
    for (int i = 0; i < requested.size(); ++i)
        buttons_.value(int(requested.at(i)))->setVisible(i < shown);
    more_->setVisible(!overflow_.isEmpty());
    int x = width() - paddingInline;
    auto placeRight = [&](QAbstractButton* button) {
        const QSize size = button->sizeHint();
        x -= size.width();
        button->setGeometry(x, paddingBlock, size.width(), size.height());
        x -= gap;
    };
    placeRight(close_);
    if (more_->isVisibleTo(this))
        placeRight(more_);
    for (auto it = std::rbegin(kLightboxToolOrder); it != std::rend(kLightboxToolOrder); ++it) {
        QAbstractButton* button = buttons_.value(int(*it));
        if (button->isVisibleTo(this))
            placeRight(button);
    }
    const QRect stage = stageRect();
    const QSize nav = previous_->sizeHint();
    const int navY = stage.center().y() - nav.height() / 2;
    const int inset = qRound(scaled(r.navInset));
    previous_->setGeometry(inset, navY, nav.width(), nav.height());
    next_->setGeometry(width() - inset - nav.width(), navY, nav.width(), nav.height());
    if (stage_) {
        const int inline_ = qRound(scaled(
            width() <= scaled(r.narrowWidth) ? r.narrowStagePaddingInline : r.stagePaddingInline));
        const int block = qRound(scaled(r.stagePaddingBlock));
        stage_->setGeometry(stage.adjusted(inline_, block, -inline_, -block));
    }
}

void Lightbox::paintEvent(QPaintEvent*) {
    const LightboxRecipe r = recipe();
    QPainter painter(this);
    painter.fillRect(rect(), r.backdrop);
    painter.setPen(r.foreground);
    const int toolbar = stageRect().top();
    if (count() > 1) {
        painter.setFont(textFont(font(), r.counterPixels, true));
        const qreal inline_ = scaled(r.toolbarPaddingInline);
        painter.drawText(QRectF(inline_, 0, width() / 2.0, toolbar), Qt::AlignVCenter,
                         tr("%1 / %2").arg(index_ + 1).arg(count()));
    }
    const LightboxItem* item = current();
    if (!item)
        return;
    const QRect stage = stageRect();
    if (builtIn()) {
        painter.save();
        painter.setClipRect(stage);
        if (item->image.isNull()) {
            const qreal box = scaled(r.placeholderIconSize);
            painter.setOpacity(r.placeholderOpacity);
            painter.drawPixmap(QPointF(stageCentre().x() - box / 2, stageCentre().y() - box / 2),
                               Icons::pixmap(u"image", box, devicePixelRatioF(), r.foreground));
        } else {
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            const QSizeF size = item->image.size();
            const qreal scale = fitScale() * view_.zoom;
            painter.translate(stageCentre() + view_.pan);
            painter.rotate(rotation());
            painter.scale(scale, scale);
            painter.drawPixmap(
                QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height()),
                item->image, QRectF(QPointF(), item->image.size()));
        }
        painter.restore();
    }
    if (!item->caption.isEmpty()) {
        painter.setFont(textFont(font(), r.captionPixels, false));
        const qreal inline_ = scaled(r.captionPaddingInline);
        const QRectF caption(inline_, stage.bottom() + 1 + scaled(r.captionPaddingTop),
                             width() - 2 * inline_, height());
        painter.drawText(caption, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
                         item->caption);
    }
}

void Lightbox::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    view_ = clamped(view_);
    place();
}

} // namespace ket
