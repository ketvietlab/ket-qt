// SPDX-License-Identifier: MPL-2.0
import QtQuick
import QtQuick.Controls.Basic as Basic
import Ket

// Menu command (`.desktop-context-menu button`); it draws ket::menuItemRecipe. `danger`
// marks a destructive command; `description` adds the secondary line (`small`).
Basic.MenuItem {
    id: control

    property bool danger
    property string description
    property string iconName
    property string shortcut
    readonly property bool reserveCheck: checkable || (menu && menu.hasCheckableItems === true)
    readonly property var recipe: KetTheme.menuItemRecipe(KetTheme.dark, KetTheme.zoom,
                                                          highlighted, enabled, danger)

    implicitWidth: leftPadding + rightPadding + contentItem.implicitWidth
    implicitHeight: Math.max(recipe.height, contentItem.implicitHeight + 2 * recipe.paddingStart)
    leftPadding: recipe.paddingStart
    rightPadding: recipe.paddingEnd
    topPadding: 0
    bottomPadding: 0
    spacing: recipe.spacing
    opacity: recipe.opacity
    hoverEnabled: true
    font.family: KetTheme.fontFamily
    font.pixelSize: recipe.fontPixels
    indicator: null
    arrow: null
    Accessible.name: text
    Accessible.description: description

    contentItem: Item {
        // Natural widths only, so the menu's width never feeds back into its items.
        implicitWidth: (control.reserveCheck ? control.recipe.checkSize + row.spacing : 0)
            + (control.iconName !== "" ? control.recipe.iconSize + row.spacing : 0)
            + Math.max(titleText.implicitWidth, descriptionText.implicitWidth)
            + (trailing.visible ? trailing.implicitWidth + 4 * row.spacing : 0)
        implicitHeight: row.implicitHeight

        Row {
            id: row
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            spacing: control.spacing

            Item {
                visible: control.reserveCheck
                width: control.recipe.checkSize
                height: titleText.implicitHeight

                KetIcon {
                    anchors.centerIn: parent
                    visible: control.checked
                    name: "check"
                    box: control.recipe.iconSize
                    color: control.recipe.text
                }
            }

            KetIcon {
                visible: control.iconName !== ""
                anchors.top: parent.top
                anchors.topMargin: Math.round((titleText.implicitHeight - box) / 2)
                name: control.iconName
                box: control.recipe.iconSize
                color: control.recipe.text
            }

            Column {
                readonly property real used: (control.reserveCheck ? control.recipe.checkSize + row.spacing : 0)
                    + (control.iconName !== "" ? control.recipe.iconSize + row.spacing : 0)
                    + (trailing.visible ? trailing.implicitWidth + 4 * row.spacing : 0)
                width: Math.max(implicitWidth, row.width - used)
                spacing: control.recipe.descriptionGap

                Text {
                    id: titleText
                    width: parent.width
                    text: control.text
                    textFormat: Text.PlainText
                    elide: Text.ElideRight
                    color: control.recipe.text
                    font: control.font
                }

                Text {
                    id: descriptionText
                    visible: control.description !== ""
                    width: parent.width
                    text: control.description
                    textFormat: Text.PlainText
                    wrapMode: Text.Wrap
                    lineHeight: 1.5
                    color: control.recipe.description
                    font.family: KetTheme.fontFamily
                    font.pixelSize: control.recipe.descriptionPixels
                }
            }

            Item {
                id: trailing
                visible: control.shortcut !== "" || control.subMenu !== null
                implicitWidth: control.subMenu ? control.recipe.iconSize : shortcutText.implicitWidth
                width: implicitWidth
                height: titleText.implicitHeight

                Text {
                    id: shortcutText
                    anchors.right: parent.right
                    visible: control.subMenu === null
                    text: control.shortcut
                    color: KetTheme.menuItemRecipe(KetTheme.dark, KetTheme.zoom,
                                                   control.highlighted, control.enabled,
                                                   false).description
                    font: control.font
                }

                KetIcon {
                    anchors.centerIn: parent
                    visible: control.subMenu !== null
                    name: "chevron-right"
                    box: control.recipe.iconSize
                    color: control.recipe.text
                }
            }
        }
    }

    background: Rectangle {
        radius: control.recipe.radius
        color: control.recipe.background
    }
}
