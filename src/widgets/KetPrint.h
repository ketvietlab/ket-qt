// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QImage>
#include <QString>
#include <QUrl>

class QWidget;

namespace ket {

// Prints `image` through the system print dialog, fitted and centred on the page. Landscape
// is preselected for a wide image, and a small image is not enlarged beyond its size at
// 96 dpi. Returns false when the image is empty, the dialog is cancelled or printing fails.
// Needs a QApplication: the dialog is a widget.
bool printImage(const QImage& image, const QString& title, QWidget* parent = nullptr);
// The same for a Qt Quick host: the `source` of a KetLightbox item (a `qrc:` or `file:` URL;
// download others first), turned by `rotation` degrees as KetLightbox.rotationAngle shows it.
bool printImage(const QUrl& source, int rotation, const QString& title, QWidget* parent = nullptr);
// The image printImage(source, rotation) sends; null when it cannot be read.
[[nodiscard]] QImage printableImage(const QUrl& source, int rotation);

// Where printImage() places an image of `size` inside a `page` of a device at `resolution`
// dots per inch.
[[nodiscard]] QRect printedImageRect(QSize size, QRect page, int resolution);

} // namespace ket
