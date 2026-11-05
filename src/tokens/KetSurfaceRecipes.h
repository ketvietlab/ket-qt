// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

namespace ket {

// A background task's state (`.bg-row.is-*`).
enum class TaskStatus { Running, Stopping, Completed, Failed, Stopped };

// The command palette (`.spotlight`): a search row over grouped results and a key-hint footer,
// opened over a backdrop near the top of the window.
struct CommandPaletteRecipe {
    QColor background;
    QColor border;
    QColor backdrop;
    Shadow shadow;
    QColor searchIcon;
    QColor text;
    QColor placeholder;
    QColor rule;
    QColor group;
    QColor footer;
    qreal width = 0;
    qreal top = 0;
    qreal radius = 0;
    qreal maxListHeight = 0;
    qreal inputPixels = 0;
    qreal searchIconSize = 0;
    qreal searchPaddingBlock = 0;
    qreal searchPaddingInline = 0;
    qreal searchGap = 0;
    qreal inputPaddingBlock = 0;
    qreal groupPixels = 0;
    int groupWeight = 0;
    qreal groupLetterSpacing = 0;
    qreal groupPaddingTop = 0;
    qreal groupPaddingInline = 0;
    qreal groupPaddingBottom = 0;
    qreal listPaddingInline = 0;
    qreal listPaddingBottom = 0;
    qreal footerPaddingBlock = 0;
    qreal footerPaddingInline = 0;
    qreal footerGap = 0;
    qreal footerPixels = 0;
    qreal emptyIconSize = 0;
    qreal emptyTitlePixels = 0;
    qreal emptyTextPixels = 0;
    qreal emptyHeight = 0;
};
[[nodiscard]] CommandPaletteRecipe commandPaletteRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const CommandPaletteRecipe& recipe);

// One command palette result (`.spotlight-result`).
struct CommandResultRecipe {
    QColor background;
    QColor icon;
    QColor title;
    QColor detail;
    QColor trailing;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal radius = 0;
    qreal iconSize = 0;
    qreal trailingSize = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal detailPixels = 0;
    qreal detailGap = 0;
};
[[nodiscard]] CommandResultRecipe commandResultRecipe(bool highlighted, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const CommandResultRecipe& recipe);

// A file explorer row (`.file-row`, `.explorer-folder summary`): an indented Két icon and name,
// with a chevron on folders and an optional status letter on files.
struct FileRowRecipe {
    QColor background;
    QColor text;
    QColor status;
    QColor focusRing;
    qreal height = 0;
    qreal folderHeight = 0;
    qreal paddingInline = 0;
    qreal spacing = 0;
    qreal folderSpacing = 0;
    qreal iconSize = 0;
    qreal chevronSize = 0;
    qreal fontPixels = 0;
    qreal statusPixels = 0;
    qreal indentStart = 0;
    qreal indentStep = 0;
    qreal opacity = 0;
};
[[nodiscard]] FileRowRecipe fileRowRecipe(bool active, const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const FileRowRecipe& recipe);

// The changed files card in the transcript (`.changed-files`): a heading, one row per file
// (`.changed-file`) and a footer.
struct ChangedFilesRecipe {
    QColor background;
    QColor border;
    QColor headingBackground;
    QColor heading;
    QColor icon;
    QColor name;
    QColor path;
    QColor added;
    QColor removed;
    QColor footer;
    QColor rowHover;
    qreal radius = 0;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal headingGap = 0;
    qreal rowPaddingBlock = 0;
    qreal rowGap = 0;
    qreal footerPaddingBlock = 0;
    qreal iconSize = 0;
    qreal arrowSize = 0;
    qreal textPixels = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    int nameWeight = 0;
    qreal pathPixels = 0;
    qreal lineHeight = 0;
};
[[nodiscard]] ChangedFilesRecipe changedFilesRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const ChangedFilesRecipe& recipe);

// Unified diff lines (`.diff-line`, `.hunk-header`): line numbers, the +/- sign and the
// monospace text on the added or removed tint.
struct DiffRecipe {
    QColor addBackground;
    QColor removeBackground;
    QColor addSign;
    QColor removeSign;
    QColor sign;
    QColor number;
    QColor text;
    QColor hunkBackground;
    QColor hunkText;
    QColor background;
    qreal lineHeight = 0;
    qreal fontPixels = 0;
    qreal numberWidth = 0;
    qreal numberGap = 0;
    qreal signWidth = 0;
    qreal codeGap = 0;
    qreal hunkPaddingBlock = 0;
    qreal hunkPaddingInline = 0;
    qreal paddingBlock = 0;
    qreal paddingEnd = 0;
};
[[nodiscard]] DiffRecipe diffRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const DiffRecipe& recipe);

// A background task above the composer (`.bg-row`): status and kind icons, the title, a muted
// meta line, and stop or dismiss buttons.
struct TaskRowRecipe {
    QColor background;
    QColor border;
    QColor status;
    QColor kind;
    QColor title;
    QColor meta;
    QColor action;
    QColor actionDanger;
    qreal minHeight = 0;
    qreal paddingStart = 0;
    qreal paddingEnd = 0;
    qreal spacing = 0;
    qreal radius = 0;
    qreal iconSize = 0;
    qreal titlePixels = 0;
    qreal metaPixels = 0;
    qreal actionSize = 0;
};
[[nodiscard]] TaskRowRecipe taskRowRecipe(TaskStatus status, bool current, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const TaskRowRecipe& recipe);

// The background task strip's toggle (`.bg-bar-header`) and its row spacing.
struct TaskBarRecipe {
    QColor background;
    QColor text;
    QColor focusRing;
    qreal height = 0;
    qreal paddingInline = 0;
    qreal radius = 0;
    qreal fontPixels = 0;
    qreal iconSize = 0;
    qreal chevronSize = 0;
    qreal spacing = 0;
    qreal rowGap = 0;
    qreal opacity = 0;
};
[[nodiscard]] TaskBarRecipe taskBarRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const TaskBarRecipe& recipe);

// An update in Settings (`.update-card`): title, detail, progress, release notes and actions.
struct UpdateCardRecipe {
    QColor background;
    QColor border;
    QColor title;
    QColor detail;
    QColor notes;
    qreal radius = 0;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal detailPixels = 0;
    qreal headerGap = 0;
    qreal headerColumnGap = 0;
    qreal sectionGap = 0;
    qreal notesPixels = 0;
    qreal notesLineHeight = 0;
    qreal notesIndent = 0;
    qreal actionsGap = 0;
    qreal actionsTop = 0;
};
[[nodiscard]] UpdateCardRecipe updateCardRecipe(bool ready, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const UpdateCardRecipe& recipe);

// The sidebar's update notice (`.update-pill`).
struct UpdatePillRecipe {
    QColor background;
    QColor border;
    QColor title;
    QColor detail;
    QColor focusRing;
    qreal paddingBlock = 0;
    qreal paddingInline = 0;
    qreal radius = 0;
    qreal spacing = 0;
    qreal iconSize = 0;
    qreal titlePixels = 0;
    int titleWeight = 0;
    qreal detailPixels = 0;
    qreal opacity = 0;
};
[[nodiscard]] UpdatePillRecipe updatePillRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const UpdatePillRecipe& recipe);

// A theme choice in Settings (`.theme-card`): a miniature of the window in `preview`'s colours
// and the label with a check. Lengths and the frame follow the current theme.
struct ThemeCardRecipe {
    QColor background;
    QColor border;
    QColor ring;
    QColor label;
    QColor check;
    QColor focusRing;
    QColor miniBackground;
    QColor miniBorder;
    QColor miniSidebar;
    QColor miniBar;
    QColor miniAccentBar;
    QColor miniTitle;
    QColor miniLine;
    QColor miniButton;
    QColor miniButtonBorder;
    qreal radius = 0;
    qreal miniHeight = 0;
    qreal miniMargin = 0;
    qreal miniSideWidth = 0;
    qreal labelPaddingBlock = 0;
    qreal labelPaddingInline = 0;
    qreal labelPixels = 0;
    qreal checkSize = 0;
    qreal opacity = 0;
};
[[nodiscard]] ThemeCardRecipe themeCardRecipe(bool selected, Theme preview,
                                              const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const ThemeCardRecipe& recipe);

// The accent sample in Settings (`.accent-swatch`).
struct AccentSwatchRecipe {
    QColor fill;
    QColor border;
    QColor ring;
    qreal size = 0;
    qreal radius = 0;
    qreal borderWidth = 0;
};
[[nodiscard]] AccentSwatchRecipe accentSwatchRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const AccentSwatchRecipe& recipe);

} // namespace ket
