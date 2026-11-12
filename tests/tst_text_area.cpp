// SPDX-License-Identifier: MPL-2.0
// The multi-line text field, its drawn `@agent/` mentions and the expandable text in Qt
// Widgets.
#include "KetAppearance.h"
#include "KetButton.h"
#include "KetExpandableText.h"
#include "KetMentionTokens.h"
#include "KetProfile.h"
#include "KetStyle.h"
#include "KetTextArea.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>
#include <QShortcut>
#include <QSignalSpy>
#include <QTest>
#include <QTextBlock>
#include <QTextEdit>
#include <QTextLayout>
#include <QVBoxLayout>

using namespace ket;

namespace {

QColor token(QStringView name) { return Tokens::color(name, Appearance::instance().theme()); }

// The highlighter's colour at `position`, or an invalid colour.
QColor drawnColor(QTextDocument* document, int position) {
    const QTextBlock block = document->findBlock(position);
    for (const QTextLayout::FormatRange& range : block.layout()->formats()) {
        const int at = position - block.position();
        if (at >= range.start && at < range.start + range.length)
            return range.format.foreground().color();
    }
    return {};
}

struct Window {
    QWidget window;
    TextArea* area = new TextArea;
    QTextEdit* edit = nullptr;

    Window() { open(); }
    void open() {
        auto* layout = new QVBoxLayout(&window);
        layout->addWidget(area);
        layout->addStretch(1);
        window.resize(480, 600);
        window.show();
        window.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        edit = area->editor();
        edit->setFocus();
    }
};

} // namespace

class TextAreaTest final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1);
    }

    void recipesComeFromTokensAndTheProfile() {
        const Profile& p = Profile::current();
        const TextAreaRecipe idle = textAreaRecipe({}, false, false, Theme::Light);
        QCOMPARE(idle.field.border, fieldRecipe({}, false, Theme::Light).border);
        QCOMPARE(idle.count, Tokens::color(u"text-muted", Theme::Light));
        QCOMPARE(idle.minLines, p.textAreaMinLines);
        QCOMPARE(idle.maxLines, p.textAreaMaxLines);
        const TextAreaRecipe over = textAreaRecipe({}, false, true, Theme::Dark);
        QCOMPARE(over.field.border, Tokens::color(u"danger", Theme::Dark));
        QCOMPARE(over.count, Tokens::color(u"danger", Theme::Dark));
        QCOMPARE(mentionTokenRecipe(false, Theme::Dark).text,
                 Tokens::color(u"accent-text", Theme::Dark));
        QCOMPARE(mentionTokenRecipe(true, Theme::Light).background,
                 Tokens::color(u"danger-bg", Theme::Light));
        QCOMPARE(expandableTextRecipe(Theme::Light).foldedLines, p.expandableFoldedLines);
        QCOMPARE(characterCountText(12, 8000), QStringLiteral("12 / 8000"));
    }

    void completeMentionsAreFound() {
        const QList<MentionSpan> spans =
            findAgentMentions(u"Ask @agent/2035, (@agent/x-1) mail@agent/9 @agent/");
        QCOMPARE(spans.size(), 2);
        QCOMPARE(spans.at(0).number, QStringLiteral("2035")); // the comma ends it
        QCOMPARE(spans.at(0).start, 4);
        QCOMPARE(spans.at(0).end, 15);
        QCOMPARE(spans.at(1).number, QStringLiteral("x-1"));
    }

    void linesGrowToTheMaximumThenScroll() {
        Window w;
        w.area->setMinimumLines(3);
        w.area->setMaximumLines(5);
        const TextAreaRecipe r = w.area->recipe();
        const int line = qFloor(qRound(r.field.fontPixels) * r.lineHeight);
        const int padding = 2 * qRound(r.paddingBlock);
        QCOMPARE(w.area->visibleLines(), 3);
        QCOMPARE(w.area->height(), 3 * line + padding);
        w.area->setText(QStringLiteral("1\n2\n3\n4"));
        QCOMPARE(w.area->visibleLines(), 4);
        QCOMPARE(w.area->height(), 4 * line + padding);
        w.area->setText(QStringLiteral("1\n2\n3\n4\n5\n6\n7\n8\n9"));
        QCOMPARE(w.area->visibleLines(), 5);
        QCOMPARE(w.area->height(), 5 * line + padding);
        QVERIFY(w.edit->verticalScrollBar()->maximum() > 0);
    }

    void pasteInsertsPlainTextAndKeepsLineBreaks() {
        Window w;
        auto* data = new QMimeData;
        data->setHtml(QStringLiteral("<b>First</b><br>second"));
        data->setText(QStringLiteral("First\nsecond\n\nfourth"));
        QApplication::clipboard()->setMimeData(data);
        w.edit->paste();
        QCOMPARE(w.area->text(), QStringLiteral("First\nsecond\n\nfourth"));
        QTextCursor cursor(w.edit->document());
        cursor.setPosition(1);
        QCOMPARE(cursor.charFormat().fontWeight(), int(QFont::Normal));
    }

    void returnBreaksTheLineAndCommandReturnIsLeftToTheWindow() {
        Window w;
        QTest::keyClicks(w.edit, QStringLiteral("One"));
        QTest::keyClick(w.edit, Qt::Key_Return);
        QTest::keyClicks(w.edit, QStringLiteral("Two"));
        QCOMPARE(w.area->text(), QStringLiteral("One\nTwo"));
        QShortcut shortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), &w.window);
        QSignalSpy activated(&shortcut, &QShortcut::activated);
        // Through the window, as the platform delivers it, so shortcuts apply.
        QTest::keyClick(w.window.windowHandle(), Qt::Key_Return, Qt::ControlModifier);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(w.area->text(), QStringLiteral("One\nTwo"));
        // Without a shortcut the key still does not break the line, with Control or Meta.
        shortcut.setEnabled(false);
        QTest::keyClick(w.edit, Qt::Key_Return, Qt::ControlModifier);
        QTest::keyClick(w.edit, Qt::Key_Return, Qt::MetaModifier);
        QCOMPARE(w.area->text(), QStringLiteral("One\nTwo"));
        // Tab leaves the field.
        auto* next = new TextArea;
        w.window.layout()->addWidget(next);
        next->show(); // added to a shown window
        QTest::keyClick(w.edit, Qt::Key_Tab);
        QVERIFY(next->editor()->hasFocus());
    }

    void theCountMarksTheFieldInvalidAndNeverCuts() {
        Window w;
        const int before = w.area->height();
        w.area->setMaxLength(10);
        QVERIFY(w.area->height() > before); // room for the count
        QTest::keyClicks(w.edit, QStringLiteral("Hello"));
        QVERIFY(!w.area->isOverLimit());
        QCOMPARE(w.edit->accessibleDescription(), QStringLiteral("5 of 10 characters"));
        QTest::keyClicks(w.edit, QStringLiteral(" there, all"));
        QCOMPARE(w.area->text().size(), 16);
        QVERIFY(w.area->isOverLimit());
        QCOMPARE(w.area->recipe().field.border, token(u"danger"));
        QCOMPARE(w.area->recipe().count, token(u"danger"));
        QCOMPARE(w.edit->accessibleDescription(),
                 QStringLiteral("6 characters over the limit of 10"));
        w.area->setMaxLength(0);
        QVERIFY(!w.area->isOverLimit());
        QCOMPARE(w.area->height(), before);
    }

    void aChosenMentionIsAnAtomicToken() {
        Window w;
        QSignalSpy queries(w.area, &TextArea::mentionQueryChanged);
        QTest::keyClicks(w.edit, QStringLiteral("Ask @agent/4"));
        QCOMPARE(queries.last().first().toString(), QStringLiteral("4"));
        w.area->setMentionItems({{QStringLiteral("4310"), QStringLiteral("Refine navigation"),
                                  QStringLiteral("ketplus")}});
        QVERIFY(w.area->isMentionOpen());
        QTest::keyClick(w.edit, Qt::Key_Return);
        QVERIFY(!w.area->isMentionOpen());
        QCOMPARE(w.area->text(), QStringLiteral("Ask @agent/4310 "));
        QCOMPARE(w.area->mentions()->numbers(), QStringList{QStringLiteral("4310")});
        QCOMPARE(drawnColor(w.edit->document(), 6), token(u"accent-text"));
        QVERIFY(w.edit->accessibleDescription().contains(
            QStringLiteral("Refine navigation (agent 4310)")));

        QTest::keyClick(w.edit, Qt::Key_Backspace); // the space after the token
        QCOMPARE(w.area->text(), QStringLiteral("Ask @agent/4310"));
        QVERIFY(!w.area->isMentionOpen()); // resting at a token's end is not a query
        QTest::keyClick(w.edit, Qt::Key_Backspace);
        QCOMPARE(w.area->text(), QStringLiteral("Ask "));
        QVERIFY(w.area->mentions()->numbers().isEmpty());

        w.area->setText(QStringLiteral("@agent/12 hi"));
        QTextCursor cursor = w.edit->textCursor();
        cursor.setPosition(0);
        w.edit->setTextCursor(cursor);
        QTest::keyClick(w.edit, Qt::Key_Delete);
        QCOMPARE(w.area->text(), QStringLiteral(" hi"));
    }

    // Typing at a token's edge changes the mention there, so the token ends.
    void typingAtATokensEdgeEndsIt() {
        Window w;
        w.area->setText(QStringLiteral("Ask @agent/12"));
        w.edit->moveCursor(QTextCursor::End);
        QTest::keyClicks(w.edit, QStringLiteral("3"));
        QCOMPARE(w.area->text(), QStringLiteral("Ask @agent/123"));
        QVERIFY(w.area->mentions()->spans().isEmpty());
        QVERIFY(drawnColor(w.edit->document(), 5) != token(u"accent-text"));
        QTest::keyClick(w.edit, Qt::Key_Backspace); // one character, not a token
        QCOMPARE(w.area->text(), QStringLiteral("Ask @agent/12"));

        w.area->setText(QStringLiteral("@agent/12 hi"));
        w.edit->moveCursor(QTextCursor::Start);
        QTest::keyClicks(w.edit, QStringLiteral("x"));
        QCOMPARE(w.area->text(), QStringLiteral("x@agent/12 hi"));
        QVERIFY(w.area->mentions()->spans().isEmpty());

        // A space keeps the token: the mention is still there.
        w.area->setText(QStringLiteral("@agent/12 hi"));
        w.edit->moveCursor(QTextCursor::Start);
        QTest::keyClicks(w.edit, QStringLiteral(" "));
        QCOMPARE(w.area->mentions()->numbers(), QStringList{QStringLiteral("12")});
        QCOMPARE(w.area->mentions()->spans().first().start, 1);
    }

    void theCaretNeverStopsInsideAToken() {
        Window w;
        w.area->setText(QStringLiteral("a @agent/12 b"));
        QTextCursor cursor = w.edit->textCursor();
        cursor.setPosition(1);
        w.edit->setTextCursor(cursor);
        QTest::keyClick(w.edit, Qt::Key_Right); // to the token's start
        QCOMPARE(w.edit->textCursor().position(), 2);
        QTest::keyClick(w.edit, Qt::Key_Right); // jumps over it
        QCOMPARE(w.edit->textCursor().position(), 11);
        QTest::keyClick(w.edit, Qt::Key_Left);
        QCOMPARE(w.edit->textCursor().position(), 2);
        cursor.setPosition(7); // from the token's start: past it, to its end
        w.edit->setTextCursor(cursor);
        QCOMPARE(w.edit->textCursor().position(), 11);
        cursor.setPosition(2);
        w.edit->setTextCursor(cursor);
        // A space at its edge keeps the token; an edit inside it makes it plain text.
        QTest::keyClicks(w.edit, QStringLiteral(" "));
        QCOMPARE(w.area->mentions()->spans().size(), 1);
        QCOMPARE(w.area->mentions()->spans().first().start, 3);
        cursor = w.edit->textCursor();
        cursor.setPosition(6);
        cursor.setPosition(9, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        QVERIFY(w.area->mentions()->spans().isEmpty());
    }

    void setTextDrawsMentionsAndTypedOnesStayPlain() {
        Window w;
        QTest::keyClicks(w.edit, QStringLiteral("@agent/12 "));
        QVERIFY(w.area->mentions()->spans().isEmpty());
        const QString text = QStringLiteral("Ask @agent/1 and\n@agent/2 now");
        w.area->setText(text);
        QCOMPARE(w.area->mentions()->numbers(),
                 (QStringList{QStringLiteral("1"), QStringLiteral("2")}));
        QCOMPARE(w.area->text(), text);
        w.area->setUnknownMentions({QStringLiteral("2")});
        QCOMPARE(drawnColor(w.edit->document(), 5), token(u"accent-text"));
        QCOMPARE(drawnColor(w.edit->document(), 18), token(u"danger"));
        QCOMPARE(w.area->text(), text);
        QVERIFY(w.edit->accessibleDescription().contains(QStringLiteral("agent 2, unknown")));
    }

    void tokensFollowTheTheme() {
        Window w;
        w.area->setText(QStringLiteral("@agent/7"));
        Appearance::instance().setMode(Appearance::Mode::Dark);
        QCOMPARE(drawnColor(w.edit->document(), 1), Tokens::color(u"accent-text", Theme::Dark));
        Appearance::instance().setMode(Appearance::Mode::Light);
        QCOMPARE(drawnColor(w.edit->document(), 1), Tokens::color(u"accent-text", Theme::Light));
    }

    void expandableTextFoldsAndUnfolds() {
        QWidget window;
        auto* layout = new QVBoxLayout(&window);
        auto* text = new ExpandableText;
        layout->addWidget(text);
        layout->addStretch(1);
        window.resize(360, 800);
        window.show();
        window.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        text->setText(QStringLiteral("One\nTwo\nThree\nFour\nFive @agent/2035\nSix"));
        QCOMPARE(text->lineCount(), 6); // line breaks are kept
        QVERIFY(text->isFoldable());
        QCOMPARE(text->visibleLineCount(), text->foldedLines());
        QVERIFY(text->toggleButton()->isVisible());
        QCOMPARE(text->toggleButton()->text(), QStringLiteral("Show more"));
        const int folded = text->height();
        QTest::mouseClick(text->toggleButton(), Qt::LeftButton);
        QVERIFY(text->isExpanded());
        QCOMPARE(text->visibleLineCount(), 6);
        QCOMPARE(text->toggleButton()->text(), QStringLiteral("Show less"));
        QTRY_VERIFY(text->height() > folded);
        QTest::mouseClick(text->toggleButton(), Qt::LeftButton);
        QVERIFY(!text->isExpanded());

        text->setText(QStringLiteral("Short"));
        QVERIFY(!text->isFoldable());
        QVERIFY(!text->toggleButton()->isVisible());
        QCOMPARE(text->focusPolicy(), Qt::NoFocus); // nothing to focus without mentions
    }

    void expandableTextMentionsAreLinks() {
        QWidget window;
        auto* layout = new QVBoxLayout(&window);
        auto* text = new ExpandableText;
        layout->addWidget(text);
        window.resize(360, 400);
        window.show();
        window.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        QSignalSpy activated(text, &ExpandableText::mentionActivated);
        text->setText(QStringLiteral("Ask @agent/2035 and @agent/9410\n2\n3\n4\nthen @agent/77"));
        text->setMentionTitle(QStringLiteral("2035"), QStringLiteral("Refine navigation"));
        QCOMPARE(text->mentions().size(), 3);
        text->setFocus();
        QCOMPARE(text->focusedMention(), 0);
        QCOMPARE(text->accessibleDescription(), QStringLiteral("Refine navigation (agent 2035)"));
        QTest::keyClick(text, Qt::Key_Return);
        QCOMPARE(activated.last().first().toString(), QStringLiteral("2035"));
        QTest::keyClick(text, Qt::Key_Right);
        QTest::keyClick(text, Qt::Key_Space);
        QCOMPARE(activated.last().first().toString(), QStringLiteral("9410"));
        // The third mention is behind the fold: focusing it opens the text.
        QVERIFY(!text->isExpanded());
        QTest::keyClick(text, Qt::Key_Right);
        QVERIFY(text->isExpanded());
        QCOMPARE(text->focusedMention(), 2);
        QTest::keyClick(text, Qt::Key_Right); // wraps around
        QCOMPARE(text->focusedMention(), 0);

        // A click on a mention activates it.
        text->clearFocus();
        // Halfway down the first line box, a few characters into the first mention.
        const int line = qFloor(qRound(text->recipe().fontPixels) * text->recipe().lineHeight);
        const QPoint inside(text->fontMetrics().horizontalAdvance(QStringLiteral("Ask @ag")),
                            line / 2);
        QTest::mouseClick(text, Qt::LeftButton, {}, inside);
        QCOMPARE(activated.last().first().toString(), QStringLiteral("2035"));
        text->clearFocus(); // the click focused it; unfocused, all mentions are described
        text->setUnknownMentions({QStringLiteral("9410")});
        QVERIFY(text->accessibleDescription().contains(QStringLiteral("agent 9410, unknown")));
        QCOMPARE(text->text().count(QStringLiteral("@agent/")), 3); // the text is unchanged
    }
};

int main(int argc, char** argv) {
    QApplication application(argc, argv);
    Style::install(application);
    TextAreaTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_text_area.moc"
