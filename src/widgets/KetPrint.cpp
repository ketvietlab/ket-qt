// SPDX-License-Identifier: MPL-2.0
#include "KetPrint.h"

#include <QImageReader>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

namespace ket {

QRect printedImageRect(QSize size, QRect page, int resolution) {
    if (size.isEmpty() || page.isEmpty())
        return {};
    // One image pixel is one logical pixel, 1/96 inch, at most.
    const QSize natural = size * (qreal(resolution) / 96.0);
    QSize fitted = size.scaled(page.size(), Qt::KeepAspectRatio);
    if (fitted.width() > natural.width())
        fitted = natural;
    return {page.x() + (page.width() - fitted.width()) / 2,
            page.y() + (page.height() - fitted.height()) / 2, fitted.width(), fitted.height()};
}

QImage printableImage(const QUrl& source, int rotation) {
    QString path;
    if (source.scheme() == u"qrc")
        path = QLatin1Char(':') + source.path();
    else if (source.isLocalFile())
        path = source.toLocalFile();
    else
        return {};
    QImageReader reader(path);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    return rotation % 360 ? image.transformed(QTransform().rotate(rotation)) : image;
}

bool printImage(const QUrl& source, int rotation, const QString& title, QWidget* parent) {
    return printImage(printableImage(source, rotation), title, parent);
}

bool printImage(const QImage& image, const QString& title, QWidget* parent) {
    if (image.isNull())
        return false;
    QPrinter printer(QPrinter::HighResolution);
    printer.setDocName(title);
    printer.setPageOrientation(image.width() > image.height() ? QPageLayout::Landscape
                                                              : QPageLayout::Portrait);
    QPrintDialog dialog(&printer, parent);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    QPainter painter;
    if (!painter.begin(&printer))
        return false;
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(printedImageRect(image.size(), painter.viewport(), printer.resolution()),
                      image);
    return painter.end();
}

} // namespace ket
