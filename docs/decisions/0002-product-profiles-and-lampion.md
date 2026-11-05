# 0002: Product profiles, and reuse in Lampion OS

Status: accepted (2026-11-05)

## Context

KetQt started from the KetPlus desktop mock: its recipes took KetPlus' literals, such as the
17 px icon box, the 30 px icon button, the 0.48 disabled opacity and the 34 px field.

Lampion OS (`ketvietlab/lampion`) is the second product that will use the kit.

- **Platform.** It is a desktop OS planned on Ubuntu with KDE Plasma and KWin, so its shell
  and system apps are Qt and QML. Today it is an HTML prototype (`ket-os.ketatlas`).
- **Design.** It composes the same Két Design System in its own way. The Glass concept has
  translucent materials (`--glass-shell`, `--glass-edge`, …) mixed from design-system tokens,
  plus a Windows-style taskbar and window chrome.
- **Versions and icons.** It pins Két DS 0.1.43 and uses the whole Két Symbol set, including
  its KDE icon theme.

If the kit kept KetPlus' metrics in its recipes, Lampion would either inherit them or have to
fork the kit.

## Decision

### Product profiles
- `ket::Profile` (`src/tokens/KetProfile.h`) holds every metric a product decides: sizes,
  spacing, weights and opacities. Colours always come from tokens and never from a profile.
- The kit ships two profiles:
  - `Profile::designSystem()` uses only Két DS tokens;
  - `Profile::ketPlus()` follows the KetPlus mock and is the default for now.
- A product builds its own profile, starting from either one, and installs it once with
  `Profile::setCurrent()` before it creates any window. Profiles are not switched at run time.
- Recipes read the current profile, so Widgets and Qt Quick stay identical under any profile.
- Product code stays in the product. Lampion's profile, its Glass materials and its shell
  components (taskbar, Start, window chrome) live in the Lampion repository. They are built
  from `ket::Tokens`, `ket::Profile` and the kit's recipes. A component or token that a
  second product needs moves into the kit, or into Két DS first.

### Tokens and icons
- **One DS version per kit release.** A product adopts the kit's pinned Két DS version.
  Lampion moves from 0.1.43 to the kit's pin when it adopts KetQt.
- **Product tokens** such as `--glass-*` are not added to the generated table. The product
  computes them from design-system tokens, for example by mixing `panel-bg` with
  transparency. A token shared by several products goes to Két DS.
- **Icons.** `ket::Icons` names icons by Két code and is not tied to KetPlus. Today the
  compiled set is the KetPlus subset.

### Planned for Lampion

| Item | Plan |
|---|---|
| Icon set | A build option selects the full Két Symbol set |
| Icon theme fallback | `ket::Icons` falls back to the installed Két Symbol icon theme (`QIcon::fromTheme`) on Plasma |
| Installed QML module | The QML module `Ket` is installable as a shared plugin under the Qt QML import path, so Plasma shells and plasmoids can `import Ket` |
| Plasma and GTK styling | A Qt Quick Controls style and a KDE colour scheme generated from the tokens, so third-party Qt apps match. These are separate targets or live in Lampion |

### Platform floor
Qt 6.8 stays the minimum. Plasma 6 on current Ubuntu releases meets it. Older Ubuntu
releases with earlier Qt are not targets.

## Consequences

- Adding a metric to a recipe means adding a `Profile` field. Set it in `designSystem()` from
  tokens, and in `ketPlus()` from the mock with its source rule. A recipe never takes a
  product literal directly.
- When every consumer installs its profile explicitly, the default can become
  `designSystem()`.
- Licensing allows OS distribution:
  - KetQt is MPL-2.0;
  - Két Symbol is CC BY 4.0, shipped with its notice;
  - KÉT Sans is SIL OFL 1.1.
