// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QColor>
#include <QFont>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVariantMap>

namespace ket {

enum class Theme { Light, Dark };

enum class TokenKind { Color, Length, Number, Duration, Easing, Shadow, Font, Other };

// Read-only access to the Két Design System tokens compiled into KetQt::Tokens.
// Names are the CSS custom property without the `--kv-` prefix, e.g. "page-bg".
// Lengths are logical pixels at 100% zoom; durations are milliseconds.
class Tokens final {
  public:
    Tokens() = delete;

    [[nodiscard]] static bool contains(QStringView name);
    [[nodiscard]] static TokenKind kind(QStringView name);
    [[nodiscard]] static QStringList names();

    // The resolved CSS value, e.g. "#f7f5f5", "32", "0 2px 6px #3d000000".
    [[nodiscard]] static QString raw(QStringView name, Theme theme);
    // An invalid QColor, 0 or an empty family list when the name is unknown or of another kind.
    [[nodiscard]] static QColor color(QStringView name, Theme theme);
    [[nodiscard]] static qreal px(QStringView name);
    [[nodiscard]] static qreal number(QStringView name);
    [[nodiscard]] static int ms(QStringView name);
    // Families of a font stack in order, without CSS generic families.
    [[nodiscard]] static QStringList fontFamilies(QStringView name);
    // The first family of the stack that is installed, or an empty string (use the platform font).
    [[nodiscard]] static QString availableFamily(QStringView name);

    // Every token as camelCase keys ("page-bg" -> "pageBg"): QColor, qreal, int or QString.
    [[nodiscard]] static QVariantMap map(Theme theme);
    [[nodiscard]] static QString camelCase(QStringView name);

    // Source of the compiled table, e.g. "@ketvietlab/design-system 0.1.44 (e8adee2d)".
    [[nodiscard]] static QString source();
};

} // namespace ket
