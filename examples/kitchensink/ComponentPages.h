// SPDX-License-Identifier: MPL-2.0
#pragma once

class QWidget;

namespace ket::kitchensink {

// Component pages: the Qt Widgets and Qt Quick versions side by side, in every variant, size
// and state, so a recipe change can be compared across toolkits.
QWidget* buttonPage();
QWidget* iconButtonPage();
QWidget* badgePage();
QWidget* tagPage();
QWidget* iconPage();

} // namespace ket::kitchensink
