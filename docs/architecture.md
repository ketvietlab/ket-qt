# KetQt architecture

```mermaid
flowchart LR
  DS["Két DS tokens.css<br/>(pinned copy)"] --> GEN["generate_tokens.py"]
  GEN --> TOK["KetQt::Tokens<br/>table · Appearance · recipes"]
  TOK --> WID["KetQt::Widgets<br/>ket::Style · components"]
  TOK --> QK["KetQt::Quick<br/>module Ket · KetTheme"]
  WID --> APPS["KetPlus · KétChat"]
  QK --> APPS
```

## Targets

| Target | Contents | Qt modules |
|---|---|---|
| `KetQt::Tokens` | `ket::Tokens` (generated table), `ket::Appearance`, `ket::*Recipe` | Core, Gui |
| `KetQt::Widgets` | `ket::Style`, `ket::Button`, `ket::IconButton`, `ket::Badge`, `ket::Tag` | Widgets |
| `KetQt::Quick` + `KetQt::QuickPlugin` | QML module `Ket`: `KetTheme`, `KetButton`, `KetIconButton`, `KetBadge`, `KetTag` | Qml, Quick, QuickControls2 |

## Tokens

- Names are the CSS custom property without `--kv-`: `ket::Tokens::color(u"page-bg", theme)`.
- Lengths are logical pixels at 100% zoom (`1rem` = 16 px). Durations are milliseconds.
  Colours are `#RRGGBB` or `#AARRGGBB`.
- Only the desktop values are generated: the base `:root` and `[data-kv-design-system]`
  blocks. Media queries, density variants and `prefers-reduced-motion` are ignored.
- QML reads the same table as `KetTheme.t` with camelCase keys (`KetTheme.t.pageBg`).

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

## Component scope and source

Components are added in the order the new KetPlus desktop mock uses them
(`ketplus.ketatlas` in `ketvietlab/ketplus`, built on the same Két DS release). A recipe
follows the mock's composition: which tokens fill, border and colour each state. Where the
mock uses a literal with no token (for example the 7 px `.btn` gap, the 30 px `.icon-btn`,
the 0.48 disabled opacity or the 18 px tag), the recipe keeps it as a named constant next
to its source rule in `KetRecipes.cpp`; when the design system adds a token, the constant
is replaced by it. Font sizes are the mock's at its default 15 px interface size.

Icons are monochrome glyphs drawn in the component's text colour (the mock's
`currentColor`), in Widgets as a mask (rendered at size × device pixel ratio, filled with
`SourceIn`, alpha kept) and in QML with `IconImage`. Apps supply the icons as a `QIcon` or
URL; the kit ships none. KetPlus uses Két Symbol (fill-based SVGs with 16, 20 and 24 px
masters, named by Két code, with a Lucide map in its manifest). Its licence is not decided,
so no Két Symbol file is committed here; a later build-time generator may write a `.qrc`
and name constants from a dist folder the app provides. Icons render at the 16 px master
size, never at fractional scales. Colours that are user data, such as tag colours, are
parameters.

| Done | Next, from the mock |
|---|---|
| Button (primary, secondary, ghost, danger, ghost danger), IconButton, Badge (neutral, brand, success, warning, …), Tag | TextField and search field, Select, Checkbox and Choice card, Field row, Banner, Toast, Dialog, Context menu, Tooltip, sidebar row, tabs, progress and spinner, empty state, kbd |

## Adding a component

1. Add a recipe (`KetRecipes.h`) built from tokens, with a token test.
2. Draw it in `src/widgets/` and `src/quick/qml/`; neither layer chooses colours itself.
3. Add a Widgets test and a Quick test that compare against the same recipe.
4. Add it to a kitchensink page (`examples/kitchensink`) in every variant, size and pinned
   state, and check light, dark and 125% zoom
   (`--page=<id> --dark --zoom=1.25 --screenshot=<file>`).

## Consuming KetQt

- Pin a release tag as a Git submodule and `add_subdirectory()` it with
  `KETQT_BUILD_TESTS=OFF` and `KETQT_BUILD_EXAMPLES=OFF`. Link the targets above.
- A Widgets app calls `ket::Style::install(app)` once after creating `QApplication`.
- A QML app links `KetQt::Quick` and `KetQt::QuickPlugin`, adds
  `Q_IMPORT_QML_PLUGIN(KetPlugin)` once and writes `import Ket`.
- `cmake --install` also installs `KetQt::Tokens` and `KetQt::Widgets` as a CMake package
  (`find_package(KetQt)`). The static QML module is not installed yet.

## Fonts

The kit does not bundle fonts. `font-sans` names KÉT Sans, then Inter. The first
installed family is used, and otherwise the platform font. An app that is licensed to
ship KÉT Sans or Inter registers it with `QFontDatabase` before creating windows.
