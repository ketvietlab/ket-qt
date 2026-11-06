// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetLightboxRecipes.h"

#include <QHash>
#include <QPixmap>
#include <QPointer>
#include <QVariantAnimation>
#include <QWidget>

class QAbstractButton;

namespace ket {

struct LightboxItem {
    QPixmap image;       // a null pixmap shows the missing-image placeholder
    QString caption;     // shown under the picture when set
    QString description; // the accessible name; the caption when empty
};

// Full-window image viewer (`interactions/lightbox`), drawn inside its window over every
// other widget, as ket::Dialog is. It shows one picture of a set with the counter, the
// toolbar tools, previous/next and the caption. Zoom (tools, keys, wheel, pinch, a tap
// for 2×), pan (drag) and a swipe to the neighbour work on the built-in picture.
//
// A host with its own image engine (for example ket::imageviewer::ImageViewer) passes it
// to setStage(): it fills the stage, the view tools only emit toolTriggered(), and the
// host reports its scale with setZoomLevel(). Print sends the built-in picture, as rotated,
// to the system print dialog; with a host stage it is the host's, like Copy, Download and
// Fullscreen always are. Colours and metrics come from ket::lightboxRecipe.
class Lightbox final : public QWidget {
    Q_OBJECT
  public:
    // `anchor` is any widget in the window to cover.
    explicit Lightbox(QWidget* anchor);

    [[nodiscard]] QList<LightboxItem> items() const { return items_; }
    void setItems(const QList<LightboxItem>& items, int index = 0);
    [[nodiscard]] int count() const { return int(items_.size()); }
    [[nodiscard]] int currentIndex() const { return index_; }
    void setCurrentIndex(int index);

    // ZoomOut | ZoomIn by default, as the design system's toolbar.
    [[nodiscard]] LightboxTools tools() const { return tools_; }
    void setTools(LightboxTools tools);
    // Previous/next wrap around the set by default; without wrapping they stop at the ends.
    [[nodiscard]] bool wraps() const { return wraps_; }
    void setWraps(bool wraps);

    // The lightbox takes ownership and deletes the previous stage; nullptr restores the
    // built-in picture.
    void setStage(QWidget* stage);
    [[nodiscard]] QWidget* stage() const { return stage_; }
    // The ZoomLevel text with a host stage: 1 is 100 %, below 0 shows a dash.
    void setZoomLevel(qreal scale);
    // A host stage's tool state; the built-in picture works its own out.
    void setToolEnabled(LightboxTool tool, bool enabled);

    // The built-in picture: zoom 1 fits, pan is the picture centre's offset from the stage's.
    [[nodiscard]] qreal zoom() const { return view_.zoom; }
    [[nodiscard]] QPointF pan() const { return view_.pan; }
    [[nodiscard]] int rotation() const { return turns_ * 90; }
    // Fitted picture pixels per image pixel; never above 1.
    [[nodiscard]] qreal fitScale() const;
    [[nodiscard]] qreal maximumZoom() const;
    [[nodiscard]] QRectF pictureRect() const; // in lightbox coordinates, at the current view
    [[nodiscard]] QRect stageRect() const;
    // The built-in picture as Print sends it: the whole image at the current rotation.
    [[nodiscard]] QImage printableImage() const;

    [[nodiscard]] bool isOpen() const { return isVisible(); }
    [[nodiscard]] QAbstractButton* toolButton(LightboxTool tool) const;
    [[nodiscard]] QAbstractButton* closeButton() const { return close_; }
    [[nodiscard]] QAbstractButton* previousButton() const { return previous_; }
    [[nodiscard]] QAbstractButton* nextButton() const { return next_; }
    [[nodiscard]] LightboxRecipe recipe() const;

  public slots:
    void open(int index = -1); // -1 keeps the current image
    void next();
    void previous();
    void zoomIn();
    void zoomOut();
    void fit();
    void toggleActualSize();
    void rotate();
    void print(); // opens the system print dialog for the built-in picture

  signals:
    void currentChanged(int index);
    void opened();
    void closed();
    void viewChanged();
    void toolTriggered(ket::LightboxTool tool);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool focusNextPrevChild(bool next) override;

  private:
    struct View {
        qreal zoom = 1;
        QPointF pan;
    };

    void step(int delta);
    void trigger(LightboxTool tool);
    void place();
    void refresh();
    void resetView();
    void setView(View view, bool animated);
    [[nodiscard]] View zoomedAt(qreal zoom, QPointF anchor) const {
        return zoomedAt(zoom, anchor, view_);
    }
    [[nodiscard]] View zoomedAt(qreal zoom, QPointF anchor, const View& from) const;
    // Where the view is heading: a step during an animation continues from its end.
    [[nodiscard]] View target() const;
    [[nodiscard]] View clamped(View view) const;
    [[nodiscard]] QSizeF orientedSize() const;
    [[nodiscard]] QPointF stageCentre() const;
    [[nodiscard]] bool builtIn() const { return !stage_; }
    [[nodiscard]] const LightboxItem* current() const;

    QList<LightboxItem> items_;
    int index_ = 0;
    LightboxTools tools_ = LightboxTool::ZoomOut | LightboxTool::ZoomIn;
    bool wraps_ = true;
    QPointer<QWidget> stage_;
    qreal hostZoomLevel_ = -1;
    QHash<int, bool> hostEnabled_;
    View view_;
    View from_;
    View to_;
    int turns_ = 0;
    QVariantAnimation animation_;
    QHash<int, QAbstractButton*> buttons_;
    QAbstractButton* close_ = nullptr;
    QAbstractButton* previous_ = nullptr;
    QAbstractButton* next_ = nullptr;
    QPointer<QWidget> restoreFocus_;
    // A press on the stage: a drag pans, a short one taps and a long one swipes.
    bool pressing_ = false;
    bool moved_ = false;
    QPointF pressAt_;
    QPointF panAtPress_;
};

} // namespace ket
