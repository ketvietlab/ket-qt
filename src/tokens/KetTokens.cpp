// SPDX-License-Identifier: MPL-2.0
#include "KetTokens.h"

#include <QFontDatabase>
#include <QHash>

namespace ket {
namespace {

struct Entry {
    QStringView name;
    TokenKind kind;
    QStringView light;
    QStringView dark;
};

constexpr Entry kTable[] = {
#include "generated/KetTokenTable.inc"
};

const QHash<QStringView, const Entry*>& index() {
    static const QHash<QStringView, const Entry*> table = [] {
        QHash<QStringView, const Entry*> result;
        result.reserve(std::size(kTable));
        for (const Entry& entry : kTable)
            result.insert(entry.name, &entry);
        return result;
    }();
    return table;
}

const Entry* find(QStringView name) { return index().value(name, nullptr); }

QStringView valueOf(const Entry& entry, Theme theme) {
    return theme == Theme::Dark ? entry.dark : entry.light;
}

QVariant typed(const Entry& entry, Theme theme) {
    const QStringView value = valueOf(entry, theme);
    switch (entry.kind) {
    case TokenKind::Color:
        return QColor::fromString(value);
    case TokenKind::Length:
    case TokenKind::Number:
        return value.toDouble();
    case TokenKind::Duration:
        return qRound(value.toDouble());
    default:
        return value.toString();
    }
}

} // namespace

bool Tokens::contains(QStringView name) { return find(name) != nullptr; }

TokenKind Tokens::kind(QStringView name) {
    const Entry* entry = find(name);
    return entry ? entry->kind : TokenKind::Other;
}

QStringList Tokens::names() {
    QStringList result;
    result.reserve(std::size(kTable));
    for (const Entry& entry : kTable)
        result.append(entry.name.toString());
    return result;
}

QString Tokens::raw(QStringView name, Theme theme) {
    const Entry* entry = find(name);
    return entry ? valueOf(*entry, theme).toString() : QString();
}

QColor Tokens::color(QStringView name, Theme theme) {
    const Entry* entry = find(name);
    return entry && entry->kind == TokenKind::Color ? QColor::fromString(valueOf(*entry, theme))
                                                    : QColor();
}

qreal Tokens::px(QStringView name) {
    const Entry* entry = find(name);
    return entry && entry->kind == TokenKind::Length ? entry->light.toDouble() : 0.0;
}

qreal Tokens::number(QStringView name) {
    const Entry* entry = find(name);
    return entry && entry->kind == TokenKind::Number ? entry->light.toDouble() : 0.0;
}

int Tokens::ms(QStringView name) {
    const Entry* entry = find(name);
    return entry && entry->kind == TokenKind::Duration ? qRound(entry->light.toDouble()) : 0;
}

QStringList Tokens::fontFamilies(QStringView name) {
    static const QStringList generic{
        QStringLiteral("serif"),         QStringLiteral("sans-serif"),
        QStringLiteral("monospace"),     QStringLiteral("ui-sans-serif"),
        QStringLiteral("ui-monospace"),  QStringLiteral("system-ui"),
        QStringLiteral("-apple-system"), QStringLiteral("BlinkMacSystemFont")};
    const Entry* entry = find(name);
    if (!entry || entry->kind != TokenKind::Font)
        return {};
    QStringList families;
    for (QStringView part : entry->light.split(u',')) {
        QString family = part.trimmed().toString();
        if (family.size() >= 2 && family.front() == u'"' && family.back() == u'"')
            family = family.mid(1, family.size() - 2);
        if (!family.isEmpty() && !generic.contains(family))
            families.append(family);
    }
    return families;
}

QString Tokens::availableFamily(QStringView name) { return installedFamily(fontFamilies(name)); }

QString Tokens::installedFamily(const QStringList& families) {
    const auto folded = [](const QString& family) {
        QString result;
        for (QChar ch : family.normalized(QString::NormalizationForm_D))
            if (ch.category() != QChar::Mark_NonSpacing)
                result.append(ch.toCaseFolded());
        return result;
    };
    const QStringList installed = QFontDatabase::families();
    for (const QString& family : families) {
        if (installed.contains(family))
            return family;
        const QString key = folded(family);
        for (const QString& candidate : installed)
            if (folded(candidate) == key)
                return candidate;
    }
    return {};
}

QVariantMap Tokens::map(Theme theme) {
    QVariantMap result;
    for (const Entry& entry : kTable)
        result.insert(camelCase(entry.name), typed(entry, theme));
    return result;
}

QString Tokens::camelCase(QStringView name) {
    QString result;
    result.reserve(name.size());
    bool upper = false;
    for (QChar ch : name) {
        if (ch == u'-') {
            upper = true;
            continue;
        }
        result.append(upper ? ch.toUpper() : ch);
        upper = false;
    }
    return result;
}

QString Tokens::source() {
    return QStringView(
#include "generated/KetTokenSource.inc"
               )
        .toString();
}

} // namespace ket
