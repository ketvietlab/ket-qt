// SPDX-License-Identifier: MPL-2.0
import QtQuick
import Ket

// A column of sidebar tree parts with the guide line of its level, as ket::SidebarTreeGuide
// draws it: a 1 px line under the chevron's centre, as long as what it holds. Below a
// team's members (level 1 and deeper) it keeps the branch gap. Parts fill its width.
Item {
    id: guide

    property int level: 0
    default property alias parts: column.data
    readonly property var columns: KetTheme.sidebarTreeColumns(level, KetTheme.zoom)
    readonly property var recipe: KetTheme.sidebarTreeGuideRecipe(KetTheme.dark, KetTheme.zoom)
    readonly property real guideLeft: line.x

    implicitWidth: column.implicitWidth
    implicitHeight: column.height + (level > 0 ? recipe.branchGap : 0)

    Rectangle {
        id: line
        objectName: "sidebarTreeGuideLine"
        x: guide.columns.guide
        width: Math.max(1, guide.recipe.width)
        height: column.height
        color: guide.recipe.color
    }

    Column {
        id: column
        width: guide.width
        onChildrenChanged: {
            for (let i = 0; i < children.length; ++i)
                children[i].width = Qt.binding(() => column.width)
        }
    }
}
