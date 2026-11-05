// SPDX-License-Identifier: MPL-2.0
#include "FoundationPages.h"

#include "KetAppearance.h"
#include "KitchenSinkCommon.h"

#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPainter>
#include <QRegularExpression>
#include <QTableWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

Theme theme() { return Appearance::instance().theme(); }

// A painted sample that repaints when the theme changes.
class Sample : public QWidget {
  public:
    explicit Sample(QWidget* parent) : QWidget(parent) {
        connect(&Appearance::instance(), &Appearance::themeChanged, this,
                qOverload<>(&QWidget::update));
    }
};

class Swatch final : public Sample {
  public:
    Swatch(QString token, QWidget* parent) : Sample(parent), token_(std::move(token)) {
        setFixedSize(176, 76);
        setToolTip(QStringLiteral("--kv-") + token_);
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QColor color = Tokens::color(token_, theme());
        const QRectF chip(0.5, 0.5, width() - 1, 36);
        const qreal radius = Tokens::px(u"radius-md");
        // A panel fill behind the chip shows translucent tokens as they sit on a panel.
        p.setPen(Qt::NoPen);
        p.setBrush(Tokens::color(u"panel-bg", theme()));
        p.drawRoundedRect(chip, radius, radius);
        p.setBrush(color);
        p.setPen(QPen(Tokens::color(u"border-default", theme()), 1));
        p.drawRoundedRect(chip, radius, radius);
        QFont font = this->font();
        font.setPixelSize(qRound(Tokens::px(u"text-xs")));
        p.setFont(font);
        p.setPen(Tokens::color(u"text-main", theme()));
        p.drawText(QRectF(0, 40, width(), 16), Qt::AlignLeft | Qt::AlignVCenter, token_);
        p.setPen(Tokens::color(u"text-muted", theme()));
        p.drawText(QRectF(0, 56, width(), 16), Qt::AlignLeft | Qt::AlignVCenter,
                   Tokens::raw(token_, theme()));
    }

  private:
    QString token_;
};

struct ColourGroup {
    const char* title;
    QStringList prefixes;
};

QStringList tokensWithPrefixes(const QStringList& prefixes) {
    QStringList result;
    for (const QString& name : Tokens::names()) {
        if (Tokens::kind(name) != TokenKind::Color)
            continue;
        for (const QString& prefix : prefixes) {
            if (name == prefix || name.startsWith(prefix + u'-')) {
                result.append(name);
                break;
            }
        }
    }
    return result;
}

QWidget* grid(const QList<QWidget*>& items, int columns, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* layout = new QGridLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(qRound(Tokens::px(u"space-4")));
    layout->setVerticalSpacing(qRound(Tokens::px(u"space-3")));
    for (int i = 0; i < items.size(); ++i)
        layout->addWidget(items[i], i / columns, i % columns, Qt::AlignLeft | Qt::AlignTop);
    layout->setColumnStretch(columns, 1);
    return host;
}

class TextSample final : public Sample {
  public:
    TextSample(QString size, QString line, int weight, bool mono, QWidget* parent,
               QString label = {})
        : Sample(parent), size_(std::move(size)), line_(std::move(line)), weight_(weight),
          mono_(mono), label_(std::move(label)) {
        setFixedHeight(qMax(28, qRound(Tokens::px(line_)) + 4));
        setMinimumWidth(640);
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        QFont label = font();
        label.setPixelSize(qRound(Tokens::px(u"text-xs")));
        p.setFont(label);
        p.setPen(Tokens::color(u"text-muted", theme()));
        const QString meta = label_.isEmpty() ? QStringLiteral("%1 · %2/%3 px")
                                                    .arg(size_)
                                                    .arg(Tokens::px(size_))
                                                    .arg(Tokens::px(line_))
                                              : label_;
        p.drawText(QRect(0, 0, 150, height()), Qt::AlignLeft | Qt::AlignVCenter, meta);
        QFont sample = font();
        const QString family = Tokens::availableFamily(mono_ ? u"font-mono" : u"font-sans");
        if (!family.isEmpty())
            sample.setFamilies({family});
        else if (mono_)
            sample.setStyleHint(QFont::Monospace);
        sample.setPixelSize(qRound(Tokens::px(size_)));
        sample.setWeight(QFont::Weight(weight_));
        p.setFont(sample);
        p.setPen(Tokens::color(u"text-main", theme()));
        p.drawText(QRect(160, 0, width() - 160, height()), Qt::AlignLeft | Qt::AlignVCenter,
                   mono_ ? QStringLiteral("const kết = ket::Tokens::px(u\"space-4\"); // 0O1lI")
                         : QStringLiteral("Két Việt — Tiếng Việt có dấu, 0123456789"));
    }

  private:
    QString size_, line_;
    int weight_;
    bool mono_;
    QString label_;
};

// A panel card that casts one shadow token.
class ShadowCard final : public Sample {
  public:
    ShadowCard(QString token, QWidget* parent) : Sample(parent), token_(std::move(token)) {
        setFixedSize(180, 96);
        connect(&Appearance::instance(), &Appearance::themeChanged, this, &ShadowCard::applyShadow);
        applyShadow();
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const qreal radius = Tokens::px(u"radius-lg");
        p.setPen(QPen(Tokens::color(u"panel-border", theme()), 1));
        p.setBrush(Tokens::color(u"panel-bg", theme()));
        p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        QFont font = this->font();
        font.setPixelSize(qRound(Tokens::px(u"text-sm")));
        font.setWeight(QFont::Medium);
        p.setFont(font);
        p.setPen(Tokens::color(u"text-main", theme()));
        p.drawText(rect().adjusted(0, 0, 0, -16), Qt::AlignCenter, token_);
        font.setPixelSize(qRound(Tokens::px(u"text-xs")));
        font.setWeight(QFont::Normal);
        p.setFont(font);
        p.setPen(Tokens::color(u"text-muted", theme()));
        p.drawText(rect().adjusted(8, 22, -8, 0), Qt::AlignCenter, Tokens::raw(token_, theme()));
    }

  private:
    void applyShadow() {
        static const QRegularExpression shadow(QStringLiteral(
            R"(^(-?[\d.]+)(?:px)? (-?[\d.]+)(?:px)? ([\d.]+)px(?: [\d.]+px)? (#[0-9a-f]+))"));
        const QRegularExpressionMatch m = shadow.match(Tokens::raw(token_, theme()));
        auto* effect = new QGraphicsDropShadowEffect(this);
        if (m.hasMatch()) {
            effect->setOffset(m.captured(1).toDouble(), m.captured(2).toDouble());
            effect->setBlurRadius(m.captured(3).toDouble());
            effect->setColor(QColor::fromString(m.captured(4)));
        }
        setGraphicsEffect(effect); // replaces and deletes the previous effect
    }

    QString token_;
};

class Bar final : public Sample {
  public:
    enum Shape { Space, Radius, Height };
    Bar(QString token, Shape shape, QWidget* parent)
        : Sample(parent), token_(std::move(token)), shape_(shape) {
        setFixedSize(560, shape == Space ? 24 : 56);
    }

  protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QFont label = font();
        label.setPixelSize(qRound(Tokens::px(u"text-xs")));
        p.setFont(label);
        p.setPen(Tokens::color(u"text-secondary", theme()));
        p.drawText(QRect(0, 0, 200, height()), Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("%1 · %2 px").arg(token_).arg(Tokens::px(token_)));
        const qreal value = Tokens::px(token_);
        p.setPen(QPen(Tokens::color(u"accent-border", theme()), 1));
        p.setBrush(Tokens::color(u"accent-subtle", theme()));
        if (shape_ == Space) {
            p.drawRect(QRectF(210.5, 6.5, qMax<qreal>(1, value), 11));
        } else if (shape_ == Radius) {
            const qreal r = qMin<qreal>(value, 24);
            p.drawRoundedRect(QRectF(210.5, 4.5, 96, 47), r, r);
        } else {
            p.drawRoundedRect(QRectF(210.5, (height() - value) / 2.0, 160, value),
                              Tokens::px(u"control-radius"), Tokens::px(u"control-radius"));
        }
    }

  private:
    QString token_;
    Shape shape_;
};

QStringList ordered(const QString& prefix, const QStringList& suffixes) {
    QStringList result;
    for (const QString& suffix : suffixes)
        result.append(prefix + suffix);
    return result;
}

} // namespace

QWidget* coloursPage() {
    PageFrame frame = makePage(
        QStringLiteral("Colours"),
        QStringLiteral("Semantic colour roles for the current theme. Components use these roles; "
                       "the reference palette below is never used directly."));
    const ColourGroup groups[] = {
        {"Surfaces",
         {QStringLiteral("page"), QStringLiteral("panel"), QStringLiteral("surface"),
          QStringLiteral("sidebar"), QStringLiteral("right-rail"), QStringLiteral("app")}},
        {"Text",
         {QStringLiteral("text"), QStringLiteral("on-accent"), QStringLiteral("tab"),
          QStringLiteral("nav")}},
        {"Borders", {QStringLiteral("border")}},
        {"Accent and actions",
         {QStringLiteral("accent"), QStringLiteral("action"), QStringLiteral("focus"),
          QStringLiteral("interactive")}},
        {"Fields", {QStringLiteral("input"), QStringLiteral("field")}},
        {"Status",
         {QStringLiteral("info"), QStringLiteral("success"), QStringLiteral("positive"),
          QStringLiteral("warning"), QStringLiteral("danger"), QStringLiteral("badge")}},
        {"Data", {QStringLiteral("chart"), QStringLiteral("table")}},
        {"Compatibility aliases (--kv-color-*)", {QStringLiteral("color")}},
        {"Reference palette", {QStringLiteral("ref")}},
    };
    for (const ColourGroup& group : groups) {
        frame.body->addWidget(sectionTitle(QString::fromLatin1(group.title), frame.page));
        QList<QWidget*> swatches;
        for (const QString& name : tokensWithPrefixes(group.prefixes))
            swatches.append(new Swatch(name, frame.page));
        frame.body->addWidget(grid(swatches, 5, frame.page));
    }
    frame.body->addStretch();
    return frame.page;
}

QWidget* typographyPage() {
    PageFrame frame = makePage(
        QStringLiteral("Typography"),
        QStringLiteral("Type scale with its line heights, weights and the monospace stack. "
                       "KÉT Sans is used when installed, then Inter, then the platform font."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Scale"), frame.page));
    const QStringList sizes{QStringLiteral("2xs"), QStringLiteral("xs"),  QStringLiteral("sm"),
                            QStringLiteral("md"),  QStringLiteral("lg"),  QStringLiteral("xl"),
                            QStringLiteral("2xl"), QStringLiteral("3xl"), QStringLiteral("4xl")};
    for (const QString& size : sizes)
        frame.body->addWidget(new TextSample(QStringLiteral("text-") + size,
                                             QStringLiteral("line-") + size, 400, false,
                                             frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Weights at text-lg"), frame.page));
    for (const char* weight : {"normal", "medium", "semibold", "bold"}) {
        const int value = qRound(Tokens::number(QStringLiteral("weight-") + QLatin1String(weight)));
        frame.body->addWidget(new TextSample(
            QStringLiteral("text-lg"), QStringLiteral("line-lg"), value, false, frame.page,
            QStringLiteral("weight-%1 · %2").arg(QLatin1String(weight)).arg(value)));
    }
    frame.body->addWidget(sectionTitle(QStringLiteral("Monospace"), frame.page));
    frame.body->addWidget(new TextSample(QStringLiteral("text-sm"), QStringLiteral("line-sm"), 400,
                                         true, frame.page));
    frame.body->addStretch();
    return frame.page;
}

QWidget* spacingPage() {
    PageFrame frame = makePage(QStringLiteral("Spacing, radius and controls"),
                               QStringLiteral("Lengths in logical pixels at 100% zoom; components "
                                              "scale them to whole pixels at other zoom levels."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Spacing"), frame.page));
    for (const QString& name : ordered(
             QStringLiteral("space-"),
             {QStringLiteral("0-5"), QStringLiteral("1"), QStringLiteral("1-5"),
              QStringLiteral("2"), QStringLiteral("2-5"), QStringLiteral("3"), QStringLiteral("4"),
              QStringLiteral("5"), QStringLiteral("6"), QStringLiteral("8"), QStringLiteral("10"),
              QStringLiteral("12"), QStringLiteral("16")}))
        frame.body->addWidget(new Bar(name, Bar::Space, frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Radius"), frame.page));
    for (const QString& name :
         ordered(QStringLiteral("radius-"),
                 {QStringLiteral("xs"), QStringLiteral("sm"), QStringLiteral("md"),
                  QStringLiteral("lg"), QStringLiteral("xl")}))
        frame.body->addWidget(new Bar(name, Bar::Radius, frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Control heights"), frame.page));
    for (const QString& name :
         ordered(QStringLiteral("control-height-"), {QStringLiteral("xs"), QStringLiteral("sm"),
                                                     QStringLiteral("md"), QStringLiteral("lg")}))
        frame.body->addWidget(new Bar(name, Bar::Height, frame.page));
    frame.body->addStretch();
    return frame.page;
}

QWidget* elevationPage() {
    PageFrame frame =
        makePage(QStringLiteral("Elevation"),
                 QStringLiteral("Shadow tokens on a panel surface. Qt draws the first "
                                "layer of each CSS shadow without spread."));
    QList<QWidget*> cards;
    for (const char* name :
         {"shadow-xs", "shadow-sm", "shadow-md", "shadow-popover", "shadow-float"})
        cards.append(new ShadowCard(QString::fromLatin1(name), frame.page));
    frame.body->addSpacing(qRound(Tokens::px(u"space-4")));
    // Room around the cards so their shadows are not clipped by the page.
    QWidget* cardGrid = grid(cards, 4, frame.page);
    const int room = qRound(Tokens::px(u"space-10"));
    cardGrid->layout()->setContentsMargins(0, 0, room, room);
    static_cast<QGridLayout*>(cardGrid->layout())->setHorizontalSpacing(room);
    static_cast<QGridLayout*>(cardGrid->layout())->setVerticalSpacing(room);
    frame.body->addWidget(cardGrid);
    frame.body->addStretch();
    return frame.page;
}

QWidget* tokensPage() {
    PageFrame frame = makePage(QStringLiteral("All tokens"),
                               QStringLiteral("Every compiled token with its light and dark value. "
                                              "Source: ") +
                                   Tokens::source());
    auto* filter = new QLineEdit(frame.page);
    filter->setPlaceholderText(QStringLiteral("Filter tokens, e.g. accent or radius"));
    filter->setClearButtonEnabled(true);
    frame.body->addWidget(filter);
    const QStringList names = Tokens::names();
    auto* table = new QTableWidget(int(names.size()), 4, frame.page);
    table->setHorizontalHeaderLabels({QStringLiteral("Token"), QStringLiteral("Kind"),
                                      QStringLiteral("Light"), QStringLiteral("Dark")});
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    static const char* kinds[] = {"Color",  "Length", "Number", "Duration",
                                  "Easing", "Shadow", "Font",   "Other"};
    for (int row = 0; row < names.size(); ++row) {
        const QString& name = names[row];
        table->setItem(row, 0, new QTableWidgetItem(name));
        table->setItem(row, 1,
                       new QTableWidgetItem(QString::fromLatin1(kinds[int(Tokens::kind(name))])));
        for (const auto [column, mode] : {std::pair{2, Theme::Light}, std::pair{3, Theme::Dark}}) {
            auto* item = new QTableWidgetItem(Tokens::raw(name, mode));
            if (Tokens::kind(name) == TokenKind::Color) {
                QPixmap chip(14, 14);
                chip.fill(Tokens::color(name, mode));
                item->setIcon(QIcon(chip));
            }
            table->setItem(row, column, item);
        }
    }
    table->setMinimumHeight(560);
    QObject::connect(filter, &QLineEdit::textChanged, table, [table](const QString& text) {
        for (int row = 0; row < table->rowCount(); ++row)
            table->setRowHidden(row,
                                !table->item(row, 0)->text().contains(text, Qt::CaseInsensitive));
    });
    frame.body->addWidget(table, 1);
    return frame.page;
}

} // namespace ket::kitchensink
