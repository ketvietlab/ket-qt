// SPDX-License-Identifier: MPL-2.0
#include "RichPages.h"

#include "KetComposer.h"
#include "KetMarkdownView.h"
#include "KetMentionPicker.h"
#include "KitchenSinkCommon.h"

#include <QHBoxLayout>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

// The sample answer, shared with RichSamples.qml.
const char kAnswer[] = R"(### Plan

The sidebar now shows **two lines** per conversation: the title, then the number and as many
whole tags as fit. See [ADR 0028](https://ketplus.app/docs/adr-0028) for the reasoning.

- Hide tags that do not fit and count them in `+N`.
- Keep the status icon when a row has no tags.

```sh
cmake --build build && ctest --test-dir build --output-on-failure
```

> Hidden tags stay searchable from the sidebar filter.)";

QList<MentionItem> conversations() {
    return {{QStringLiteral("4821"), QStringLiteral("Navigation rail polish"),
             QStringLiteral("ketplus · develop")},
            {QStringLiteral("4310"), QStringLiteral("Startup time regression"),
             QStringLiteral("ketplus · perf/startup")},
            {QStringLiteral("5590"), QStringLiteral("Atlas bundle validation"),
             QStringLiteral("ketplus-atlas · main")},
            {QStringLiteral("7142"), QStringLiteral("Sync conflict on resume"),
             QStringLiteral("ketplus · fix/sync")}};
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

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("RichSamples"), parent, {{QStringLiteral("mode"), mode}});
}

QWidget* answer(QWidget* parent) {
    auto* view = new MarkdownView(QString::fromUtf8(kAnswer), parent);
    view->setFixedWidth(640);
    return view;
}

QWidget* mentions(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-4")));
    auto* sample = new MentionPicker(host);
    sample->setItems(conversations());
    sample->setHighlightedIndex(1);
    sample->setFixedSize(sample->sizeHint());
    column->addWidget(sample);

    // Type @agent/ to open the picker above the composer; it filters by number or title.
    auto* field = new Composer(host);
    field->setPlaceholderText(QStringLiteral("Type @agent/ to mention a conversation"));
    QObject::connect(field, &Composer::mentionQueryChanged, field, [field](const QString& query) {
        QList<MentionItem> found;
        for (const MentionItem& item : conversations())
            if (item.number.startsWith(query) || item.title.contains(query, Qt::CaseInsensitive))
                found.append(item);
        field->setMentionItems(found);
    });
    field->setFixedWidth(620);
    column->addWidget(field);
    return host;
}

QWidget* usage(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-5")));
    for (const auto& [value, hovered] : {std::pair{0.12, false}, std::pair{0.46, true},
                                         std::pair{0.84, false}, std::pair{0.97, false}}) {
        auto* pair = new QHBoxLayout;
        pair->setSpacing(qRound(Tokens::px(u"space-2")));
        auto* ring = new UsageRing(host);
        ring->setValue(value);
        ring->setPreviewHovered(hovered);
        pair->addWidget(ring);
        row->addLayout(pair);
        pair->addWidget(new TokenLabel(
            QStringLiteral("%1%%2").arg(qRound(value * 100)).arg(hovered ? " · hover" : ""),
            QStringLiteral("text-muted"), QStringLiteral("text-sm"), 400, host));
    }
    row->addStretch();
    return host;
}

} // namespace

QWidget* richPage() {
    PageFrame frame = makePage(
        QStringLiteral("Rich transcript"),
        QStringLiteral("Markdown answers with code blocks, the @agent/ mention picker and the "
                       "context usage ring, from ket::markdownRecipe, codeBlockRecipe, "
                       "mentionPickerRecipe, mentionOptionRecipe and usageRingRecipe."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Markdown answer"), frame.page));
    frame.body->addWidget(
        toolkits(answer(frame.page), quick(QStringLiteral("answer"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Mention picker · live"), frame.page));
    frame.body->addWidget(
        toolkits(mentions(frame.page), quick(QStringLiteral("mentions"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Context usage"), frame.page));
    frame.body->addWidget(
        toolkits(usage(frame.page), quick(QStringLiteral("usage"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
