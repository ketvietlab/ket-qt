// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QVariantMap>

namespace ket {

// Component specifications resolved from tokens. The Widgets and Qt Quick layers draw
// from these so a variant's colours, sizes and states are defined exactly once.
//
// Compositions follow the KetPlus desktop mock (`ketplus.ketatlas`, the `.btn`, `.icon-btn`,
// `.badge` and `.conversation-tag` rules), which is built on the same Két DS release. Where
// the mock uses a literal that has no token, the recipe names it next to its source rule.

// GhostDanger is the mock's `.btn.ghost.danger`: a transparent destructive action.
enum class ButtonVariant { Primary, Secondary, Ghost, Danger, GhostDanger };
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

struct TagRecipe {
    QColor background;
    QColor text;
    qreal height = 0;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal maxWidth = 0; // longer labels elide
    qreal fontPixels = 0;
    int fontWeight = 400;
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
// The square, borderless icon-only button (`.icon-btn`). Small is the compact header size.
[[nodiscard]] ButtonRecipe iconButtonRecipe(ControlSize size, const InteractionState& state,
                                            Theme theme);
[[nodiscard]] BadgeRecipe badgeRecipe(BadgeTone tone, Theme theme);
// A label-only chip tinted with `color`, a colour the app owns (tag colours are user data,
// not design-system tokens). `overflow` is the muted "+N" chip; `hovered` is the filter hover.
[[nodiscard]] TagRecipe tagRecipe(const QColor& color, bool overflow, bool hovered, Theme theme);

// camelCase maps of the recipes, for QML.
[[nodiscard]] QVariantMap toVariantMap(const ButtonRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const BadgeRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TagRecipe& recipe);

} // namespace ket
