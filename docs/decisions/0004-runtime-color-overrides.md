# 0004: Runtime color overrides for existing tokens

Status: accepted (2026-11-13)

## Context

Applications need user-selected color schemes while the Widgets and Quick layers
continue sharing one recipe implementation. Updating only QApplication's palette
cannot change recipes that read Tokens directly. Product formats and persistence
must remain outside the shared kit, including for Lampion.

## Decision

Appearance accepts a replacement QHash of QColor overrides per Light/Dark branch.
Only existing color tokens can be overridden; rejected names are returned to the
caller. Generated tables remain the authoritative names, types and defaults. All
color read APIs expose the same overrides, while metrics and profiles stay fixed.

Each explicit set/clear on the GUI thread emits exactly one existing themeChanged
notification after publication, including no-op and inactive-branch requests. The
signal denotes refreshed appearance values as well as light/dark changes. Existing
widget and QML observers therefore update without a new application migration path
or any view/model recreation. Blocking intermediate signals allows the host to
batch its mode and color changes before publishing the final set once.

Concurrent readers use a read/write lock around runtime maps. The writer releases
the lock before notifying observers. The kit owns no file schema, storage, importer,
network operation or application-specific role mapping.

## Consequences

Consumers that do not use overrides retain exact generated values and existing
behavior. The application translates its colors into kit token names and handles
rejected keys. Missing override entries always fall back to static defaults.
Keeping notifications explicit also refreshes recipes that read a fixed theme
branch, while leaving mode, brightness, zoom and geometry unchanged.

## Alternatives considered

A second QML-only palette would split the two rendering layers. Product-specific
tokens or file readers would violate the shared-kit boundary. Mutating generated
source files would change defaults for every consumer and require rebuilding.
