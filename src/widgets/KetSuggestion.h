// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetChatRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// A starter prompt on the welcome screen (`.suggestion`): a Két icon, a title, a muted
// caption and a trailing arrow. Colours and metrics come from ket::suggestionRecipe.
class Suggestion final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit Suggestion(const QString& title = {}, const QString& caption = {},
                        const QString& iconCode = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString caption() const { return caption_; }
    void setCaption(const QString& caption);
    void setIconCode(const QString& code);
    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] SuggestionRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    [[nodiscard]] QFont titleFont(const SuggestionRecipe& r) const;
    [[nodiscard]] QFont captionFont(const SuggestionRecipe& r) const;

    QString caption_;
    QString iconCode_;
    bool hovered_ = false;
    std::optional<InteractionState> preview_;
};

} // namespace ket
