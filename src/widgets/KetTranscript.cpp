// SPDX-License-Identifier: MPL-2.0
#include "KetTranscript.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetParagraph.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

QFont sized(const QWidget* widget, qreal pixels, int weight = 400) {
    QFont font = widget->font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

void followAppearance(QWidget* widget) {
    QObject::connect(&Appearance::instance(), &Appearance::themeChanged, widget,
                     qOverload<>(&QWidget::update));
    QObject::connect(&Appearance::instance(), &Appearance::zoomChanged, widget,
                     &QWidget::updateGeometry);
}

// The default avatar: a rounded accent tile with the `bot` symbol, or the app's icon.
class Avatar final : public QWidget {
  public:
    explicit Avatar(QWidget* parent) : QWidget(parent) { followAppearance(this); }

    void setIcon(const QIcon& icon) {
        icon_ = icon;
        update();
    }

    QSize sizeHint() const override {
        const int size = qRound(scaled(assistantMessageRecipe(theme()).avatarSize));
        return {size, size};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const AssistantMessageRecipe r = assistantMessageRecipe(theme());
        const qreal size = scaled(r.avatarSize);
        const qreal radius = scaled(r.avatarRadius);
        const QRectF tile(0, 0, size, size);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!icon_.isNull()) {
            QPainterPath clip;
            clip.addRoundedRect(tile, radius, radius);
            painter.setClipPath(clip);
            painter.drawPixmap(tile.toRect(),
                               icon_.pixmap(tile.size().toSize(), devicePixelRatioF()));
            return;
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.avatarBackground);
        painter.drawRoundedRect(tile, radius, radius);
        const qreal box = scaled(r.avatarIconSize);
        painter.drawPixmap(QPointF(qRound((size - box) / 2), qRound((size - box) / 2)),
                           Icons::pixmap(u"bot", box, devicePixelRatioF(), r.avatarIcon));
    }

  private:
    QIcon icon_;
};

} // namespace

ReadingText::ReadingText(const QString& text, QWidget* parent) : QWidget(parent), text_(text) {
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    followAppearance(this);
}

void ReadingText::setText(const QString& text) {
    text_ = text;
    updateGeometry();
    update();
}

QFont ReadingText::readingFont() const {
    return sized(this, assistantMessageRecipe(theme()).bodyPixels);
}

int ReadingText::heightForWidth(int width) const {
    detail::Paragraph paragraph(text_, readingFont(), assistantMessageRecipe(theme()).lineHeight);
    paragraph.layout(width);
    return qCeil(paragraph.height());
}

QSize ReadingText::sizeHint() const {
    const int width = qRound(scaled(Tokens::px(u"space-16")) * 6);
    return {width, heightForWidth(width)};
}

void ReadingText::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (const int wanted = heightForWidth(width()); wanted != minimumHeight())
        setMinimumHeight(wanted);
}

void ReadingText::paintEvent(QPaintEvent*) {
    detail::Paragraph paragraph(text_, readingFont(), assistantMessageRecipe(theme()).lineHeight);
    paragraph.layout(width());
    QPainter painter(this);
    painter.setPen(assistantMessageRecipe(theme()).body);
    paragraph.draw(painter, QPointF());
}

UserMessage::UserMessage(const QString& text, QWidget* parent) : QWidget(parent), text_(text) {
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setAccessibleName(text);
    followAppearance(this);
}

void UserMessage::setText(const QString& text) {
    text_ = text;
    setAccessibleName(text);
    updateGeometry();
    update();
}

QFont UserMessage::bubbleFont() const { return sized(this, userMessageRecipe(theme()).fontPixels); }

QRectF UserMessage::bubbleFor(int width, qreal* textWidth) const {
    const UserMessageRecipe r = userMessageRecipe(theme());
    const qreal inline_ = scaled(r.paddingInline);
    const qreal block = scaled(r.paddingBlock);
    const qreal maxText = qMax<qreal>(1, width * r.maxWidth - 2 * inline_);
    detail::Paragraph paragraph(text_, bubbleFont(), r.lineHeight);
    const qreal used = qMin(maxText, qCeil(paragraph.layout(maxText)) + 1.0);
    if (textWidth)
        *textWidth = used;
    const qreal bubbleWidth = used + 2 * inline_;
    return {width - bubbleWidth, 0, bubbleWidth, paragraph.height() + 2 * block};
}

QRect UserMessage::bubbleRect() const { return bubbleFor(width()).toAlignedRect(); }

int UserMessage::heightForWidth(int width) const {
    return qCeil(bubbleFor(width).height() + scaled(userMessageRecipe(theme()).spacingAfter));
}

QSize UserMessage::sizeHint() const {
    const int width = qRound(scaled(Tokens::px(u"space-16")) * 8);
    return {width, heightForWidth(width)};
}

void UserMessage::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (const int wanted = heightForWidth(width()); wanted != minimumHeight())
        setMinimumHeight(wanted);
}

void UserMessage::paintEvent(QPaintEvent*) {
    const UserMessageRecipe r = userMessageRecipe(theme());
    qreal textWidth = 0;
    const QRectF bubble = bubbleFor(width(), &textWidth);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(r.background);
    painter.drawRoundedRect(bubble, radius, radius);
    detail::Paragraph paragraph(text_, bubbleFont(), r.lineHeight);
    paragraph.layout(textWidth);
    painter.setPen(r.text);
    paragraph.draw(painter,
                   bubble.topLeft() + QPointF(scaled(r.paddingInline), scaled(r.paddingBlock)));
}

AssistantMessage::AssistantMessage(const QString& name, const QString& time, QWidget* parent)
    : QWidget(parent), avatar_(new Avatar(this)), name_(new QLabel(name)), time_(new QLabel(time)),
      column_(new QVBoxLayout) {
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    row->addWidget(avatar_, 0, Qt::AlignTop);
    row->addLayout(column_, 1);
    auto* meta = new QHBoxLayout;
    meta->setContentsMargins(0, 0, 0, 0);
    meta->addWidget(name_);
    meta->addWidget(time_);
    meta->addStretch(1);
    column_->setContentsMargins(0, 0, 0, 0);
    column_->setSpacing(0);
    column_->addLayout(meta);
    time_->setVisible(!time.isEmpty());
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            &AssistantMessage::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this,
            &AssistantMessage::applyRecipe);
    applyRecipe();
}

void AssistantMessage::setName(const QString& name) { name_->setText(name); }

void AssistantMessage::setTime(const QString& time) {
    time_->setText(time);
    time_->setVisible(!time.isEmpty());
}

void AssistantMessage::setAvatar(const QIcon& avatar) {
    static_cast<Avatar*>(avatar_)->setIcon(avatar);
}

void AssistantMessage::setBody(QWidget* body) {
    if (body_)
        body_->deleteLater();
    body_ = body;
    if (body_)
        column_->addWidget(body_);
}

void AssistantMessage::setText(const QString& text) {
    if (auto* reading = qobject_cast<ReadingText*>(body_)) {
        reading->setText(text);
        return;
    }
    setBody(new ReadingText(text));
}

void AssistantMessage::applyRecipe() {
    const AssistantMessageRecipe r = assistantMessageRecipe(theme());
    auto* row = static_cast<QHBoxLayout*>(layout());
    row->setSpacing(qRound(scaled(r.spacing)));
    row->setContentsMargins(0, 0, 0, qRound(scaled(r.spacingAfter)));
    auto* meta = static_cast<QHBoxLayout*>(column_->itemAt(0)->layout());
    meta->setSpacing(qRound(scaled(r.metaGap)));
    meta->setContentsMargins(0, 0, 0, qRound(scaled(r.metaBottom)));
    name_->setFont(sized(this, r.namePixels, r.nameWeight));
    time_->setFont(sized(this, r.timePixels));
    for (const auto& [label, color] : {std::pair{name_, r.name}, std::pair{time_, r.time}}) {
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, color);
        label->setPalette(palette);
    }
    avatar_->setFixedSize(avatar_->sizeHint());
    avatar_->update();
}

DayDivider::DayDivider(const QString& text, QWidget* parent) : QWidget(parent), text_(text) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    followAppearance(this);
}

void DayDivider::setText(const QString& text) {
    text_ = text;
    update();
}

QFont DayDivider::dividerFont() const { return sized(this, dayDividerRecipe(theme()).fontPixels); }

QSize DayDivider::sizeHint() const {
    const DayDividerRecipe r = dayDividerRecipe(theme());
    const QFontMetricsF metrics(dividerFont());
    return {qCeil(metrics.horizontalAdvance(text_) + 4 * scaled(r.spacing)),
            qCeil(metrics.height() + scaled(r.marginTop) + scaled(r.marginBottom))};
}

void DayDivider::paintEvent(QPaintEvent*) {
    const DayDividerRecipe r = dayDividerRecipe(theme());
    const QFont font = dividerFont();
    const QFontMetricsF metrics(font);
    const qreal textWidth = metrics.horizontalAdvance(text_);
    const qreal top = scaled(r.marginTop);
    const qreal centre = top + metrics.height() / 2;
    const qreal gap = text_.isEmpty() ? 0 : scaled(r.spacing);
    const qreal textLeft = (width() - textWidth) / 2;
    QPainter painter(this);
    painter.setPen(QPen(r.line, 1));
    const qreal y = qFloor(centre) + 0.5;
    painter.drawLine(QPointF(0, y), QPointF(textLeft - gap, y));
    painter.drawLine(QPointF(textLeft + textWidth + gap, y), QPointF(width(), y));
    painter.setFont(font);
    painter.setPen(r.text);
    painter.drawText(QRectF(textLeft, top, textWidth, metrics.height()), Qt::AlignCenter, text_);
}

} // namespace ket
