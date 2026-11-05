// SPDX-License-Identifier: MPL-2.0
// The rich transcript in Qt Widgets: Markdown answers, code blocks, the @agent/ mention
// picker and the usage ring.
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetComposer.h"
#include "KetMarkdownView.h"
#include "KetMentionPicker.h"
#include "KetStyle.h"

#include <QApplication>
#include <QClipboard>
#include <QSignalSpy>
#include <QTest>
#include <QTextEdit>

using namespace ket;

class RichWidgetsTest final : public QObject {
    Q_OBJECT
  private slots:
    void init() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1.0);
    }

    void markdownViewsSplitCodeBlocks() {
        MarkdownView view(QStringLiteral("Intro with a [link](https://ketviet.vn).\n\n"
                                         "```sh\nctest --test-dir build\n```\n\nOutro."));
        const QList<CodeBlock*> blocks = view.findChildren<CodeBlock*>();
        QCOMPARE(blocks.size(), 1);
        QCOMPARE(blocks.first()->language(), QStringLiteral("sh"));
        QCOMPARE(view.findChildren<MarkdownText*>().size(), 2);
        MarkdownView quoted(QStringLiteral("Lead\n\n> A quote with [a link](https://ketviet.vn)."));
        QSignalSpy links(&quoted, &MarkdownView::linkActivated);
        QCOMPARE(quoted.findChildren<MarkdownText*>().size(), 2);
        emit quoted.findChildren<MarkdownText*>().last()->anchorClicked(
            QUrl(QStringLiteral("https://ketviet.vn")));
        QCOMPARE(links.count(), 1);
        view.resize(600, 400);
        QVERIFY(!view.grab().isNull());
        auto* prose = view.findChildren<MarkdownText*>().first();
        QVERIFY(prose->heightForWidth(200) > prose->heightForWidth(600) ||
                prose->heightForWidth(200) == prose->heightForWidth(600));
        QVERIFY(blocks.first()->heightForWidth(600) > 2 * 17);

        QSignalSpy copied(blocks.first(), &CodeBlock::copied);
        blocks.first()->copyButton()->click();
        QCOMPARE(copied.count(), 1);
        QCOMPARE(QGuiApplication::clipboard()->text(), QStringLiteral("ctest --test-dir build"));
    }

    void composersCompleteAgentMentions() {
        QWidget window;
        window.resize(800, 600);
        auto* composer = new Composer(&window);
        composer->setGeometry(100, 400, 600, composer->sizeHint().height());
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        QSignalSpy queries(composer, &Composer::mentionQueryChanged);
        QSignalSpy closed(composer, &Composer::mentionClosed);
        composer->editor()->setFocus();
        QTest::keyClicks(composer->editor(), QStringLiteral("Ask @agent/4"));
        QVERIFY(!queries.isEmpty());
        QCOMPARE(queries.last().first().toString(), QStringLiteral("4"));
        composer->setMentionItems({{QStringLiteral("4821"), QStringLiteral("Navigation"),
                                    QStringLiteral("ketplus · develop")},
                                   {QStringLiteral("4310"), QStringLiteral("Startup"),
                                    QStringLiteral("ketplus · develop")}});
        QVERIFY(composer->isMentionOpen());
        MentionPicker* picker = composer->mentionPicker();
        QCOMPARE(picker->parentWidget(), &window);
        // The menu overlaps the composer's top edge from above.
        QVERIFY(picker->geometry().bottom() > composer->geometry().top());
        QVERIFY(picker->geometry().top() < composer->geometry().top());
        QTest::keyClick(composer->editor(), Qt::Key_Down);
        QCOMPARE(picker->highlightedIndex(), 1);
        QTest::keyClick(composer->editor(), Qt::Key_Return);
        QCOMPARE(composer->text(), QStringLiteral("Ask @agent/4310 "));
        QVERIFY(!composer->isMentionOpen());

        QTest::keyClicks(composer->editor(), QStringLiteral("@agent/"));
        composer->setMentionItems({});
        QVERIFY(composer->isMentionOpen());
        QTest::keyClick(composer->editor(), Qt::Key_Escape);
        QVERIFY(!composer->isMentionOpen());
        QVERIFY(!closed.isEmpty());
    }

    void usageRingsReportTheirShare() {
        UsageRing ring;
        QCOMPARE(ring.sizeHint(), QSize(19, 19));
        ring.setValue(0.42);
        QCOMPARE(ring.accessibleName(), QStringLiteral("Context 42% used"));
        ring.setValue(3);
        QCOMPARE(ring.value(), 1.0);
        ring.resize(ring.sizeHint());
        QVERIFY(!ring.grab().isNull());
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    RichWidgetsTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_rich_widgets.moc"
