// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QVariantMap>

namespace ket {

// Component specifications resolved from tokens. The Widgets and Qt Quick layers draw
// from these so a variant's colours, sizes and states are defined exactly once.

enum class ButtonVariant { Primary, Secondary, Ghost, Danger };
enum class ControlSize { Small, Medium };
enum class BadgeTone { Neutral, Accent, Info, Success, Warning, Danger };

struct InteractionState {
    bool hovered = false;
    bool pressed = false;
    bool enabled = true;
    bool focused = false;
};

struct ButtonRecipe {
    QColor background;
    QColor border; // transparent when the variant has none
    QColor text;
    QColor focusRing; // transparent unless focused
    qreal height = 0;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal iconSize = 0;
    qreal spacing = 0;
    qreal fontPixels = 0;
    int fontWeight = 400;
    qreal opacity = 1.0;
};

struct BadgeRecipe {
    QColor background;
    QColor border;
    QColor text;
    qreal height = 0;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal fontPixels = 0;
    int fontWeight = 400;
};

[[nodiscard]] ButtonRecipe buttonRecipe(ButtonVariant variant, ControlSize size,
                                        const InteractionState& state, Theme theme);
[[nodiscard]] BadgeRecipe badgeRecipe(BadgeTone tone, Theme theme);

// camelCase maps of the recipes, for QML.
[[nodiscard]] QVariantMap toVariantMap(const ButtonRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const BadgeRecipe& recipe);

} // namespace ket
