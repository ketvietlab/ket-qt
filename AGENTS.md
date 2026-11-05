# Agent instructions

KetQt is public (MPL-2.0). Everything committed is published.

## Start here

Read `docs/architecture.md` and `docs/decisions/` before changing code.

## Rules

- Tokens come only from the pinned Két Design System `tokens.css`. Never edit
  `src/tokens/generated/` or `tokens/ket-tokens.json` by hand; run
  `python3 tools/generate_tokens.py`.
- A component is a recipe first (`src/tokens/KetRecipes.*`). The Widgets and Qt Quick
  layers draw recipes and never pick colours or sizes themselves.
- Every component ships in both layers, with tests that compare them against the same
  recipe, and appears in the kitchensink (`examples/kitchensink`) in every variant,
  size and state (`setPreviewState()` / `previewState` pin a state).
- Keep Qt 6.8 compatibility. Do not use native platform styles for kit components.
- No app-specific code (KetPlus, KétChat, Lampion OS) and no fonts, credentials or machine
  paths. Product metrics live only in `ket::Profile` (decision 0002); colours only in tokens.
- Format C++ with the repository `.clang-format`. Run `ctest` and `git diff --check`
  before committing.
- Work on `develop`; `main` is for releases. Consumers pin release tags.
