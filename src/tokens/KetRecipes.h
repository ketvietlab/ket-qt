// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QVariantMap>

namespace ket {

// Component specifications resolved from tokens. The Widgets and Qt Quick layers draw
// from these so a variant's colours, sizes and states are defined exactly once.
//
// Colours come from tokens. Sizes, spacing, weights and opacities that a product decides
// come from the current ket::Profile (KetProfile.h): ketPlus() by default, which follows
// the KetPlus desktop mock, or designSystem() and product profiles such as Lampion's.

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

// Text fields and selects (`input`, `select`, `.search-field`).
struct FieldRecipe {
    QColor background;
    QColor border;
    QColor text;
    QColor placeholder;
    QColor icon; // leading icon and the select chevron
    qreal height = 0;
    qreal radius = 0;
    qreal borderWidth = 1; // 2 when focused
    qreal paddingInline = 0;
    qreal iconSize = 0;
    qreal spacing = 0; // icon to text
    qreal fontPixels = 0;
    qreal opacity = 1.0;
};

enum class ToggleKind { Checkbox, Radio };

// The checkbox or radio indicator and its inline label (`.remote-check`).
struct ToggleRecipe {
    QColor background;
    QColor border;
    QColor mark; // check or dot; transparent when unchecked
    QColor focusRing;
    QColor label;
    qreal size = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal fontPixels = 0;
    qreal opacity = 1.0;
};

// A bordered option with a title and description (`.choice`), holding a toggle.
struct ChoiceRecipe {
    QColor background;
    QColor border;
    QColor title;
    QColor description;
    QColor focusRing;
    qreal padding = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal descriptionPixels = 0;
    qreal opacity = 1.0;
};

// A labelled settings row (`.field-row`): label column, control, help or error text.
struct FieldRowRecipe {
    QColor label;
    QColor help;
    QColor error;
    qreal labelWidth = 0;
    qreal gap = 0;
    qreal labelTop = 0;
    qreal labelPixels = 0;
    qreal helpPixels = 0;
    qreal helpGap = 0;
    qreal rowGap = 0;
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

// `invalid` draws the danger border, as an `.error-text` field does.
[[nodiscard]] FieldRecipe fieldRecipe(const InteractionState& state, bool invalid, Theme theme);
[[nodiscard]] ToggleRecipe toggleRecipe(ToggleKind kind, bool checked,
                                        const InteractionState& state, Theme theme);
[[nodiscard]] ChoiceRecipe choiceRecipe(bool checked, const InteractionState& state, Theme theme);
[[nodiscard]] FieldRowRecipe fieldRowRecipe(Theme theme);

// camelCase maps of the recipes, for QML.
[[nodiscard]] QVariantMap toVariantMap(const ButtonRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const BadgeRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TagRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const FieldRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ToggleRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ChoiceRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const FieldRowRecipe& recipe);

} // namespace ket
