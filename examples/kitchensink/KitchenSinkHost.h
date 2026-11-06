// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QObject>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

namespace ket::kitchensink {

// What a Qt Quick host does for the kit's host actions; here, the image viewer's Print.
class KitchenSinkHost final : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
  public:
    using QObject::QObject;
    Q_INVOKABLE bool printImage(const QUrl& source, int rotation, const QString& title) const;
};

} // namespace ket::kitchensink
