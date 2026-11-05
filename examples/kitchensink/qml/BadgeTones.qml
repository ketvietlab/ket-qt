// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// KetBadge in every tone; ComponentPages.cpp draws the Widgets twin.
Row {
    spacing: KetTheme.px(KetTheme.t.space15, KetTheme.zoom)

    KetBadge { text: "Neutral" }
    KetBadge { text: "Accent (brand)"; tone: KetTheme.BadgeTone.Accent }
    KetBadge { text: "Info"; tone: KetTheme.BadgeTone.Info }
    KetBadge { text: "Success"; tone: KetTheme.BadgeTone.Success }
    KetBadge { text: "Warning"; tone: KetTheme.BadgeTone.Warning }
    KetBadge { text: "Danger"; tone: KetTheme.BadgeTone.Danger }
}
