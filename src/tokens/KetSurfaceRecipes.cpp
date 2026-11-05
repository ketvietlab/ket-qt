// SPDX-License-Identifier: MPL-2.0
// Surfaces: command palette, file rows, changed files, diffs, background tasks, updates and
// theme choices.
#include "KetSurfaceRecipes.h"

#include "KetProfile.h"

namespace ket {
namespace {

QColor token(QStringView name, Theme theme) { return Tokens::color(name, theme); }

[[maybe_unused]] QColor transparent() { return QColor(Qt::transparent); }

const Profile& profile() { return Profile::current(); }

QVariantMap mapOf(std::initializer_list<std::pair<const char*, QVariant>> entries) {
    QVariantMap map;
    for (const auto& [key, value] : entries)
        map.insert(QString::fromLatin1(key), value);
    return map;
}

QColor backdrop() {
    // As the dialog's: the light theme's main text colour, translucent, in both themes.
    QColor color = token(u"text-main", Theme::Light);
    color.setAlphaF(float(profile().backdropOpacity));
    return color;
}

QColor taskStatusColor(TaskStatus status, Theme theme) {
    switch (status) {
    case TaskStatus::Completed:
        return token(u"success", theme);
    case TaskStatus::Failed:
        return token(u"danger", theme);
    case TaskStatus::Running:
    case TaskStatus::Stopping:
        return token(u"accent-text", theme);
    case TaskStatus::Stopped:
        break;
    }
    return token(u"text-muted", theme);
}

} // namespace

CommandPaletteRecipe commandPaletteRecipe(Theme theme) {
    CommandPaletteRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"border-default", theme);
    recipe.backdrop = backdrop();
    recipe.shadow = shadowToken(u"shadow-popover", theme);
    recipe.searchIcon = token(u"accent-text", theme);
    recipe.text = token(u"text-main", theme);
    recipe.placeholder = token(u"text-muted", theme);
    recipe.rule = token(u"panel-border", theme);
    recipe.group = token(u"text-muted", theme);
    recipe.footer = token(u"text-muted", theme);
    recipe.width = profile().paletteWidth;
    recipe.top = profile().paletteTop;
    recipe.radius = profile().popoverRadius;
    recipe.maxListHeight = profile().paletteMaxListHeight;
    recipe.inputPixels = profile().paletteInputPixels;
    recipe.searchIconSize = profile().paletteSearchIconSize;
    recipe.searchPaddingBlock = profile().paletteSearchPaddingBlock;
    recipe.searchPaddingInline = profile().paletteSearchPaddingInline;
    recipe.searchGap = profile().paletteSearchGap;
    recipe.inputPaddingBlock = profile().paletteInputPaddingBlock;
    recipe.groupPixels = profile().paletteGroupPixels;
    recipe.groupWeight = profile().paletteGroupWeight;
    recipe.groupLetterSpacing = profile().paletteGroupLetterSpacing;
    recipe.groupPaddingTop = profile().paletteGroupPaddingTop;
    recipe.groupPaddingInline = profile().paletteGroupPaddingInline;
    recipe.groupPaddingBottom = profile().paletteGroupPaddingBottom;
    recipe.listPaddingInline = profile().paletteListPaddingInline;
    recipe.listPaddingBottom = profile().paletteListPaddingBottom;
    recipe.footerPaddingBlock = profile().paletteFooterPaddingBlock;
    recipe.footerPaddingInline = profile().paletteFooterPaddingInline;
    recipe.footerGap = profile().paletteFooterGap;
    recipe.footerPixels = profile().paletteFooterPixels;
    recipe.emptyIconSize = profile().paletteEmptyIconSize;
    recipe.emptyTitlePixels = profile().paletteInputPixels;
    recipe.emptyTextPixels = profile().paletteResultPixels;
    recipe.emptyHeight = profile().paletteEmptyHeight;
    return recipe;
}

QVariantMap toVariantMap(const CommandPaletteRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"backdrop", r.backdrop},
                  {"shadowX", r.shadow.x},
                  {"shadowY", r.shadow.y},
                  {"shadowBlur", r.shadow.blur},
                  {"shadowColor", r.shadow.color},
                  {"searchIcon", r.searchIcon},
                  {"text", r.text},
                  {"placeholder", r.placeholder},
                  {"rule", r.rule},
                  {"group", r.group},
                  {"footer", r.footer},
                  {"width", r.width},
                  {"top", r.top},
                  {"radius", r.radius},
                  {"maxListHeight", r.maxListHeight},
                  {"inputPixels", r.inputPixels},
                  {"searchIconSize", r.searchIconSize},
                  {"searchPaddingBlock", r.searchPaddingBlock},
                  {"searchPaddingInline", r.searchPaddingInline},
                  {"searchGap", r.searchGap},
                  {"inputPaddingBlock", r.inputPaddingBlock},
                  {"groupPixels", r.groupPixels},
                  {"groupWeight", r.groupWeight},
                  {"groupLetterSpacing", r.groupLetterSpacing},
                  {"groupPaddingTop", r.groupPaddingTop},
                  {"groupPaddingInline", r.groupPaddingInline},
                  {"groupPaddingBottom", r.groupPaddingBottom},
                  {"listPaddingInline", r.listPaddingInline},
                  {"listPaddingBottom", r.listPaddingBottom},
                  {"footerPaddingBlock", r.footerPaddingBlock},
                  {"footerPaddingInline", r.footerPaddingInline},
                  {"footerGap", r.footerGap},
                  {"footerPixels", r.footerPixels},
                  {"emptyIconSize", r.emptyIconSize},
                  {"emptyTitlePixels", r.emptyTitlePixels},
                  {"emptyTextPixels", r.emptyTextPixels},
                  {"emptyHeight", r.emptyHeight}});
}

CommandResultRecipe commandResultRecipe(bool highlighted, Theme theme) {
    CommandResultRecipe recipe;
    recipe.background = highlighted ? token(u"accent-subtle", theme) : transparent();
    recipe.icon = token(highlighted ? u"accent-text" : u"text-muted", theme);
    recipe.title = token(u"text-main", theme);
    recipe.detail = token(u"text-muted", theme);
    recipe.trailing = token(u"text-muted", theme);
    recipe.paddingBlock = profile().paletteResultPaddingBlock;
    recipe.paddingInline = profile().paletteResultPaddingInline;
    recipe.spacing = profile().paletteSearchGap;
    recipe.radius = profile().paletteResultRadius;
    recipe.iconSize = profile().iconSize;
    recipe.trailingSize = profile().paletteTrailingSize;
    recipe.titlePixels = profile().paletteResultPixels;
    recipe.titleWeight = profile().paletteResultWeight;
    recipe.detailPixels = profile().paletteGroupPixels;
    recipe.detailGap = profile().paletteDetailGap;
    return recipe;
}

QVariantMap toVariantMap(const CommandResultRecipe& r) {
    return mapOf({{"background", r.background},
                  {"icon", r.icon},
                  {"title", r.title},
                  {"detail", r.detail},
                  {"trailing", r.trailing},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"radius", r.radius},
                  {"iconSize", r.iconSize},
                  {"trailingSize", r.trailingSize},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"detailPixels", r.detailPixels},
                  {"detailGap", r.detailGap}});
}

FileRowRecipe fileRowRecipe(bool active, const InteractionState& state, Theme theme) {
    FileRowRecipe recipe;
    const bool live = state.enabled;
    recipe.background = active                                     ? token(u"accent-subtle", theme)
                        : live && (state.hovered || state.pressed) ? token(u"surface-hover", theme)
                                                                   : transparent();
    recipe.text = token(active ? u"accent-text" : u"text-secondary", theme);
    recipe.status = token(u"warning", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.height = profile().fileRowHeight;
    recipe.folderHeight = profile().folderRowHeight;
    recipe.paddingInline = profile().fileRowPaddingInline;
    recipe.spacing = profile().fileRowGap;
    recipe.folderSpacing = profile().folderRowGap;
    recipe.iconSize = profile().fileRowIconSize;
    recipe.chevronSize = profile().fileRowChevronSize;
    recipe.fontPixels = profile().fileRowPixels;
    recipe.statusPixels = profile().fileRowStatusPixels;
    recipe.indentStart = profile().fileRowIndentStart;
    recipe.indentStep = profile().fileRowIndentStep;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const FileRowRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"status", r.status},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"folderHeight", r.folderHeight},
                  {"paddingInline", r.paddingInline},
                  {"spacing", r.spacing},
                  {"folderSpacing", r.folderSpacing},
                  {"iconSize", r.iconSize},
                  {"chevronSize", r.chevronSize},
                  {"fontPixels", r.fontPixels},
                  {"statusPixels", r.statusPixels},
                  {"indentStart", r.indentStart},
                  {"indentStep", r.indentStep},
                  {"opacity", r.opacity}});
}

ChangedFilesRecipe changedFilesRecipe(Theme theme) {
    ChangedFilesRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(u"panel-border", theme);
    recipe.headingBackground = token(u"panel-bg-subtle", theme);
    recipe.heading = token(u"text-main", theme);
    recipe.icon = token(u"text-muted", theme);
    recipe.name = token(u"text-main", theme);
    recipe.path = token(u"text-muted", theme);
    recipe.added = token(u"success", theme);
    recipe.removed = token(u"danger", theme);
    recipe.footer = token(u"text-muted", theme);
    recipe.rowHover = token(u"surface-hover", theme);
    recipe.radius = profile().changedRadius;
    recipe.paddingBlock = profile().changedPaddingBlock;
    recipe.paddingInline = profile().changedPaddingInline;
    recipe.headingGap = profile().changedHeadingGap;
    recipe.rowPaddingBlock = profile().changedRowPaddingBlock;
    recipe.rowGap = profile().changedRowGap;
    recipe.footerPaddingBlock = profile().changedFooterPaddingBlock;
    recipe.iconSize = profile().smallIconSize;
    recipe.arrowSize = profile().changedArrowSize;
    recipe.textPixels = profile().changedTextPixels;
    recipe.titlePixels = profile().changedTitlePixels;
    recipe.titleWeight = profile().changedTitleWeight;
    recipe.nameWeight = profile().changedNameWeight;
    recipe.pathPixels = profile().changedPathPixels;
    recipe.lineHeight = profile().changedLineHeight;
    return recipe;
}

QVariantMap toVariantMap(const ChangedFilesRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"headingBackground", r.headingBackground},
                  {"heading", r.heading},
                  {"icon", r.icon},
                  {"name", r.name},
                  {"path", r.path},
                  {"added", r.added},
                  {"removed", r.removed},
                  {"footer", r.footer},
                  {"rowHover", r.rowHover},
                  {"radius", r.radius},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"headingGap", r.headingGap},
                  {"rowPaddingBlock", r.rowPaddingBlock},
                  {"rowGap", r.rowGap},
                  {"footerPaddingBlock", r.footerPaddingBlock},
                  {"iconSize", r.iconSize},
                  {"arrowSize", r.arrowSize},
                  {"textPixels", r.textPixels},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"nameWeight", r.nameWeight},
                  {"pathPixels", r.pathPixels},
                  {"lineHeight", r.lineHeight}});
}

DiffRecipe diffRecipe(Theme theme) {
    DiffRecipe recipe;
    recipe.addBackground = token(u"success-bg", theme);
    recipe.removeBackground = token(u"danger-bg", theme);
    recipe.addSign = token(u"success", theme);
    recipe.removeSign = token(u"danger", theme);
    recipe.sign = token(u"text-muted", theme);
    recipe.number = token(u"text-disabled", theme);
    recipe.text = token(u"text-main", theme);
    recipe.hunkBackground = token(u"accent-subtle", theme);
    recipe.hunkText = token(u"accent-text", theme);
    recipe.background = token(u"panel-bg", theme);
    recipe.lineHeight = profile().diffLineHeight;
    recipe.fontPixels = profile().codePixels;
    recipe.numberWidth = profile().diffNumberWidth;
    recipe.numberGap = profile().diffNumberGap;
    recipe.signWidth = profile().diffSignWidth;
    recipe.codeGap = profile().diffCodeGap;
    recipe.hunkPaddingBlock = profile().diffHunkPaddingBlock;
    recipe.hunkPaddingInline = profile().diffHunkPaddingInline;
    recipe.paddingBlock = profile().diffPaddingBlock;
    recipe.paddingEnd = profile().diffPaddingEnd;
    return recipe;
}

QVariantMap toVariantMap(const DiffRecipe& r) {
    return mapOf({{"addBackground", r.addBackground},
                  {"removeBackground", r.removeBackground},
                  {"addSign", r.addSign},
                  {"removeSign", r.removeSign},
                  {"sign", r.sign},
                  {"number", r.number},
                  {"text", r.text},
                  {"hunkBackground", r.hunkBackground},
                  {"hunkText", r.hunkText},
                  {"background", r.background},
                  {"lineHeight", r.lineHeight},
                  {"fontPixels", r.fontPixels},
                  {"numberWidth", r.numberWidth},
                  {"numberGap", r.numberGap},
                  {"signWidth", r.signWidth},
                  {"codeGap", r.codeGap},
                  {"hunkPaddingBlock", r.hunkPaddingBlock},
                  {"hunkPaddingInline", r.hunkPaddingInline},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingEnd", r.paddingEnd}});
}

TaskRowRecipe taskRowRecipe(TaskStatus status, bool current, Theme theme) {
    TaskRowRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(current ? u"border-strong" : u"border-default", theme);
    recipe.status = taskStatusColor(status, theme);
    recipe.kind = token(u"text-muted", theme);
    recipe.title = token(u"text-main", theme);
    recipe.meta = token(status == TaskStatus::Failed ? u"danger" : u"text-muted", theme);
    recipe.action = token(u"text-secondary", theme);
    recipe.actionDanger = token(u"danger", theme);
    recipe.minHeight = profile().taskRowHeight;
    recipe.paddingStart = profile().taskRowPaddingStart;
    recipe.paddingEnd = profile().taskRowPaddingEnd;
    recipe.spacing = profile().taskRowGap;
    recipe.radius = profile().taskRowRadius;
    recipe.iconSize = profile().taskRowIconSize;
    recipe.titlePixels = profile().taskRowPixels;
    recipe.metaPixels = profile().taskMetaPixels;
    recipe.actionSize = profile().groupHeaderActionSize;
    return recipe;
}

QVariantMap toVariantMap(const TaskRowRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"status", r.status},
                  {"kind", r.kind},
                  {"title", r.title},
                  {"meta", r.meta},
                  {"action", r.action},
                  {"actionDanger", r.actionDanger},
                  {"minHeight", r.minHeight},
                  {"paddingStart", r.paddingStart},
                  {"paddingEnd", r.paddingEnd},
                  {"spacing", r.spacing},
                  {"radius", r.radius},
                  {"iconSize", r.iconSize},
                  {"titlePixels", r.titlePixels},
                  {"metaPixels", r.metaPixels},
                  {"actionSize", r.actionSize}});
}

TaskBarRecipe taskBarRecipe(const InteractionState& state, Theme theme) {
    TaskBarRecipe recipe;
    const bool live = state.enabled;
    recipe.background =
        live && (state.hovered || state.pressed) ? token(u"surface-hover", theme) : transparent();
    recipe.text = token(u"text-secondary", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.height = profile().taskBarHeight;
    recipe.paddingInline = profile().taskBarPaddingInline;
    recipe.radius = profile().taskBarRadius;
    recipe.fontPixels = profile().taskMetaPixels;
    recipe.iconSize = profile().taskBarIconSize;
    recipe.chevronSize = profile().taskBarChevronSize;
    recipe.spacing = profile().taskBarGap;
    recipe.rowGap = profile().taskBarRowGap;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const TaskBarRecipe& r) {
    return mapOf({{"background", r.background},
                  {"text", r.text},
                  {"focusRing", r.focusRing},
                  {"height", r.height},
                  {"paddingInline", r.paddingInline},
                  {"radius", r.radius},
                  {"fontPixels", r.fontPixels},
                  {"iconSize", r.iconSize},
                  {"chevronSize", r.chevronSize},
                  {"spacing", r.spacing},
                  {"rowGap", r.rowGap},
                  {"opacity", r.opacity}});
}

UpdateCardRecipe updateCardRecipe(bool ready, Theme theme) {
    UpdateCardRecipe recipe;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(ready ? u"success-border" : u"panel-border", theme);
    recipe.title = token(u"text-main", theme);
    recipe.detail = token(u"text-muted", theme);
    recipe.notes = token(u"text-secondary", theme);
    recipe.radius = profile().changedRadius;
    recipe.paddingBlock = profile().updatePaddingBlock;
    recipe.paddingInline = profile().updatePaddingInline;
    recipe.titlePixels = profile().updateTitlePixels;
    recipe.titleWeight = profile().updateTitleWeight;
    recipe.detailPixels = profile().changedTextPixels;
    recipe.headerGap = profile().updateHeaderGap;
    recipe.headerColumnGap = profile().updateHeaderColumnGap;
    recipe.sectionGap = profile().updateSectionGap;
    recipe.notesPixels = profile().changedTitlePixels;
    recipe.notesLineHeight = profile().updateNotesLineHeight;
    recipe.notesIndent = profile().updateNotesIndent;
    recipe.actionsGap = profile().updateActionsGap;
    recipe.actionsTop = profile().updateActionsTop;
    return recipe;
}

QVariantMap toVariantMap(const UpdateCardRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"title", r.title},
                  {"detail", r.detail},
                  {"notes", r.notes},
                  {"radius", r.radius},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"detailPixels", r.detailPixels},
                  {"headerGap", r.headerGap},
                  {"headerColumnGap", r.headerColumnGap},
                  {"sectionGap", r.sectionGap},
                  {"notesPixels", r.notesPixels},
                  {"notesLineHeight", r.notesLineHeight},
                  {"notesIndent", r.notesIndent},
                  {"actionsGap", r.actionsGap},
                  {"actionsTop", r.actionsTop}});
}

UpdatePillRecipe updatePillRecipe(const InteractionState& state, Theme theme) {
    UpdatePillRecipe recipe;
    const bool live = state.enabled;
    recipe.background = token(u"accent-subtle", theme);
    recipe.border = token(u"accent-border", theme);
    recipe.title = token(u"accent-text", theme);
    recipe.detail = token(u"text-secondary", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.paddingBlock = profile().updatePillPaddingBlock;
    recipe.paddingInline = profile().updatePillPaddingInline;
    recipe.radius = profile().updatePillRadius;
    recipe.spacing = profile().updatePillGap;
    recipe.iconSize = profile().updatePillIconSize;
    recipe.titlePixels = profile().changedTitlePixels;
    recipe.titleWeight = profile().changedTitleWeight;
    recipe.detailPixels = profile().changedPathPixels;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const UpdatePillRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"title", r.title},
                  {"detail", r.detail},
                  {"focusRing", r.focusRing},
                  {"paddingBlock", r.paddingBlock},
                  {"paddingInline", r.paddingInline},
                  {"radius", r.radius},
                  {"spacing", r.spacing},
                  {"iconSize", r.iconSize},
                  {"titlePixels", r.titlePixels},
                  {"titleWeight", r.titleWeight},
                  {"detailPixels", r.detailPixels},
                  {"opacity", r.opacity}});
}

ThemeCardRecipe themeCardRecipe(bool selected, Theme preview, const InteractionState& state,
                                Theme theme) {
    ThemeCardRecipe recipe;
    const bool live = state.enabled;
    recipe.background = token(u"panel-bg", theme);
    recipe.border = token(selected ? u"accent" : u"border-default", theme);
    recipe.ring = selected ? token(u"accent", theme) : transparent();
    recipe.label = token(u"text-main", theme);
    recipe.check = token(selected ? u"accent-text" : u"text-muted", theme);
    recipe.focusRing = live && state.focused ? token(u"focus-border", theme) : transparent();
    recipe.miniBackground = token(u"panel-bg", preview);
    recipe.miniBorder = token(u"panel-border", preview);
    recipe.miniSidebar = token(u"sidebar-bg", preview);
    recipe.miniBar = token(u"border-default", preview);
    recipe.miniAccentBar = token(u"accent-muted", preview);
    recipe.miniTitle = token(u"text-muted", preview);
    recipe.miniLine = token(u"panel-border", preview);
    recipe.miniButton = token(u"accent-subtle", preview);
    recipe.miniButtonBorder = token(u"accent-border", preview);
    recipe.radius = profile().changedRadius;
    recipe.miniHeight = profile().themeMiniHeight;
    recipe.miniMargin = profile().themeMiniMargin;
    recipe.miniSideWidth = profile().themeMiniSideWidth;
    recipe.labelPaddingBlock = profile().themeLabelPaddingBlock;
    recipe.labelPaddingInline = profile().themeLabelPaddingInline;
    recipe.labelPixels = profile().changedTitlePixels;
    recipe.checkSize = profile().updatePillIconSize;
    recipe.opacity = live ? 1.0 : profile().disabledOpacity;
    return recipe;
}

QVariantMap toVariantMap(const ThemeCardRecipe& r) {
    return mapOf({{"background", r.background},
                  {"border", r.border},
                  {"ring", r.ring},
                  {"label", r.label},
                  {"check", r.check},
                  {"focusRing", r.focusRing},
                  {"miniBackground", r.miniBackground},
                  {"miniBorder", r.miniBorder},
                  {"miniSidebar", r.miniSidebar},
                  {"miniBar", r.miniBar},
                  {"miniAccentBar", r.miniAccentBar},
                  {"miniTitle", r.miniTitle},
                  {"miniLine", r.miniLine},
                  {"miniButton", r.miniButton},
                  {"miniButtonBorder", r.miniButtonBorder},
                  {"radius", r.radius},
                  {"miniHeight", r.miniHeight},
                  {"miniMargin", r.miniMargin},
                  {"miniSideWidth", r.miniSideWidth},
                  {"labelPaddingBlock", r.labelPaddingBlock},
                  {"labelPaddingInline", r.labelPaddingInline},
                  {"labelPixels", r.labelPixels},
                  {"checkSize", r.checkSize},
                  {"opacity", r.opacity}});
}

AccentSwatchRecipe accentSwatchRecipe(Theme theme) {
    AccentSwatchRecipe recipe;
    recipe.fill = token(u"accent", theme);
    recipe.border = token(u"accent-subtle", theme);
    recipe.ring = token(u"accent-border", theme);
    recipe.size = profile().avatarDiameter;
    recipe.radius = profile().accentSwatchRadius;
    recipe.borderWidth = profile().accentSwatchBorder;
    return recipe;
}

QVariantMap toVariantMap(const AccentSwatchRecipe& r) {
    return mapOf({{"fill", r.fill},
                  {"border", r.border},
                  {"ring", r.ring},
                  {"size", r.size},
                  {"radius", r.radius},
                  {"borderWidth", r.borderWidth}});
}

} // namespace ket
