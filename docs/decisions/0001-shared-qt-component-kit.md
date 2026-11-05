# 0001: One Qt component kit built from the Két Design System

Status: accepted (2026-11-05)

## Context

KetPlus (MPL-2.0, public) draws its shell with Qt Widgets and its Agent Workspace with
Qt Quick. KétChat desktop (private) draws with Qt Widgets and `QPainter`. Each app keeps
its own palette (`ThemePalette`, `ketchat::ui::Theme`) and styles controls inline:
KetPlus' Agent Workspace alone restyles `Button` 34 times. Colours, sizes and states
drift between apps and between Widgets and QML in the same app, and the apps will ship
on Windows and Linux, where native styles differ again.

The Két Design System (`@ketvietlab/design-system`, MIT) already defines the tokens as
CSS custom properties (`tokens.css`, 325 desktop tokens with light and dark values).

KétView is out of scope: its shells are SwiftUI and WinUI 3 (KétView decisions 0008 and
0009), not Qt.

## Decision

- A public repository, `ketvietlab/ket-qt`, under MPL-2.0, holds the kit. Apps pin a
  release tag.
- **Tokens** come only from the Két Design System. `tools/generate_tokens.py` reads a
  pinned copy of `tokens.css` (version, revision and sha256 in
  `third_party/ket-design-system/SOURCE.json`) and writes a compiled C++ table and a
  JSON review copy. Apps never hard-code a design-system colour or size.
- **One theme state**: `ket::Appearance` follows the system colour scheme unless pinned
  and owns the interface zoom. The Widgets and Qt Quick layers both observe it.
- **Recipes define components once.** `ket::buttonRecipe` and `ket::badgeRecipe` turn
  a variant, size, interaction state and theme into colours and metrics. The two
  layers only draw recipes, so a Widgets button and a QML button cannot disagree.
- **Two thin layers**:
  - `KetQt::Widgets`: `ket::Style`, a Fusion-based `QProxyStyle` (Fusion on every
    platform for identical rendering), plus `ket::Button` and `ket::Badge`.
  - `KetQt::Quick`: the static QML module `Ket` with the `KetTheme` singleton and
    `KetButton` and `KetBadge`.
- Qt 6.8 is the minimum (KétChat ships 6.8; KetPlus 6.11).

## Consequences

- A new component lands as a recipe first, then in both layers with tests that compare
  them, and in the gallery.
- Changing a design-system token means updating the pinned `tokens.css`, regenerating,
  and reviewing the JSON diff; a CTest check fails if the generated files are stale.
- KetPlus and KétChat migrate screen by screen: first their palettes read
  `ket::Tokens`, then inline controls move to kit components.
- Platform look and feel is deliberately uniform: native macOS, Windows or GTK styles
  are not used for kit components.
