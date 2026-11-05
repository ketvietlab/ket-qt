// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRichRecipes.h"

#include <QAbstractButton>
#include <QWidget>

namespace ket {

// One `@agent/` completion: the conversation number, its title and where it lives.
struct MentionItem {
    QString number;
    QString title;
    QString place;
};

// The `@agent/` completion menu (`.mention-menu`): a title, up to the recipe's number of
// options, or an empty message. It never takes focus; the composer forwards the arrow keys,
// Enter, Tab and Escape. Colours and metrics come from ket::mentionPickerRecipe and
// ket::mentionOptionRecipe.
class MentionPicker final : public QWidget {
    Q_OBJECT
  public:
    explicit MentionPicker(QWidget* parent = nullptr);

    void setItems(const QList<MentionItem>& items);
    [[nodiscard]] QList<MentionItem> items() const { return items_; }
    void setTitle(const QString& title);
    void setEmptyText(const QString& text);
    [[nodiscard]] int highlightedIndex() const { return highlighted_; }
    void setHighlightedIndex(int index);
    // Moves the highlight by `step`, wrapping around.
    void moveHighlight(int step);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QRect optionRect(int index) const;
    // The room the shadow takes around the menu's surface.
    [[nodiscard]] QMargins shadowMargins() const;

  signals:
    void activated(const ket::MentionItem& item);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    [[nodiscard]] QFont sized(qreal pixels, int weight = 400, bool mono = false) const;
    [[nodiscard]] qreal titleHeight() const;

    QList<MentionItem> items_;
    QString title_;
    QString emptyText_;
    int highlighted_ = 0;
};

// Context usage as a ring (`.usage-ring`); click it for the app's usage details. The tooltip
// and accessible name state the share used. Colours come from ket::usageRingRecipe.
class UsageRing final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit UsageRing(QWidget* parent = nullptr);

    [[nodiscard]] qreal value() const { return value_; }
    void setValue(qreal used); // 0..1
    void setPreviewHovered(bool hovered);

    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    qreal value_ = 0;
    bool hovered_ = false;
    bool previewHovered_ = false;
};

} // namespace ket
