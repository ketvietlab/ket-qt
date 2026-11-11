# KetQt architecture

```mermaid
flowchart LR
  DS["Két DS tokens.css<br/>(pinned copy)"] --> GEN["generate_tokens.py"]
  GEN --> TOK["KetQt::Tokens<br/>table · Appearance · recipes"]
  TOK --> ICN["KetQt::Icons<br/>Két Symbol"]
  ICN --> WID
  ICN --> QK
  TOK --> WID["KetQt::Widgets<br/>ket::Style · components"]
  TOK --> QK["KetQt::Quick<br/>module Ket · KetTheme"]
  WID --> APPS["KetPlus · KétChat"]
  QK --> APPS
```

## Targets

| Target | Contents | Qt modules |
|---|---|---|
| `KetQt::Tokens` | `ket::Tokens` (generated table), `ket::Appearance`, `ket::Profile`, `ket::*Recipe` | Core, Gui |
| `KetQt::Icons` | `ket::Icons`: Két Symbol resources, size and weight rules | Gui, Svg |
| `KetQt::Widgets` | `ket::Style`, `ket::Button`, `ket::IconButton`, `ket::Badge`, `ket::Tag`, `ket::Icon`, `ket::TextField`, `ket::Select`, `ket::Checkbox`, `ket::Radio`, `ket::Choice`, `ket::FieldRow` | Widgets |
| `KetQt::Quick` + `KetQt::QuickPlugin` | QML module `Ket`: `KetTheme`, `KetButton`, `KetIconButton`, `KetBadge`, `KetTag`, `KetIcon`, `KetTextField`, `KetSelect`, `KetCheckbox`, `KetRadio`, `KetChoice`, `KetFieldRow` | Qml, Quick, QuickControls2 |

## Tokens

- Names are the CSS custom property without `--kv-`: `ket::Tokens::color(u"page-bg", theme)`.
- Lengths are logical pixels at 100% zoom (`1rem` = 16 px). Durations are milliseconds.
  Colours are `#RRGGBB` or `#AARRGGBB`.
- Only the desktop values are generated: the base `:root` and `[data-kv-design-system]`
  blocks. Media queries, density variants and `prefers-reduced-motion` are ignored.
- QML reads the same table as `KetTheme.t` with camelCase keys (`KetTheme.t.pageBg`).
- A terminal draws the 16 ANSI colours from `terminal-ansi-0` … `terminal-ansi-15`
  (`KetTheme.t.terminalAnsi0` …) on `page-bg`. Each reads at 4.5:1 or more on that
  background, except black (0) on dark and bright white (15) on light (`tst_tokens`).

### Updating the design system

1. Copy `tokens.css` from the new `@ketvietlab/design-system` revision into
   `third_party/ket-design-system/`, and update `SOURCE.json` (version, revision, sha256).
2. Run `python3 tools/generate_tokens.py` and review the diff of `tokens/ket-tokens.json`.
3. Run the tests and compare the kitchensink pages in light and dark.

## Appearance and zoom

`ket::Appearance::instance()` is the only theme state. `Mode::System` follows
`QStyleHints::colorScheme()`; `setMode()` pins Light or Dark. `setZoom()` scales token
lengths to whole pixels (hairlines of 1 px or less stay as they are). Apps that already
have an appearance setting forward it here instead of keeping a second theme.

In QML, pass `KetTheme.dark` and `KetTheme.zoom` to recipe functions and `KetTheme.px()`
so bindings re-evaluate; recipes returned to QML are already scaled.

## Component scope and product profiles

Components are added in the order the KetPlus desktop mock uses them (`ketplus.ketatlas` in
`ketvietlab/ketplus`, built on the same Két DS release).

A recipe takes its colours from tokens, choosing which token fills, borders and colours
each state. It takes the metrics a product decides from the current `ket::Profile`
(decision 0002): sizes, spacing, weights and opacities.

- **`Profile::ketPlus()`** is the default. It holds the KetPlus mock's literals, each next to
  its source rule, for example the 7 px `.btn` gap, the 30 px `.icon-btn`, the 0.48
  disabled opacity and the 18 px tag. Its font sizes are the mock's at its default 15 px
  interface size.
- **`Profile::designSystem()`** uses tokens only.
- **Other products**, such as Lampion OS, install their own profile before creating
  windows.

A family of parts that must line up keeps its metrics together in one nested struct of the
profile, such as `Profile::sidebarTree` (`ket::SidebarTreeMetrics`). The sidebar tree's
parts take their columns from `ket::sidebarTreeColumns(level, zoom)`, which rounds each
edge from its own offset, so a header's text and the text of the rows under it share one
pixel column at every zoom in both layers (`tests/tst_sidebar_tree.cpp`).

Focus is drawn inside a control's bounds (a 2 px `focus-border` border on fields, a ring
around toggles), not as the mock's outside outline, which Qt Widgets cannot paint.

Icons are Két Symbol 0.11.0 (`third_party/ket-symbol`, CC BY 4.0, separate files with
their own notice), compiled into `KetQt::Icons` by `tools/generate_icons.py`:

- `ket::Icons` names icons by Két code; `codeForLucide()` maps KetPlus' Lucide names.
- A box draws the nearest master (14, 16, 20 or 24 px; the smaller on a tie): 14 px compact
  icons their own drawing, 17-18 px the 16 px master, 19-22 px the 20 px master and 23 px or
  more the 24 px master. The master is drawn at its own size and centred on whole device
  pixels (`offset()`); only a box smaller than its master scales it down.
- Light is the default weight; Regular is for emphasis. A filled variant never marks
  selection.
- `ket::Icons::icon()` is a `QIcon` with those rules; `ket::Icon` and `KetIcon` draw a
  standalone icon. Components draw icons in their text colour (the mock's `currentColor`):
  rendered at size × device pixel ratio and filled with `SourceIn`, which keeps the alpha.
  `KetButton` and `KetIconButton` take a Két code as `iconName`; `icon.source` still takes
  any other monochrome icon.

Colours that are user data, such as tag colours, are parameters.

| Done | Next, from the mock |
|---|---|
| Button (primary, secondary, ghost, danger, ghost danger), IconButton, Badge (neutral, brand, success, warning, …), Tag, Icon, TextField (with a leading icon for search), Select, Checkbox, Radio, Choice card, Field row, Banner, Toast, Dialog, Context menu (also the Select popup), Tooltip, Nav item, Kbd, Segmented control, Tab strip, Progress bar, Activity indicator, Empty state, Conversation row (two lines with tags), Composer, User and assistant messages, Reading text, Day divider, Activity row, Markdown with code blocks and quotes, Mention picker, Usage ring, Popover (usage panel), Meter (usage stat, sidebar quota), Attachment chip, Suggestion, Question card, Activity group, Gallery, Slider and step slider, Avatar, Tree item, Group header, Sidebar tree (repository header, team branch, section label, one-line conversation row, guide line; columns per level from `ket::sidebarTreeColumns` and `Profile::sidebarTree`), Route tabs, Pane header, Breadcrumb, Status bar, Command palette, File explorer row, Changed files, Diff lines, Background tasks, Update card and pill, Theme card, Accent swatch, Image viewer (lightbox, thumbnail, round tool; from the design system's `interactions/lightbox`) (pill tabs are the Segmented control; the search field is a TextField with a leading icon) | None: every component of the mock is in the kit |

The surfaces only present data. `ket::DiffView` and `KetDiffView` draw lines that the app
supplies (`parseUnified()` only reads a unified diff's hunks and line numbers); the file
row, changed-files card, task bar and command palette hold no file system, Git, process or
search logic. The app filters the palette's groups as its query changes.

### Overlays

- **Dialogs and toasts draw inside their window**, as the mock's `.modal-backdrop` and
  `.toast` do, not as separate top-level windows. `ket::Dialog` covers its window, keeps
  Tab inside the card and rejects on Escape, the close button or a backdrop click.
  The backdrop (`ket::modalBackdrop`) dims the window in both themes: translucent ink in
  the light theme, black at `Profile::backdropOpacityDark` in the dark one.
  Its card takes the recipe's width unless `setCardWidth()` sets the mock's `width: px(N)`.
  `setChromeless()` hands the whole card to a body that draws its own header and footer, and
  the card follows its content's height while open.
  A window container (a `QQuickView` or web view) is a native child window above the
  window's own surface, so when the window holds one, a dialog or toast becomes a native
  child too, raised above it, and draws a snapshot of the window under its translucent parts.
  `KetDialog` is a modal `Dialog` in the window overlay.
- **Menus are drawn by `ket::Style`**, so every `QMenu` and the `QComboBox` popup
  (`SH_ComboBox_Popup`) look like `.desktop-context-menu`. `Style::setDanger()`,
  `Style::setWarning()` and `Style::setDescription()` mark a destructive command or an
  elevated choice and add its secondary line; `QMenu::addSection()` draws a muted heading,
  as `KetMenuSection` does. macOS
  hides shortcuts in context menus (`Qt::AA_DontShowShortcutsInContextMenus`); `KetMenuItem`
  shows its `shortcut` everywhere.
- **Tooltips use the accent background**, like the native KetPlus `KetToolTip`.
- **Tab strips and progress bars are drawn by `ket::Style`** for every top `QTabBar`
  (`.file-tabs`) and horizontal `QProgressBar` (`progress`); other orientations stay Fusion.
  An indeterminate bar sweeps a segment, which the mock does not define.
- **The activity indicator** is the mock's `.working-bars`; `ket::activityBarFrame()` holds
  the keyframes, so both layers animate identically and screenshots can freeze a frame.
- **Conversation blocks** follow the mock's `conversation-tags.css` and the
  desktop-alignment overrides. Conversation rows show whole tags only and count the rest
  in `+N`. The assistant avatar defaults to a Két Symbol `bot` tile: the mock's Két mark is
  a trademark the kit does not license, so apps pass their own mark.
- **Markdown answers** are split by `ket::splitMarkdown()` into prose, fenced code and
  top-level quotes. Prose renders in a `QTextDocument` that `ket::styleMarkdown()` restyles
  with the `.answer` metrics. Code blocks and quotes are separate surfaces, because a text
  document cannot draw a rounded code surface, a copy button or the quote's accent bar.
  Code blocks wrap long lines; inline code is monospace without a background.
- **The `@agent/` mention picker** opens above the composer, overlapping its top edge. The
  composer keeps focus and forwards Up, Down, Enter, Tab and Esc. `ket::agentMention()` ports
  the mock's `K.conversationMention`, and the app answers `mentionQueryChanged` with items.
- **The usage ring** turns warning at 80 % and danger at 95 %, which the mock leaves to the
  app. Meters use the same thresholds, where the mock fills every track with the accent.
- **Popovers** are in-window panels like dialogs: `ket::Popover::openAbove()` reparents to
  the anchor's window and closes on Esc, the close button or a press outside. They use the
  `shadow-md` token; the mock's `.usage-popover` literal (`0 12px 36px` at 15 %) is close to
  it and colours stay tokens.
- **Sliders are drawn by `ket::Style`** for every horizontal `QSlider`, like tab strips and
  progress bars; the mock uses the native range input, so the track and handle follow the
  accent tokens rather than a mock literal.
- **Group headers** keep their action's place while it is hidden, so the name never moves
  when the header is hovered or focused.
- **Activity groups** hold any rows, including a `ket::Gallery` of screenshots; the gallery
  emits `activated(index)` and the app opens its own preview.
- **The image viewer** (`ket::Lightbox`, `KetLightbox`) follows the design system's lightbox
  (decision 0003). It stays dark in both themes, where the design system's colour mix turns
  it light in the dark theme. Hosts add Rotate, Copy, Download, Print and Full screen tools; Print
  opens the system dialog through `ket::printImage()`, so `KetQt::Widgets` needs Qt Print
  Support. A
  Widgets host can put its own image engine on the stage. The Quick thumbnail masks its
  picture with `MultiEffect` from `QtQuick.Effects`, the one effect in the kit: a Canvas
  crop is not sharp at 2×.
- **Line heights are CSS line boxes**, font size × line height in whole pixels. Qt's
  proportional line height scales the font's line spacing instead, so both layers use fixed
  line boxes (`QTextBlockFormat::FixedHeight`, `Text.FixedHeight`).
- **Shadows** are parsed from shadow tokens (`ket::shadowToken`) and drawn with concentric
  translucent layers (`KetShadow`), so neither layer needs a graphics effect or a shader.

## Adding a component

1. Add a recipe (`KetRecipes.h`) built from tokens, with a token test. A new product metric
   becomes a `ket::Profile` field, set in both `designSystem()` and `ketPlus()`.
2. Draw it in `src/widgets/` and `src/quick/qml/`; neither layer chooses colours itself.
3. Add a Widgets test and a Quick test that compare against the same recipe.
4. Add it to a kitchensink page (`examples/kitchensink`) in every variant, size and pinned
   state, and check light, dark and 125% zoom
   (`--page=<id> --dark --zoom=1.25 --screenshot=<file>`, `--full` for the whole page).
   Run `--smoke` on a real display as well as offscreen: it also checks that every Qt Quick
   view matches its root size.

## Consuming KetQt

- Pin a release tag as a Git submodule and `add_subdirectory()` it with
  `KETQT_BUILD_TESTS=OFF` and `KETQT_BUILD_EXAMPLES=OFF`. Link the targets above.
- A Widgets app calls `ket::Style::install(app)` once after creating `QApplication`.
- A QML app links `KetQt::Quick` and `KetQt::QuickPlugin`, adds
  `Q_IMPORT_QML_PLUGIN(KetPlugin)` once and writes `import Ket`.
- `cmake --install` also installs `KetQt::Tokens`, `KetQt::Icons` and `KetQt::Widgets`, with the
  Két Symbol notice and licence, as a CMake package
  (`find_package(KetQt)`). The static QML module is not installed yet.
- `KETQT_INSTALL` (on only when KetQt is the top-level project) controls those install
  rules. An app that builds the kit with FetchContent or `add_subdirectory()` links it
  statically and leaves it off, so `cmake --install` of the app does not copy KetQt's
  headers, archives and package into the app's install tree.

## Fonts

The kit does not bundle fonts. `font-sans` names KÉT Sans, then Inter. The first
installed family is used, and otherwise the platform font. An app that is licensed to
ship KÉT Sans or Inter registers it with `QFontDatabase` before creating windows.

Code (diffs, code blocks, inline code and paths) uses `Profile::monoFamilies`:
the KetPlus profile follows the mock's `--kv-font-mono` (KÉT Sans Mono, SF Mono, Menlo,
then the `font-mono` token), and the Két DS profile uses the `font-mono` token alone.
`Profile::monoFamily()` returns the first installed one.

Family names match without regard to case or accents
(`Tokens::installedFamily`), because the KÉT Sans font files register as "KET Sans" and
"KET Sans Mono". The kitchensink's `--fonts=<dir>` registers local font files for review
screenshots; no font file is committed.
