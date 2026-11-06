// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "KetRecipes.h"

#include <QFlags>

namespace ket {

// Toolbar tools of the image viewer. ZoomOut and ZoomIn are the design system's; the rest
// are for hosts that edit or keep images (a chat's photos, an editor's image files).
// ZoomLevel shows the scale as a percentage and toggles fit and 100 % when clicked.
enum class LightboxTool {
    ZoomOut = 0x1,
    ZoomLevel = 0x2,
    ZoomIn = 0x4,
    Rotate = 0x8,
    Copy = 0x10,
    Download = 0x20,
    Fullscreen = 0x40,
};
Q_DECLARE_FLAGS(LightboxTools, LightboxTool)
Q_DECLARE_OPERATORS_FOR_FLAGS(LightboxTools)

// The toolbar's order, left to right, in both layers; Close always ends the row.
inline constexpr LightboxTool kLightboxToolOrder[] = {
    LightboxTool::ZoomOut, LightboxTool::ZoomLevel, LightboxTool::ZoomIn,     LightboxTool::Rotate,
    LightboxTool::Copy,    LightboxTool::Download,  LightboxTool::Fullscreen,
};

// The thumbnail that opens the viewer (`[data-ui="lightbox-thumb"][data-size]`).
enum class LightboxThumbSize { Small, Medium, Large };

// The full-window image viewer (`[data-ui="lightbox-layer"]`): a dark backdrop, a toolbar
// with the counter and the tools, the picture on a padded stage, previous/next buttons on
// the sides and a caption. It reads the same in both themes, like a photo viewer.
struct LightboxRecipe {
    QColor backdrop;
    QColor foreground; // counter, caption, glyphs and the focus ring
    qreal toolbarPaddingBlock = 0;
    qreal toolbarPaddingInline = 0;
    qreal toolbarGap = 0;
    qreal counterPixels = 0;
    qreal counterLineHeight = 0;
    qreal zoomLevelWidth = 0; // the ZoomLevel tool, wide enough for "3200%"
    qreal stagePaddingBlock = 0;
    qreal stagePaddingInline = 0;
    qreal narrowStagePaddingInline = 0;
    qreal narrowWidth = 0; // at or below this window width the narrow padding applies
    qreal navInset = 0;
    qreal captionPixels = 0;
    qreal captionLineHeight = 0;
    qreal captionPaddingTop = 0;
    qreal captionPaddingInline = 0;
    qreal captionPaddingBottom = 0;
    qreal placeholderIconSize = 0; // an image that is missing or failed to load
    qreal placeholderOpacity = 0;
    // Zoom is relative to the fitted picture: 1 fits, and the picture never fits larger
    // than its own pixels.
    qreal maxZoom = 0;
    qreal zoomStep = 0;  // per toolbar or keyboard step
    qreal tapZoom = 0;   // a tap on the fitted picture zooms to this
    qreal wheelStep = 0; // factor per wheel notch
    qreal swipeDistance = 0;
    int transitionMs = 0;
};
[[nodiscard]] LightboxRecipe lightboxRecipe(Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const LightboxRecipe& recipe);

// The round toolbar and previous/next buttons (`[data-ui="lightbox-tool"]`).
struct LightboxToolRecipe {
    QColor background;
    QColor glyph;
    QColor focusRing;
    qreal size = 0;
    qreal iconSize = 0;
    qreal focusWidth = 0;
    qreal focusOffset = 0;
    qreal opacity = 1;
};
[[nodiscard]] LightboxToolRecipe lightboxToolRecipe(const InteractionState& state, Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const LightboxToolRecipe& recipe);

struct LightboxThumbRecipe {
    QColor background;
    QColor border;
    QColor text; // the empty placeholder's
    QColor focusRing;
    qreal size = 0;
    qreal radius = 0;
    qreal textPixels = 0;
    qreal lineHeight = 0;
    qreal focusWidth = 0;
    qreal focusOffset = 0;
};
[[nodiscard]] LightboxThumbRecipe lightboxThumbRecipe(LightboxThumbSize size, bool focused,
                                                      Theme theme);
[[nodiscard]] QVariantMap toVariantMap(const LightboxThumbRecipe& recipe);

} // namespace ket
