// SPDX-License-Identifier: MPL-2.0
// Building a kit widget must not make a window of its own. A child created without a parent
// and shown before it joins a layout becomes a real top-level window for a moment (on macOS
// a native one, created and hidden again while the pointer is reset each time), so every
// public Widgets component is built here under a parent that is not shown, and under one
// that is.
#include "KetActivityGroup.h"
#include "KetActivityIndicator.h"
#include "KetActivityRow.h"
#include "KetAppearance.h"
#include "KetAttachmentChip.h"
#include "KetAvatar.h"
#include "KetBadge.h"
#include "KetBanner.h"
#include "KetButton.h"
#include "KetChangedFiles.h"
#include "KetChoice.h"
#include "KetChrome.h"
#include "KetCommandPalette.h"
#include "KetComposer.h"
#include "KetConversationRow.h"
#include "KetDialog.h"
#include "KetDiffView.h"
#include "KetEmptyState.h"
#include "KetExpandableText.h"
#include "KetFieldRow.h"
#include "KetFileRow.h"
#include "KetGallery.h"
#include "KetIcon.h"
#include "KetLightbox.h"
#include "KetLightboxThumb.h"
#include "KetMarkdownView.h"
#include "KetMentionPicker.h"
#include "KetMeter.h"
#include "KetNavItem.h"
#include "KetPopover.h"
#include "KetQuestionCard.h"
#include "KetRouteTabs.h"
#include "KetSegmentedControl.h"
#include "KetSelect.h"
#include "KetSidebarTree.h"
#include "KetStepSlider.h"
#include "KetSuggestion.h"
#include "KetTag.h"
#include "KetTaskBar.h"
#include "KetTextArea.h"
#include "KetTextField.h"
#include "KetThemeCard.h"
#include "KetToast.h"
#include "KetToggle.h"
#include "KetTranscript.h"
#include "KetTreeItem.h"
#include "KetUpdateCard.h"

#include <QApplication>
#include <QEvent>
#include <QLabel>
#include <QPointer>
#include <QTest>

#include <functional>

using namespace ket;

namespace {

using Factory = std::function<QWidget*(QWidget* parent)>;

// Counts the widgets that are windows while they are shown or given a native window: what
// the screen would see flash up, whatever they are later reparented to.
class WindowWatcher final : public QObject {
  public:
    WindowWatcher() { qApp->installEventFilter(this); }
    ~WindowWatcher() override { qApp->removeEventFilter(this); }

    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::Show || event->type() == QEvent::WinIdChange ||
            event->type() == QEvent::PlatformSurface) {
            if (const auto* widget = qobject_cast<QWidget*>(watched);
                widget != nullptr && widget->isWindow())
                names.append(QString::fromLatin1(widget->metaObject()->className()));
        }
        return false;
    }

    QStringList names;
};

template <typename T, typename... Args> Factory make(Args... args) {
    return [=](QWidget* parent) -> QWidget* { return new T(args..., parent); };
}

// Components that take the window they overlay instead of a parent.
template <typename T, typename... Args> Factory overlay(Args... args) {
    return [=](QWidget* anchor) -> QWidget* { return new T(args..., anchor); };
}

QList<QPair<QString, Factory>> components() {
    const QString text = QStringLiteral("Text");
    const QString longer = QStringLiteral("A longer line of text that has to wrap");
    QList<QPair<QString, Factory>> list = {
        {"ActivityGroup", make<ActivityGroup>(text)},
        {"ActivityIndicator", make<ActivityIndicator>()},
        {"ActivityRow", make<ActivityRow>(text, QStringLiteral("file"))},
        {"AttachmentChip", make<AttachmentChip>(text, QStringLiteral("file"))},
        {"Avatar", make<Avatar>(text)},
        {"Badge", make<Badge>(text, BadgeTone::Accent)},
        {"Banner (info, with message)", make<Banner>(BannerTone::Info, text, longer)},
        {"Banner (warning, with message)", make<Banner>(BannerTone::Warning, text, longer)},
        {"Banner (no message)", make<Banner>(BannerTone::Danger, text, QString())},
        {"Button", make<Button>(text, ButtonVariant::Primary)},
        {"IconButton", make<IconButton>(QIcon(), text)},
        {"ChangedFileRow", make<ChangedFileRow>(text, QStringLiteral("src/a.cpp"), 3, 1)},
        {"ChangedFiles", make<ChangedFiles>()},
        {"Choice", make<Choice>(text, longer, ToggleKind::Radio)},
        {"Checkbox", make<Checkbox>(text)},
        {"Radio", make<Radio>(text)},
        {"StatusItem", make<StatusItem>(text, QStringLiteral("file"))},
        {"StatusBar", make<StatusBar>()},
        {"Breadcrumb", make<Breadcrumb>()},
        {"PaneHeader", make<PaneHeader>(text, QStringLiteral("file"))},
        {"Composer", make<Composer>()},
        {"ConversationRow", make<ConversationRow>(text)},
        {"DiffView", make<DiffView>()},
        {"EmptyState", make<EmptyState>(QStringLiteral("file"), text, longer)},
        {"ExpandableText", make<ExpandableText>()},
        {"FieldRow",
         [](QWidget* parent) -> QWidget* {
             return new FieldRow(QStringLiteral("Label"), new QWidget, QStringLiteral("Help"),
                                 parent);
         }},
        {"FileRow", make<FileRow>(text, FileRow::Kind::File, 0)},
        {"Gallery", make<Gallery>()},
        {"Icon", make<Icon>(QStringLiteral("file"), 16.0)},
        {"LightboxThumb", make<LightboxThumb>(QPixmap(), text)},
        {"MarkdownView",
         make<MarkdownView>(QStringLiteral("# Title\n\ntext\n\n```cpp\nint a;\n```"))},
        {"MarkdownText", make<MarkdownText>(QStringLiteral("text"))},
        {"CodeBlock", make<CodeBlock>(QStringLiteral("int a;"), QStringLiteral("cpp"))},
        {"MentionPicker", make<MentionPicker>()},
        {"UsageRing", make<UsageRing>()},
        {"Meter", make<Meter>(text)},
        {"NavItem", make<NavItem>(text, QStringLiteral("file"))},
        {"Kbd", make<Kbd>(text)},
        {"Popover", make<Popover>(text)},
        {"QuestionCard", make<QuestionCard>(longer)},
        {"RouteTabs", make<RouteTabs>()},
        {"SegmentedControl",
         make<SegmentedControl>(QStringList{QStringLiteral("One"), QStringLiteral("Two")})},
        {"Select", make<Select>()},
        {"SidebarRepositoryHeader", make<SidebarRepositoryHeader>(text)},
        {"SidebarTeamBranch", make<SidebarTeamBranch>(text)},
        {"SidebarSectionLabel", make<SidebarSectionLabel>(text)},
        {"SidebarRow", make<SidebarRow>(text)},
        {"SidebarTreeGuide", make<SidebarTreeGuide>(1)},
        {"StepSlider", make<StepSlider>(text)},
        {"Suggestion", make<Suggestion>(text, longer, QStringLiteral("file"))},
        {"Tag", make<Tag>(text, QColor())},
        {"TaskRow", make<TaskRow>(text, QStringLiteral("terminal"))},
        {"TaskBar", make<TaskBar>()},
        {"TextArea", make<TextArea>()},
        {"TextField", make<TextField>()},
        {"ThemeCard", make<ThemeCard>(text, Theme::Dark)},
        {"AccentSwatch", make<AccentSwatch>()},
        {"ReadingText", make<ReadingText>(longer)},
        {"UserMessage", make<UserMessage>(longer)},
        {"AssistantMessage", make<AssistantMessage>(text, QStringLiteral("09:41"))},
        {"DayDivider", make<DayDivider>(text)},
        {"TreeItem", make<TreeItem>(text, QStringLiteral("file"))},
        {"GroupHeader", make<GroupHeader>(text)},
        {"UpdateCard", make<UpdateCard>(text)},
        {"UpdatePill", make<UpdatePill>(text, longer)},
        // These cover the window they open over, as a child of it.
        {"Dialog", overlay<Dialog>(text)},
        {"Toast", overlay<Toast>(text)},
        {"CommandPalette", [](QWidget* anchor) -> QWidget* { return new CommandPalette(anchor); }},
        {"Lightbox", [](QWidget* anchor) -> QWidget* { return new Lightbox(anchor); }},
    };
    return list;
}

// Top-level widgets that are on screen or have a native window. A hidden popup a component
// owns (the lightbox's "more" menu) is a top-level widget too, but never reaches the screen.
int windowsInUse() {
    int count = 0;
    for (const QWidget* widget : QApplication::topLevelWidgets())
        if (widget->isVisible() || widget->testAttribute(Qt::WA_WState_Created))
            ++count;
    return count;
}

} // namespace

class NoStrayWindowsTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() { Appearance::instance(); }

    void everyComponentIsBuiltWithoutAWindow_data() {
        QTest::addColumn<int>("index");
        QTest::addColumn<bool>("parentShown");
        const auto list = components();
        for (int i = 0; i < list.size(); ++i) {
            QTest::addRow("%s, parent hidden", qPrintable(list[i].first)) << i << false;
            QTest::addRow("%s, parent shown", qPrintable(list[i].first)) << i << true;
        }
    }

    void everyComponentIsBuiltWithoutAWindow() {
        QFETCH(int, index);
        QFETCH(bool, parentShown);
        const auto list = components();
        QWidget parent;
        parent.resize(420, 320);
        if (parentShown) {
            parent.show();
            QVERIFY(QTest::qWaitForWindowExposed(&parent));
        }
        const int windows = QGuiApplication::allWindows().size();
        const int topLevels = windowsInUse();
        QStringList stray;
        QPointer<QWidget> widget;
        {
            WindowWatcher watcher;
            widget = list[index].second(&parent);
            QVERIFY(widget);
            stray = watcher.names;
        }
        QVERIFY2(stray.isEmpty(), qPrintable("shown as a window while built: " + stray.join(", ")));
        QCOMPARE(QGuiApplication::allWindows().size(), windows);
        QCOMPARE(windowsInUse(), topLevels);
        if (!parentShown) {
            QVERIFY(!widget->testAttribute(Qt::WA_WState_Created));
            const auto children = widget->findChildren<QWidget*>();
            for (const QWidget* child : children)
                QVERIFY2(!child->testAttribute(Qt::WA_WState_Created),
                         qPrintable(QString::fromLatin1(child->metaObject()->className())));
        }
        delete widget;
    }

    void bannerMessageStaysOnOffWithTheText() {
        QWidget parent;
        Banner banner(BannerTone::Info, QStringLiteral("Title"), QString(), &parent);
        parent.resize(420, 120);
        parent.show();
        QVERIFY(QTest::qWaitForWindowExposed(&parent));
        const auto labels = banner.findChildren<QLabel*>();
        // Title and message are the banner's two labels.
        QLabel* message = nullptr;
        for (QLabel* label : labels)
            if (label->text() != banner.title())
                message = label;
        QVERIFY(message);
        QVERIFY(!message->isVisible());
        banner.setMessage(QStringLiteral("Details"));
        QVERIFY(message->isVisible());
        banner.setMessage(QString());
        QVERIFY(!message->isVisible());
        Banner withMessage(BannerTone::Warning, QStringLiteral("Title"), QStringLiteral("More"),
                           &parent);
        withMessage.show();
        QCOMPARE(withMessage.findChildren<QLabel*>().size(), 2);
        for (QLabel* label : withMessage.findChildren<QLabel*>())
            QVERIFY(label->isVisible());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    NoStrayWindowsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_no_stray_windows.moc"
