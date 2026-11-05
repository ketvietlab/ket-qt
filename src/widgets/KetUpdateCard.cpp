// SPDX-License-Identifier: MPL-2.0
#include "KetUpdateCard.h"

#include "KetAppearance.h"
#include "KetIcons.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

void styleLabel(QLabel* label, qreal pixels, int weight, const QColor& color) {
    QFont font = label->font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    label->setFont(font);
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, color);
    label->setPalette(palette);
}

} // namespace

UpdateCard::UpdateCard(const QString& title, QWidget* parent)
    : QWidget(parent), title_(new QLabel(title, this)), header_(new QHBoxLayout),
      detail_(new QLabel(this)), progressHost_(new QWidget(this)),
      progress_(new QProgressBar(progressHost_)), notesHost_(new QWidget(this)),
      actionsHost_(new QWidget(this)), actions_(new QHBoxLayout(actionsHost_)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    header_->setContentsMargins(0, 0, 0, 0);
    header_->addWidget(title_);
    header_->addStretch(1);
    detail_->setWordWrap(true);
    detail_->hide();
    progress_->setTextVisible(false);
    (new QVBoxLayout(progressHost_))->addWidget(progress_);
    progressHost_->hide();
    auto* notes = new QVBoxLayout(notesHost_);
    notes->setSpacing(0);
    notesHost_->hide();
    actions_->addStretch(1);
    actionsHost_->hide();
    auto* column = new QVBoxLayout(this);
    column->setSpacing(0);
    column->addLayout(header_);
    column->addWidget(detail_);
    column->addWidget(progressHost_);
    column->addWidget(notesHost_);
    column->addWidget(actionsHost_);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &UpdateCard::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &UpdateCard::applyRecipe);
    applyRecipe();
}

void UpdateCard::setTitle(const QString& title) { title_->setText(title); }

void UpdateCard::setDetail(const QString& detail) {
    detail_->setText(detail);
    detail_->setVisible(!detail.isEmpty());
}

void UpdateCard::setNotes(const QStringList& notes) {
    for (QLabel* label : notesHost_->findChildren<QLabel*>(Qt::FindDirectChildrenOnly))
        delete label;
    for (const QString& note : notes) {
        auto* label = new QLabel(QStringLiteral("•  ") + note, notesHost_);
        label->setWordWrap(true);
        notesHost_->layout()->addWidget(label);
    }
    notesHost_->setVisible(!notes.isEmpty());
    applyRecipe();
}

void UpdateCard::setProgress(int percent) {
    progressHost_->setVisible(percent >= 0);
    progress_->setValue(qBound(0, percent, 100));
}

void UpdateCard::setReady(bool ready) {
    ready_ = ready;
    update();
}

void UpdateCard::addBadge(QWidget* badge) {
    badge->setParent(this);
    header_->insertWidget(header_->count() - 1, badge);
}

void UpdateCard::addAction(QWidget* action) {
    action->setParent(actionsHost_);
    actions_->insertWidget(actions_->count() - 1, action);
    actionsHost_->show();
}

void UpdateCard::applyRecipe() {
    const UpdateCardRecipe r = updateCardRecipe(ready_, Appearance::instance().theme());
    const int block = qRound(scaled(r.paddingBlock)) + 1;
    const int inline_ = qRound(scaled(r.paddingInline)) + 1;
    layout()->setContentsMargins(inline_, block, inline_, block);
    // `header` wraps the detail to its own row: 10 px between items, 6 px between rows.
    header_->setSpacing(qRound(scaled(r.headerColumnGap)));
    styleLabel(title_, r.titlePixels, r.titleWeight, r.title);
    styleLabel(detail_, r.detailPixels, QFont::Normal, r.detail);
    detail_->setContentsMargins(0, qRound(scaled(r.headerGap)), 0, 0);
    progressHost_->layout()->setContentsMargins(0, qRound(scaled(r.sectionGap)), 0, 0);
    // Each note is a bulleted line at the mock's line height.
    notesHost_->layout()->setContentsMargins(qRound(scaled(r.notesIndent)),
                                             qRound(scaled(Tokens::px(u"space-3"))), 0, 0);
    for (QLabel* label : notesHost_->findChildren<QLabel*>(Qt::FindDirectChildrenOnly)) {
        styleLabel(label, r.notesPixels, QFont::Normal, r.notes);
        const qreal leading =
            scaled(r.notesPixels) * r.notesLineHeight - QFontMetricsF(label->font()).height();
        const int half = qMax(0, qRound(leading / 2));
        label->setContentsMargins(0, half, 0, half);
    }
    actions_->setSpacing(qRound(scaled(r.actionsGap)));
    actions_->setContentsMargins(0, qRound(scaled(r.actionsTop)), 0, 0);
    update();
}

void UpdateCard::paintEvent(QPaintEvent*) {
    const UpdateCardRecipe r = updateCardRecipe(ready_, Appearance::instance().theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

UpdatePill::UpdatePill(const QString& title, const QString& detail, QWidget* parent)
    : QAbstractButton(parent), detail_(detail) {
    setText(title);
    setAccessibleDescription(detail);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void UpdatePill::setDetail(const QString& detail) {
    detail_ = detail;
    setAccessibleDescription(detail);
    updateGeometry();
    update();
}

void UpdatePill::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    update();
}

UpdatePillRecipe UpdatePill::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()});
    return updatePillRecipe(state, Appearance::instance().theme());
}

QSize UpdatePill::sizeHint() const {
    const UpdatePillRecipe r = recipe();
    QFont title = font();
    title.setPixelSize(qRound(scaled(r.titlePixels)));
    title.setWeight(QFont::Weight(r.titleWeight));
    QFont small = font();
    small.setPixelSize(qRound(scaled(r.detailPixels)));
    const qreal height =
        QFontMetricsF(title).height() + (detail_.isEmpty() ? 0 : QFontMetricsF(small).height());
    const qreal width = 2 * scaled(r.paddingInline) + scaled(r.iconSize) + scaled(r.spacing) +
                        QFontMetricsF(title).horizontalAdvance(text());
    return {qCeil(width) + 2, qCeil(2 * scaled(r.paddingBlock) + height) + 2};
}

void UpdatePill::paintEvent(QPaintEvent*) {
    const UpdatePillRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    painter.setPen(QPen(r.focusRing.alpha() ? r.focusRing : r.border, r.focusRing.alpha() ? 2 : 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    const qreal icon = scaled(r.iconSize);
    qreal x = 1 + scaled(r.paddingInline);
    painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                       Icons::pixmap(u"download", icon, devicePixelRatioF(), r.title));
    x += icon + scaled(r.spacing);
    QFont title = font();
    title.setPixelSize(qRound(scaled(r.titlePixels)));
    title.setWeight(QFont::Weight(r.titleWeight));
    QFont small = font();
    small.setPixelSize(qRound(scaled(r.detailPixels)));
    const QFontMetricsF titleMetrics(title);
    const QFontMetricsF smallMetrics(small);
    const qreal textHeight =
        titleMetrics.height() + (detail_.isEmpty() ? 0 : smallMetrics.height());
    qreal y = (height() - textHeight) / 2;
    const qreal available = width() - x - scaled(r.paddingInline) - 1;
    painter.setFont(title);
    painter.setPen(r.title);
    painter.drawText(QRectF(x, y, available, titleMetrics.height()), Qt::AlignLeft,
                     titleMetrics.elidedText(text(), Qt::ElideRight, available));
    if (!detail_.isEmpty()) {
        y += titleMetrics.height();
        painter.setFont(small);
        painter.setPen(r.detail);
        painter.drawText(QRectF(x, y, available, smallMetrics.height()), Qt::AlignLeft,
                         smallMetrics.elidedText(detail_, Qt::ElideRight, available));
    }
}

} // namespace ket
