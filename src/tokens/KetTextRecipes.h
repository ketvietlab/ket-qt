// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QStringList>
#include <QVariantMap>

namespace ket {

// Multi-line text field (`textarea`). The frame, colours and inline padding are the single-line
// field's (ket::fieldRecipe); the field grows from minLines to maxLines line boxes and then
// scrolls. With a character limit, the `n / max` count sits below it, right-aligned.
struct TextAreaRecipe {
    FieldRecipe field; // `field.height` is unused
    QColor count;      // muted, danger above the limit
    qreal paddingBlock = 0;
    qreal lineHeight = 1.0;
    int minLines = 1;
    int maxLines = 1;
    qreal countPixels = 0;
    qreal countGap = 0;
};

// `invalid` is the host's mark; `overLimit` also makes the field invalid.
[[nodiscard]] TextAreaRecipe textAreaRecipe(const InteractionState& state, bool invalid,
                                            bool overLimit, Theme theme);

// A drawn `@agent/` mention: the accent text on its subtle fill, or the danger colours when the
// host marks the conversation unknown.
struct MentionTokenRecipe {
    QColor text;
    QColor background;
    int weight = 400;
};

[[nodiscard]] MentionTokenRecipe mentionTokenRecipe(bool unknown, Theme theme);

// Read-only multi-line text folded to `foldedLines` with a Show more / Show less button.
// Mentions are links drawn as mention tokens; the focused one gets the focus colour.
struct ExpandableTextRecipe {
    QColor text;
    QColor focus; // the mention that has keyboard focus
    MentionTokenRecipe mention;
    MentionTokenRecipe unknownMention;
    qreal fontPixels = 0;
    qreal lineHeight = 1.0;
    int foldedLines = 1;
    qreal toggleGap = 0;
};

[[nodiscard]] ExpandableTextRecipe expandableTextRecipe(Theme theme);

// `n / max`, and its spoken form ("12 of 8000 characters", "40 characters over the limit").
[[nodiscard]] QString characterCountText(qsizetype count, qsizetype max);
[[nodiscard]] QString characterCountDescription(qsizetype count, qsizetype max);

// Rich text for an expandable text: `text` escaped with its line breaks kept, and each mention
// a link `agent:<number>` in the mention colours (`unknown` numbers in the danger ones). The
// mention at `focusedMention` (an index into findAgentMentions()) is underlined in the focus
// colour; -1 marks none.
[[nodiscard]] QString mentionLinkedHtml(QStringView text, const ExpandableTextRecipe& recipe,
                                        const QStringList& unknown, int focusedMention);
// The number in a link from mentionLinkedHtml(), or an empty string.
[[nodiscard]] QString mentionLinkNumber(QStringView link);

[[nodiscard]] QVariantMap toVariantMap(const TextAreaRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const MentionTokenRecipe& recipe);
[[nodiscard]] QVariantMap toVariantMap(const ExpandableTextRecipe& recipe);

} // namespace ket
