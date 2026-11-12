// SPDX-License-Identifier: MPL-2.0
#include "TextPages.h"

#include "KetExpandableText.h"
#include "KetMentionTokens.h"
#include "KetTextArea.h"
#include "KitchenSinkCommon.h"

#include <QGridLayout>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

const QString kPlaceholder = QStringLiteral("Add context, links or @agent/ mentions");

const QString kLive = QStringLiteral("Ask @agent/2035 how the sidebar groups conversations.\n"
                                     "Keep the release notes short; @agent/9999 is unknown.");

const QString kLong =
    QStringLiteral("Ship the new navigation and the repository picker.\n"
                   "@agent/2035 owns the sidebar; ask @agent/9410 about the startup flow.\n"
                   "Settings keep their sections, with zoom and text size applied at once.\n"
                   "The release notes stay in English.\n"
                   "Every change lands behind review, and the leader merges.");

// Sample conversations for the live picker.
QList<MentionItem> conversations(const QString& query) {
    const QList<MentionItem> all = {
        {QStringLiteral("2035"), QStringLiteral("Refine navigation"), QStringLiteral("desktop")},
        {QStringLiteral("9410"), QStringLiteral("Review the startup flow"),
         QStringLiteral("develop")},
        {QStringLiteral("4821"), QStringLiteral("Prepare the release"), QStringLiteral("desktop")}};
    QList<MentionItem> shown;
    for (const MentionItem& item : all)
        if (item.number.startsWith(query) || item.title.contains(query, Qt::CaseInsensitive))
            shown.append(item);
    return shown;
}

struct StateColumn {
    const char* label;
    InteractionState state;
    bool invalid = false;
};

const StateColumn kStates[] = {
    {"Rest", {}},
    {"Hover", {true, false, true, false}},
    {"Focus", {false, false, true, true}},
    {"Disabled", {false, false, false, false}},
    {"Invalid", {}, true},
};

QWidget* caption(const QString& text, QWidget* parent) {
    return new TokenLabel(text, QStringLiteral("text-muted"), QStringLiteral("text-xs"), 500,
                          parent);
}

QWidget* toolkits(QWidget* widgets, QWidget* quick, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-2")));
    for (const auto& [title, content] :
         {std::pair{"Qt Widgets", widgets}, std::pair{"Qt Quick", quick}}) {
        column->addWidget(new TokenLabel(QString::fromLatin1(title),
                                         QStringLiteral("text-secondary"),
                                         QStringLiteral("text-sm"), 600, host));
        column->addWidget(content, 0, Qt::AlignLeft | Qt::AlignTop);
        column->addSpacing(qRound(Tokens::px(u"space-3")));
    }
    return host;
}

QWidget* stateMatrix(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* layout = new QGridLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(qRound(Tokens::px(u"space-4")));
    layout->setVerticalSpacing(qRound(Tokens::px(u"space-2")));
    for (int c = 0; c < int(std::size(kStates)); ++c) {
        layout->addWidget(caption(QString::fromLatin1(kStates[c].label), host), 0, c);
        auto* area = new TextArea(host);
        area->setFixedWidth(qRound(Tokens::px(u"space-16") * 3.5));
        area->setPlaceholderText(kPlaceholder);
        area->setPreviewState(kStates[c].state);
        area->setEnabled(kStates[c].state.enabled);
        area->setInvalid(kStates[c].invalid);
        if (c == 1)
            area->setText(QStringLiteral("Line breaks\nare kept"));
        layout->addWidget(area, 1, c, Qt::AlignTop);
    }
    return host;
}

QWidget* liveFields(QWidget* parent) {
    auto* host = new QWidget(parent);
    host->setFixedWidth(qRound(Tokens::px(u"space-16") * 9));
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* area = new TextArea(host);
    area->setAccessibleName(QStringLiteral("Context"));
    area->setPlaceholderText(kPlaceholder);
    area->setMaxLength(160);
    area->setText(kLive);
    area->mentions()->setTitle(QStringLiteral("2035"), QStringLiteral("Refine navigation"));
    area->setUnknownMentions({QStringLiteral("9999")});
    QObject::connect(area, &TextArea::mentionQueryChanged, area,
                     [area](const QString& query) { area->setMentionItems(conversations(query)); });
    column->addWidget(area);
    auto* over = new TextArea(host);
    over->setAccessibleName(QStringLiteral("Summary"));
    over->setMinimumLines(2);
    over->setMaxLength(40);
    over->setText(QStringLiteral("A summary that runs past its limit of forty characters."));
    column->addWidget(over);
    return host;
}

QWidget* expandable(QWidget* parent) {
    auto* host = new QWidget(parent);
    host->setFixedWidth(qRound(Tokens::px(u"space-16") * 9));
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    for (const bool expanded : {false, true}) {
        auto* text = new ExpandableText(host);
        text->setText(kLong);
        text->setMentionTitle(QStringLiteral("2035"), QStringLiteral("Refine navigation"));
        text->setUnknownMentions({QStringLiteral("9410")});
        text->setExpanded(expanded);
        column->addWidget(text);
    }
    return host;
}

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("TextSamples"), parent, {{QStringLiteral("mode"), mode}});
}

} // namespace

QWidget* textPage() {
    PageFrame frame =
        makePage(QStringLiteral("Multi-line text"),
                 QStringLiteral(
                     "The multi-line field (ket::textAreaRecipe) with its character count and "
                     "drawn @agent/ mentions (ket::mentionTokenRecipe), and the expandable text "
                     "(ket::expandableTextRecipe). Type @agent/ in a live field for the picker."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Multi-line field"), frame.page));
    frame.body->addWidget(
        toolkits(stateMatrix(frame.page), quick(QStringLiteral("states"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Count and mentions · live"), frame.page));
    frame.body->addWidget(
        toolkits(liveFields(frame.page), quick(QStringLiteral("live"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Expandable text · live"), frame.page));
    frame.body->addWidget(toolkits(expandable(frame.page),
                                   quick(QStringLiteral("expandable"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
