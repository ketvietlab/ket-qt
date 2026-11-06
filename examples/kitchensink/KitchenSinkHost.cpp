// SPDX-License-Identifier: MPL-2.0
#include "KitchenSinkHost.h"

#include "KetPrint.h"

namespace ket::kitchensink {

bool KitchenSinkHost::printImage(const QUrl& source, int rotation, const QString& title) const {
    return ket::printImage(source, rotation, title);
}

} // namespace ket::kitchensink
