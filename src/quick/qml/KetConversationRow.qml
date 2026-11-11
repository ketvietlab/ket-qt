// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Two-line sidebar conversation row; it draws ket::conversationRowRecipe and
// ket::conversationStatusRecipe, as ket::ConversationRow does. `tags` is a list of
// { label, color, locked }; whole tags that fit are shown, the rest counted in +N. Without tags the
// meta line shows the status label.
Basic.AbstractButton {
    id: control

    property string number
    property var tags: []
    property int status: KetTheme.ConversationStatus.NotStarted
    property string statusLabel: KetTheme.conversationStatusLabel(status)
    property var previewState: null
    readonly property var recipe: previewState
        ? KetTheme.conversationRowRecipe(KetTheme.dark, KetTheme.zoom, checked,
                                         !!previewState.hovered, previewState.enabled !== false,
                                         !!previewState.focused)
        : KetTheme.conversationRowRecipe(KetTheme.dark, KetTheme.zoom, checked, hovered, enabled,
                                         visualFocus)
    readonly property var statusRecipe: KetTheme.conversationStatusRecipe(KetTheme.dark, status)
    readonly property var moreRecipe: KetTheme.tagRecipe(KetTheme.dark, KetTheme.zoom,
                                                         "transparent", true, false)
    // The lock a locked tag ({ label, color, locked: true }) leads with.
    readonly property real lockedIcon: {
        const locked = KetTheme.lockedTagRecipe(KetTheme.dark, KetTheme.zoom, "transparent")
        return locked.iconSize + locked.iconGap
    }
    // How many whole tags fit, keeping room for +N while any tag is left out.
    readonly property int visibleTagCount: {
        const room = tagArea.width
        const gap = recipe.tagGap
        const chip = (text, locked) => Math.min(
            tagMetrics.advanceWidth(text) + 2 * moreRecipe.paddingInline
                + (locked ? lockedIcon : 0), recipe.tagMaxWidth)
        let x = 0
        for (let i = 0; i < tags.length; ++i) {
            const remaining = tags.length - i - 1
            const reserve = remaining ? gap + chip("+" + remaining, false) : 0
            if (x + chip(tags[i].label, !!tags[i].locked) + reserve > room)
                return i
            x += chip(tags[i].label, !!tags[i].locked) + gap
        }
        return tags.length
    }

    checkable: true
    hoverEnabled: true
    implicitHeight: recipe.height
    implicitWidth: KetTheme.px(240, KetTheme.zoom)
    padding: 0
    opacity: recipe.opacity
    Accessible.name: text
    Accessible.description: statusLabel

    FontMetrics {
        id: tagMetrics
        font.family: KetTheme.fontFamily
        font.pixelSize: control.moreRecipe.fontPixels
        font.weight: control.moreRecipe.fontWeight
    }

    // A working icon turns once per spin period on KetTheme's stepped clock.
    readonly property bool spins: statusRecipe.spinning && visible
    property bool holdsClock: false
    function syncClock() {
        if (spins !== holdsClock) {
            holdsClock = spins
            KetTheme.holdAnimationClock(spins)
        }
    }
    onSpinsChanged: syncClock()
    Component.onCompleted: syncClock()
    Component.onDestruction: if (holdsClock) KetTheme.holdAnimationClock(false)

    contentItem: Item {
        KetIcon {
            id: indicator
            x: parent.width - control.recipe.paddingInline - width
            y: control.recipe.paddingBlock + control.recipe.indicatorTop
            name: control.statusRecipe.icon
            box: control.recipe.indicatorSize
            rotation: control.holdsClock
                      ? 360 * (KetTheme.animationClock % control.statusRecipe.spinMs)
                        / control.statusRecipe.spinMs
                      : 0
            color: control.statusRecipe.iconColor
        }

        Text {
            x: control.recipe.paddingInline
            y: control.recipe.paddingBlock
            width: indicator.x - control.recipe.spacing - x
            height: control.recipe.titleLineHeight
            verticalAlignment: Text.AlignVCenter
            text: control.text
            elide: Text.ElideRight
            color: control.recipe.title
            font.family: KetTheme.fontFamily
            font.pixelSize: control.recipe.titlePixels
            font.weight: control.recipe.titleWeight
        }

        Row {
            id: meta
            x: control.recipe.paddingInline
            y: control.recipe.paddingBlock + control.recipe.titleLineHeight + control.recipe.lineGap
            width: indicator.x - control.recipe.spacing - x
            height: control.recipe.metaHeight
            spacing: control.recipe.metaGap

            Text {
                id: numberText
                visible: control.number !== ""
                anchors.verticalCenter: parent.verticalCenter
                text: control.number
                color: control.recipe.number
                font.family: KetTheme.monoFontFamily
                font.pixelSize: control.recipe.numberPixels
            }

            Item {
                id: tagArea
                visible: control.tags.length > 0
                width: meta.width - (numberText.visible ? numberText.width + meta.spacing : 0)
                height: parent.height

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: control.recipe.tagGap

                    Repeater {
                        model: control.tags.slice(0, control.visibleTagCount)
                        KetTag {
                            required property var modelData
                            text: modelData.label
                            tagColor: modelData.color
                            locked: !!modelData.locked
                            maxWidth: control.recipe.tagMaxWidth
                        }
                    }

                    KetTag {
                        visible: control.visibleTagCount < control.tags.length
                        text: "+" + (control.tags.length - control.visibleTagCount)
                        overflow: true
                    }
                }
            }

            Text {
                visible: control.tags.length === 0
                anchors.verticalCenter: parent.verticalCenter
                width: meta.width - (numberText.visible ? numberText.width + meta.spacing : 0)
                text: control.statusLabel
                elide: Text.ElideRight
                color: control.statusRecipe.labelColor
                font.family: KetTheme.fontFamily
                font.pixelSize: control.recipe.metaPixels
                font.weight: control.recipe.metaWeight
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
        border.width: control.recipe.focusRing.a > 0 ? 2 : 0
        border.color: control.recipe.focusRing
    }
}
