// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QWidget>

namespace ket {

// A labelled usage meter: a label and its value on one line, an optional track and a muted
// caption below (`.usage-stat` with `progress` in the usage popover, or the compact `.quota`
// in the sidebar). A negative fraction hides the track, for values that are not reported.
// Colours and metrics come from ket::meterRecipe.
class Meter final : public QWidget {
    Q_OBJECT
  public:
    explicit Meter(const QString& label = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString label() const { return label_; }
    void setLabel(const QString& label);
    [[nodiscard]] QString valueText() const { return value_; }
    void setValueText(const QString& value);
    [[nodiscard]] qreal fraction() const { return fraction_; }
    void setFraction(qreal fraction); // 0..1, or negative for no track
    [[nodiscard]] QString caption() const { return caption_; }
    void setCaption(const QString& caption);
    [[nodiscard]] bool isCompact() const { return compact_; }
    void setCompact(bool compact);

    [[nodiscard]] MeterRecipe recipe() const;
    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void changed();
    [[nodiscard]] QFont labelFont(const MeterRecipe& r) const;
    [[nodiscard]] QFont captionFont(const MeterRecipe& r) const;

    QString label_;
    QString value_;
    QString caption_;
    qreal fraction_ = -1;
    bool compact_ = false;
};

} // namespace ket
