// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

namespace ket {

// The states a conversation shows in the sidebar (K.conversationStatus in the mock).
enum class ConversationStatus { NotStarted, Working, NeedsInput, Error, Completed, Stopped };

// A two-line sidebar conversation row: the title, then the number with tags or the status.
struct ConversationRowRecipe {
    QColor background;
    QColor title;
    QColor number;
    QColor meta;
    QColor focusRing;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal lineGap = 0;
    qreal titlePixels = 0;
    qreal titleLineHeight = 0;
    int titleWeight = 400;
    qreal numberPixels = 0;
    qreal metaPixels = 0;
    qreal metaHeight = 0;
    qreal metaGap = 0; // between the number and the tags or status
    qreal tagGap = 0;
    qreal tagMaxWidth = 0;
    qreal indicatorSize = 0;
    qreal indicatorTop = 0;
    qreal opacity = 1.0;
};

// The trailing status icon and the meta-line status label of a conversation.
struct ConversationStatusRecipe {
    QString icon; // Két Symbol code
    QColor iconColor;
    QColor labelColor;
    bool spinning = false;
    int spinMs = 0;
};

struct ComposerRecipe {
    QColor background;
    QColor border;
    QColor ring; // transparent unless focused
    QColor text;
    QColor placeholder;
    Shadow shadow;
    qreal radius = 0;
    qreal padding = 0;
    qreal ringWidth = 0;
    qreal textPixels = 0;
    qreal lineHeight = 1.0; // multiple of the font size
    qreal textPaddingInline = 0;
    qreal textPaddingBlock = 0;
    qreal minInputHeight = 0;
    int maxLines = 0;
    qreal sendSize = 0;
    qreal attachIconSize = 0; // glyph of the leading add button (IconButton::setGlyphSize)
    qreal toolbarHeight = 0;
    qreal toolbarGap = 0;
};

// The composer's one action button (`.send-button`): a circle that sends, or stops the
// running turn while the message box is empty. Sending is disabled with nothing typed.
enum class ComposerAction { Send, Stop };
struct ComposerActionRecipe {
    QColor background;
    QColor icon;
    QColor focusRing; // transparent unless focused
    qreal size = 0;
    qreal iconSize = 0;
    qreal stopGlyph = 0; // the stop square's side
    qreal stopRadius = 0;
    qreal opacity = 1.0;
};
[[nodiscard]] ComposerAction composerAction(bool busy, bool hasText);
[[nodiscard]] ComposerActionRecipe composerActionRecipe(ComposerAction action, bool enabled,
                                                        const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const ComposerActionRecipe& recipe);

struct UserMessageRecipe {
    QColor background;
    QColor text;
    qreal radius = 0;
    qreal paddingInline = 0;
    qreal paddingBlock = 0;
    qreal fontPixels = 0;
    qreal lineHeight = 1.0;
    qreal maxWidth = 1.0; // fraction of the available width
    qreal spacingAfter = 0;
};

struct AssistantMessageRecipe {
    QColor avatarBackground;
    QColor avatarIcon;
    QColor name;
    QColor time;
    QColor body;
    qreal avatarSize = 0;
    qreal avatarRadius = 0;
    qreal avatarIconSize = 0;
    qreal spacing = 0;
    qreal namePixels = 0;
    int nameWeight = 400;
    qreal timePixels = 0;
    qreal metaGap = 0;
    qreal metaBottom = 0;
    qreal bodyPixels = 0;
    qreal lineHeight = 1.0;
    qreal spacingAfter = 0;
};

struct DayDividerRecipe {
    QColor line;
    QColor text;
    qreal fontPixels = 0;
    qreal spacing = 0;
    qreal marginTop = 0;
    qreal marginBottom = 0;
};

// One tool or command step in the transcript (`.activity-row`); `expandable` rows end in a
// chevron that turns when expanded.
struct ActivityRowRecipe {
    QColor background;
    QColor text;
    QColor detail;
    QColor focusRing;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal radius = 0;
    qreal fontPixels = 0;
    qreal detailPixels = 0;
    qreal iconSize = 0;
    qreal chevronSize = 0;
    qreal opacity = 1.0;
};

[[nodiscard]] ConversationRowRecipe
conversationRowRecipe(bool selected, const InteractionState& state, Theme theme);
[[nodiscard]] ConversationStatusRecipe conversationStatusRecipe(ConversationStatus status,
                                                                Theme theme);
// The mock's status label, in English; apps translate it.
[[nodiscard]] QString conversationStatusLabel(ConversationStatus status);
[[nodiscard]] ComposerRecipe composerRecipe(bool focused, Theme theme);
[[nodiscard]] UserMessageRecipe userMessageRecipe(Theme theme);
[[nodiscard]] AssistantMessageRecipe assistantMessageRecipe(Theme theme);
[[nodiscard]] DayDividerRecipe dayDividerRecipe(Theme theme);
[[nodiscard]] ActivityRowRecipe activityRowRecipe(const InteractionState& state, Theme theme);

[[nodiscard]] QVariantMap toVariantMap(const ConversationRowRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ConversationStatusRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ComposerRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const UserMessageRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const AssistantMessageRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const DayDividerRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ActivityRowRecipe& recipe);

} // namespace ket
