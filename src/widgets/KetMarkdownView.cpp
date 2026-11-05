// SPDX-License-Identifier: MPL-2.0
#include "KetMarkdownView.h"

#include "KetAppearance.h"
#include "KetButton.h"
#include "KetIcons.h"
#include "KetMarkdown.h"
#include "KetParagraph.h"
#include "KetProfile.h"

#include <QBoxLayout>
#include <QClipboard>
#include <QGuiApplication>
#include <QPainter>
#include <QScrollBar>
#include <QtMath>

namespace ket {
namespace {

qreal scaled(qreal length) { return Appearance::instance().scaled(length); }

Theme theme() { return Appearance::instance().theme(); }

constexpr int kCopiedMs = 1500; // how long the copy button shows the check

} // namespace

CodeBlock::CodeBlock(const QString& code, const QString& language, QWidget* parent)
    : QWidget(parent), code_(code), language_(language),
      copy_(new IconButton(Icons::icon(u"copy"), tr("Copy code"), this)) {
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setAccessibleName(language.isEmpty() ? tr("Code") : tr("%1 code").arg(language));
    copy_->setControlSize(ControlSize::Small);
    copiedReset_.setSingleShot(true);
    copiedReset_.setInterval(kCopiedMs);
    connect(&copiedReset_, &QTimer::timeout, this,
            [this] { copy_->setIcon(Icons::icon(u"copy")); });
    connect(copy_, &QAbstractButton::clicked, this, &CodeBlock::copy);
    connect(&Appearance::instance(), &Appearance::themeChanged, this,
            qOverload<>(&QWidget::update));
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &QWidget::updateGeometry);
}

void CodeBlock::setCode(const QString& code) {
    code_ = code;
    updateGeometry();
    update();
}

void CodeBlock::setLanguage(const QString& language) {
    language_ = language;
    setAccessibleName(language.isEmpty() ? tr("Code") : tr("%1 code").arg(language));
}

QFont CodeBlock::codeFont() const {
    QFont font = this->font();
    if (const QString family = Profile::current().monoFamily(); !family.isEmpty())
        font.setFamilies({family});
    font.setPixelSize(qRound(scaled(codeBlockRecipe(theme()).fontPixels)));
    return font;
}

int CodeBlock::heightForWidth(int width) const {
    const CodeBlockRecipe r = codeBlockRecipe(theme());
    const qreal padding = scaled(r.padding);
    // The copy button keeps its corner clear of the first line.
    const qreal textWidth = width - 2 * padding - copy_->sizeHint().width();
    detail::Paragraph paragraph(code_, codeFont(), r.lineHeight);
    paragraph.layout(qMax<qreal>(1, textWidth));
    return qCeil(paragraph.height() + 2 * padding + 2 * scaled(r.margin));
}

QSize CodeBlock::sizeHint() const {
    const int width = qRound(scaled(Tokens::px(u"space-16")) * 8);
    return {width, heightForWidth(width)};
}

void CodeBlock::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    const CodeBlockRecipe r = codeBlockRecipe(theme());
    const int inset = qRound(scaled(r.margin) + scaled(r.padding) / 2);
    copy_->move(width() - copy_->width() - qRound(scaled(r.padding) / 2), inset);
    if (const int wanted = heightForWidth(width()); wanted != minimumHeight())
        setMinimumHeight(wanted);
}

void CodeBlock::paintEvent(QPaintEvent*) {
    const CodeBlockRecipe r = codeBlockRecipe(theme());
    const qreal margin = scaled(r.margin);
    const qreal padding = scaled(r.padding);
    const QRectF surface = QRectF(rect()).adjusted(0, margin, 0, -margin);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const qreal radius = scaled(r.radius);
    painter.setPen(QPen(r.border, 1));
    painter.setBrush(r.background);
    painter.drawRoundedRect(surface.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    detail::Paragraph paragraph(code_, codeFont(), r.lineHeight);
    paragraph.layout(qMax<qreal>(1, width() - 2 * padding - copy_->sizeHint().width()));
    painter.setPen(r.text);
    paragraph.draw(painter, surface.topLeft() + QPointF(padding, padding));
}

void CodeBlock::copy() {
    QGuiApplication::clipboard()->setText(code_);
    copy_->setIcon(Icons::icon(u"check"));
    copiedReset_.start();
    emit copied();
}

MarkdownText::MarkdownText(const QString& markdown, QWidget* parent)
    : QTextBrowser(parent), markdown_(markdown) {
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
    setFrameShape(QFrame::NoFrame);
    setOpenLinks(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->setAutoFillBackground(false);
    setAutoFillBackground(false);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(&Appearance::instance(), &Appearance::themeChanged, this, &MarkdownText::render);
    connect(&Appearance::instance(), &Appearance::zoomChanged, this, &MarkdownText::render);
    render();
}

void MarkdownText::setMarkdownText(const QString& markdown) {
    markdown_ = markdown;
    render();
}

void MarkdownText::render() {
    const MarkdownRecipe r = markdownRecipe(theme());
    setMarkdown(markdown_);
    styleMarkdown(document(), r, Appearance::instance().zoom(), font(),
                  Profile::current().monoFamily());
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, r.text);
    palette.setColor(QPalette::Link, r.link);
    setPalette(palette);
    fit();
}

int MarkdownText::heightForWidth(int width) const {
    QTextDocument* copy = document()->clone();
    copy->setTextWidth(width);
    const int height = qCeil(copy->size().height());
    delete copy;
    return height;
}

QSize MarkdownText::sizeHint() const {
    const int width = qRound(scaled(Tokens::px(u"space-16")) * 8);
    return {width, heightForWidth(width)};
}

void MarkdownText::resizeEvent(QResizeEvent* event) {
    QTextBrowser::resizeEvent(event);
    fit();
}

void MarkdownText::fit() {
    document()->setTextWidth(viewport()->width());
    const int height = qCeil(document()->size().height());
    if (height != minimumHeight())
        setMinimumHeight(height);
    setMaximumHeight(height);
}

namespace {

// A top-level quote: Markdown on the quote surface with the accent bar on its leading edge.
class Quote final : public QWidget {
  public:
    Quote(const QString& markdown, QWidget* parent) : QWidget(parent) {
        auto* column = new QVBoxLayout(this);
        text_ = new MarkdownText(markdown, this);
        column->addWidget(text_);
        connect(&Appearance::instance(), &Appearance::themeChanged, this, &Quote::restyle);
        connect(&Appearance::instance(), &Appearance::zoomChanged, this, &Quote::restyle);
        restyle();
    }

    [[nodiscard]] MarkdownText* text() const { return text_; }

  protected:
    void paintEvent(QPaintEvent*) override {
        const MarkdownRecipe recipe = markdownRecipe(Appearance::instance().theme());
        const qreal margin = Appearance::instance().scaled(recipe.paragraphSpacing);
        const QRectF surface = QRectF(rect()).adjusted(0, margin, 0, -margin);
        QPainter painter(this);
        painter.fillRect(surface, recipe.quoteBackground);
        painter.fillRect(QRectF(surface.left(), surface.top(),
                                Appearance::instance().scaled(recipe.quoteBorderWidth),
                                surface.height()),
                         recipe.quoteBorder);
    }

  private:
    void restyle() {
        const MarkdownRecipe recipe = markdownRecipe(Appearance::instance().theme());
        const Appearance& appearance = Appearance::instance();
        const int padding = qRound(appearance.scaled(recipe.quotePadding));
        const int margin = qRound(appearance.scaled(recipe.paragraphSpacing));
        layout()->setContentsMargins(padding + qRound(appearance.scaled(recipe.quoteBorderWidth)),
                                     padding + margin, padding, padding + margin);
        update();
    }

    MarkdownText* text_;
};

} // namespace

MarkdownView::MarkdownView(const QString& markdown, QWidget* parent)
    : QWidget(parent), layout_(new QVBoxLayout(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);
    // Code blocks carry their own margins; prose blocks end without spacing.
    layout_->setSpacing(0);
    setMarkdown(markdown);
}

void MarkdownView::setMarkdown(const QString& markdown) {
    markdown_ = markdown;
    while (QLayoutItem* item = layout_->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    for (const MarkdownSegment& segment : splitMarkdown(markdown)) {
        if (segment.kind == MarkdownSegment::Kind::Code) {
            layout_->addWidget(new CodeBlock(segment.text, segment.language, this));
            continue;
        }
        if (segment.kind == MarkdownSegment::Kind::Quote) {
            auto* quote = new Quote(segment.text, this);
            connect(quote->text(), &QTextBrowser::anchorClicked, this,
                    &MarkdownView::linkActivated);
            layout_->addWidget(quote);
            continue;
        }
        auto* text = new MarkdownText(segment.text, this);
        connect(text, &QTextBrowser::anchorClicked, this, &MarkdownView::linkActivated);
        layout_->addWidget(text);
    }
}

} // namespace ket
