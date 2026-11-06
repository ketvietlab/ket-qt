# 0003: The image viewer draws the design system's chrome; hosts may bring the engine

Status: accepted, 2026-11-06

## Context

KétChat shows conversation photos with Két Image Viewer (`ketvietlab/ket-image-viewer`,
MIT, Qt Widgets): asynchronous decoding, a bounded cache, rotation, copy and atomic export.
Its built-in toolbar and thumbnail list are plain Qt widgets, so KétChat restyles them with
style sheets and palette fixes. KetPlus needs the same viewer, mostly in Qt Quick. The design
system already defines the look and behaviour in `interactions/lightbox`.

## Decision

- The kit implements the lightbox in both layers from one recipe set:
  `ket::lightboxRecipe`, `ket::lightboxToolRecipe` and `ket::lightboxThumbRecipe`. The parts
  are the full-window viewer (`ket::Lightbox`, `KetLightbox`), the opening thumbnail
  (`LightboxThumb`) and the round tool (`LightboxButton`).
- Both layers have a built-in picture with the design system's behaviour. Zoom 1 fits and
  goes up to 4× the fit, and always far enough to reach 100 %. Tools and keys step by 0.5,
  a tap zooms to 2×, the wheel and pinch zoom about the pointer, a drag pans within the
  picture's edges, and a swipe at fit size moves to the neighbour.
- The kit does not depend on Két Image Viewer. A Widgets host passes the engine's
  `ImageViewer` (toolbar and thumbnails hidden) to `ket::Lightbox::setStage()`. View tools
  then only emit `toolTriggered()`, and the host reports its scale with `setZoomLevel()`.
- Beyond the design system's zoom-out, zoom-in and close, the toolbar can show:
  - ZoomLevel, a percentage that toggles fit and 100 %;
  - Rotate, Copy, Download, Print and Full screen.
  Copy, Download and Full screen are always host actions.
- Print opens the system print dialog (`ket::printImage()`, Qt Print Support in
  `KetQt::Widgets`), fitted to the page and never enlarged beyond 96 dpi.
  - `ket::Lightbox` prints its built-in picture itself.
  - `KetLightbox` only emits the tool, because the Quick layer has no print dialog. A
    `QApplication` host calls `ket::printImage(source, rotationAngle, title)`.
  - The tool draws Két Symbol `printer`, vendored for it from ket-art 518527ad.
- The viewer is dark in both themes: light-theme `text-main` at 92 % behind light-theme
  `panel-bg` glyphs. The design system mixes the current theme's tokens, which makes the
  dark-theme viewer light; the kit does not copy that, and the difference is to be raised with
  the design system.
- Két Symbol has no clockwise rotate, so Rotate draws `restore` mirrored until one is added.

## Consequences

KétChat can drop its toolbar style sheet: it keeps the engine and puts it on the kit's
stage. KetPlus uses `KetLightbox` directly. A Quick host that needs asynchronous decoding
or caching beyond `Image` must supply its own image provider.
