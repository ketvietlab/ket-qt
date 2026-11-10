// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QAbstractButton>

#include <optional>

namespace ket {

// Sidebar navigation row (`.nav-item`): a Két icon, a label and an optional trailing count
// (`.nav-count`) or shortcut (`kbd`). It is checkable; put rows in a QButtonGroup to keep
// one selected. Colours and metrics come from ket::navItemRecipe. The focus ring follows
// keyboard focus only (Tab, Backtab or a shortcut), as KetNavItem.qml's visualFocus does.
class NavItem final : public QAbstractButton {
    Q_OBJECT
  public:
    explicit NavItem(const QString& text = {}, const QString& iconCode = {},
                     QWidget* parent = nullptr);

    [[nodiscard]] QString iconCode() const { return iconCode_; }
    void setIconCode(const QString& code);
    // A count replaces the shortcut; std::nullopt hides it.
    [[nodiscard]] std::optional<int> count() const { return count_; }
    void setCount(std::optional<int> count);
    [[nodiscard]] QString shortcutText() const { return shortcutText_; }
    void setShortcutText(const QString& text);

    void setPreviewState(std::optional<InteractionState> state);

    [[nodiscard]] NavItemRecipe recipe() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

  private:
    [[nodiscard]] QFont labelFont(const NavItemRecipe& r) const;
    [[nodiscard]] QFont trailingFont(const NavItemRecipe& r) const;
    [[nodiscard]] QString trailingText() const;

    QString iconCode_;
    std::optional<int> count_;
    QString shortcutText_;
    bool hovered_ = false;
    // Like Qt Quick's visualFocus: the ring shows for keyboard focus, not after a click.
    bool visualFocus_ = false;
    std::optional<InteractionState> preview_;
};

// A keyboard shortcut label (`kbd`), muted and never wrapped.
class Kbd final : public QWidget {
    Q_OBJECT
  public:
    explicit Kbd(const QString& text = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont kbdFont() const;

    QString text_;
};

} // namespace ket
