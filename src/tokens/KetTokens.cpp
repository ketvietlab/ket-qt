// SPDX-License-Identifier: MPL-2.0
#include "KetTokens.h"

#include <QFontDatabase>
#include <QHash>
#include <QReadWriteLock>
#include <atomic>
#include <optional>

namespace ket {
namespace {

struct Entry {
    QStringView name;
    TokenKind kind;
    QStringView light;
    QStringView dark;
};

// Entry addresses are stable for the lifetime of the generated table. Runtime
// paint reads therefore need neither a temporary QString nor a second name lookup.
struct OverrideBranch {
    std::atomic<bool> active{false};
    QHash<const Entry*, QColor> colors;
};
struct ColorOverrides {
    QReadWriteLock lock;
    OverrideBranch light;
    OverrideBranch dark;
};
ColorOverrides& overrides() {
    static ColorOverrides state;
    return state;
}
std::optional<QColor> overrideColor(const Entry& entry, Theme theme) {
    auto& state = overrides();
    const auto& branch = theme == Theme::Dark ? state.dark : state.light;
    // The default path never takes the map lock. Writers publish the flag only
    // after replacing the complete map while holding the write lock.
    if (!branch.active.load(std::memory_order_acquire))
        return std::nullopt;
    const QReadLocker locker(&state.lock);
    const auto it = branch.colors.constFind(&entry);
    return it == branch.colors.cend() ? std::nullopt : std::optional<QColor>(*it);
}

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

QColor colorOf(const Entry& entry, Theme theme) {
    if (const auto color = overrideColor(entry, theme))
        return *color;
    return QColor::fromString(valueOf(entry, theme));
}

QVariant typed(const Entry& entry, Theme theme) {
    const QStringView value = valueOf(entry, theme);
    switch (entry.kind) {
    case TokenKind::Color:
        return colorOf(entry, theme);
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
    if (entry && entry->kind == TokenKind::Color) {
        if (const auto color = overrideColor(*entry, theme))
            return color->name(color->alpha() == 255 ? QColor::HexRgb : QColor::HexArgb);
    }
    return entry ? valueOf(*entry, theme).toString() : QString();
}

QColor Tokens::color(QStringView name, Theme theme) {
    const Entry* entry = find(name);
    if (!entry || entry->kind != TokenKind::Color)
        return {};
    return colorOf(*entry, theme);
}

QStringList Tokens::replaceColorOverrides(Theme theme, const QHash<QString, QColor>& colors) {
    QHash<const Entry*, QColor> accepted;
    QStringList rejected;
    for (auto it = colors.cbegin(); it != colors.cend(); ++it) {
        const auto* entry = find(it.key());
        if (!entry || entry->kind != TokenKind::Color || !it.value().isValid())
            rejected.append(it.key());
        else
            accepted.insert(entry, it.value());
    }
    rejected.sort();
    auto& state = overrides();
    const QWriteLocker locker(&state.lock);
    auto& branch = theme == Theme::Dark ? state.dark : state.light;
    branch.colors.swap(accepted);
    branch.active.store(!branch.colors.isEmpty(), std::memory_order_release);
    return rejected;
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
