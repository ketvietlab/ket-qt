// SPDX-License-Identifier: MPL-2.0
#include "KetThemeCard.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

// The miniature's own drawing (bars, lines and buttons) follows the mock's sketch. It is
// an illustration, not product chrome, so its proportions are fixed here.
constexpr qreal kSidePaddingBlock = 15;
constexpr qreal kSidePaddingInline = 7;
constexpr qreal kSideBar = 4;
constexpr qreal kSideBarGap = 11;
constexpr qreal kMainPaddingBlock = 18;
constexpr qreal kMainPaddingInline = 13;
constexpr qreal kTitleHeight = 5;
constexpr qreal kLineHeight = 3;
constexpr qreal kLineGap = 8;
constexpr qreal kButtonHeight = 25;
constexpr qreal kButtonGap = 6;
constexpr qreal kButtonMargin = 13;
constexpr qreal kMiniRadius = 4;

void drawMini(QPainter& painter, const QRectF& box, const ThemeCardRecipe& r) {
    painter.save();
    QPainterPath clip;
    clip.addRoundedRect(box, scaled(kMiniRadius), scaled(kMiniRadius));
    painter.setClipPath(clip);
    painter.fillRect(box, r.miniBackground);
    const QRectF side(box.left(), box.top(), scaled(r.miniSideWidth), box.height());
    painter.fillRect(side, r.miniSidebar);
    painter.fillRect(QRectF(side.right() - 1, box.top(), 1, box.height()), r.miniBorder);
    painter.setPen(Qt::NoPen);
    qreal y = box.top() + scaled(kSidePaddingBlock);
    for (int i = 0; i < 3; ++i) {
        painter.setBrush(i == 1 ? r.miniAccentBar : r.miniBar);
        const QRectF bar(side.left() + scaled(kSidePaddingInline), y,
                         side.width() - 1 - 2 * scaled(kSidePaddingInline), scaled(kSideBar));
        painter.drawRoundedRect(bar, scaled(3), scaled(3));
        y += scaled(kSideBar + kSideBarGap);
    }
    const qreal left = side.right() + scaled(kMainPaddingInline);
    const qreal width = box.right() - left - scaled(kMainPaddingInline);
    y = box.top() + scaled(kMainPaddingBlock);
    painter.setBrush(r.miniTitle);
    painter.drawRoundedRect(QRectF(left, y, width * 0.6, scaled(kTitleHeight)), scaled(2),
                            scaled(2));
    y += scaled(kTitleHeight + kLineGap);
    painter.fillRect(QRectF(left, y, width * 0.8, scaled(kLineHeight)), r.miniLine);
    y += scaled(kLineHeight + kButtonMargin);
    const qreal button = (width - scaled(kButtonGap)) / 2;
    painter.setPen(QPen(r.miniButtonBorder, 1));
    painter.setBrush(r.miniButton);
    for (int i = 0; i < 2; ++i)
        painter.drawRoundedRect(
            QRectF(left + i * (button + scaled(kButtonGap)), y, button, scaled(kButtonHeight))
                .adjusted(0.5, 0.5, -0.5, -0.5),
            scaled(3), scaled(3));
    y += scaled(kButtonHeight + kButtonMargin);
    painter.fillRect(QRectF(left, y, width * 0.8, scaled(kLineHeight)), r.miniLine);
    painter.restore();
    painter.setPen(QPen(r.miniBorder, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), scaled(kMiniRadius),
                            scaled(kMiniRadius));
}

} // namespace

ThemeCard::ThemeCard(const QString& title, Theme preview, QWidget* parent)
    : QAbstractButton(parent), preview_(preview) {
    setText(title);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void ThemeCard::setPreview(Theme preview) {
    preview_ = preview;
    update();
}

void ThemeCard::setPreviewState(std::optional<InteractionState> state) {
    previewState_ = state;
    update();
}

ThemeCardRecipe ThemeCard::recipe() const {
    const InteractionState state =
        previewState_.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
    return themeCardRecipe(isChecked(), preview_, state, Appearance::instance().theme());
}

QSize ThemeCard::sizeHint() const {
    const ThemeCardRecipe r = recipe();
    QFont label = font();
    label.setPixelSize(qRound(scaled(r.labelPixels)));
    const qreal labelRow =
        qMax(QFontMetricsF(label).height(), scaled(r.checkSize)) + 2 * scaled(r.labelPaddingBlock);
    const qreal height = 2 + scaled(r.miniMargin) + scaled(r.miniHeight) + labelRow;
    return {qRound(scaled(r.miniSideWidth) * 5), qCeil(height)};
}

void ThemeCard::paintEvent(QPaintEvent*) {
    const ThemeCardRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    // The selected ring is a second accent pixel outside the border, as in the mock.
    const QRectF frame = QRectF(rect()).adjusted(1, 1, -1, -1);
    painter.setPen(QPen(r.background, 0));
    painter.setBrush(r.background);
    painter.drawRoundedRect(frame, radius, radius);
    const qreal margin = scaled(r.miniMargin);
    const QRectF mini(frame.left() + 1 + margin, frame.top() + 1 + margin,
                      frame.width() - 2 - 2 * margin, scaled(r.miniHeight));
    drawMini(painter, mini, r);
    const QColor ring = r.focusRing.alpha() ? r.focusRing : r.ring;
    painter.setBrush(Qt::NoBrush);
    if (ring.alpha()) {
        painter.setPen(QPen(ring, 2));
        painter.drawRoundedRect(frame, radius, radius);
    } else {
        painter.setPen(QPen(r.border, 1));
        painter.drawRoundedRect(frame.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    }
    const QRectF row(frame.left() + 1 + scaled(r.labelPaddingInline), mini.bottom(),
                     frame.width() - 2 - 2 * scaled(r.labelPaddingInline),
                     frame.bottom() - 1 - mini.bottom());
    QFont label = font();
    label.setPixelSize(qRound(scaled(r.labelPixels)));
    painter.setFont(label);
    painter.setPen(r.label);
    painter.drawText(row, Qt::AlignLeft | Qt::AlignVCenter, text());
    const qreal check = scaled(r.checkSize);
    painter.drawPixmap(
        QPointF(qRound(row.right() - check), qRound(row.center().y() - check / 2)),
        Icons::pixmap(isChecked() ? u"success" : u"circle", check, devicePixelRatioF(), r.check));
}

AccentSwatch::AccentSwatch(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

QSize AccentSwatch::sizeHint() const {
    const AccentSwatchRecipe r = accentSwatchRecipe(Appearance::instance().theme());
    const int side = qRound(scaled(r.size)) + 2; // the outer 1px ring
    return {side, side};
}

void AccentSwatch::paintEvent(QPaintEvent*) {
    const AccentSwatchRecipe r = accentSwatchRecipe(Appearance::instance().theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    const QRectF outer = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    painter.setPen(QPen(r.ring, 1));
    painter.setBrush(r.border);
    painter.drawRoundedRect(outer, radius + 1, radius + 1);
    const qreal frame = scaled(r.borderWidth);
    const qreal inner = qMax<qreal>(0, radius - frame);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.fill);
    painter.drawRoundedRect(QRectF(rect()).adjusted(1 + frame, 1 + frame, -1 - frame, -1 - frame),
                            inner, inner);
}

} // namespace ket
