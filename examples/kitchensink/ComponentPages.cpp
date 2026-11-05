// SPDX-License-Identifier: MPL-2.0
#include "ComponentPages.h"

#include "KetBadge.h"
#include "KetButton.h"
#include "KitchenSinkCommon.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QQuickItem>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

struct StateColumn {
    const char* label;
    InteractionState state;
};

const StateColumn kStates[] = {
    {"Rest", {}},
    {"Hover", {true, false, true, false}},
    {"Pressed", {true, true, true, false}},
    {"Focus", {false, false, true, true}},
    {"Disabled", {false, false, false, false}},
};

const std::pair<const char*, ButtonVariant> kVariants[] = {
    {"Primary", ButtonVariant::Primary},
    {"Secondary", ButtonVariant::Secondary},
    {"Ghost", ButtonVariant::Ghost},
    {"Danger", ButtonVariant::Danger},
};

QWidget* caption(const QString& text, QWidget* parent) {
    return new TokenLabel(text, QStringLiteral("text-muted"), QStringLiteral("text-xs"), 500,
                          parent);
}

// The Widgets version above its Qt Quick twin, each titled with its toolkit.
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

QWidget* buttonMatrix(ControlSize size, QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* grid = new QGridLayout(host);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(qRound(Tokens::px(u"space-3")));
    grid->setVerticalSpacing(qRound(Tokens::px(u"space-3")));
    for (int c = 0; c < int(std::size(kStates)); ++c)
        grid->addWidget(caption(QString::fromLatin1(kStates[c].label), host), 0, c + 1);
    for (int r = 0; r < int(std::size(kVariants)); ++r) {
        grid->addWidget(caption(QString::fromLatin1(kVariants[r].first), host), r + 1, 0);
        for (int c = 0; c < int(std::size(kStates)); ++c) {
            auto* button =
                new Button(QString::fromLatin1(kVariants[r].first), kVariants[r].second, host);
            button->setControlSize(size);
            button->setPreviewState(kStates[c].state);
            button->setEnabled(kStates[c].state.enabled);
            grid->addWidget(button, r + 1, c + 1, Qt::AlignLeft);
        }
    }
    return host;
}

QWidget* liveButtons(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    for (const auto& [label, variant] : kVariants)
        row->addWidget(new Button(QString::fromLatin1(label), variant, host));
    row->addStretch();
    return host;
}

} // namespace

QWidget* buttonPage() {
    PageFrame frame = makePage(
        QStringLiteral("Button"),
        QStringLiteral("ket::Button and KetButton draw ket::buttonRecipe. Each cell pins one "
                       "interaction state with setPreviewState() / previewState."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Medium · control-height-md"), frame.page));
    frame.body->addWidget(toolkits(buttonMatrix(ControlSize::Medium, frame.page),
                                   quickView(QStringLiteral("ButtonMatrix"), frame.page),
                                   frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Small · control-height-sm"), frame.page));
    auto* smallQuick = quickView(QStringLiteral("ButtonMatrix"), frame.page);
    if (QQuickItem* root = smallQuick->rootObject())
        root->setProperty("small", true);
    frame.body->addWidget(
        toolkits(buttonMatrix(ControlSize::Small, frame.page), smallQuick, frame.page));
    frame.body->addWidget(
        sectionTitle(QStringLiteral("Live · hover, press and Tab through"), frame.page));
    frame.body->addWidget(toolkits(
        liveButtons(frame.page), quickView(QStringLiteral("ButtonLive"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

QWidget* badgePage() {
    PageFrame frame = makePage(QStringLiteral("Badge"),
                               QStringLiteral("ket::Badge and KetBadge draw ket::badgeRecipe in "
                                              "every tone. Badges are labels, never buttons."));
    auto* widgets = new QWidget(frame.page);
    auto* row = new QHBoxLayout(widgets);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-1-5")));
    const std::pair<const char*, BadgeTone> tones[] = {
        {"Neutral", BadgeTone::Neutral}, {"Accent", BadgeTone::Accent},
        {"Info", BadgeTone::Info},       {"Success", BadgeTone::Success},
        {"Warning", BadgeTone::Warning}, {"Danger", BadgeTone::Danger}};
    for (const auto& [label, tone] : tones)
        row->addWidget(new Badge(QString::fromLatin1(label), tone, widgets));
    row->addStretch();
    frame.body->addWidget(sectionTitle(QStringLiteral("Tones"), frame.page));
    frame.body->addWidget(
        toolkits(widgets, quickView(QStringLiteral("BadgeTones"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
