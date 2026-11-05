// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QStringList>

namespace ket {

// Light is the default; Regular is for emphasis (an active navigation item, a primary
// action). A filled variant never marks selection.
enum class IconWeight { Light, Regular };

// Két Symbol icons (CC BY 4.0, third_party/ket-symbol) compiled into KetQt::Icons.
// Icons are named by Két code; KetPlus' Lucide names map through codeForLucide().
//
// Sizes follow the drawn masters (14, 16, 20 and 24 px): a box uses the nearest master, the
// smaller one on a tie, so 14 px compact icons use their own drawing, 17-18 px boxes the
// 16 px master, 19-22 px the 20 px master and 23 px or more the 24 px master. A master is
// drawn at its own size and centred on whole device pixels (a 17 px box holds the 16 px
// drawing); only a box smaller than its master scales it down.
class Icons final {
  public:
    [[nodiscard]] static bool contains(QStringView code);
    [[nodiscard]] static QStringList codes();
    // The Két code for a KetPlus Lucide name, or an empty string.
    [[nodiscard]] static QString codeForLucide(QStringView lucide);

    [[nodiscard]] static int masterSize(qreal box);
    // Logical pixels the drawing occupies inside `box`.
    [[nodiscard]] static qreal drawSize(qreal box);
    // Offset of the drawing inside `box`, rounded down to whole device pixels.
    [[nodiscard]] static qreal offset(qreal box, qreal devicePixelRatio);
    // `:/ket-symbol/<weight>/<size>/<code>.svg`; prefix "qrc" for QML URLs.
    [[nodiscard]] static QString resourcePath(QStringView code, qreal box,
                                              IconWeight weight = IconWeight::Light);

    // The icon drawn in `color` at `box` logical pixels, for `devicePixelRatio`.
    [[nodiscard]] static QPixmap pixmap(QStringView code, qreal box, qreal devicePixelRatio,
                                        const QColor& color, IconWeight weight = IconWeight::Light);
    // A QIcon that renders with the rules above (black; kit components tint it).
    [[nodiscard]] static QIcon icon(QStringView code, IconWeight weight = IconWeight::Light);
};

} // namespace ket
