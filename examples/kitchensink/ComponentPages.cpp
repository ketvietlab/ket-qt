// SPDX-License-Identifier: MPL-2.0
#include "ComponentPages.h"

#include "KetAppearance.h"
#include "KetBadge.h"
#include "KetButton.h"
#include "KetTag.h"
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
    {"Ghost danger", ButtonVariant::GhostDanger},
};

QIcon sampleIcon(const char* name) {
    return QIcon(QStringLiteral(":/qt/qml/KetKitchenSink/icons/%1.svg").arg(QLatin1String(name)));
}

// Sample tag colours in light and dark. Tag colours belong to the app (KetPlus keeps them
// with each tag), so the kit takes them as a parameter; these mirror the KetPlus mock.
struct SampleTag {
    const char* label;
    QRgb light;
    QRgb dark;
};

const SampleTag kSampleTags[] = {
    {"Blue", 0x2f6fde, 0x7aa7ff},  {"Green", 0x23875a, 0x5fcf95}, {"Violet", 0x7a55d6, 0xb39dff},
    {"Amber", 0xb7791f, 0xf0b45a}, {"Teal", 0x1c8a92, 0x5fc9cf},  {"Red", 0xd1453b, 0xff8a80},
    {"Pink", 0xc2417f, 0xf28cc0},  {"Slate", 0x64748b, 0xa3b0c2},
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

QWidget* iconButtonMatrix(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* grid = new QGridLayout(host);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(qRound(Tokens::px(u"space-4")));
    grid->setVerticalSpacing(qRound(Tokens::px(u"space-3")));
    for (int c = 0; c < int(std::size(kStates)); ++c)
        grid->addWidget(caption(QString::fromLatin1(kStates[c].label), host), 0, c + 1);
    const std::pair<const char*, ControlSize> sizes[] = {{"Medium", ControlSize::Medium},
                                                         {"Small", ControlSize::Small}};
    for (int r = 0; r < int(std::size(sizes)); ++r) {
        grid->addWidget(caption(QString::fromLatin1(sizes[r].first), host), r + 1, 0);
        for (int c = 0; c < int(std::size(kStates)); ++c) {
            auto* button = new IconButton(sampleIcon("close"), QStringLiteral("Close"), host);
            button->setControlSize(sizes[r].second);
            button->setPreviewState(kStates[c].state);
            button->setEnabled(kStates[c].state.enabled);
            grid->addWidget(button, r + 1, c + 1, Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    return host;
}

QWidget* liveIconButtons(QWidget* parent) {
    auto* host = new QWidget(parent);
    auto* row = new QHBoxLayout(host);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(qRound(Tokens::px(u"space-1")));
    row->addWidget(new IconButton(sampleIcon("plus"), QStringLiteral("New conversation"), host));
    row->addWidget(new IconButton(sampleIcon("more"), QStringLiteral("More actions"), host));
    row->addWidget(new IconButton(sampleIcon("close"), QStringLiteral("Close"), host));
    row->addSpacing(qRound(Tokens::px(u"space-4")));
    auto* save = new Button(QStringLiteral("Save changes"), ButtonVariant::Primary, host);
    save->setIcon(sampleIcon("check"));
    row->addWidget(save);
    auto* add = new Button(QStringLiteral("Tag this flow"), ButtonVariant::Ghost, host);
    add->setIcon(sampleIcon("plus"));
    row->addWidget(add);
    row->addStretch();
    return host;
}

QWidget* hboxHost(QWidget* parent, QHBoxLayout** layout) {
    auto* host = new QWidget(parent);
    *layout = new QHBoxLayout(host);
    (*layout)->setContentsMargins(0, 0, 0, 0);
    (*layout)->setSpacing(qRound(Tokens::px(u"space-1")));
    return host;
}

QColor sampleColor(const SampleTag& tag) {
    return QColor(Appearance::instance().dark() ? tag.dark : tag.light);
}

// A row of tags; colours follow the theme like an app's tag palette would.
QWidget* tagRow(QWidget* parent, std::optional<bool> hovered) {
    QHBoxLayout* row = nullptr;
    QWidget* host = hboxHost(parent, &row);
    for (const SampleTag& sample : kSampleTags) {
        auto* tag = new Tag(QString::fromLatin1(sample.label), sampleColor(sample), host);
        tag->setPreviewHovered(hovered);
        QObject::connect(&Appearance::instance(), &Appearance::themeChanged, tag,
                         [tag, sample] { tag->setColor(sampleColor(sample)); });
        row->addWidget(tag);
    }
    row->addStretch();
    return host;
}

QWidget* tagEdgeCases(QWidget* parent) {
    QHBoxLayout* row = nullptr;
    QWidget* host = hboxHost(parent, &row);
    const SampleTag& blue = kSampleTags[0];
    auto* filter = new Tag(QStringLiteral("Filter on hover"), sampleColor(blue), host);
    filter->setFilterable(true);
    auto* longTag =
        new Tag(QStringLiteral("A label longer than the chip allows"), sampleColor(blue), host);
    auto* more = new Tag(QStringLiteral("+2"), {}, host);
    more->setOverflow(true);
    for (Tag* tag : {filter, longTag}) {
        QObject::connect(&Appearance::instance(), &Appearance::themeChanged, tag,
                         [tag] { tag->setColor(sampleColor(kSampleTags[0])); });
    }
    row->addWidget(filter);
    row->addWidget(longTag);
    row->addWidget(more);
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

QWidget* iconButtonPage() {
    PageFrame frame = makePage(
        QStringLiteral("Icon button"),
        QStringLiteral("ket::IconButton and KetIconButton draw ket::iconButtonRecipe: a square, "
                       "borderless button whose label is its accessible name and tooltip. Icons "
                       "take the text colour in every button."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Sizes and states"), frame.page));
    frame.body->addWidget(toolkits(iconButtonMatrix(frame.page),
                                   quickView(QStringLiteral("IconButtonMatrix"), frame.page),
                                   frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Live · with icon buttons"), frame.page));
    frame.body->addWidget(toolkits(liveIconButtons(frame.page),
                                   quickView(QStringLiteral("IconButtonLive"), frame.page),
                                   frame.page));
    frame.body->addStretch();
    return frame.page;
}

QWidget* tagPage() {
    PageFrame frame = makePage(
        QStringLiteral("Tag"),
        QStringLiteral("ket::Tag and KetTag draw ket::tagRecipe: a label-only chip tinted with a "
                       "colour the app owns. Filterable tags tint more on hover; long labels "
                       "elide; the overflow chip counts hidden tags."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Rest"), frame.page));
    auto* restQuick = quickView(QStringLiteral("TagSamples"), frame.page);
    frame.body->addWidget(toolkits(tagRow(frame.page, false), restQuick, frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Hover"), frame.page));
    auto* hoverQuick = quickView(QStringLiteral("TagSamples"), frame.page);
    if (QQuickItem* root = hoverQuick->rootObject())
        root->setProperty("mode", QStringLiteral("hover"));
    frame.body->addWidget(toolkits(tagRow(frame.page, true), hoverQuick, frame.page));
    frame.body->addWidget(
        sectionTitle(QStringLiteral("Filterable, elided and overflow"), frame.page));
    auto* edgeQuick = quickView(QStringLiteral("TagSamples"), frame.page);
    if (QQuickItem* root = edgeQuick->rootObject())
        root->setProperty("mode", QStringLiteral("edge"));
    frame.body->addWidget(toolkits(tagEdgeCases(frame.page), edgeQuick, frame.page));
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
        {"Neutral", BadgeTone::Neutral}, {"Accent (brand)", BadgeTone::Accent},
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
