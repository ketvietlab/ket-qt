// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// Live KetButtons: hover, press and Tab through them.
Row {
    spacing: KetTheme.px(KetTheme.t.space2, KetTheme.zoom)

    KetButton { text: "Primary"; variant: KetTheme.ButtonVariant.Primary }
    KetButton { text: "Secondary" }
    KetButton { text: "Ghost"; variant: KetTheme.ButtonVariant.Ghost }
    KetButton { text: "Danger"; variant: KetTheme.ButtonVariant.Danger }
    KetButton { text: "Ghost danger"; variant: KetTheme.ButtonVariant.GhostDanger }
}
