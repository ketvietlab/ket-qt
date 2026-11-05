// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QWidget>

namespace ket {

// A file attached to the composer (`.attachment-chip`): a Két icon, the elided name and a
// remove button. Clicking the × or pressing Delete or Backspace on the focused chip emits
// removeRequested(). Colours and metrics come from ket::attachmentChipRecipe.
class AttachmentChip final : public QWidget {
    Q_OBJECT
  public:
    explicit AttachmentChip(const QString& name = {},
                            const QString& iconCode = QStringLiteral("file"),
                            QWidget* parent = nullptr);

    [[nodiscard]] QString name() const { return name_; }
    void setName(const QString& name);
    void setIconCode(const QString& code);
    [[nodiscard]] QRect removeRect() const;

    [[nodiscard]] AttachmentChipRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;

  signals:
    void removeRequested();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

  private:
    [[nodiscard]] QFont textFont() const;

    QString name_;
    QString iconCode_;
    bool removeHovered_ = false;
};

} // namespace ket
