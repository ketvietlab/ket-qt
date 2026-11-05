// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// A file explorer row (`.file-row`) or folder (`.explorer-folder summary`): indented by
// depth, a Két icon and the name, a chevron on folders and an optional status letter on
// files (such as "M"). Folders are checkable and checked while expanded; files take
// setActive() for the open file. Colours and metrics come from ket::fileRowRecipe.
class FileRow final : public QAbstractButton {
    Q_OBJECT
  public:
    enum class Kind { File, Folder };

    explicit FileRow(const QString& name = {}, Kind kind = Kind::File, int depth = 0,
                     QWidget* parent = nullptr);

    [[nodiscard]] Kind kind() const { return kind_; }
    [[nodiscard]] int depth() const { return depth_; }
    void setDepth(int depth);
    void setIconCode(const QString& code); // default: file / folder / folder-open
    void setStatus(const QString& status);
    [[nodiscard]] bool isActive() const { return active_; }
    void setActive(bool active);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] FileRowRecipe recipe() const;
    [[nodiscard]] qreal indent() const; // the leading padding at this depth, scaled
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    [[nodiscard]] QFont textFont() const;

    Kind kind_;
    int depth_;
    QString iconCode_;
    QString status_;
    bool active_ = false;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

} // namespace ket
