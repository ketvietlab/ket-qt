// SPDX-License-Identifier: MPL-2.0
#include "KetIcons.h"

#include <QHash>
#include <QIconEngine>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QtMath>

static void initKetSymbolResources() { Q_INIT_RESOURCE(ket_symbol); }

namespace ket {
namespace {

struct LucideRow {
    const char* lucide;
    const char* code;
};

constexpr int kSizes[] = {
#define KET_SYMBOL_SIZE(px) px,
#include "generated/KetSymbolNames.inc"
#undef KET_SYMBOL_SIZE
};

constexpr const char* kCodes[] = {
#define KET_SYMBOL_CODE(identifier, code) code,
#include "generated/KetSymbolNames.inc"
#undef KET_SYMBOL_CODE
};

constexpr LucideRow kLucide[] = {
#define KET_SYMBOL_LUCIDE(lucide, code, status) {lucide, code},
#include "generated/KetSymbolNames.inc"
#undef KET_SYMBOL_LUCIDE
};

void ensureResources() {
    static const bool loaded = [] {
        initKetSymbolResources();
        return true;
    }();
    Q_UNUSED(loaded);
}

const QHash<QString, QString>& lucideIndex() {
    static const QHash<QString, QString> index = [] {
        QHash<QString, QString> result;
        for (const LucideRow& row : kLucide)
            result.insert(QString::fromLatin1(row.lucide), QString::fromLatin1(row.code));
        return result;
    }();
    return index;
}

QLatin1String weightName(IconWeight weight) {
    return weight == IconWeight::Regular ? QLatin1String("regular") : QLatin1String("light");
}

// Renders the master into a `box` x `box` image, black, at whole device pixels.
QImage render(QStringView code, qreal box, qreal dpr, IconWeight weight) {
    ensureResources();
    const int side = qMax(1, qRound(box * dpr));
    QImage image(side, side, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    image.setDevicePixelRatio(dpr);
    QSvgRenderer renderer(Icons::resourcePath(code, box, weight));
    if (!renderer.isValid())
        return image;
    const qreal size = Icons::drawSize(box);
    const qreal origin = Icons::offset(box, dpr);
    QPainter painter(&image);
    renderer.render(&painter, QRectF(origin, origin, size, size));
    return image;
}

class SymbolEngine final : public QIconEngine {
  public:
    SymbolEngine(QString code, IconWeight weight) : code_(std::move(code)), weight_(weight) {}

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode, QIcon::State) override {
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
        const int box = qMin(rect.width(), rect.height());
        const QImage image = render(code_, box, dpr, weight_);
        painter->drawImage(QRectF(rect.x() + (rect.width() - box) / 2.0,
                                  rect.y() + (rect.height() - box) / 2.0, box, box),
                           image);
    }

    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override {
        return scaledPixmap(size, mode, state, 1.0);
    }

    QPixmap scaledPixmap(const QSize& size, QIcon::Mode, QIcon::State, qreal scale) override {
        const int box = qMin(size.width(), size.height());
        return QPixmap::fromImage(render(code_, box, scale, weight_));
    }

    QIconEngine* clone() const override { return new SymbolEngine(code_, weight_); }
    QString key() const override { return QStringLiteral("KetSymbol"); }
    QString iconName() override { return code_; }
    bool isNull() override { return !Icons::contains(code_); }

  private:
    QString code_;
    IconWeight weight_;
};

} // namespace

bool Icons::contains(QStringView code) {
    for (const char* known : kCodes) {
        if (code == QLatin1String(known))
            return true;
    }
    return false;
}

QStringList Icons::codes() {
    QStringList result;
    result.reserve(int(std::size(kCodes)));
    for (const char* code : kCodes)
        result.append(QString::fromLatin1(code));
    return result;
}

QString Icons::codeForLucide(QStringView lucide) { return lucideIndex().value(lucide.toString()); }

// The nearest drawn master, the smaller one on a tie: with 14/16/20/24 px masters a 17 or
// 18 px box draws 16, 19-22 px draws 20 and 23 px or more draws 24.
int Icons::masterSize(qreal box) {
    int best = kSizes[0];
    for (const int size : kSizes) {
        if (qAbs(size - box) < qAbs(best - box))
            best = size;
    }
    return best;
}

qreal Icons::drawSize(qreal box) { return qMin<qreal>(box, masterSize(box)); }

qreal Icons::offset(qreal box, qreal devicePixelRatio) {
    return qFloor((box - drawSize(box)) * devicePixelRatio / 2.0) / devicePixelRatio;
}

QString Icons::resourcePath(QStringView code, qreal box, IconWeight weight) {
    return QStringLiteral(":/ket-symbol/%1/%2/%3.svg")
        .arg(weightName(weight))
        .arg(masterSize(box))
        .arg(code);
}

QPixmap Icons::pixmap(QStringView code, qreal box, qreal devicePixelRatio, const QColor& color,
                      IconWeight weight) {
    QImage image = render(code, box, devicePixelRatio, weight);
    QPainter painter(&image);
    // Keeps the drawing's alpha, including the 0.35 ghost layers of level icons.
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(QRectF(0, 0, box, box), color);
    painter.end();
    return QPixmap::fromImage(image);
}

QIcon Icons::icon(QStringView code, IconWeight weight) {
    return QIcon(new SymbolEngine(code.toString(), weight));
}

} // namespace ket
