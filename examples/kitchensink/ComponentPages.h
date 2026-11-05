// SPDX-License-Identifier: MPL-2.0
#pragma once

class QWidget;

namespace ket::kitchensink {

// Component pages: the Qt Widgets and Qt Quick versions side by side, in every variant, size
// and state, so a recipe change can be compared across toolkits.
QWidget* buttonPage();
QWidget* badgePage();

} // namespace ket::kitchensink
