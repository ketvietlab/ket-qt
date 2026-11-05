// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// KetTag samples: `mode` is "rest", "hover" or "edge" (filterable, elided and overflow).
// The colours mirror ComponentPages.cpp; tag colours belong to the app, not the kit.
Row {
    id: samples

    property string mode: "rest"
    readonly property var colors: [
        { label: "Blue", light: "#2f6fde", dark: "#7aa7ff" },
        { label: "Green", light: "#23875a", dark: "#5fcf95" },
        { label: "Violet", light: "#7a55d6", dark: "#b39dff" },
        { label: "Amber", light: "#b7791f", dark: "#f0b45a" },
        { label: "Teal", light: "#1c8a92", dark: "#5fc9cf" },
        { label: "Red", light: "#d1453b", dark: "#ff8a80" },
        { label: "Pink", light: "#c2417f", dark: "#f28cc0" },
        { label: "Slate", light: "#64748b", dark: "#a3b0c2" }
    ]
    readonly property color blue: KetTheme.dark ? colors[0].dark : colors[0].light

    spacing: KetTheme.px(KetTheme.t.space1, KetTheme.zoom)

    Repeater {
        model: samples.mode === "edge" ? [] : samples.colors
        KetTag {
            required property var modelData
            text: modelData.label
            tagColor: KetTheme.dark ? modelData.dark : modelData.light
            previewHovered: samples.mode === "hover"
        }
    }

    KetTag {
        visible: samples.mode === "edge"
        text: "Filter on hover"
        tagColor: samples.blue
        filterable: true
    }
    KetTag {
        visible: samples.mode === "edge"
        text: "A label longer than the chip allows"
        tagColor: samples.blue
    }
    KetTag {
        visible: samples.mode === "edge"
        text: "+2"
        overflow: true
    }
}
