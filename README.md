# KetQt

Két Design System components for Qt 6.8 or later, shared by KetPlus, KétChat and Lampion
OS. Qt Widgets and Qt Quick draw the same components from the same tokens and recipes, so an
app looks the same in both toolkits and on macOS, Windows and Linux.

- `KetQt::Tokens`: design-system tokens generated from the pinned Két DS `tokens.css`,
  the shared theme state (`ket::Appearance`) and component recipes.
- `KetQt::Icons`: Két Symbol icons with their size and weight rules (`ket::Icons`).
- `KetQt::Widgets`: `ket::Style` (Fusion-based), `ket::Button`, `ket::IconButton`,
  `ket::Badge`, `ket::Tag`, `ket::Icon`, `ket::TextField`, `ket::Select`, `ket::Checkbox`,
  `ket::Radio`, `ket::Choice`, `ket::FieldRow`, `ket::Banner`, `ket::Toast`, `ket::Dialog`;
  `ket::Style` also draws every `QMenu`, the select popup and `QToolTip`.
- `KetQt::Quick`: the QML module `Ket` with `KetTheme`, `KetButton`, `KetIconButton`,
  `KetBadge`, `KetTag`, `KetIcon`, `KetTextField`, `KetSelect`, `KetCheckbox`, `KetRadio`,
  `KetChoice`, `KetFieldRow`, `KetBanner`, `KetToast`, `KetDialog`, `KetMenu`, `KetMenuItem`,
  `KetMenuSeparator`, `KetToolTip`.

Components follow the new KetPlus desktop mock and are added in the order KetPlus uses
them; see [docs/architecture.md](docs/architecture.md#component-scope-and-source).

See [docs/architecture.md](docs/architecture.md),
[decision 0001](docs/decisions/0001-shared-qt-component-kit.md) and
[decision 0002](docs/decisions/0002-product-profiles-and-lampion.md) (product profiles, Lampion OS).

## Build and test

```sh
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/<platform>
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests run with `QT_QPA_PLATFORM=offscreen`.

## Kitchensink

`examples/kitchensink` shows every foundation (colours, typography, spacing and radius,
elevation, all tokens) and every component, with the Qt Widgets version above its Qt Quick
twin in each variant, size and state. Theme and zoom switch live from the toolbar.

```sh
open build/examples/kitchensink/ketqt_kitchensink.app        # macOS
build/examples/kitchensink/ketqt_kitchensink                 # Windows, Linux
ketqt_kitchensink --page=button --dark --zoom=1.25 --screenshot=button-dark.png
ketqt_kitchensink --page=overlays --light --full --screenshot=overlays.png   # pin a theme
ketqt_kitchensink --page=forms --full --screenshot=forms.png   # the whole scrolled page
ketqt_kitchensink --smoke   # every page, light and dark; non-zero on QML errors (a CTest)
```

## Using it

```cpp
#include "KetButton.h"
#include "KetProfile.h"
#include "KetStyle.h"

ket::Profile::setCurrent(ket::Profile::ketPlus()); // or designSystem(), or a product's own
ket::Style::install(app);
auto* save = new ket::Button(tr("Save"), ket::ButtonVariant::Primary);
```

```qml
import Ket

KetButton { text: qsTr("Save"); variant: KetTheme.ButtonVariant.Primary }
```

## Licence

KetQt is licensed under the [Mozilla Public License 2.0](LICENSE). The pinned
`third_party/ket-design-system/tokens.css` is MIT-licensed
([licence](third_party/ket-design-system/LICENSE)). The Két Symbol icons in
`third_party/ket-symbol/` are separate files under CC BY 4.0
([notice](third_party/ket-symbol/NOTICE)): "Két Symbol © 2026 Két Việt, CC BY 4.0".
The names Két, Két Symbol and Lampion are trademarks and are not licensed. Run
`python3 tools/generate_icons.py` after changing them. Qt is used under its LGPL 3.0,
GPL or commercial terms as the consuming app chooses.
