// SPDX-License-Identifier: MPL-2.0
#include "KetRouteTabs.h"

#include "KetAppearance.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

class RouteTab final : public QAbstractButton {
  public:
    RouteTab(const QString& text, QWidget* parent) : QAbstractButton(parent) {
        setText(text);
        setCheckable(true);
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    [[nodiscard]] RouteTabRecipe recipe() const {
        return routeTabRecipe(isChecked(),
                              InteractionState{underMouse(), isDown(), isEnabled(), hasFocus()},
                              Appearance::instance().theme());
    }

    [[nodiscard]] QFont textFont() const {
        QFont font = this->font();
        font.setPixelSize(qRound(scaled(recipe().fontPixels)));
        return font;
    }

    [[nodiscard]] QSize sizeHint() const override {
        const RouteTabRecipe r = recipe();
        const QFontMetricsF metrics(textFont());
        return {qCeil(metrics.horizontalAdvance(text())),
                qCeil(scaled(r.paddingTop) + metrics.height() + scaled(r.paddingBottom) +
                      scaled(r.indicatorHeight))};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const RouteTabRecipe r = recipe();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(r.opacity);
        if (r.focusRing.alpha()) {
            painter.setPen(QPen(r.focusRing, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), 3, 3);
        }
        const qreal indicator = scaled(r.indicatorHeight);
        painter.setFont(textFont());
        painter.setPen(r.text);
        painter.drawText(
            QRectF(0, scaled(r.paddingTop), width(), QFontMetricsF(textFont()).height()),
            Qt::AlignHCenter | Qt::AlignTop, text());
        painter.fillRect(QRectF(0, height() - indicator, width(), indicator), r.indicator);
    }
    void enterEvent(QEnterEvent* event) override {
        update();
        QAbstractButton::enterEvent(event);
    }
    void leaveEvent(QEvent* event) override {
        update();
        QAbstractButton::leaveEvent(event);
    }
    void keyPressEvent(QKeyEvent* event) override {
        const int step = event->key() == Qt::Key_Right ? 1 : event->key() == Qt::Key_Left ? -1 : 0;
        if (step != 0) {
            if (auto* tabs = qobject_cast<RouteTabs*>(parentWidget())) {
                const int next = qBound(0, tabs->currentIndex() + step, tabs->count() - 1);
                tabs->setCurrentIndex(next);
                tabs->tab(next)->setFocus(Qt::TabFocusReason);
            }
            return;
        }
        QAbstractButton::keyPressEvent(event);
    }
};

} // namespace

RouteTabs::RouteTabs(QWidget* parent)
    : QWidget(parent), group_(new QButtonGroup(this)), row_(new QHBoxLayout(this)) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    group_->setExclusive(true);
    row_->setContentsMargins(0, 0, 0, 0);
    row_->addStretch(1);
    connect(group_, &QButtonGroup::idToggled, this, [this](int id, bool checked) {
        if (checked)
            emit currentChanged(id);
    });
    connect(&Appearance::instance(), &Appearance::themeChanged, this, [this] { update(); });
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &RouteTabs::applyRecipe);
    applyRecipe();
}

int RouteTabs::addTab(const QString& text) {
    auto* tab = new RouteTab(text, this);
    const int index = count();
    group_->addButton(tab, index);
    row_->insertWidget(index, tab);
    if (index == 0)
        tab->setChecked(true);
    return index;
}

int RouteTabs::count() const { return int(group_->buttons().size()); }

int RouteTabs::currentIndex() const { return group_->checkedId(); }

void RouteTabs::setCurrentIndex(int index) {
    if (QAbstractButton* button = group_->button(index))
        button->setChecked(true);
}

QAbstractButton* RouteTabs::tab(int index) const { return group_->button(index); }

void RouteTabs::applyRecipe() {
    const RouteTabRecipe r = routeTabRecipe(false, {}, Appearance::instance().theme());
    row_->setSpacing(qRound(scaled(r.spacing)));
    for (QAbstractButton* button : group_->buttons())
        button->updateGeometry();
    update();
}

void RouteTabs::paintEvent(QPaintEvent*) {
    const RouteTabRecipe r = routeTabRecipe(false, {}, Appearance::instance().theme());
    QPainter painter(this);
    painter.fillRect(QRect(0, height() - 1, width(), 1), r.rule);
}

} // namespace ket
