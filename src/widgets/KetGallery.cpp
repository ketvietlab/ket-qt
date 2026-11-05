// SPDX-License-Identifier: MPL-2.0
#include "KetGallery.h"

#include "KetAppearance.h"
#include "KetProfile.h"

#include <QAbstractButton>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

GalleryRecipe recipeFor(bool hovered) {
    return galleryRecipe(hovered, Appearance::instance().theme());
}

QFont captionFont(const QWidget* widget, const GalleryRecipe& r) {
    QFont font = widget->font();
    font.setPixelSize(qRound(scaled(r.captionPixels)));
    return font;
}

// `.activity-thumb`: the image cropped to a square, then its caption.
class Thumb final : public QAbstractButton {
  public:
    Thumb(const GalleryImage& image, QWidget* parent) : QAbstractButton(parent), image_(image) {
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::PointingHandCursor);
        setText(image.caption);
        setAccessibleName(image.caption.isEmpty() ? tr("Screenshot") : image.caption);
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const bool hovered = underMouse() || hasFocus();
        const GalleryRecipe r = recipeFor(hovered);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        const qreal ring = scaled(r.ringWidth);
        const qreal side = width() - 2 * ring;
        const QRectF tile(ring, ring, side, side);
        const qreal radius = scaled(r.radius);
        if (r.ring.alpha()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(r.ring);
            painter.drawRoundedRect(tile.adjusted(-ring, -ring, ring, ring), radius + ring,
                                    radius + ring);
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(tile, radius, radius);
        if (!image_.image.isNull()) {
            painter.save();
            QPainterPath clip;
            clip.addRoundedRect(tile, radius, radius);
            painter.setClipPath(clip);
            // object-fit: cover
            const QSizeF source = image_.image.deviceIndependentSize();
            const qreal scale = qMax(side / source.width(), side / source.height());
            const QSizeF drawn = source * scale;
            painter.drawPixmap(
                QRectF(tile.center() - QPointF(drawn.width(), drawn.height()) / 2, drawn),
                image_.image, QRectF(QPointF(), image_.image.size()));
            painter.restore();
        }
        painter.setPen(QPen(r.border, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(tile.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        if (!text().isEmpty()) {
            const QFont font = captionFont(this, r);
            const QFontMetricsF metrics(font);
            painter.setFont(font);
            painter.setPen(r.caption);
            const qreal y = tile.bottom() + ring + scaled(r.captionGap);
            painter.drawText(QRectF(ring, y, side, metrics.height()), Qt::AlignLeft,
                             metrics.elidedText(text(), Qt::ElideRight, side));
        }
    }
    void enterEvent(QEnterEvent* event) override {
        update();
        QAbstractButton::enterEvent(event);
    }
    void leaveEvent(QEvent* event) override {
        update();
        QAbstractButton::leaveEvent(event);
    }

  private:
    GalleryImage image_;
};

// `.activity-more`: the `+N` tile.
class More final : public QAbstractButton {
  public:
    More(int count, QWidget* parent) : QAbstractButton(parent) {
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::PointingHandCursor);
        setText(QStringLiteral("+%1").arg(count));
        setAccessibleName(tr("Show %n more", nullptr, count));
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const GalleryRecipe r = recipeFor(underMouse() || hasFocus());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const qreal ring = scaled(r.ringWidth);
        const qreal side = width() - 2 * ring;
        const QRectF tile(ring, ring, side, side);
        const qreal radius = scaled(r.moreRadius);
        painter.setPen(QPen(r.border, 1));
        painter.setBrush(r.background);
        painter.drawRoundedRect(tile.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(Profile::current().activityRowPixels)));
        painter.setFont(font);
        painter.setPen(r.more);
        painter.drawText(tile, Qt::AlignCenter, text());
    }
};

} // namespace

Gallery::Gallery(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, [this] {
        for (QWidget* tile : std::as_const(tiles_))
            tile->update();
    });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, [this] {
        updateGeometry();
        place();
    });
}

void Gallery::setImages(const QList<GalleryImage>& images) {
    images_ = images;
    rebuild();
}

void Gallery::setMaxVisible(int count) {
    maxVisible_ = qMax(0, count);
    rebuild();
}

void Gallery::setExpanded(bool expanded) {
    expanded_ = expanded;
    rebuild();
}

void Gallery::rebuild() {
    qDeleteAll(tiles_);
    tiles_.clear();
    const bool limited = !expanded_ && maxVisible_ > 0 && images_.size() > maxVisible_;
    const int shown = limited ? maxVisible_ : int(images_.size());
    for (int i = 0; i < shown; ++i) {
        auto* thumb = new Thumb(images_.at(i), this);
        connect(thumb, &QAbstractButton::clicked, this, [this, i] { emit activated(i); });
        tiles_.append(thumb);
    }
    if (limited) {
        auto* more = new More(int(images_.size()) - shown, this);
        connect(more, &QAbstractButton::clicked, this, [this] { setExpanded(true); });
        tiles_.append(more);
    }
    for (QWidget* tile : std::as_const(tiles_))
        tile->show();
    updateGeometry();
    place();
}

QSize Gallery::tileSize() const {
    const GalleryRecipe r = recipeFor(false);
    const int ring = qRound(scaled(r.ringWidth));
    const int side = qRound(scaled(r.tileSize)) + 2 * ring;
    const bool captions = std::any_of(images_.cbegin(), images_.cend(),
                                      [](const GalleryImage& i) { return !i.caption.isEmpty(); });
    const int caption =
        captions ? qCeil(scaled(r.captionGap) + QFontMetricsF(captionFont(this, r)).height()) : 0;
    return {side, side + caption};
}

int Gallery::columns(int width) const {
    const GalleryRecipe r = recipeFor(false);
    const int gap = qRound(scaled(r.spacing));
    const int inner = width - 2 * qRound(scaled(r.paddingInline));
    return qMax(1, (inner + gap) / (tileSize().width() + gap));
}

int Gallery::heightForWidth(int width) const {
    if (tiles_.isEmpty())
        return 0;
    const GalleryRecipe r = recipeFor(false);
    const int rows = int((tiles_.size() + columns(width) - 1) / columns(width));
    return qRound(scaled(r.paddingTop)) + qRound(scaled(r.paddingBottom)) +
           rows * tileSize().height() + (rows - 1) * qRound(scaled(r.spacing));
}

QSize Gallery::sizeHint() const {
    const GalleryRecipe r = recipeFor(false);
    const int count = int(qMax<qsizetype>(1, tiles_.size()));
    const int width = 2 * qRound(scaled(r.paddingInline)) + count * tileSize().width() +
                      (count - 1) * qRound(scaled(r.spacing));
    return {width, heightForWidth(width)};
}

void Gallery::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    place();
}

void Gallery::place() {
    const GalleryRecipe r = recipeFor(false);
    const int gap = qRound(scaled(r.spacing));
    const QSize tile = tileSize();
    const int perRow = columns(width());
    const int ring = qRound(scaled(r.ringWidth));
    for (int i = 0; i < tiles_.size(); ++i) {
        const int x = qRound(scaled(r.paddingInline)) - ring + (i % perRow) * (tile.width() + gap);
        const int y = qRound(scaled(r.paddingTop)) - ring + (i / perRow) * (tile.height() + gap);
        tiles_.at(i)->setGeometry(QRect(QPoint(x, y), tile));
    }
}

} // namespace ket
