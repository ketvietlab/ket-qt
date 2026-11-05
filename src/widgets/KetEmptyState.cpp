// SPDX-License-Identifier: MPL-2.0
#include "KetEmptyState.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcon.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QLabel>

namespace ket {

EmptyState::EmptyState(const QString& iconCode, const QString& title, const QString& message,
                       QWidget* parent)
    : QWidget(parent), icon_(new Icon(iconCode)), title_(new QLabel(title)), message_(new QLabel),
      messageText_(message), layout_(new QVBoxLayout(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->addStretch(1);
    layout_->addWidget(icon_, 0, Qt::AlignHCenter);
    layout_->addWidget(title_, 0, Qt::AlignHCenter);
    // Centred between stretches, so the wrapped label still gets its height for width.
    auto* messageRow = new QHBoxLayout;
    messageRow->addStretch(1);
    messageRow->addWidget(message_, 100);
    messageRow->addStretch(1);
    layout_->addLayout(messageRow);
    layout_->addStretch(1);
    title_->setTextFormat(Qt::PlainText);
    // QLabel has no line height; the message is rich text that carries the recipe's.
    message_->setTextFormat(Qt::RichText);
    for (QLabel* label : {title_, message_}) {
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignHCenter);
    }
    message_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    icon_->setVisible(!iconCode.isEmpty());
    title_->setVisible(!title.isEmpty());
    message_->setVisible(!message.isEmpty());
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &EmptyState::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &EmptyState::applyRecipe);
    applyRecipe();
}

void EmptyState::setIconCode(const QString& code) {
    icon_->setCode(code);
    icon_->setVisible(!code.isEmpty());
}

QString EmptyState::title() const { return title_->text(); }

void EmptyState::setTitle(const QString& title) {
    title_->setText(title);
    title_->setVisible(!title.isEmpty());
}

void EmptyState::setMessage(const QString& message) {
    messageText_ = message;
    message_->setVisible(!message.isEmpty());
    renderMessage();
}

void EmptyState::renderMessage() {
    const EmptyStateRecipe r = emptyStateRecipe(Appearance::instance().theme());
    message_->setText(QStringLiteral("<div align=\"center\" style=\"line-height:%1%\">%2</div>")
                          .arg(qRound(r.lineHeight * 100))
                          .arg(messageText_.toHtmlEscaped()));
}

Button* EmptyState::setAction(const QString& text, const QString& iconCode) {
    if (!action_) {
        action_ = new Button(text, ButtonVariant::Secondary, this);
        // Before the trailing stretch.
        layout_->insertWidget(layout_->count() - 1, action_, 0, Qt::AlignHCenter);
    }
    action_->setText(text);
    action_->setIcon(iconCode.isEmpty() ? QIcon() : Icons::icon(iconCode));
    return action_;
}

void EmptyState::applyRecipe() {
    const EmptyStateRecipe r = emptyStateRecipe(Appearance::instance().theme());
    const Appearance& a = Appearance::instance();
    layout_->setSpacing(qRound(a.scaled(r.spacing)));
    icon_->setIconSize(r.iconSize);
    icon_->setColor(r.icon);

    QFont titleFont = font();
    titleFont.setPixelSize(qRound(a.scaled(r.titlePixels)));
    titleFont.setWeight(QFont::Weight(r.titleWeight));
    title_->setFont(titleFont);
    QFont messageFont = font();
    messageFont.setPixelSize(qRound(a.scaled(r.messagePixels)));
    message_->setFont(messageFont);
    message_->setMaximumWidth(qRound(a.scaled(r.messageMaxWidth)));
    renderMessage();
    for (const auto& [label, color] :
         {std::pair{title_, r.title}, std::pair{message_, r.message}}) {
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, color);
        label->setPalette(palette);
    }
}

} // namespace ket
