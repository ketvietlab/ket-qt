// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetMarkdown.h"
#include "KetTextRecipes.h"

#include <QHash>
#include <QWidget>

class QTextDocument;

namespace ket {

class Button;

// Read-only multi-line text folded to foldedLines() lines, with a Show more / Show less button
// when it is longer; it draws ket::expandableTextRecipe. Line breaks are kept. Each
// `@agent/<number>` is a link drawn as a mention token: a click, or Return or Space on the
// mention that has keyboard focus (the arrow keys move it), emits mentionActivated().
class ExpandableText final : public QWidget {
    Q_OBJECT
  public:
    explicit ExpandableText(QWidget* parent = nullptr);

    [[nodiscard]] QString text() const { return text_; }
    void setText(const QString& text);
    [[nodiscard]] int foldedLines() const;
    void setFoldedLines(int lines); // 0 restores the recipe's default
    [[nodiscard]] bool isExpanded() const { return expanded_; }
    void setExpanded(bool expanded);
    // More lines than foldedLines(): the button shows.
    [[nodiscard]] bool isFoldable() const;
    [[nodiscard]] int lineCount() const;
    [[nodiscard]] int visibleLineCount() const;

    [[nodiscard]] QList<MentionSpan> mentions() const { return spans_; }
    void setUnknownMentions(const QStringList& numbers);
    void setMentionTitle(const QString& number, const QString& title);
    [[nodiscard]] int focusedMention() const { return focused_; }
    void setFocusedMention(int index); // -1 for none
    [[nodiscard]] Button* toggleButton() const { return toggle_; }
    [[nodiscard]] ExpandableTextRecipe recipe() const;

    [[nodiscard]] bool hasHeightForWidth() const override { return true; }
    [[nodiscard]] int heightForWidth(int width) const override;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

  signals:
    void mentionActivated(const QString& number);
    void expandedChanged(bool expanded);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    void rebuild();
    void layoutDocument(int width) const;
    void syncAccessibility();
    [[nodiscard]] qreal lineBox() const;
    [[nodiscard]] int textHeight(int width) const;
    [[nodiscard]] int lineOf(int position) const;
    [[nodiscard]] QString linkAt(const QPoint& point) const;

    QString text_;
    QTextDocument* document_;
    Button* toggle_;
    QList<MentionSpan> spans_;
    QStringList unknown_;
    QHash<QString, QString> titles_;
    int foldedLines_ = 0;
    int focused_ = -1;
    bool expanded_ = false;
};

} // namespace ket
