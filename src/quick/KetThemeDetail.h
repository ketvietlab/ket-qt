// SPDX-License-Identifier: MPL-2.0
// Helpers shared by the KetTheme translation units; not installed.
#pragma once

#include "KetAppearance.h"

#include <QVariantMap>

#include <initializer_list>

namespace ket::detail {

inline Theme themeOf(bool dark) { return dark ? Theme::Dark : Theme::Light; }

// Recipes hold lengths at 100 %; QML receives them scaled to the zoom.
inline void scaleLengths(QVariantMap& recipe, qreal zoom, std::initializer_list<QStringView> keys) {
    for (QStringView key : keys) {
        const QString name = key.toString();
        recipe.insert(name, Appearance::scale(recipe.value(name).toReal(), zoom));
    }
}

inline constexpr std::initializer_list<QStringView> kShadowLengths = {u"shadowX", u"shadowY",
                                                                      u"shadowBlur"};

} // namespace ket::detail
