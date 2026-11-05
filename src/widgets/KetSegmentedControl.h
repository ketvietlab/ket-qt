// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QWidget>

#include <optional>

namespace ket {

// Segmented control (`.pill-tabs`): one selected option on a subtle track. Arrow keys,
// Home and End move the selection; clicks select. Colours and metrics come from
// ket::segmentedRecipe and ket::segmentRecipe.
class SegmentedControl final : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
  public:
    explicit SegmentedControl(const QStringList& options = {}, QWidget* parent = nullptr);

    [[nodiscard]] QStringList options() const { return options_; }
    void setOptions(const QStringList& options);
    [[nodiscard]] int currentIndex() const { return current_; }
    void setCurrentIndex(int index);
    // Stretches segments to share the width equally (`.provider-tabs .btn { flex: 1 }`).
    void setExpanding(bool expanding);

    // Pins the hovered segment for documentation pages; std::nullopt returns to live state.
    void setPreviewHovered(std::optional<int> index);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override { return sizeHint(); }
    [[nodiscard]] QRect segmentRect(int index) const;

  signals:
    void currentChanged(int index);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

  private:
    [[nodiscard]] QFont segmentFont() const;
    [[nodiscard]] qreal naturalWidth(int index) const;
    [[nodiscard]] int segmentAt(const QPoint& position) const;

    QStringList options_;
    int current_ = 0;
    int hovered_ = -1;
    bool expanding_ = false;
    std::optional<int> previewHovered_;
};

} // namespace ket
