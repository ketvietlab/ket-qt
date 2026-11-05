// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetSurfaceRecipes.h"

#include <QWidget>

namespace ket {

struct DiffLine {
    enum class Kind { Context, Add, Remove, Hunk };
    Kind kind = Kind::Context;
    int number = 0; // the new file's line, or the old file's for removed lines; 0 hides it
    QString text;
};

// Unified diff lines (`.diff-lines`): `@@` hunk headers on the accent tint, then numbered
// lines with their sign on the added or removed tint. Lines do not wrap; put the view in a
// scroll area for long lines. Colours and metrics come from ket::diffRecipe.
class DiffView final : public QWidget {
    Q_OBJECT
  public:
    explicit DiffView(QWidget* parent = nullptr);

    [[nodiscard]] QList<DiffLine> lines() const { return lines_; }
    void setLines(const QList<DiffLine>& lines);
    // Parses `git diff` output (hunks onward; file headers are skipped).
    [[nodiscard]] static QList<DiffLine> parseUnified(const QString& diff);

    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont monoFont() const;
    [[nodiscard]] qreal rowHeight(const DiffLine& line) const;

    QList<DiffLine> lines_;
};

} // namespace ket
