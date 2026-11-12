# Toast placement fixture

Captured from the real Widgets regression `toastsAvoidVisibleBottomRegions`, using
Qt offscreen at 800×600. These are generic kit fixtures, not application screenshots.
The input region is a plain labelled widget; no account, workspace or native settings
are used. The application integration must additionally verify its real composer.

- `toast-default.png`: existing bottom placement overlaps the reserved input area.
- `toast-avoiding.png`: the same toast is above the input region with the recipe gap.
- `toast-grown.png`: the input region grows upward and the toast follows it.

The geometry test also checks ancestor movement, window resize, hiding/showing,
clearing the reservation and destroying the input region. The Quick regression
checks reserved-height changes, host resize and reset with a real ApplicationWindow.

Validation: standalone Debug kit build; offscreen ctest 2/2 passed, Widgets 37 cases
and Quick 22 cases, with no failures or skips. No shipping application was launched.
