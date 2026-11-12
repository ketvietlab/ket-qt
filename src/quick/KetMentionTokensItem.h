// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QStringList>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

namespace ket {

class MentionTokens;

// QML type `KetMentionTokens`: ket::MentionTokens on a TextEdit's document, so KetTextArea
// draws and edits mentions as ket::TextArea does. Set `document: input.textDocument`.
class QuickMentionTokens : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(KetMentionTokens)
    Q_PROPERTY(QQuickTextDocument* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(bool dark READ dark WRITE setDark NOTIFY darkChanged)
    Q_PROPERTY(QStringList unknown READ unknown WRITE setUnknown NOTIFY unknownChanged)
    Q_PROPERTY(QStringList numbers READ numbers NOTIFY spansChanged)
    // "Mentions: …" for assistive technology, or "".
    Q_PROPERTY(QString spoken READ spoken NOTIFY spansChanged)
  public:
    explicit QuickMentionTokens(QObject* parent = nullptr);

    [[nodiscard]] QQuickTextDocument* document() const { return document_; }
    void setDocument(QQuickTextDocument* document);
    [[nodiscard]] bool dark() const { return dark_; }
    void setDark(bool dark);
    [[nodiscard]] QStringList unknown() const { return unknown_; }
    void setUnknown(const QStringList& numbers);
    [[nodiscard]] QStringList numbers() const;
    [[nodiscard]] QString spoken() const;

    // ket::MentionTokens::insert(): the caret position after the mention.
    Q_INVOKABLE int insert(int start, int end, const QString& number, const QString& title = {});
    Q_INVOKABLE void reset();
    // { start, end } that Backspace (`backward`) or Delete removes at `position`, or null.
    Q_INVOKABLE QVariant deletion(int position, bool backward) const;
    Q_INVOKABLE int snap(int position, int previous) const;
    // Whether a drawn token starts at `start` (a caret at its end is not a query).
    Q_INVOKABLE bool isToken(int start) const;
    Q_INVOKABLE void setTitle(const QString& number, const QString& title);

  signals:
    void documentChanged();
    void darkChanged();
    void unknownChanged();
    void spansChanged();

  private:
    QPointer<QQuickTextDocument> document_;
    QPointer<MentionTokens> tokens_;
    QStringList unknown_;
    bool dark_ = false;
};

} // namespace ket
