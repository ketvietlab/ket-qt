// SPDX-License-Identifier: MPL-2.0
#include "KetTaskBar.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

QString statusIcon(TaskStatus status) {
    switch (status) {
    case TaskStatus::Running:
    case TaskStatus::Stopping:
        return QStringLiteral("loading");
    case TaskStatus::Completed:
        return QStringLiteral("success");
    case TaskStatus::Failed:
        return QStringLiteral("error");
    case TaskStatus::Stopped:
        break;
    }
    return QStringLiteral("stop-circle");
}

// `.bg-bar-header`: the task count with a chevron.
class Toggle final : public QAbstractButton {
  public:
    explicit Toggle(QWidget* parent) : QAbstractButton(parent) {
        setCheckable(true);
        setChecked(true);
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    [[nodiscard]] TaskBarRecipe recipe() const {
        return taskBarRecipe(InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()},
                             Appearance::instance().theme());
    }

    [[nodiscard]] QFont textFont() const {
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(recipe().fontPixels)));
        return font;
    }

    [[nodiscard]] QSize sizeHint() const override {
        const TaskBarRecipe r = recipe();
        return {qCeil(2 * scaled(r.paddingInline) + scaled(r.iconSize) + scaled(r.chevronSize) +
                      2 * scaled(r.spacing) + QFontMetricsF(textFont()).horizontalAdvance(text())),
                qRound(scaled(r.height))};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const TaskBarRecipe r = recipe();
        const qreal dpr = devicePixelRatioF();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        const qreal radius = scaled(r.radius);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(QRectF(rect()), radius, radius);
        if (r.focusRing.alpha()) {
            painter.setPen(QPen(r.focusRing, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), radius, radius);
        }
        qreal x = scaled(r.paddingInline);
        const qreal icon = scaled(r.iconSize);
        painter.drawPixmap(QPointF(qRound(x), qRound((height() - icon) / 2)),
                           Icons::pixmap(u"loading", icon, dpr, r.text));
        x += icon + scaled(r.spacing);
        const QFont font = textFont();
        const qreal textWidth = QFontMetricsF(font).horizontalAdvance(text());
        painter.setFont(font);
        painter.setPen(r.text);
        painter.drawText(QRectF(x, 0, textWidth + 1, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         text());
        x += textWidth + scaled(r.spacing);
        const qreal chevron = scaled(r.chevronSize);
        painter.drawPixmap(
            QPointF(qRound(x), qRound((height() - chevron) / 2)),
            Icons::pixmap(isChecked() ? u"chevron-down" : u"chevron-up", chevron, dpr, r.text));
    }
    void enterEvent(QEnterEvent* event) override {
        update();
        QAbstractButton::enterEvent(event);
    }
    void leaveEvent(QEvent* event) override {
        update();
        QAbstractButton::leaveEvent(event);
    }
};

} // namespace

TaskRow::TaskRow(const QString& title, const QString& kindIcon, QWidget* parent)
    : QWidget(parent), title_(title), kindIcon_(kindIcon),
      stop_(new IconButton(Icons::icon(u"stop"), tr("Stop %1").arg(title), this)),
      dismiss_(new IconButton(Icons::icon(u"close"), tr("Dismiss"), this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAccessibleName(title);
    for (IconButton* button : {stop_, dismiss_})
        button->setControlSize(ControlSize::Small);
    connect(stop_, &QAbstractButton::clicked, this, &TaskRow::stopRequested);
    connect(dismiss_, &QAbstractButton::clicked, this, &TaskRow::dismissRequested);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &TaskRow::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &TaskRow::applyRecipe);
    applyRecipe();
    setStatus(TaskStatus::Running);
}

void TaskRow::setTitle(const QString& title) {
    title_ = title;
    setAccessibleName(title);
    stop_->setLabel(tr("Stop %1").arg(title));
    update();
}

void TaskRow::setMeta(const QString& meta) {
    meta_ = meta;
    setAccessibleDescription(meta);
    update();
}

void TaskRow::setStatus(TaskStatus status) {
    status_ = status;
    const bool live = status == TaskStatus::Running || status == TaskStatus::Stopping;
    stop_->setVisible(live);
    stop_->setEnabled(status == TaskStatus::Running);
    dismiss_->setVisible(!live);
    place();
    update();
}

void TaskRow::setCurrent(bool current) {
    current_ = current;
    update();
}

TaskRowRecipe TaskRow::recipe() const {
    return taskRowRecipe(status_, current_, Appearance::instance().theme());
}

QSize TaskRow::sizeHint() const {
    const TaskRowRecipe r = recipe();
    return {qRound(scaled(Tokens::px(u"space-16")) * 5), qRound(scaled(r.minHeight))};
}

void TaskRow::applyRecipe() {
    const int action = qRound(scaled(recipe().actionSize));
    for (IconButton* button : {stop_, dismiss_})
        button->setFixedSize(action, action);
    updateGeometry();
    place();
    update();
}

void TaskRow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    place();
}

void TaskRow::place() {
    const TaskRowRecipe r = recipe();
    const int right = width() - qRound(scaled(r.paddingEnd));
    for (IconButton* button : {stop_, dismiss_})
        button->move(right - button->width(), (height() - button->height()) / 2);
}

qreal TaskRow::textLeft() const {
    const TaskRowRecipe r = recipe();
    return scaled(r.paddingStart) + 2 * (scaled(r.iconSize) + scaled(r.spacing));
}

void TaskRow::paintEvent(QPaintEvent*) {
    const TaskRowRecipe r = recipe();
    const qreal dpr = devicePixelRatioF();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    qreal x = scaled(r.paddingStart);
    const qreal icon = scaled(r.iconSize);
    const qreal y = qRound((height() - icon) / 2);
    painter.drawPixmap(QPointF(qRound(x), y),
                       Icons::pixmap(statusIcon(status_), icon, dpr, r.status));
    x += icon + scaled(r.spacing);
    painter.drawPixmap(QPointF(qRound(x), y), Icons::pixmap(kindIcon_, icon, dpr, r.kind));
    x = textLeft();
    const qreal right = (stop_->isVisible() ? stop_ : dismiss_)->x() - scaled(r.spacing);
    QFont title = font();
    title.setPixelSize(qRound(scaled(r.titlePixels)));
    const QFontMetricsF titleMetrics(title);
    // `.bg-title` keeps its width; the meta line takes what is left.
    const qreal titleWidth = qMin(titleMetrics.horizontalAdvance(title_), right - x);
    painter.setFont(title);
    painter.setPen(r.title);
    painter.drawText(QRectF(x, 0, titleWidth, height()), Qt::AlignLeft | Qt::AlignVCenter,
                     titleMetrics.elidedText(title_, Qt::ElideRight, titleWidth));
    x += titleWidth + scaled(r.spacing);
    if (!meta_.isEmpty() && x < right) {
        QFont meta = font();
        meta.setPixelSize(qRound(scaled(r.metaPixels)));
        painter.setFont(meta);
        painter.setPen(r.meta);
        painter.drawText(QRectF(x, 0, right - x, height()), Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetricsF(meta).elidedText(meta_, Qt::ElideRight, right - x));
    }
}

TaskBar::TaskBar(QWidget* parent)
    : QWidget(parent), toggle_(new Toggle(this)), rowsHost_(new QWidget(this)),
      rows_(new QVBoxLayout(rowsHost_)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    rows_->setContentsMargins(0, 0, 0, 0);
    auto* column = new QVBoxLayout(this);
    column->setContentsMargins(0, 0, 0, 0);
    column->addWidget(toggle_, 0, Qt::AlignLeft);
    column->addWidget(rowsHost_);
    connect(toggle_, &QAbstractButton::toggled, this, [this](bool expanded) {
        rowsHost_->setVisible(expanded);
        emit toggled(expanded);
    });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &TaskBar::applyRecipe);
    applyRecipe();
    updateToggle();
}

TaskRow* TaskBar::addTask(const QString& title, const QString& kindIcon) {
    auto* row = new TaskRow(title, kindIcon, rowsHost_);
    rows_->addWidget(row);
    connect(row, &TaskRow::dismissRequested, this, [this, row] { removeTask(row); });
    updateToggle();
    return row;
}

void TaskBar::removeTask(TaskRow* row) {
    rows_->removeWidget(row);
    row->hide();
    row->deleteLater();
    updateToggle();
}

QList<TaskRow*> TaskBar::tasks() const {
    QList<TaskRow*> result;
    for (int i = 0; i < rows_->count(); ++i)
        if (auto* row = qobject_cast<TaskRow*>(rows_->itemAt(i)->widget()))
            result.append(row);
    return result;
}

bool TaskBar::isExpanded() const { return toggle_->isChecked(); }

void TaskBar::setExpanded(bool expanded) { toggle_->setChecked(expanded); }

void TaskBar::updateToggle() {
    const int count = rows_->count();
    toggle_->setText(count == 1 ? tr("1 background task") : tr("%1 background tasks").arg(count));
    toggle_->updateGeometry();
}

void TaskBar::applyRecipe() {
    const TaskBarRecipe r = taskBarRecipe({}, Appearance::instance().theme());
    layout()->setSpacing(qRound(scaled(r.rowGap)));
    rows_->setSpacing(qRound(scaled(r.rowGap)));
    toggle_->updateGeometry();
}

} // namespace ket
