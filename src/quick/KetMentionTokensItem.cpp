// SPDX-License-Identifier: MPL-2.0
#include "KetMentionTokensItem.h"

#include "KetMentionTokens.h"

namespace ket {

QuickMentionTokens::QuickMentionTokens(QObject* parent) : QObject(parent) {}

void QuickMentionTokens::setDocument(QQuickTextDocument* document) {
    if (document_ == document)
        return;
    delete tokens_;
    document_ = document;
    if (document_ && document_->textDocument()) {
        tokens_ = new MentionTokens(document_->textDocument());
        tokens_->setTheme(dark_ ? Theme::Dark : Theme::Light);
        tokens_->setUnknown(unknown_);
        connect(tokens_, &MentionTokens::spansChanged, this, &QuickMentionTokens::spansChanged);
    }
    emit documentChanged();
    emit spansChanged();
}

void QuickMentionTokens::setDark(bool dark) {
    if (dark_ == dark)
        return;
    dark_ = dark;
    if (tokens_)
        tokens_->setTheme(dark_ ? Theme::Dark : Theme::Light);
    emit darkChanged();
}

void QuickMentionTokens::setUnknown(const QStringList& numbers) {
    if (unknown_ == numbers)
        return;
    unknown_ = numbers;
    if (tokens_)
        tokens_->setUnknown(numbers);
    emit unknownChanged();
    emit spansChanged();
}

QStringList QuickMentionTokens::numbers() const {
    return tokens_ ? tokens_->numbers() : QStringList();
}

QString QuickMentionTokens::spoken() const {
    return tokens_ ? tokens_->spokenMentions() : QString();
}

int QuickMentionTokens::insert(int start, int end, const QString& number, const QString& title) {
    return tokens_ ? tokens_->insert(start, end, number, title) : end;
}

void QuickMentionTokens::reset() {
    if (tokens_)
        tokens_->reset();
}

QVariant QuickMentionTokens::deletion(int position, bool backward) const {
    if (!tokens_)
        return {};
    const auto range = tokens_->deletion(position, backward);
    if (!range)
        return {};
    return QVariantMap{{QStringLiteral("start"), range->first},
                       {QStringLiteral("end"), range->second}};
}

int QuickMentionTokens::snap(int position, int previous) const {
    return tokens_ ? tokens_->snap(position, previous) : position;
}

bool QuickMentionTokens::isToken(int start) const {
    return tokens_ && tokens_->deletion(start + 1, true).has_value();
}

void QuickMentionTokens::setTitle(const QString& number, const QString& title) {
    if (!tokens_)
        return;
    tokens_->setTitle(number, title);
    emit spansChanged();
}

} // namespace ket
