// SPDX-License-Identifier: MPL-2.0
// KetQt gallery: the Widgets and Qt Quick components side by side, so a change to a
// recipe can be compared on macOS, Windows and Linux. --dark starts in dark mode and
// --screenshot=<file> saves the window and exits (for review evidence).
#include "KetAppearance.h"
#include "KetBadge.h"
#include "KetButton.h"
#include "KetStyle.h"

#include <QApplication>
#include <QCheckBox>
#include <QCommandLineParser>
#include <QHBoxLayout>
#include <QLabel>
#include <QQuickWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

QWidget* widgetsRow(QWidget* parent) {
    auto* panel = new QWidget(parent);
    auto* column = new QVBoxLayout(panel);
    column->addWidget(new QLabel(QStringLiteral("Qt Widgets"), panel));
    auto* buttons = new QHBoxLayout;
    buttons->addWidget(new Button(QStringLiteral("Primary"), ButtonVariant::Primary, panel));
    buttons->addWidget(new Button(QStringLiteral("Secondary"), ButtonVariant::Secondary, panel));
    buttons->addWidget(new Button(QStringLiteral("Ghost"), ButtonVariant::Ghost, panel));
    buttons->addWidget(new Button(QStringLiteral("Delete"), ButtonVariant::Danger, panel));
    auto* disabled = new Button(QStringLiteral("Disabled"), ButtonVariant::Secondary, panel);
    disabled->setEnabled(false);
    buttons->addWidget(disabled);
    buttons->addStretch();
    column->addLayout(buttons);
    auto* badges = new QHBoxLayout;
    const std::pair<const char*, BadgeTone> tones[] = {
        {"Neutral", BadgeTone::Neutral}, {"Accent", BadgeTone::Accent},
        {"Info", BadgeTone::Info},       {"Success", BadgeTone::Success},
        {"Warning", BadgeTone::Warning}, {"Danger", BadgeTone::Danger}};
    for (const auto& [label, tone] : tones)
        badges->addWidget(new Badge(QString::fromLatin1(label), tone, panel));
    badges->addStretch();
    column->addLayout(badges);
    return panel;
}

} // namespace

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    QCommandLineParser parser;
    parser.addOption({QStringLiteral("dark"), QStringLiteral("Start in dark mode.")});
    parser.addOption({QStringLiteral("screenshot"),
                      QStringLiteral("Save the window to <file> and exit."),
                      QStringLiteral("file")});
    parser.process(application);
    Appearance::instance().setMode(parser.isSet(QStringLiteral("dark")) ? Appearance::Mode::Dark
                                                                        : Appearance::Mode::Light);
    Style::install(application);

    QWidget window;
    window.setWindowTitle(QStringLiteral("KetQt gallery · %1").arg(Tokens::source()));
    auto* layout = new QVBoxLayout(&window);
    auto* dark = new QCheckBox(QStringLiteral("Dark"), &window);
    dark->setChecked(Appearance::instance().dark());
    QObject::connect(dark, &QCheckBox::toggled, [](bool on) {
        Appearance::instance().setMode(on ? Appearance::Mode::Dark : Appearance::Mode::Light);
    });
    layout->addWidget(dark);
    layout->addWidget(widgetsRow(&window));
    auto* quick = new QQuickWidget(&window);
    quick->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quick->loadFromModule(QStringLiteral("KetGallery"), QStringLiteral("Gallery"));
    quick->setMinimumSize(560, 140);
    layout->addWidget(quick);
    window.resize(640, 320);
    window.show();

    if (const QString file = parser.value(QStringLiteral("screenshot")); !file.isEmpty()) {
        QTimer::singleShot(300, &window, [&window, file] {
            window.grab().save(file);
            QApplication::quit();
        });
    }
    return QApplication::exec();
}
