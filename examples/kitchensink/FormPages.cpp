// SPDX-License-Identifier: MPL-2.0
#include "FormPages.h"

#include "KetChoice.h"
#include "KetFieldRow.h"
#include "KetSelect.h"
#include "KetTextField.h"
#include "KetToggle.h"
#include "KitchenSinkCommon.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace ket::kitchensink {
namespace {

struct StateColumn {
    const char* label;
    InteractionState state;
};

const StateColumn kFieldStates[] = {
    {"Rest", {}},
    {"Hover", {true, false, true, false}},
    {"Focus", {false, false, true, true}},
    {"Disabled", {false, false, false, false}},
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

QGridLayout* grid(QWidget* host) {
    auto* layout = new QGridLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(qRound(Tokens::px(u"space-4")));
    layout->setVerticalSpacing(qRound(Tokens::px(u"space-3")));
    return layout;
}

// Text fields, search fields and selects in each pinned state, plus an invalid field.
QWidget* fieldMatrix(QWidget* parent) {
    auto* host = new QWidget(parent);
    QGridLayout* layout = grid(host);
    for (int c = 0; c < int(std::size(kFieldStates)); ++c)
        layout->addWidget(caption(QString::fromLatin1(kFieldStates[c].label), host), 0, c + 1);
    layout->addWidget(caption(QStringLiteral("Invalid"), host), 0,
                      int(std::size(kFieldStates)) + 1);
    const char* rows[] = {"Text field", "Search", "Select"};
    for (int r = 0; r < 3; ++r) {
        layout->addWidget(caption(QString::fromLatin1(rows[r]), host), r + 1, 0);
        for (int c = 0; c <= int(std::size(kFieldStates)); ++c) {
            const bool invalid = c == int(std::size(kFieldStates));
            if (invalid && r == 2)
                continue;
            const InteractionState state = invalid ? InteractionState{} : kFieldStates[c].state;
            QWidget* control = nullptr;
            if (r < 2) {
                auto* field = new TextField(host);
                field->setPlaceholderText(r == 0 ? QStringLiteral("Name")
                                                 : QStringLiteral("Search"));
                if (r == 1)
                    field->setLeadingIcon(QStringLiteral("search"));
                field->setPreviewState(state);
                field->setInvalid(invalid);
                field->setEnabled(state.enabled);
                control = field;
            } else {
                auto* select = new Select(host);
                select->addItems({QStringLiteral("KÉT Sans"), QStringLiteral("Menlo")});
                select->setPreviewState(state);
                select->setEnabled(state.enabled);
                control = select;
            }
            control->setFixedWidth(qRound(Tokens::px(u"space-16") * 2.5));
            layout->addWidget(control, r + 1, c + 1);
        }
    }
    return host;
}

QWidget* toggleMatrix(QWidget* parent) {
    auto* host = new QWidget(parent);
    QGridLayout* layout = grid(host);
    const StateColumn states[] = {
        {"Rest", {}},
        {"Hover", {true, false, true, false}},
        {"Focus", {false, false, true, true}},
        {"Disabled", {false, false, false, false}},
    };
    for (int c = 0; c < int(std::size(states)); ++c)
        layout->addWidget(caption(QString::fromLatin1(states[c].label), host), 0, c + 1);
    int row = 1;
    for (const ToggleKind kind : {ToggleKind::Checkbox, ToggleKind::Radio}) {
        for (const bool checked : {false, true}) {
            layout->addWidget(caption(QStringLiteral("%1 · %2").arg(
                                          kind == ToggleKind::Radio ? u"Radio" : u"Checkbox",
                                          checked ? u"on" : u"off"),
                                      host),
                              row, 0);
            for (int c = 0; c < int(std::size(states)); ++c) {
                Toggle* toggle =
                    kind == ToggleKind::Radio
                        ? static_cast<Toggle*>(new Radio(QStringLiteral("Label"), host))
                        : new Checkbox(QStringLiteral("Label"), host);
                toggle->setAutoExclusive(false);
                toggle->setChecked(checked);
                toggle->setPreviewState(states[c].state);
                toggle->setEnabled(states[c].state.enabled);
                layout->addWidget(toggle, row, c + 1);
            }
            ++row;
        }
    }
    return host;
}

// The mock's update preferences (checkboxes) and permission mode (radios).
QWidget* choiceList(QWidget* parent) {
    auto* host = new QWidget(parent);
    host->setFixedWidth(qRound(Tokens::px(u"space-16") * 7));
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-2")));
    auto* wrapped = new Checkbox(
        QStringLiteral("A checkbox whose long label wraps under itself in a narrow column"), host);
    wrapped->setWordWrap(true);
    column->addWidget(wrapped);
    auto* autoCheck =
        new Choice(QStringLiteral("Check for updates automatically"),
                   QStringLiteral("Once a day while KetPlus is open."), ToggleKind::Checkbox, host);
    autoCheck->setChecked(true);
    column->addWidget(autoCheck);
    column->addWidget(new Choice(QStringLiteral("Download updates in the background"),
                                 QStringLiteral("You still choose when KetPlus restarts."),
                                 ToggleKind::Checkbox, host));
    auto* group = new QButtonGroup(host);
    const std::pair<const char*, const char*> modes[] = {
        {"Read only", "The agent can read files and run safe commands."},
        {"Workspace write", "Edits inside this worktree; asks before anything else."},
        {"Full access", "No sandbox. Use only on a machine you trust."}};
    for (const auto& [title, description] : modes) {
        auto* choice = new Choice(QString::fromLatin1(title), QString::fromLatin1(description),
                                  ToggleKind::Radio, host);
        group->addButton(choice);
        choice->setChecked(QLatin1String(title) == QLatin1String("Workspace write"));
        column->addWidget(choice);
    }
    return host;
}

QWidget* settingsForm(QWidget* parent) {
    auto* host = new QWidget(parent);
    host->setFixedWidth(qRound(Tokens::px(u"space-16") * 9));
    auto* column = new QVBoxLayout(host);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(qRound(Tokens::px(u"space-5")));
    auto* font = new Select;
    font->addItem(QStringLiteral("KÉT Sans"));
    column->addWidget(
        new FieldRow(QStringLiteral("Interface font"), font,
                     QStringLiteral("KetPlus uses KÉT Sans throughout the interface."), host));
    auto* width = new Select;
    width->addItems({QStringLiteral("Default · 760 px"), QStringLiteral("Wide · 864 px"),
                     QStringLiteral("Extra wide · 1200 px"), QStringLiteral("Full width")});
    column->addWidget(new FieldRow(
        QStringLiteral("Conversation width"), width,
        QStringLiteral("Applies to Agent Workspace. Atlas keeps its compact reading column."),
        host));
    auto* name = new TextField;
    name->setText(QStringLiteral("ketplus desktop"));
    auto* nameRow = new FieldRow(QStringLiteral("Worktree name"), name, {}, host);
    nameRow->setError(QStringLiteral("Use letters, numbers and dashes only."));
    column->addWidget(nameRow);
    auto* branch = new TextField;
    branch->setText(QStringLiteral("integration"));
    auto* stacked =
        new FieldRow(QStringLiteral("Integration branch (stacked)"), branch,
                     QStringLiteral("The label sits above the control in a narrow form."), host);
    stacked->setStacked(true);
    column->addWidget(stacked);
    return host;
}

QWidget* quick(const QString& mode, QWidget* parent) {
    return quickView(QStringLiteral("FormSamples"), parent, {{QStringLiteral("mode"), mode}});
}

} // namespace

QWidget* formPage() {
    PageFrame frame = makePage(
        QStringLiteral("Forms"),
        QStringLiteral("Text fields, search, selects, checkboxes, radios, choice cards and field "
                       "rows, from ket::fieldRecipe, ket::toggleRecipe, ket::choiceRecipe and "
                       "ket::fieldRowRecipe. Pinned states use setPreviewState() / previewState."));
    frame.body->addWidget(sectionTitle(QStringLiteral("Fields and selects"), frame.page));
    frame.body->addWidget(
        toolkits(fieldMatrix(frame.page), quick(QStringLiteral("fields"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Checkbox and radio"), frame.page));
    frame.body->addWidget(toolkits(toggleMatrix(frame.page),
                                   quick(QStringLiteral("toggles"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Choice cards · live"), frame.page));
    frame.body->addWidget(
        toolkits(choiceList(frame.page), quick(QStringLiteral("choices"), frame.page), frame.page));
    frame.body->addWidget(sectionTitle(QStringLiteral("Field rows · live"), frame.page));
    frame.body->addWidget(
        toolkits(settingsForm(frame.page), quick(QStringLiteral("rows"), frame.page), frame.page));
    frame.body->addStretch();
    return frame.page;
}

} // namespace ket::kitchensink
