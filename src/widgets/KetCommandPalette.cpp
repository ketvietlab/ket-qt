// SPDX-License-Identifier: MPL-2.0
#include "KetCommandPalette.h"

#include "KetAppearance.h"
#include "KetIcons.h"
#include "KetShadow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QtMath>

#include <algorithm>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

QFont sized(const QWidget* widget, qreal pixels, int weight = QFont::Normal) {
    QFont font = widget->font();
    font.setPixelSize(qRound(scaled(pixels)));
    font.setWeight(QFont::Weight(weight));
    return font;
}

// `.spotlight > footer`: key hints, each key in a small box.
class Footer final : public QWidget {
  public:
    using QWidget::QWidget;

    [[nodiscard]] QSize sizeHint() const override {
        const CommandPaletteRecipe r = commandPaletteRecipe(theme());
        return {0, qCeil(2 * scaled(r.footerPaddingBlock) +
                         QFontMetricsF(sized(this, r.footerPixels)).height() + 4)};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const CommandPaletteRecipe r = commandPaletteRecipe(theme());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(QRect(0, 0, width(), 1), r.rule);
        const QFont font = sized(this, r.footerPixels);
        const QFontMetricsF metrics(font);
        painter.setFont(font);
        qreal x = scaled(r.footerPaddingInline);
        const qreal y = (height() - metrics.height()) / 2;
        const struct {
            const char* key;
            const char* label;
        } hints[] = {{"↑↓", "to navigate"}, {"↵", "to open"}, {"esc", "to close"}};
        for (const auto& hint : hints) {
            const QString key = QString::fromUtf8(hint.key);
            const QRectF box(x, y - 2, metrics.horizontalAdvance(key) + 6, metrics.height() + 4);
            painter.setPen(QPen(r.rule, 1));
            painter.setBrush(r.background);
            painter.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), 3, 3);
            painter.setPen(r.footer);
            painter.drawText(box, Qt::AlignCenter, key);
            x = box.right() + 4;
            const QString label = CommandPalette::tr(hint.label);
            painter.drawText(QPointF(x, y + metrics.ascent()), label);
            x += metrics.horizontalAdvance(label) + scaled(r.footerGap);
        }
    }
};

} // namespace

// The grouped results (`#spotlight-results`), painted; rows are hit-tested by index.
class CommandList final : public QWidget {
  public:
    explicit CommandList(CommandPalette* palette) : QWidget(palette), palette_(palette) {
        setMouseTracking(true);
    }

    struct Row {
        int item = -1; // -1 for a group label
        QRectF rect;
        QString text;
    };

    [[nodiscard]] QList<Row> rows(int width) const {
        const CommandPaletteRecipe p = commandPaletteRecipe(theme());
        const CommandResultRecipe r = commandResultRecipe(false, theme());
        const qreal inline_ = scaled(p.listPaddingInline);
        const qreal groupHeight = scaled(p.groupPaddingTop) + scaled(p.groupPaddingBottom) +
                                  QFontMetricsF(sized(this, p.groupPixels)).height();
        const qreal itemHeight =
            2 * scaled(r.paddingBlock) + QFontMetricsF(sized(this, r.titlePixels)).height() +
            scaled(r.detailGap) + QFontMetricsF(sized(this, r.detailPixels)).height();
        QList<Row> result;
        qreal y = 0;
        int index = 0;
        for (const CommandGroup& group : palette_->groups_) {
            if (group.items.isEmpty())
                continue;
            if (!group.title.isEmpty()) {
                result.append(
                    {-1, QRectF(inline_, y, width - 2 * inline_, groupHeight), group.title});
                y += groupHeight;
            }
            for (const CommandItem& item : group.items) {
                result.append(
                    {index++, QRectF(inline_, y, width - 2 * inline_, itemHeight), item.title});
                y += itemHeight;
            }
        }
        return result;
    }

    [[nodiscard]] int contentHeight(int width) const {
        const CommandPaletteRecipe p = commandPaletteRecipe(theme());
        const QList<Row> all = rows(width);
        if (all.isEmpty())
            return qRound(scaled(p.emptyHeight));
        return qCeil(all.constLast().rect.bottom() + scaled(p.listPaddingBottom));
    }

    [[nodiscard]] QRect itemRect(int index) const {
        for (const Row& row : rows(width()))
            if (row.item == index)
                return row.rect.toAlignedRect();
        return {};
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        const CommandPaletteRecipe p = commandPaletteRecipe(theme());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const QList<Row> all = rows(width());
        if (all.isEmpty()) {
            paintEmpty(painter, p);
            return;
        }
        for (const Row& row : all) {
            if (row.item < 0) {
                QFont font = sized(this, p.groupPixels, p.groupWeight);
                font.setLetterSpacing(QFont::AbsoluteSpacing, p.groupLetterSpacing);
                painter.setFont(font);
                painter.setPen(p.group);
                painter.drawText(row.rect.adjusted(scaled(p.groupPaddingInline),
                                                   scaled(p.groupPaddingTop), 0, 0),
                                 Qt::AlignLeft | Qt::AlignTop, row.text.toUpper());
                continue;
            }
            paintItem(painter, row, *palette_->itemAt(row.item),
                      row.item == palette_->highlighted_);
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        for (const Row& row : rows(width()))
            if (row.item >= 0 && row.rect.contains(event->position())) {
                palette_->setHighlightedIndex(row.item);
                break;
            }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        for (const Row& row : rows(width()))
            if (row.item >= 0 && row.rect.contains(event->position()))
                palette_->activate(row.item);
    }

  private:
    void paintItem(QPainter& painter, const Row& row, const CommandItem& item,
                   bool highlighted) const {
        const CommandResultRecipe r = commandResultRecipe(highlighted, theme());
        const qreal dpr = devicePixelRatioF();
        const qreal radius = scaled(r.radius);
        painter.setPen(Qt::NoPen);
        painter.setBrush(r.background);
        painter.drawRoundedRect(row.rect, radius, radius);
        qreal x = row.rect.left() + scaled(r.paddingInline);
        const qreal icon = scaled(r.iconSize);
        if (!item.iconCode.isEmpty()) {
            painter.drawPixmap(QPointF(qRound(x), qRound(row.rect.center().y() - icon / 2)),
                               Icons::pixmap(item.iconCode, icon, dpr, r.icon));
            x += icon + scaled(r.spacing);
        }
        qreal right = row.rect.right() - scaled(r.paddingInline);
        const QFont small = sized(this, r.detailPixels);
        const QFontMetricsF smallMetrics(small);
        if (highlighted) {
            const QString key = QStringLiteral("↵");
            const QRectF box(right - smallMetrics.horizontalAdvance(key) - 8,
                             row.rect.center().y() - smallMetrics.height() / 2 - 2,
                             smallMetrics.horizontalAdvance(key) + 8, smallMetrics.height() + 4);
            painter.setPen(QPen(commandPaletteRecipe(theme()).rule, 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), 3, 3);
            painter.setFont(small);
            painter.setPen(r.trailing);
            painter.drawText(box, Qt::AlignCenter, key);
            right = box.left() - scaled(r.spacing);
        } else {
            const qreal box = scaled(r.trailingSize);
            painter.drawPixmap(
                QPointF(qRound(right - box), qRound(row.rect.center().y() - box / 2)),
                Icons::pixmap(u"arrow-up-right", box, dpr, r.trailing));
            right -= box + scaled(r.spacing);
        }
        const QFont title = sized(this, r.titlePixels, r.titleWeight);
        const QFontMetricsF titleMetrics(title);
        const qreal top = row.rect.top() + scaled(r.paddingBlock);
        painter.setFont(title);
        painter.setPen(r.title);
        painter.drawText(QRectF(x, top, right - x, titleMetrics.height()), Qt::AlignLeft,
                         titleMetrics.elidedText(item.title, Qt::ElideRight, right - x));
        if (!item.detail.isEmpty()) {
            painter.setFont(small);
            painter.setPen(r.detail);
            painter.drawText(QRectF(x, top + titleMetrics.height() + scaled(r.detailGap), right - x,
                                    smallMetrics.height()),
                             Qt::AlignLeft,
                             smallMetrics.elidedText(item.detail, Qt::ElideMiddle, right - x));
        }
    }

    void paintEmpty(QPainter& painter, const CommandPaletteRecipe& p) const {
        const qreal icon = scaled(p.emptyIconSize);
        const QFont title = sized(this, p.emptyTitlePixels, 600);
        const QFont text = sized(this, p.emptyTextPixels);
        const qreal total =
            icon + 15 + QFontMetricsF(title).height() + 9 + QFontMetricsF(text).height();
        qreal y = (height() - total) / 2;
        painter.drawPixmap(QPointF(qRound((width() - icon) / 2), qRound(y)),
                           Icons::pixmap(u"search", icon, devicePixelRatioF(), p.placeholder));
        y += icon + 15;
        painter.setFont(title);
        painter.setPen(p.text);
        painter.drawText(QRectF(0, y, width(), QFontMetricsF(title).height()), Qt::AlignHCenter,
                         palette_->emptyTitle_);
        y += QFontMetricsF(title).height() + 9;
        painter.setFont(text);
        painter.setPen(p.placeholder);
        painter.drawText(QRectF(0, y, width(), QFontMetricsF(text).height()), Qt::AlignHCenter,
                         palette_->emptyMessage_);
    }

    CommandPalette* palette_;
};

CommandPalette::CommandPalette(QWidget* anchor)
    : QWidget(anchor->window()), panel_(new QWidget(this)), search_(new QLineEdit(panel_)),
      scroll_(new QScrollArea(panel_)), list_(new CommandList(this)), footer_(new Footer(panel_)),
      emptyTitle_(tr("No results")), emptyMessage_(tr("Try another word or a file name.")) {
    hide();
    setAttribute(Qt::WA_TranslucentBackground);
    search_->setFrame(false);
    search_->setPlaceholderText(tr("Search commands, files and conversations"));
    search_->setAccessibleName(tr("Search"));
    search_->installEventFilter(this);
    search_->setAttribute(Qt::WA_MacShowFocusRect, false);
    scroll_->setFrameShape(QFrame::NoFrame);
    scroll_->setWidgetResizable(false);
    scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_->viewport()->setAutoFillBackground(false);
    scroll_->setWidget(list_);
    list_->setAutoFillBackground(false);
    auto* column = new QVBoxLayout(panel_);
    column->setContentsMargins(1, 1, 1, 1);
    column->setSpacing(0);
    column->addWidget(search_);
    column->addWidget(scroll_);
    column->addWidget(footer_);
    parentWidget()->installEventFilter(this);
    connect(search_, &QLineEdit::textChanged, this, [this](const QString& query) {
        setHighlightedIndex(0);
        emit queryChanged(query);
    });
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &CommandPalette::applyRecipe);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &CommandPalette::applyRecipe);
    applyRecipe();
}

void CommandPalette::setGroups(const QList<CommandGroup>& groups) {
    groups_ = groups;
    highlighted_ = qBound(0, highlighted_, qMax(0, itemCount() - 1));
    place();
    list_->update();
}

void CommandPalette::setPlaceholderText(const QString& text) { search_->setPlaceholderText(text); }

void CommandPalette::setEmptyText(const QString& title, const QString& message) {
    emptyTitle_ = title;
    emptyMessage_ = message;
    list_->update();
}

QString CommandPalette::query() const { return search_->text(); }

int CommandPalette::itemCount() const {
    int count = 0;
    for (const CommandGroup& group : groups_)
        count += int(group.items.size());
    return count;
}

const CommandItem* CommandPalette::itemAt(int index) const {
    for (const CommandGroup& group : groups_) {
        if (index < group.items.size())
            return &group.items.at(index);
        index -= int(group.items.size());
    }
    return nullptr;
}

void CommandPalette::setHighlightedIndex(int index) {
    const int count = itemCount();
    highlighted_ = count == 0 ? 0 : (index % count + count) % count;
    if (const QRect rect = list_->itemRect(highlighted_); rect.isValid())
        scroll_->ensureVisible(rect.center().x(), rect.center().y(), 0, rect.height() / 2 + 1);
    list_->update();
}

void CommandPalette::activate(int index) {
    if (const CommandItem* item = itemAt(index)) {
        const CommandItem chosen = *item;
        dismiss();
        emit activated(chosen);
    }
}

void CommandPalette::open() {
    restoreFocus_ = QApplication::focusWidget();
    search_->clear();
    highlighted_ = 0;
    place();
    show();
    raise();
    search_->setFocus(Qt::PopupFocusReason);
}

void CommandPalette::dismiss() {
    if (!isVisible())
        return;
    hide();
    if (restoreFocus_)
        restoreFocus_->setFocus(Qt::PopupFocusReason);
    emit closed();
}

void CommandPalette::applyRecipe() {
    const CommandPaletteRecipe r = commandPaletteRecipe(theme());
    const int block = qRound(scaled(r.searchPaddingBlock));
    const int inline_ = qRound(scaled(r.searchPaddingInline));
    const int icon = qRound(scaled(r.searchIconSize));
    const int gap = qRound(scaled(r.searchGap));
    search_->setFont(sized(this, r.inputPixels));
    search_->setTextMargins(inline_ + icon + gap, block + qRound(scaled(r.inputPaddingBlock)),
                            inline_, block + qRound(scaled(r.inputPaddingBlock)));
    QPalette palette = search_->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, r.text);
    palette.setColor(QPalette::PlaceholderText, r.placeholder);
    search_->setPalette(palette);
    footer_->updateGeometry();
    place();
    update();
}

void CommandPalette::place() {
    if (!parentWidget())
        return;
    setGeometry(parentWidget()->rect());
    const CommandPaletteRecipe r = commandPaletteRecipe(theme());
    const int width = qMin(qRound(scaled(r.width)), parentWidget()->width() - 2 * 25);
    list_->resize(width - 2, list_->contentHeight(width - 2));
    // The list scrolls past its maximum or when the window is too short for it.
    const int chrome = 2 + search_->sizeHint().height() + footer_->sizeHint().height();
    const int listHeight = qMax(0, std::min({list_->height(), qRound(scaled(r.maxListHeight)),
                                             parentWidget()->height() - 2 * 25 - chrome}));
    scroll_->setFixedHeight(listHeight);
    const int height = chrome + listHeight;
    const int top = qMin(qRound(scaled(r.top)), qMax(25, parentWidget()->height() - height - 25));
    panel_->setGeometry((this->width() - width) / 2, top, width, height);
}

void CommandPalette::paintEvent(QPaintEvent*) {
    const CommandPaletteRecipe r = commandPaletteRecipe(theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), r.backdrop);
    const QRectF panel = panel_->geometry();
    const qreal radius = scaled(r.radius);
    detail::paintShadow(painter, panel, radius, detail::scaled(r.shadow));
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(panel.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    // The search row's icon and hairline.
    const QRect field = search_->geometry().translated(panel_->pos());
    const qreal icon = scaled(r.searchIconSize);
    painter.drawPixmap(QPointF(field.left() + qRound(scaled(r.searchPaddingInline)),
                               qRound(field.center().y() - icon / 2)),
                       Icons::pixmap(u"search", icon, devicePixelRatioF(), r.searchIcon));
    painter.fillRect(QRect(field.left(), field.bottom(), field.width(), 1), r.rule);
}

void CommandPalette::mousePressEvent(QMouseEvent* event) {
    if (!panel_->geometry().contains(event->position().toPoint()))
        dismiss();
}

bool CommandPalette::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible())
        place();
    if (watched == search_ && event->type() == QEvent::KeyPress) {
        const auto* key = static_cast<QKeyEvent*>(event);
        switch (key->key()) {
        case Qt::Key_Down:
            setHighlightedIndex(highlighted_ + 1);
            return true;
        case Qt::Key_Up:
            setHighlightedIndex(highlighted_ - 1);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            activate(highlighted_);
            return true;
        case Qt::Key_Escape:
            dismiss();
            return true;
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

} // namespace ket
