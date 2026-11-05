// SPDX-License-Identifier: MPL-2.0
#include "KetBanner.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcon.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>

namespace ket {

Banner::Banner(BannerTone tone, const QString& title, const QString& message, QWidget* parent)
    : QWidget(parent), tone_(tone), icon_(new Icon), title_(new QLabel(title)),
      message_(new QLabel(message)), layout_(new QHBoxLayout(this)) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setAttribute(Qt::WA_StyledBackground, false);
    for (QLabel* label : {title_, message_}) {
        label->setWordWrap(true);
        label->setTextFormat(Qt::PlainText);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    }
    message_->setVisible(!message.isEmpty());

    auto* text = new QVBoxLayout;
    text->setContentsMargins(0, 0, 0, 0);
    text->addWidget(title_);
    text->addWidget(message_);
    layout_->addWidget(icon_, 0, Qt::AlignTop);
    layout_->addLayout(text, 1);

    connect(&Appearance::instance(), &Appearance::themeChanged, this, &Banner::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Banner::applyRecipe);
    applyRecipe();
}

void Banner::setTone(BannerTone tone) {
    if (tone_ == tone)
        return;
    tone_ = tone;
    applyRecipe();
}

QString Banner::title() const { return title_->text(); }

void Banner::setTitle(const QString& title) { title_->setText(title); }

QString Banner::message() const { return message_->text(); }

void Banner::setMessage(const QString& message) {
    message_->setText(message);
    message_->setVisible(!message.isEmpty());
}

Button* Banner::setAction(const QString& text, const QString& iconCode) {
    if (!action_) {
        action_ = new Button(text, ButtonVariant::Secondary, this);
        action_->setControlSize(ControlSize::Small);
        layout_->addWidget(action_, 0, Qt::AlignVCenter);
    }
    action_->setText(text);
    action_->setIcon(iconCode.isEmpty() ? QIcon() : Icons::icon(iconCode));
    return action_;
}

BannerRecipe Banner::recipe() const { return bannerRecipe(tone_, Appearance::instance().theme()); }

void Banner::applyRecipe() {
    const BannerRecipe r = recipe();
    const Appearance& appearance = Appearance::instance();
    const int inline_ = qRound(appearance.scaled(r.paddingInline));
    const int block = qRound(appearance.scaled(r.paddingBlock));
    layout_->setContentsMargins(inline_, block, inline_, block);
    layout_->setSpacing(qRound(appearance.scaled(r.spacing)));
    if (auto* text = qobject_cast<QVBoxLayout*>(layout_->itemAt(1)->layout()))
        text->setSpacing(qRound(appearance.scaled(r.messageGap)));

    icon_->setCode(r.icon);
    icon_->setIconSize(r.iconSize);
    icon_->setColor(r.text);

    QFont titleFont = font();
    titleFont.setPixelSize(qRound(appearance.scaled(r.titlePixels)));
    titleFont.setWeight(QFont::Weight(r.titleWeight));
    title_->setFont(titleFont);
    QFont messageFont = font();
    messageFont.setPixelSize(qRound(appearance.scaled(r.messagePixels)));
    message_->setFont(messageFont);
    for (QLabel* label : {title_, message_}) {
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, r.text);
        label->setPalette(palette);
    }
    update();
}

void Banner::paintEvent(QPaintEvent*) {
    const BannerRecipe r = recipe();
    const qreal radius = Appearance::instance().scaled(r.radius);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

} // namespace ket
