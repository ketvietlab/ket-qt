# KetQt

Két Design System components for Qt 6.8 or later, shared by KetPlus, KétChat and Lampion
OS. Qt Widgets and Qt Quick draw the same components from the same tokens and recipes, so an
app looks the same in both toolkits and on macOS, Windows and Linux.

- `KetQt::Tokens`: design-system tokens generated from the pinned Két DS `tokens.css`,
  the shared theme state (`ket::Appearance`) and component recipes.
- `KetQt::Icons`: Két Symbol icons with their size and weight rules (`ket::Icons`).
- `KetQt::Widgets`: `ket::Style` (Fusion-based), `ket::Button`, `ket::IconButton`,
  `ket::Badge`, `ket::Tag`, `ket::Icon`, `ket::TextField`, `ket::Select`, `ket::Checkbox`,
  `ket::Radio`, `ket::Choice`, `ket::FieldRow`, `ket::Banner`, `ket::Toast`, `ket::Dialog`,
  `ket::NavItem`, `ket::Kbd`, `ket::SegmentedControl`, `ket::ActivityIndicator`,
  `ket::EmptyState`, `ket::ConversationRow`, `ket::Composer`, `ket::UserMessage`,
  `ket::AssistantMessage`, `ket::ReadingText`, `ket::DayDivider`, `ket::ActivityRow`,
  `ket::MarkdownView`, `ket::MarkdownText`, `ket::CodeBlock`, `ket::MentionPicker`,
  `ket::UsageRing`, `ket::Popover`, `ket::Meter`, `ket::AttachmentChip`,
  `ket::Suggestion`, `ket::QuestionCard`, `ket::ActivityGroup`, `ket::Gallery`,
  `ket::StepSlider`, `ket::Avatar`, `ket::TreeItem`, `ket::GroupHeader`,
  `ket::SidebarRepositoryHeader`, `ket::SidebarTeamBranch`, `ket::SidebarSectionLabel`,
  `ket::SidebarRow`, `ket::SidebarTreeGuide`, `ket::RouteTabs`,
  `ket::StatusBar`, `ket::Breadcrumb`, `ket::PaneHeader`, `ket::CommandPalette`,
  `ket::FileRow`, `ket::ChangedFiles`, `ket::DiffView`, `ket::TaskBar`, `ket::TaskRow`,
  `ket::UpdateCard`, `ket::UpdatePill`, `ket::ThemeCard`, `ket::AccentSwatch`,
  `ket::Lightbox`, `ket::LightboxThumb`, `ket::LightboxButton`;
  `ket::Style` also draws every `QMenu`, the select popup, `QToolTip`, top
  `QTabBar`s, horizontal `QProgressBar`s and horizontal `QSlider`s.
- `KetQt::Quick`: the QML module `Ket` with `KetTheme`, `KetButton`, `KetIconButton`,
  `KetBadge`, `KetTag`, `KetIcon`, `KetTextField`, `KetSelect`, `KetCheckbox`, `KetRadio`,
  `KetChoice`, `KetFieldRow`, `KetBanner`, `KetToast`, `KetDialog`, `KetMenu`, `KetMenuItem`,
  `KetMenuSeparator`, `KetMenuSection`, `KetToolTip`, `KetNavItem`, `KetKbd`, `KetSegmentedControl`, `KetTabBar`,
  `KetTabButton`, `KetProgressBar`, `KetActivityIndicator`, `KetEmptyState`,
  `KetConversationRow`, `KetComposer`, `KetUserMessage`, `KetAssistantMessage`, `KetReadingText`,
  `KetDayDivider`, `KetActivityRow`, `KetMarkdown`, `KetMarkdownText`, `KetCodeBlock`,
  `KetMentionPicker`, `KetUsageRing`, `KetPopover`, `KetPopoverText`, `KetPopoverDivider`,
  `KetMeter`, `KetAttachmentChip`, `KetSuggestion`, `KetQuestionCard`, `KetActivityGroup`,
  `KetGallery`, `KetSlider`, `KetStepSlider`, `KetAvatar`, `KetTreeItem`, `KetGroupHeader`,
  `KetSidebarRepositoryHeader`, `KetSidebarTeamBranch`, `KetSidebarSectionLabel`,
  `KetSidebarRow`, `KetSidebarTreeGuide`,
  `KetRouteTabs`, `KetStatusBar`, `KetStatusItem`, `KetBreadcrumb`, `KetPaneHeader`,
  `KetCommandPalette`, `KetCommandResult`, `KetFileRow`, `KetChangedFiles`, `KetDiffView`,
  `KetTaskBar`, `KetTaskRow`, `KetUpdateCard`, `KetUpdatePill`, `KetThemeCard`,
  `KetAccentSwatch`, `KetLightbox`, `KetLightboxThumb`, `KetLightboxButton`.

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

## Runtime colors

Applications can replace existing color-token values without changing the pinned
names or metrics. The same values feed Widgets and QML recipes:

```cpp
#include "KetAppearance.h"

auto& appearance = ket::Appearance::instance();
const QStringList rejected = appearance.setColorOverrides(ket::Theme::Dark, {
    {QStringLiteral("page-bg"), QColor(24, 28, 32)},
    {QStringLiteral("action-primary-bg"), QColor(80, 120, 200)}
});
// Handle rejected names, non-color tokens or invalid colors in the host UI.
appearance.clearColorOverrides(ket::Theme::Dark);
```

Set/clear on the GUI thread. Each call emits one `themeChanged` notification after
publishing the complete branch, so existing Widgets and `KetTheme` bindings refresh
in place. Clear restores the generated defaults. See
[the override contract](docs/architecture.md#runtime-color-overrides) for replacement,
alpha, threading and notification semantics. No application or file-format policy
is added to the kit.
