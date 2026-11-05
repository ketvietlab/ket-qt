// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetIcons.h"

#include <QWidget>

#include <optional>

namespace ket {

// A standalone Két Symbol icon (the mock's inline `.icon`), drawn by ket::Icons at
// `size` logical pixels (scaled with the interface zoom). The colour defaults to
// `text-main` and follows the theme; setColor() pins an app colour.
class Icon final : public QWidget {
    Q_OBJECT
  public:
    explicit Icon(const QString& code = {}, qreal size = 16, QWidget* parent = nullptr);

    [[nodiscard]] QString code() const { return code_; }
    void setCode(const QString& code);
    [[nodiscard]] qreal iconSize() const { return size_; }
    void setIconSize(qreal size);
    [[nodiscard]] IconWeight weight() const { return weight_; }
    void setWeight(IconWeight weight);
    [[nodiscard]] QColor color() const;
    void setColor(std::optional<QColor> color);

    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString code_;
    qreal size_;
    IconWeight weight_ = IconWeight::Light;
    std::optional<QColor> color_;
};

} // namespace ket
