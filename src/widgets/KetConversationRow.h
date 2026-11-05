// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetConversationRecipes.h"

#include <QAbstractButton>
#include <QElapsedTimer>
#include <QTimer>

#include <optional>

namespace ket {

// A tag on a conversation row: a label and the app's tag colour.
struct ConversationTag {
    QString label;
    QColor color;
};

// Two-line sidebar conversation row (`.sidebar-context .conversation`): the title, then the
// conversation number with as many whole tags as fit (and `+N`), or the status label when
// there are no tags. The status icon trails the row and spins while working. It is
// checkable; a QButtonGroup keeps one selected. Colours and metrics come from
// ket::conversationRowRecipe and ket::conversationStatusRecipe.
class ConversationRow final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit ConversationRow(const QString& title = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString number() const { return number_; }
    void setNumber(const QString& number);
    [[nodiscard]] QList<ConversationTag> tags() const { return tags_; }
    void setTags(const QList<ConversationTag>& tags);
    [[nodiscard]] ConversationStatus status() const { return status_; }
    void setStatus(ConversationStatus status);
    // Overrides the English status label; an empty string restores it.
    void setStatusLabel(const QString& label);
    [[nodiscard]] QString statusLabel() const;

    void setPreviewState(std::optional<InteractionState> state);
    // The number of tags drawn at the current width (the rest are counted in +N).
    [[nodiscard]] int visibleTagCount() const;

    [[nodiscard]] ConversationRowRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

  private:
    struct Chip {
        QString text;
        QRectF rect;
        TagRecipe recipe;
        bool overflow = false; // the +N chip
    };
    [[nodiscard]] qreal tagsLeft(const ConversationRowRecipe& r) const;
    [[nodiscard]] qreal tagsRight(const ConversationRowRecipe& r) const;
    [[nodiscard]] QList<Chip> layoutChips(qreal left, qreal right) const;
    [[nodiscard]] QFont font(qreal pixels, int weight = 400, bool mono = false) const;
    void updateSpinner();

    QString number_;
    QList<ConversationTag> tags_;
    ConversationStatus status_ = ConversationStatus::NotStarted;
    QString statusLabel_;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
    QElapsedTimer clock_;
    QTimer frame_;
};

} // namespace ket
