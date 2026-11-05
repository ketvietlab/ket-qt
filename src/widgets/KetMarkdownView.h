// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRichRecipes.h"

#include <QTextBrowser>
#include <QTimer>

class QVBoxLayout;

namespace ket {

class IconButton;

// A fenced code block: monospace text on a subtle rounded surface with a copy button.
// Long lines wrap, as the mock's `pre-wrap` command output does. Colours and metrics come
// from ket::codeBlockRecipe.
class CodeBlock final : public QWidget {
    Q_OBJECT
  public:
    explicit CodeBlock(const QString& code = {}, const QString& language = {},
                       QWidget* parent = nullptr);

    [[nodiscard]] QString code() const { return code_; }
    void setCode(const QString& code);
    [[nodiscard]] QString language() const { return language_; }
    void setLanguage(const QString& language);
    [[nodiscard]] IconButton* copyButton() const { return copy_; }

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  signals:
    void copied();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    [[nodiscard]] QFont codeFont() const;
    void copy();

    QString code_;
    QString language_;
    IconButton* copy_;
    QTimer copiedReset_;
};

// Read-only Markdown prose styled by ket::markdownRecipe; it grows to its content and
// reports links through anchorClicked() instead of opening them.
class MarkdownText final : public QTextBrowser {
    Q_OBJECT
  public:
    explicit MarkdownText(const QString& markdown = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString markdown() const { return markdown_; }
    void setMarkdownText(const QString& markdown);

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void render();
    void fit();

    QString markdown_;
};

// An assistant answer: Markdown prose and fenced code blocks in order (ket::splitMarkdown).
class MarkdownView final : public QWidget {
    Q_OBJECT
  public:
    explicit MarkdownView(const QString& markdown = {}, QWidget* parent = nullptr);

    [[nodiscard]] QString markdown() const { return markdown_; }
    void setMarkdown(const QString& markdown);

  signals:
    void linkActivated(const QUrl& url);

  private:
    QString markdown_;
    QVBoxLayout* layout_;
};

} // namespace ket
