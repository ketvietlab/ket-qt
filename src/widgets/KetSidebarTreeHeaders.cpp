// SPDX-License-Identifier: MPL-2.0
// The sidebar tree's repository header, section label and guide.
#include "KetSidebarTree.h"

#include "KetBadge.h"
#include "KetButton.h"
#include "KetSidebarTreeDetail.h"

#include <QBoxLayout>
#include <QEvent>
#include <QtMath>

namespace ket {

using namespace sidebar_tree;

// SidebarRepositoryHeader

SidebarRepositoryHeader::SidebarRepositoryHeader(const QString& name, QWidget* parent)
    : QAbstractButton(parent),
      action_(new IconButton(Icons::icon(u"plus"), tr("New conversation"), this)),
      count_(new Badge({}, BadgeTone::Neutral, this)) {
    setText(name);
    setCheckable(true);
    setChecked(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    action_->setControlSize(ControlSize::Small);
    action_->installEventFilter(this);
    retainWhenHidden(action_);
    count_->setAttribute(Qt::WA_TransparentForMouseEvents);
    count_->setVisible(false);
    connect(action_, &QAbstractButton::clicked, this, &SidebarRepositoryHeader::actionTriggered);
    connect(this, &QAbstractButton::toggled, this, [this] {
        updateDescription();
        layoutTrailing();
        update();
    });
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            &SidebarRepositoryHeader::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this,
            &SidebarRepositoryHeader::applyRecipe);
    applyRecipe();
    updateDescription();
}

void SidebarRepositoryHeader::setConversationCount(int count) {
    conversations_ = qMax(0, count);
    count_->setText(QString::number(conversations_));
    count_->setVisible(true);
    updateDescription();
    layoutTrailing();
}

void SidebarRepositoryHeader::setWaitingCount(int count) {
    waiting_ = qMax(0, count);
    updateDescription();
    layoutTrailing();
    update();
}

bool SidebarRepositoryHeader::isWaitingDotShown() const { return waiting_ > 0 && !isExpanded(); }

void SidebarRepositoryHeader::setSelected(bool selected) {
    selected_ = selected;
    update();
}

void SidebarRepositoryHeader::setDivided(bool divided) {
    divided_ = divided;
    applyRecipe();
}

void SidebarRepositoryHeader::setPreviewState(std::optional<InteractionState> state) {
    preview_ = state;
    layoutTrailing();
    update();
}

SidebarRepositoryRecipe SidebarRepositoryHeader::recipe() const {
    const InteractionState state =
        preview_.value_or(InteractionState{hovered_, isDown(), isEnabled(), hasFocus()});
    return sidebarRepositoryRecipe(selected_, state, theme());
}

SidebarTreeColumns SidebarRepositoryHeader::columns() const {
    return sidebarTreeColumns(0, Appearance::instance().zoom());
}

QRect SidebarRepositoryHeader::rowRect() const {
    const int lead = divided_ ? qRound(scaled(recipe().dividerLead)) : 0;
    return rect().adjusted(0, lead, 0, 0);
}

QFont SidebarRepositoryHeader::nameFont() const {
    const SidebarRepositoryRecipe r = recipe();
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.namePixels)));
    font.setWeight(QFont::Weight(r.nameWeight));
    font.setCapitalization(QFont::AllUppercase);
    font.setLetterSpacing(QFont::AbsoluteSpacing, r.letterSpacing * Appearance::instance().zoom());
    return font;
}

QRect SidebarRepositoryHeader::markRect() const {
    return squareAt(columns().mark, rowRect(), scaled(recipe().iconSize));
}

bool SidebarRepositoryHeader::hot() const {
    return preview_ ? preview_->hovered || preview_->focused
                    : hovered_ || hasFocus() || action_->hasFocus();
}

QRect SidebarRepositoryHeader::nameRect() const {
    const SidebarRepositoryRecipe r = recipe();
    const QRect row = rowRect();
    const int gap = qRound(scaled(r.trailingGap));
    // The name ends before the first trailing part shown: the action, the dot or the count.
    int right = width() - qRound(scaled(r.trailingEnd));
    if (!count_->isHidden())
        right = count_->x() - gap;
    if (isWaitingDotShown())
        right -= qRound(scaled(r.dotSize)) + gap;
    if (hot())
        right = action_->x() - gap;
    const int left = qRound(columns().text);
    return {left, row.top(), qMax(0, right - left), row.height()};
}

QSize SidebarRepositoryHeader::sizeHint() const {
    const SidebarRepositoryRecipe r = recipe();
    const int lead = divided_ ? qRound(scaled(r.dividerLead)) : 0;
    return {qCeil(columns().text + QFontMetricsF(nameFont()).horizontalAdvance(text())) +
                action_->sizeHint().width() + count_->sizeHint().width() +
                qRound(scaled(r.trailingEnd)),
            lead + qRound(scaled(r.height))};
}

QSize SidebarRepositoryHeader::minimumSizeHint() const {
    return {qRound(columns().text) * 2, sizeHint().height()};
}

void SidebarRepositoryHeader::applyRecipe() {
    const SidebarRepositoryRecipe r = recipe();
    const int lead = divided_ ? qRound(scaled(r.dividerLead)) : 0;
    setFixedHeight(lead + qRound(scaled(r.height)));
    updateGeometry();
    layoutTrailing();
    update();
}

void SidebarRepositoryHeader::layoutTrailing() {
    const SidebarRepositoryRecipe r = recipe();
    const QRect row = rowRect();
    const int gap = qRound(scaled(r.trailingGap));
    int right = width() - qRound(scaled(r.trailingEnd));
    if (!count_->isHidden()) {
        const QSize size = count_->sizeHint();
        count_->setGeometry(right - size.width(), row.top() + (row.height() - size.height()) / 2,
                            size.width(), size.height());
        right = count_->x() - gap;
    }
    if (isWaitingDotShown())
        right -= qRound(scaled(r.dotSize)) + gap;
    const QSize action = action_->sizeHint();
    action_->setGeometry(right - action.width(), row.top() + (row.height() - action.height()) / 2,
                         action.width(), action.height());
    action_->setVisible(hot());
}

void SidebarRepositoryHeader::updateDescription() {
    setAccessibleName(text());
    setAccessibleDescription(sidebarRepositoryDescription(isExpanded(), conversations_, waiting_));
}

void SidebarRepositoryHeader::paintEvent(QPaintEvent*) {
    const SidebarRepositoryRecipe r = recipe();
    const SidebarTreeColumns c = columns();
    const QRect row = rowRect();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(r.opacity);
    const qreal radius = scaled(r.radius);
    if (r.background.alpha()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(QRectF(row), radius, radius);
    }
    paintFocus(painter, QRectF(row), r.focusRing, radius);
    drawIcon(painter, this, sidebarChevronIcon(isExpanded()),
             squareAt(c.chevron, row, scaled(r.chevronSize)), r.chevron);
    drawIcon(painter, this, isExpanded() ? QStringLiteral("folder-open") : QStringLiteral("folder"),
             markRect(), r.icon);
    const QRect name = nameRect();
    const QFont font = nameFont();
    painter.setFont(font);
    painter.setPen(r.name);
    painter.drawText(name, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(font).elidedText(text(), Qt::ElideMiddle, name.width()));
    if (isWaitingDotShown()) {
        const qreal dot = scaled(r.dotSize);
        const qreal right = (!count_->isHidden() ? count_->x() : width() - scaled(r.trailingEnd)) -
                            scaled(r.trailingGap);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.dot);
        painter.drawEllipse(QRectF(right - dot, row.center().y() + 0.5 - dot / 2, dot, dot));
    }
}

void SidebarRepositoryHeader::keyPressEvent(QKeyEvent* event) {
    if (activates(event) && !event->isAutoRepeat()) {
        click();
        return;
    }
    QAbstractButton::keyPressEvent(event);
}

void SidebarRepositoryHeader::resizeEvent(QResizeEvent* event) {
    QAbstractButton::resizeEvent(event);
    layoutTrailing();
}

void SidebarRepositoryHeader::enterEvent(QEnterEvent* event) {
    hovered_ = true;
    layoutTrailing();
    update();
    QAbstractButton::enterEvent(event);
}

void SidebarRepositoryHeader::leaveEvent(QEvent* event) {
    hovered_ = false;
    layoutTrailing();
    update();
    QAbstractButton::leaveEvent(event);
}

bool SidebarRepositoryHeader::event(QEvent* event) {
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)
        QMetaObject::invokeMethod(this, &SidebarRepositoryHeader::layoutTrailing,
                                  Qt::QueuedConnection);
    return QAbstractButton::event(event);
}

bool SidebarRepositoryHeader::eventFilter(QObject* watched, QEvent* event) {
    if (watched == action_ &&
        (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut))
        // Queued: Tab from the header moves focus to the action after the header's FocusOut.
        QMetaObject::invokeMethod(this, &SidebarRepositoryHeader::layoutTrailing,
                                  Qt::QueuedConnection);
    return QAbstractButton::eventFilter(watched, event);
}

// SidebarSectionLabel

SidebarSectionLabel::SidebarSectionLabel(const QString& text, QWidget* parent)
    : QWidget(parent), action_(new IconButton(Icons::icon(u"plus"), tr("New conversation"), this)),
      text_(text) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    action_->setControlSize(ControlSize::Small);
    connect(action_, &QAbstractButton::clicked, this, &SidebarSectionLabel::actionTriggered);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this,
            &SidebarSectionLabel::applyRecipe);
    applyRecipe();
    updateAccessibility();
}

void SidebarSectionLabel::setText(const QString& text) {
    text_ = text;
    updateAccessibility();
    update();
}

void SidebarSectionLabel::setCount(int count) {
    count_ = count;
    updateAccessibility();
    update();
}

QString SidebarSectionLabel::displayText() const {
    return count_ < 0 ? text_ : QStringLiteral("%1 (%2)").arg(text_).arg(count_);
}

void SidebarSectionLabel::setLevel(int level) {
    level_ = qMax(0, level);
    update();
}

SidebarSectionRecipe SidebarSectionLabel::recipe() const { return sidebarSectionRecipe(theme()); }

SidebarTreeColumns SidebarSectionLabel::columns() const {
    return sidebarTreeColumns(level_, Appearance::instance().zoom());
}

QFont SidebarSectionLabel::textFont() const {
    const SidebarSectionRecipe r = recipe();
    QFont font = this->font();
    font.setPixelSize(qRound(scaled(r.pixels)));
    font.setWeight(QFont::Weight(r.weight));
    return font;
}

QRect SidebarSectionLabel::markRect() const {
    return squareAt(columns().mark, rect(), scaled(recipe().iconSize));
}

QRect SidebarSectionLabel::textRect() const {
    const int left = qRound(columns().text);
    const int right = action_->x() - qRound(scaled(recipe().actionEnd));
    return {left, 0, qMax(0, right - left), height()};
}

QSize SidebarSectionLabel::sizeHint() const {
    return {qCeil(columns().text + QFontMetricsF(textFont()).horizontalAdvance(displayText())) +
                action_->sizeHint().width() + qRound(scaled(recipe().actionEnd)) * 2,
            qRound(scaled(recipe().height))};
}

QSize SidebarSectionLabel::minimumSizeHint() const {
    return {qRound(columns().text) * 2, sizeHint().height()};
}

void SidebarSectionLabel::applyRecipe() {
    setFixedHeight(qRound(scaled(recipe().height)));
    updateGeometry();
    update();
}

void SidebarSectionLabel::updateAccessibility() {
    action_->setLabel(tr("New conversation"));
    setAccessibleName(count_ < 0 ? text_ : QStringLiteral("%1, %2").arg(text_).arg(count_));
}

void SidebarSectionLabel::paintEvent(QPaintEvent*) {
    const SidebarSectionRecipe r = recipe();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawIcon(painter, this, r.icon, markRect(), r.iconColor);
    const QRect box = textRect();
    const QFont font = textFont();
    painter.setFont(font);
    painter.setPen(r.text);
    painter.drawText(box, Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(font).elidedText(displayText(), Qt::ElideRight, box.width()));
}

void SidebarSectionLabel::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    const QSize size = action_->sizeHint();
    action_->setGeometry(width() - qRound(scaled(recipe().actionEnd)) - size.width(),
                         (height() - size.height()) / 2, size.width(), size.height());
}

// SidebarTreeGuide

SidebarTreeGuide::SidebarTreeGuide(int level, QWidget* parent) : QWidget(parent), level_(level) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* parts = new QVBoxLayout(this);
    parts->setSpacing(0);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this,
            &SidebarTreeGuide::applyRecipe);
    applyRecipe();
}

void SidebarTreeGuide::setLevel(int level) {
    level_ = qMax(0, level);
    applyRecipe();
}

void SidebarTreeGuide::applyRecipe() {
    // A team's members keep a little space below them, before the next part.
    const int gap = level_ > 0 ? qRound(scaled(sidebarTreeGuideRecipe(theme()).branchGap)) : 0;
    partsLayout()->setContentsMargins(0, 0, 0, gap);
    update();
}

void SidebarTreeGuide::addPart(QWidget* part) { partsLayout()->addWidget(part); }

QVBoxLayout* SidebarTreeGuide::partsLayout() const { return static_cast<QVBoxLayout*>(layout()); }

QRect SidebarTreeGuide::guideRect() const {
    const SidebarTreeColumns c = sidebarTreeColumns(level_, Appearance::instance().zoom());
    const int width = qMax(1, qRound(sidebarTreeGuideRecipe(theme()).width));
    return {qRound(c.guide), 0, width, height() - partsLayout()->contentsMargins().bottom()};
}

void SidebarTreeGuide::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(guideRect(), sidebarTreeGuideRecipe(theme()).color);
}

} // namespace ket
