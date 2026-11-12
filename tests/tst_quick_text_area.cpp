// SPDX-License-Identifier: MPL-2.0
// KetTextArea, its drawn `@agent/` mentions and KetExpandableText in Qt Quick, against the
// recipes ket::TextArea and ket::ExpandableText draw.
#include "KetAppearance.h"
#include "KetTextRecipes.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickTextDocument>
#include <QQuickView>
#include <QSignalSpy>
#include <QTest>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <QtQml/qqmlextensionplugin.h>

#include <cmath>
#include <memory>

Q_IMPORT_QML_PLUGIN(KetPlugin)

using namespace ket;

namespace {

QColor token(QStringView name) { return Tokens::color(name, Appearance::instance().theme()); }

QColor drawnColor(QQuickItem* input, int position) {
    auto* document = input->property("textDocument").value<QQuickTextDocument*>()->textDocument();
    const QTextBlock block = document->findBlock(position);
    for (const QTextLayout::FormatRange& range : block.layout()->formats()) {
        const int at = position - block.position();
        if (at >= range.start && at < range.start + range.length)
            return range.format.foreground().color();
    }
    return {};
}

const QByteArray kAreaWindow = QByteArrayLiteral(R"(
import QtQuick
import Ket
Item {
    width: 480; height: 600
    property alias area: area
    property alias next: next
    property int shortcuts: 0
    property string query
    property int closed: 0
    property string description: area.input.Accessible.description
    property alias hostShortcut: hostShortcut
    Shortcut { id: hostShortcut; sequence: "Ctrl+Return"; onActivated: shortcuts++ }
    Column {
        width: parent.width
        KetTextArea {
            id: area; width: parent.width; focus: true
            Accessible.name: "Context"
            onMentionQuery: q => query = q
            onMentionClosed: closed++
        }
        KetTextField { id: next; width: parent.width }
    }
}
)");

const QByteArray kTextWindow = QByteArrayLiteral(R"(
import QtQuick
import Ket
Item {
    width: 360; height: 600
    property alias text: text
    property string activated
    KetExpandableText {
        id: text; width: parent.width
        mentionTitles: ({ "2035": "Refine navigation" })
        onMentionActivated: number => activated = number
    }
}
)");

} // namespace

class QuickTextAreaTest final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        Appearance::instance().setMode(Appearance::Mode::Light);
        Appearance::instance().setZoom(1);
    }

    void recipesMatchTheWidgetsLayer() {
        QVERIFY(open(kAreaWindow));
        const QVariantMap look = area()->property("recipe").toMap();
        // The field has the focus.
        const TextAreaRecipe r =
            textAreaRecipe({false, false, true, true}, false, false, Theme::Light);
        QCOMPARE(look.value(QStringLiteral("border")).value<QColor>(), r.field.border);
        QCOMPARE(look.value(QStringLiteral("borderWidth")).toReal(), r.field.borderWidth);
        QCOMPARE(look.value(QStringLiteral("paddingBlock")).toReal(), r.paddingBlock);
        QCOMPARE(look.value(QStringLiteral("minLines")).toInt(), r.minLines);
        QCOMPARE(look.value(QStringLiteral("count")).value<QColor>(), r.count);
    }

    void linesGrowToTheMaximumThenScroll() {
        QVERIFY(open(kAreaWindow));
        area()->setProperty("minimumLines", 3);
        area()->setProperty("maximumLines", 5);
        const qreal line = area()->property("lineBox").toReal();
        const qreal padding = 2 * textAreaRecipe({}, false, false, Theme::Light).paddingBlock;
        QCOMPARE(area()->property("visibleLines").toInt(), 3);
        QCOMPARE(area()->implicitHeight(), 3 * line + padding);
        area()->setProperty("text", QStringLiteral("1\n2\n3\n4"));
        QTRY_COMPARE(area()->property("visibleLines").toInt(), 4);
        QCOMPARE(area()->implicitHeight(), 4 * line + padding);
        area()->setProperty("text", QStringLiteral("1\n2\n3\n4\n5\n6\n7\n8\n9"));
        QTRY_COMPARE(area()->property("visibleLines").toInt(), 5);
        QCOMPARE(area()->implicitHeight(), 5 * line + padding);
        QVERIFY(input()->height() > area()->implicitHeight()); // the rest scrolls
    }

    void pasteInsertsPlainTextAndKeepsLineBreaks() {
        QVERIFY(open(kAreaWindow));
        auto* data = new QMimeData;
        data->setHtml(QStringLiteral("<b>First</b><br>second"));
        data->setText(QStringLiteral("First\nsecond\n\nfourth"));
        QGuiApplication::clipboard()->setMimeData(data);
        QMetaObject::invokeMethod(input(), "paste");
        QCOMPARE(text(), QStringLiteral("First\nsecond\n\nfourth"));
    }

    void returnBreaksTheLineAndCommandReturnIsLeftToTheWindow() {
        QVERIFY(open(kAreaWindow));
        type(QStringLiteral("One"));
        QTest::keyClick(view_.get(), Qt::Key_Return);
        type(QStringLiteral("Two"));
        QCOMPARE(text(), QStringLiteral("One\nTwo"));
        QTest::keyClick(view_.get(), Qt::Key_Return, Qt::ControlModifier);
        QCOMPARE(root()->property("shortcuts").toInt(), 1);
        QCOMPARE(text(), QStringLiteral("One\nTwo"));
        // Without a shortcut the key still does not break the line, with Control or Meta.
        root()->property("hostShortcut").value<QObject*>()->setProperty("enabled", false);
        QTest::keyClick(view_.get(), Qt::Key_Return, Qt::ControlModifier);
        QTest::keyClick(view_.get(), Qt::Key_Return, Qt::MetaModifier);
        QCOMPARE(text(), QStringLiteral("One\nTwo"));
        QTest::keyClick(view_.get(), Qt::Key_Tab);
        QVERIFY(root()->property("next").value<QQuickItem*>()->hasActiveFocus());
        QCOMPARE(text(), QStringLiteral("One\nTwo")); // no tab character
    }

    void theCountMarksTheFieldInvalidAndNeverCuts() {
        QVERIFY(open(kAreaWindow));
        area()->setProperty("maxLength", 10);
        type(QStringLiteral("Hello"));
        QCOMPARE(root()->property("description").toString(), QStringLiteral("5 of 10 characters"));
        auto* count = area()->findChild<QQuickItem*>(QStringLiteral("textAreaCount"));
        QCOMPARE(count->property("text").toString(), QStringLiteral("5 / 10"));
        type(QStringLiteral(" there, all"));
        QCOMPARE(text().size(), 16);
        QVERIFY(area()->property("overLimit").toBool());
        const QVariantMap look = area()->property("recipe").toMap();
        QCOMPARE(look.value(QStringLiteral("border")).value<QColor>(), token(u"danger"));
        QCOMPARE(count->property("color").value<QColor>(), token(u"danger"));
        QCOMPARE(root()->property("description").toString(),
                 QStringLiteral("6 characters over the limit of 10"));
    }

    void aChosenMentionIsAnAtomicToken() {
        QVERIFY(open(kAreaWindow));
        type(QStringLiteral("Ask @agent/4"));
        QCOMPARE(root()->property("query").toString(), QStringLiteral("4"));
        area()->setProperty(
            "mentionItems",
            QVariantList{QVariantMap{{QStringLiteral("number"), QStringLiteral("4310")},
                                     {QStringLiteral("title"), QStringLiteral("Refine navigation")},
                                     {QStringLiteral("place"), QStringLiteral("ketplus")}}});
        auto* picker = area()->property("mentionPicker").value<QObject*>();
        QTRY_VERIFY(picker->property("visible").toBool());
        QTest::keyClick(view_.get(), Qt::Key_Return);
        QCOMPARE(text(), QStringLiteral("Ask @agent/4310 "));
        QVERIFY(!picker->property("visible").toBool());
        auto* tokens = area()->property("mentions").value<QObject*>();
        QCOMPARE(tokens->property("numbers").toStringList(), QStringList{QStringLiteral("4310")});
        QCOMPARE(drawnColor(input(), 6), token(u"accent-text"));
        QVERIFY(root()
                    ->property("description")
                    .toString()
                    .contains(QStringLiteral("Refine navigation (agent 4310)")));
        QTest::keyClick(view_.get(), Qt::Key_Backspace);
        QCOMPARE(text(), QStringLiteral("Ask @agent/4310"));
        QVERIFY(!picker->property("visible").toBool());
        QTest::keyClick(view_.get(), Qt::Key_Backspace);
        QCOMPARE(text(), QStringLiteral("Ask "));

        area()->setProperty("text", QStringLiteral("@agent/12 hi"));
        input()->setProperty("cursorPosition", 0);
        QTest::keyClick(view_.get(), Qt::Key_Delete);
        QCOMPARE(text(), QStringLiteral(" hi"));
    }

    // Typing at a token's edge changes the mention there, so the token ends.
    void typingAtATokensEdgeEndsIt() {
        QVERIFY(open(kAreaWindow));
        auto* tokens = area()->property("mentions").value<QObject*>();
        area()->setProperty("text", QStringLiteral("Ask @agent/12"));
        input()->setProperty("cursorPosition", 13);
        type(QStringLiteral("3"));
        QCOMPARE(text(), QStringLiteral("Ask @agent/123"));
        QVERIFY(tokens->property("numbers").toStringList().isEmpty());
        QVERIFY(drawnColor(input(), 5) != token(u"accent-text"));
        QTest::keyClick(view_.get(), Qt::Key_Backspace);
        QCOMPARE(text(), QStringLiteral("Ask @agent/12"));

        area()->setProperty("text", QStringLiteral("@agent/12 hi"));
        input()->setProperty("cursorPosition", 0);
        type(QStringLiteral("x"));
        QCOMPARE(text(), QStringLiteral("x@agent/12 hi"));
        QVERIFY(tokens->property("numbers").toStringList().isEmpty());
    }

    // As ket::TextArea: the picker closes when the field loses the focus.
    void thePickerClosesWhenTheFieldLosesFocus() {
        QVERIFY(open(kAreaWindow));
        type(QStringLiteral("Ask @agent/4"));
        area()->setProperty("mentionItems",
                            QVariantList{QVariantMap{
                                {QStringLiteral("number"), QStringLiteral("4310")},
                                {QStringLiteral("title"), QStringLiteral("Refine navigation")}}});
        auto* picker = area()->property("mentionPicker").value<QObject*>();
        QTRY_VERIFY(picker->property("visible").toBool());
        const int closed = root()->property("closed").toInt();
        root()->property("next").value<QQuickItem*>()->forceActiveFocus();
        QTRY_VERIFY(!picker->property("visible").toBool());
        QCOMPARE(root()->property("closed").toInt(), closed + 1);
        QCOMPARE(text(), QStringLiteral("Ask @agent/4"));
    }

    void theCaretNeverStopsInsideAToken() {
        QVERIFY(open(kAreaWindow));
        area()->setProperty("text", QStringLiteral("a @agent/12 b"));
        input()->setProperty("cursorPosition", 1);
        QTest::keyClick(view_.get(), Qt::Key_Right);
        QCOMPARE(input()->property("cursorPosition").toInt(), 2);
        QTest::keyClick(view_.get(), Qt::Key_Right);
        QCOMPARE(input()->property("cursorPosition").toInt(), 11);
        QTest::keyClick(view_.get(), Qt::Key_Left);
        QCOMPARE(input()->property("cursorPosition").toInt(), 2);
    }

    void setTextDrawsMentionsAndUnknownOnesAreMarked() {
        QVERIFY(open(kAreaWindow));
        type(QStringLiteral("@agent/12 "));
        auto* tokens = area()->property("mentions").value<QObject*>();
        QVERIFY(tokens->property("numbers").toStringList().isEmpty()); // typed, not chosen
        const QString value = QStringLiteral("Ask @agent/1 and\n@agent/2 now");
        area()->setProperty("text", value);
        QCOMPARE(tokens->property("numbers").toStringList(),
                 (QStringList{QStringLiteral("1"), QStringLiteral("2")}));
        area()->setProperty("unknownMentions", QStringList{QStringLiteral("2")});
        QCOMPARE(drawnColor(input(), 5), token(u"accent-text"));
        QCOMPARE(drawnColor(input(), 18), token(u"danger"));
        QCOMPARE(text(), value);
        QVERIFY(root()
                    ->property("description")
                    .toString()
                    .contains(QStringLiteral("agent 2, unknown")));
    }

    void expandableTextFoldsAndUnfolds() {
        QVERIFY(open(kTextWindow));
        auto* text = root()->property("text").value<QQuickItem*>();
        text->setProperty("text", QStringLiteral("One\nTwo\nThree\nFour\nFive @agent/2035\nSix"));
        QTRY_COMPARE(text->property("lineCount").toInt(), 6);
        const ExpandableTextRecipe r = expandableTextRecipe(Theme::Light);
        QCOMPARE(text->property("folded").toInt(), r.foldedLines);
        QVERIFY(text->property("foldable").toBool());
        QCOMPARE(text->property("visibleLineCount").toInt(), r.foldedLines);
        auto* toggle = text->property("toggleButton").value<QQuickItem*>();
        QVERIFY(toggle->isVisible());
        QCOMPARE(toggle->property("text").toString(), QStringLiteral("Show more"));
        const qreal folded = text->implicitHeight();
        QMetaObject::invokeMethod(toggle, "clicked");
        QVERIFY(text->property("expanded").toBool());
        QCOMPARE(text->property("visibleLineCount").toInt(), 6);
        QCOMPARE(toggle->property("text").toString(), QStringLiteral("Show less"));
        QVERIFY(text->implicitHeight() > folded);
        text->setProperty("text", QStringLiteral("Short"));
        QTRY_VERIFY(!text->property("foldable").toBool());
        QVERIFY(!toggle->isVisible());
        QVERIFY(!text->property("activeFocusOnTab").toBool());
    }

    void expandableTextMentionsAreLinks() {
        QVERIFY(open(kTextWindow));
        auto* text = root()->property("text").value<QQuickItem*>();
        text->setProperty(
            "text", QStringLiteral("Ask @agent/2035 and @agent/9410\n2\n3\n4\nthen @agent/77"));
        QTRY_COMPARE(text->property("lineCount").toInt(), 5);
        text->forceActiveFocus();
        QCOMPARE(text->property("focusedMention").toInt(), 0);
        QCOMPARE(accessibleDescription(text), QStringLiteral("Refine navigation (agent 2035)"));
        QTest::keyClick(view_.get(), Qt::Key_Return);
        QCOMPARE(root()->property("activated").toString(), QStringLiteral("2035"));
        QTest::keyClick(view_.get(), Qt::Key_Right);
        QTest::keyClick(view_.get(), Qt::Key_Space);
        QCOMPARE(root()->property("activated").toString(), QStringLiteral("9410"));
        QVERIFY(!text->property("expanded").toBool());
        QTest::keyClick(view_.get(), Qt::Key_Right); // behind the fold
        QVERIFY(text->property("expanded").toBool());
        QCOMPARE(text->property("focusedMention").toInt(), 2);

        // A click on a mention activates it.
        auto* body = text->findChild<QQuickItem*>(QStringLiteral("expandableTextBody"));
        QRectF at;
        QMetaObject::invokeMethod(body, "positionToRectangle", Q_RETURN_ARG(QRectF, at),
                                  Q_ARG(int, 7));
        root()->setProperty("activated", QString());
        QTest::mouseClick(view_.get(), Qt::LeftButton, {}, body->mapToScene(at.center()).toPoint());
        QTRY_COMPARE(root()->property("activated").toString(), QStringLiteral("2035"));
    }

  private:
    // QTest has no keyClicks() for a window.
    void type(const QString& text) {
        for (const QChar c : text)
            QTest::keyClick(view_.get(), c.toLatin1());
    }

    bool open(const QByteArray& source) {
        view_ = std::make_unique<QQuickView>(&engine_, nullptr);
        QQmlComponent component(&engine_);
        component.setData(source, QUrl());
        if (component.isError()) {
            qWarning().noquote() << component.errorString();
            return false;
        }
        view_->setContent(QUrl(), &component, component.create());
        view_->show();
        view_->requestActivate();
        if (!QTest::qWaitForWindowActive(view_.get()))
            return false;
        if (auto* area = this->area())
            static_cast<QQuickItem*>(area)->forceActiveFocus();
        if (QQuickItem* field = input())
            field->forceActiveFocus();
        return true;
    }

    [[nodiscard]] QQuickItem* root() const { return view_->rootObject(); }
    [[nodiscard]] QQuickItem* area() const { return root()->property("area").value<QQuickItem*>(); }
    [[nodiscard]] QQuickItem* input() const {
        return area() ? area()->property("input").value<QQuickItem*>() : nullptr;
    }
    [[nodiscard]] QString text() const { return area()->property("text").toString(); }
    static QString accessibleDescription(QQuickItem* item) {
        QQmlComponent probe(qmlEngine(item));
        probe.setData("import QtQuick\nQtObject { property var target; "
                      "property string value: target.Accessible.description }",
                      QUrl());
        std::unique_ptr<QObject> object(probe.createWithInitialProperties(
            {{QStringLiteral("target"), QVariant::fromValue(item)}}));
        return object ? object->property("value").toString() : QString();
    }

    QQmlEngine engine_;
    std::unique_ptr<QQuickView> view_;
};

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    QuickTextAreaTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_quick_text_area.moc"
