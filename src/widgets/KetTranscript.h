// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetConversationRecipes.h"

#include <QIcon>
#include <QWidget>

class QLabel;
class QVBoxLayout;

namespace ket {

// Plain wrapped reading text with the transcript's line height (`.answer`, `.assistant-body`).
// Rich content (Markdown, code) belongs to the app; this is its plain-text body.
class ReadingText final : public QWidget {
    Q_OBJECT
  public:
    explicit ReadingText(const QString& text = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    [[nodiscard]] QFont readingFont() const;

    QString text_;
};

// The user's message (`.user-bubble`): an accent bubble at the trailing edge, at most the
// recipe's share of the width. Colours and metrics come from ket::userMessageRecipe.
class UserMessage final : public QWidget {
    Q_OBJECT
  public:
    explicit UserMessage(const QString& text = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    // The bubble's rectangle at the current width.
    [[nodiscard]] QRect bubbleRect() const;

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    [[nodiscard]] QFont bubbleFont() const;
    [[nodiscard]] QRectF bubbleFor(int width, qreal* textWidth = nullptr) const;

    QString text_;
};

// An assistant message (`.assistant-message`): an avatar, the name and time, then the body.
// The avatar defaults to a Két Symbol `bot` tile; apps pass their own mark.
class AssistantMessage final : public QWidget {
    Q_OBJECT
  public:
    explicit AssistantMessage(const QString& name = {}, const QString& time = {},
                              QWidget* parent = nullptr);

    void setName(const QString& name);
    void setTime(const QString& time);
    void setAvatar(const QIcon& avatar);
    // The body; the message takes ownership and deletes the previous one.
    void setBody(QWidget* body);
    [[nodiscard]] QWidget* body() const { return body_; }
    // Shortcut for a ReadingText body.
    void setText(const QString& text);

  private:
    void applyRecipe();

    QWidget* avatar_;
    QLabel* name_;
    QLabel* time_;
    QVBoxLayout* column_;
    QWidget* body_ = nullptr;
};

// A date between turns (`.day-divider`): the label between two rules.
class DayDivider final : public QWidget {
    Q_OBJECT
  public:
    explicit DayDivider(const QString& text = {}, QWidget* parent = nullptr);

    void setText(const QString& text);
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    [[nodiscard]] QFont dividerFont() const;

    QString text_;
};

} // namespace ket
