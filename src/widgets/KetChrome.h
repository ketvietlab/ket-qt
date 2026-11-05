// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetControlRecipes.h"

#include <QStringList>
#include <QWidget>

class QHBoxLayout;

namespace ket {

// One muted item in a ket::StatusBar: an optional Két icon and its text.
class StatusItem final : public QWidget {
    Q_OBJECT
  public:
    explicit StatusItem(const QString& text = {}, const QString& iconCode = {},
                        QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    void setIconCode(const QString& code);
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont textFont() const;

    QString text_;
    QString iconCode_;
};

// The window status bar (`.statusbar`): items on the left and a wider-spaced group on the
// right (`.status-right`). Colours and metrics come from ket::statusBarRecipe.
class StatusBar final : public QWidget {
    Q_OBJECT
  public:
    enum class Side { Left, Right };

    explicit StatusBar(QWidget* parent = nullptr);

    StatusItem* addItem(const QString& text, const QString& iconCode = {}, Side side = Side::Left);
    // For the app's own widgets, such as an icon button.
    void addWidget(QWidget* widget, Side side = Side::Right);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QHBoxLayout* left_;
    QHBoxLayout* right_;
};

// A file path bar (`.file-breadcrumb`): the folders joined by " / ", a chevron and the
// file, then the app's actions on the right. Colours and metrics come from
// ket::breadcrumbRecipe.
class Breadcrumb final : public QWidget {
    Q_OBJECT
  public:
    explicit Breadcrumb(QWidget* parent = nullptr);

    [[nodiscard]] QStringList path() const { return path_; }
    void setPath(const QStringList& path); // folders, then the file
    [[nodiscard]] QHBoxLayout* actions() const { return actions_; }

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QStringList path_;
    QWidget* trail_;
    QHBoxLayout* actions_;
};

// A side pane's header (`.context-tabs`): a Két icon and the title, then the pane's actions.
// Colours and metrics come from ket::paneHeaderRecipe.
class PaneHeader final : public QWidget {
    Q_OBJECT
  public:
    explicit PaneHeader(const QString& title = {}, const QString& iconCode = {},
                        QWidget* parent = nullptr);

    [[nodiscard]] QString title() const { return title_; }
    void setTitle(const QString& title);
    void setIconCode(const QString& code);
    [[nodiscard]] QHBoxLayout* actions() const { return actions_; }

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    void applyRecipe();

    QString title_;
    QString iconCode_;
    QWidget* label_;
    QHBoxLayout* actions_;
};

} // namespace ket
