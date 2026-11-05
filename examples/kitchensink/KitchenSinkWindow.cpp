// SPDX-License-Identifier: MPL-2.0
#include "KitchenSinkWindow.h"

#include "ComponentPages.h"
#include "FoundationPages.h"
#include "KetAppearance.h"
#include "KitchenSinkCommon.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPainter>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

struct PageEntry {
    const char* id;
    const char* section;
    const char* title;
    QWidget* (*create)();
};

const PageEntry kPages[] = {
    {"colours", "Foundations", "Colours", coloursPage},
    {"typography", "Foundations", "Typography", typographyPage},
    {"spacing", "Foundations", "Spacing and radius", spacingPage},
    {"elevation", "Foundations", "Elevation", elevationPage},
    {"tokens", "Foundations", "All tokens", tokensPage},
    {"button", "Components", "Button", buttonPage},
    {"badge", "Components", "Badge", badgePage},
};

Theme theme() { return Appearance::instance().theme(); }

// Sidebar rows drawn from the design system's sidebar and navigation tokens.
class NavDelegate final : public QStyledItemDelegate {
  public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        const bool header = !(index.flags() & Qt::ItemIsSelectable);
        const bool selected = option.state & QStyle::State_Selected;
        const bool hovered = option.state & QStyle::State_MouseOver;
        const int inset = qRound(Tokens::px(u"space-2"));
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        const QRectF row = QRectF(option.rect).adjusted(inset, 1, -inset, -1);
        if (!header && (selected || hovered)) {
            const qreal radius = Tokens::px(u"radius-md");
            painter->setPen(Qt::NoPen);
            painter->setBrush(
                Tokens::color(selected ? u"nav-selected" : u"surface-hover", theme()));
            painter->drawRoundedRect(row, radius, radius);
        }
        QFont font = option.font;
        font.setPixelSize(qRound(Tokens::px(header ? u"text-xs" : u"text-sm")));
        font.setWeight(header ? QFont::DemiBold : (selected ? QFont::Medium : QFont::Normal));
        painter->setFont(font);
        painter->setPen(Tokens::color(header     ? u"text-muted"
                                      : selected ? u"nav-selected-text"
                                                 : u"text-main",
                                      theme()));
        const QRectF text = row.adjusted(Tokens::px(u"space-2-5"), header ? 6 : 0, 0, 0);
        painter->drawText(text, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        const bool header = !(index.flags() & Qt::ItemIsSelectable);
        const int height = qRound(Tokens::px(u"sidebar-item-height")) + (header ? 10 : 2);
        return {option.rect.width(), height};
    }
};

// A one-pixel rule in a border token, for the sidebar edge.
class Rule final : public QWidget {
  public:
    explicit Rule(QWidget* parent) : QWidget(parent) {
        setFixedWidth(1);
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter(this).fillRect(rect(), Tokens::color(u"sidebar-border", theme()));
    }
};

QComboBox* themeSelector(QWidget* parent) {
    auto* combo = new QComboBox(parent);
    combo->addItem(QStringLiteral("System"), int(Appearance::Mode::System));
    combo->addItem(QStringLiteral("Light"), int(Appearance::Mode::Light));
    combo->addItem(QStringLiteral("Dark"), int(Appearance::Mode::Dark));
    combo->setCurrentIndex(combo->findData(int(Appearance::instance().mode())));
    QObject::connect(combo, &QComboBox::currentIndexChanged, combo, [combo] {
        Appearance::instance().setMode(Appearance::Mode(combo->currentData().toInt()));
    });
    return combo;
}

QComboBox* zoomSelector(QWidget* parent) {
    auto* combo = new QComboBox(parent);
    for (const qreal zoom : {0.875, 1.0, 1.125, 1.25, 1.5})
        combo->addItem(QStringLiteral("%1%").arg(qRound(zoom * 100)), zoom);
    const int current = combo->findData(Appearance::instance().zoom());
    combo->setCurrentIndex(current >= 0 ? current : 1);
    QObject::connect(combo, &QComboBox::currentIndexChanged, combo,
                     [combo] { Appearance::instance().setZoom(combo->currentData().toReal()); });
    return combo;
}

} // namespace

KitchenSinkWindow::KitchenSinkWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle(QStringLiteral("KetQt kitchensink · %1").arg(Tokens::source()));
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    nav_ = new QListWidget(this);
    nav_->setFixedWidth(qRound(Tokens::px(u"sidebar-width")));
    nav_->setFrameShape(QFrame::NoFrame);
    nav_->setItemDelegate(new NavDelegate(nav_));
    nav_->setMouseTracking(true);
    nav_->setContentsMargins(0, qRound(Tokens::px(u"space-3")), 0, 0);
    const auto paintSidebar = [this] {
        QPalette palette = nav_->palette();
        palette.setColor(QPalette::Base, Tokens::color(u"sidebar-bg", theme()));
        nav_->setPalette(palette);
    };
    connect(&Appearance::instance(), &Appearance::themeChanged, nav_, paintSidebar);
    paintSidebar();
    pages_ = new QStackedWidget(this);
    QString section;
    for (const PageEntry& entry : kPages) {
        if (section != QLatin1String(entry.section)) {
            section = QString::fromLatin1(entry.section);
            auto* header = new QListWidgetItem(section, nav_);
            header->setFlags(Qt::NoItemFlags);
        }
        auto* item = new QListWidgetItem(QString::fromLatin1(entry.title), nav_);
        item->setData(Qt::UserRole, pages_->count());
        ids_.append(QString::fromLatin1(entry.id));
        pages_->addWidget(entry.create());
    }
    connect(nav_, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* item) {
        if (item && item->data(Qt::UserRole).isValid())
            pages_->setCurrentIndex(item->data(Qt::UserRole).toInt());
    });

    auto* right = new QVBoxLayout;
    right->setSpacing(0);
    auto* bar = new QHBoxLayout;
    const int inset = qRound(Tokens::px(u"space-4"));
    bar->setContentsMargins(inset, qRound(Tokens::px(u"space-2")), inset,
                            qRound(Tokens::px(u"space-2")));
    bar->addWidget(new TokenLabel(QStringLiteral("Két Design System for Qt · ") + Tokens::source(),
                                  QStringLiteral("text-muted"), QStringLiteral("text-sm"), 400,
                                  this, false));
    bar->addStretch();
    bar->addWidget(new TokenLabel(QStringLiteral("Theme"), QStringLiteral("text-secondary"),
                                  QStringLiteral("text-sm"), 500, this));
    bar->addWidget(themeSelector(this));
    bar->addSpacing(inset);
    bar->addWidget(new TokenLabel(QStringLiteral("Zoom"), QStringLiteral("text-secondary"),
                                  QStringLiteral("text-sm"), 500, this));
    bar->addWidget(zoomSelector(this));
    right->addLayout(bar);
    right->addWidget(pages_, 1);
    root->addWidget(nav_);
    root->addWidget(new Rule(this));
    root->addLayout(right, 1);
    showPage(ids_.constFirst());
}

QStringList KitchenSinkWindow::pageIds() {
    QStringList ids;
    for (const PageEntry& entry : kPages)
        ids.append(QString::fromLatin1(entry.id));
    return ids;
}

bool KitchenSinkWindow::showPage(const QString& id) {
    const int index = int(ids_.indexOf(id));
    if (index < 0)
        return false;
    for (int row = 0; row < nav_->count(); ++row) {
        QListWidgetItem* item = nav_->item(row);
        if (item->data(Qt::UserRole).isValid() && item->data(Qt::UserRole).toInt() == index) {
            nav_->setCurrentItem(item);
            break;
        }
    }
    pages_->setCurrentIndex(index);
    return true;
}

} // namespace ket::kitchensink
