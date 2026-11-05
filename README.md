# KetQt

Két Design System components for Qt 6.8 or later, shared by KetPlus and KétChat. Qt
Widgets and Qt Quick draw the same components from the same tokens and recipes, so an
app looks the same in both toolkits and on macOS, Windows and Linux.

- `KetQt::Tokens`: design-system tokens generated from the pinned Két DS `tokens.css`,
  the shared theme state (`ket::Appearance`) and component recipes.
- `KetQt::Widgets`: `ket::Style` (Fusion-based), `ket::Button`, `ket::Badge`.
- `KetQt::Quick`: the QML module `Ket` with `KetTheme`, `KetButton`, `KetBadge`.

See [docs/architecture.md](docs/architecture.md) and
[decision 0001](docs/decisions/0001-shared-qt-component-kit.md).

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
ketqt_kitchensink --smoke   # every page, light and dark; non-zero on QML errors (a CTest)
```

## Using it

```cpp
#include "KetButton.h"
#include "KetStyle.h"

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
([licence](third_party/ket-design-system/LICENSE)). Qt is used under its LGPL 3.0,
GPL or commercial terms as the consuming app chooses.
