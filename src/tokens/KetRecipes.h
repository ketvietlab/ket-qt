// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetTokens.h"

#include <QVariantMap>

#include <utility>

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
    // A locked tag leads with this icon; empty for every other tag.
    QString iconCode;
    qreal iconSize = 0;
    qreal iconGap = 0;
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
    qreal descriptionGap = 0;
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

// One parsed shadow token layer ("x y blur colour"; spread is ignored).
struct Shadow {
    qreal x = 0;
    qreal y = 0;
    qreal blur = 0;
    QColor color;
};

[[nodiscard]] Shadow shadowToken(QStringView name, Theme theme);

enum class BannerTone { Info, Warning, Danger };

// An inline notice with an icon, a title, a message and an optional action (`.banner`).
struct BannerRecipe {
    QColor background;
    QColor border;
    QColor text;  // title, message and icon
    QString icon; // Két Symbol code
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal messagePixels = 0;
    qreal messageGap = 0;
};

// A transient confirmation at the bottom of the window (`.toast`).
struct ToastRecipe {
    QColor background;
    QColor border;
    QColor text;
    QColor icon;
    Shadow shadow;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal fontPixels = 0;
    qreal bottomOffset = 0;
    // The card's widest width, and its least distance from the window's sides; longer text
    // wraps onto more lines.
    qreal maxWidth = 0;
    qreal sideMargin = 0;
    // The Két symbol before the text.
    QString iconName;
    int durationMs = 0;
};

// What covers the window behind a modal surface (dialog, command palette). It must darken:
// the light theme uses its translucent ink; a dark window is darker than that ink, so the
// dark theme uses black at Profile::backdropOpacityDark.
[[nodiscard]] QColor modalBackdrop(Theme theme);

// A modal dialog over the window (`.modal`, `.modal-backdrop`).
struct DialogRecipe {
    QColor backdrop;
    QColor background;
    QColor border;
    QColor divider;
    QColor footer;
    QColor title;
    Shadow shadow;
    qreal width = 0;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal headerPaddingBlock = 0;
    qreal bodyPadding = 0;
    qreal footerPaddingBlock = 0;
    qreal footerGap = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal margin = 0; // minimum space around the dialog inside the window
};

// Context menus and select popups (`.desktop-context-menu`); one recipe per item state.
struct MenuRecipe {
    QColor background;
    QColor border;
    QColor separator;
    QColor sectionText; // a group heading such as "Mode"
    Shadow shadow;
    qreal minWidth = 0;
    qreal maxWidth = 0;
    qreal padding = 0;
    qreal radius = 0;
    qreal separatorMargin = 0;
    qreal sectionHeight = 0;
    qreal sectionPixels = 0;
};

struct MenuItemRecipe {
    QColor background; // transparent unless highlighted
    QColor text;
    QColor description;
    qreal height = 0;
    qreal radius = 0;
    qreal paddingStart = 0;
    qreal paddingEnd = 0;
    qreal spacing = 0;
    qreal checkSize = 0; // the check or radio column
    qreal iconSize = 0;
    qreal fontPixels = 0;
    qreal descriptionPixels = 0;
    qreal descriptionGap = 0;
    qreal opacity = 1.0;
};

struct TooltipRecipe {
    QColor background;
    QColor text;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal maxWidth = 0;
    qreal fontPixels = 0;
    int delayMs = 0;
};

// A sidebar navigation row (`.nav-item`) with an optional count (`.nav-count`) or kbd.
struct NavItemRecipe {
    QColor background;
    QColor text; // label and icon
    QColor focusRing;
    QColor countBorder;
    QColor countText;
    QColor kbdText;
    int fontWeight = 400;
    qreal height = 0;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal fontPixels = 0;
    qreal countHeight = 0;
    qreal countPadding = 0;
    qreal countRadius = 0;
    qreal countFontPixels = 0;
    qreal kbdFontPixels = 0;
    qreal opacity = 1.0;
};

// A keyboard shortcut label (`kbd`).
struct KbdRecipe {
    QColor text;
    qreal fontPixels = 0;
};

// The segmented control's track (`.pill-tabs`) and one segment in a given state.
struct SegmentedRecipe {
    QColor background;
    QColor border;
    qreal padding = 0; // including the border
    qreal radius = 0;
    qreal spacing = 0;
};

struct SegmentRecipe {
    QColor background;
    QColor text;
    QColor focusRing;
    Shadow shadow;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal fontPixels = 0;
    qreal opacity = 1.0;
};

// The underlined tab strip (`.file-tabs`) and one tab in a given state.
struct TabBarRecipe {
    QColor background;
    QColor border; // the strip's bottom edge and each tab's right edge
    qreal height = 0;
};

struct TabRecipe {
    QColor background;
    QColor text;
    QColor indicator; // the bottom border; transparent unless selected
    QColor focusRing;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal indicatorHeight = 0;
    qreal fontPixels = 0;
    qreal opacity = 1.0;
};

struct ProgressRecipe {
    QColor track;
    QColor fill;
    qreal height = 0;
    qreal radius = 0;
    qreal busyFraction = 0; // the moving segment of an indeterminate bar
    int busyDurationMs = 0;
};

// The animated working indicator (`.working-bars`): bars that shrink and fade in turn.
struct ActivityRecipe {
    QColor color;
    qreal barWidth = 0;
    qreal barHeight = 0;
    qreal minBarHeight = 0;
    qreal spacing = 0;
    qreal radius = 0;
    qreal minOpacity = 1.0;
    int durationMs = 0;
    int staggerMs = 0;
    int bars = 3;
};

// A centred placeholder for an empty view (`.empty-state`).
struct EmptyStateRecipe {
    QColor icon;
    QColor title;
    QColor message;
    qreal iconSize = 0;
    qreal spacing = 0;
    qreal titlePixels = 0;
    int titleWeight = 400;
    qreal messagePixels = 0;
    qreal messageMaxWidth = 0;
    qreal lineHeight = 1.0;
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
// A tag that reads as fixed (a role, not a label the user set): a steady tint that does not
// answer the pointer, stronger text and a leading lock. It is neither removable nor
// filterable.
[[nodiscard]] TagRecipe lockedTagRecipe(const QColor& color, Theme theme);

// `invalid` draws the danger border, as an `.error-text` field does.
[[nodiscard]] FieldRecipe fieldRecipe(const InteractionState& state, bool invalid, Theme theme);
[[nodiscard]] ToggleRecipe toggleRecipe(ToggleKind kind, bool checked,
                                        const InteractionState& state, Theme theme);
[[nodiscard]] ChoiceRecipe choiceRecipe(bool checked, const InteractionState& state, Theme theme);
[[nodiscard]] FieldRowRecipe fieldRowRecipe(Theme theme);
[[nodiscard]] BannerRecipe bannerRecipe(BannerTone tone, Theme theme);
// Success confirms a finished action; Neutral carries any other message, failures included,
// without claiming success.
enum class ToastTone { Success, Neutral };
[[nodiscard]] ToastRecipe toastRecipe(Theme theme, ToastTone tone = ToastTone::Success);
[[nodiscard]] DialogRecipe dialogRecipe(Theme theme);
[[nodiscard]] MenuRecipe menuRecipe(Theme theme);
// Danger is a destructive command; Warning is an elevated choice such as full access.
enum class MenuItemTone { Normal, Danger, Warning };
// `highlighted` is hover or keyboard selection.
[[nodiscard]] MenuItemRecipe menuItemRecipe(bool highlighted, bool enabled, MenuItemTone tone,
                                            Theme theme);
[[nodiscard]] TooltipRecipe tooltipRecipe(Theme theme);

// camelCase maps of the recipes, for QML.
[[nodiscard]] NavItemRecipe navItemRecipe(bool selected, const InteractionState& state,
                                          Theme theme);
[[nodiscard]] KbdRecipe kbdRecipe(Theme theme);
[[nodiscard]] SegmentedRecipe segmentedRecipe(Theme theme);
[[nodiscard]] SegmentRecipe segmentRecipe(bool selected, const InteractionState& state,
                                          Theme theme);
[[nodiscard]] TabBarRecipe tabBarRecipe(Theme theme);
[[nodiscard]] TabRecipe tabRecipe(bool selected, const InteractionState& state, Theme theme);
[[nodiscard]] ProgressRecipe progressRecipe(Theme theme);
// `color` overrides the default success colour (an invalid colour keeps it).
[[nodiscard]] ActivityRecipe activityRecipe(Theme theme, const QColor& color = {});
// The bar's height and opacity `elapsedMs` into the animation (CSS `working` keyframes).
[[nodiscard]] std::pair<qreal, qreal> activityBarFrame(const ActivityRecipe& recipe, int bar,
                                                       qint64 elapsedMs);
[[nodiscard]] EmptyStateRecipe emptyStateRecipe(Theme theme);

[[nodiscard]] QVariantMap toVariantMap(const ButtonRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const BadgeRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TagRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const FieldRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ToggleRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ChoiceRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const FieldRowRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const BannerRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ToastRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const DialogRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const MenuRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const MenuItemRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TooltipRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const NavItemRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const KbdRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SegmentedRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const SegmentRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TabBarRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const TabRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ProgressRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ActivityRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const EmptyStateRecipe& recipe);

} // namespace ket
