// SPDX-License-Identifier: MPL-2.0
#include "KetSyntax.h"

#include <QRegularExpression>
#include <QSet>
#include <QTextCharFormat>

namespace ket {

namespace {

enum class Family { None, CLike, Python, Shell, CMake, Json, Yaml };

Family familyOf(QStringView language) {
    const QString name = language.trimmed().toString().toLower();
    static const QSet<QString> cLike{
        QStringLiteral("c"),     QStringLiteral("h"),          QStringLiteral("cpp"),
        QStringLiteral("c++"),   QStringLiteral("cc"),         QStringLiteral("hpp"),
        QStringLiteral("cxx"),   QStringLiteral("objc"),       QStringLiteral("js"),
        QStringLiteral("jsx"),   QStringLiteral("javascript"), QStringLiteral("ts"),
        QStringLiteral("tsx"),   QStringLiteral("typescript"), QStringLiteral("qml"),
        QStringLiteral("java"),  QStringLiteral("kotlin"),     QStringLiteral("kt"),
        QStringLiteral("swift"), QStringLiteral("go"),         QStringLiteral("rust"),
        QStringLiteral("rs"),    QStringLiteral("cs"),         QStringLiteral("csharp")};
    if (cLike.contains(name))
        return Family::CLike;
    if (name == u"py" || name == u"python")
        return Family::Python;
    if (name == u"sh" || name == u"bash" || name == u"zsh" || name == u"shell" ||
        name == u"console" || name == u"shell-session")
        return Family::Shell;
    if (name == u"cmake")
        return Family::CMake;
    if (name == u"json" || name == u"jsonc")
        return Family::Json;
    if (name == u"yaml" || name == u"yml" || name == u"toml")
        return Family::Yaml;
    return Family::None;
}

const QSet<QString>& keywordsOf(Family family) {
    static const QSet<QString> none;
    static const QSet<QString> cLike = [] {
        QSet<QString> words;
        for (const char* word :
             {"alignas",  "auto",      "await",    "bool",      "break",     "case",
              "catch",    "char",      "class",    "const",     "constexpr", "continue",
              "default",  "delete",    "do",       "double",    "else",      "enum",
              "explicit", "export",    "extends",  "false",     "final",     "float",
              "fn",       "for",       "func",     "function",  "if",        "impl",
              "import",   "in",        "inline",   "int",       "interface", "let",
              "long",     "match",     "mut",      "namespace", "new",       "noexcept",
              "null",     "nullptr",   "operator", "override",  "package",   "private",
              "property", "protected", "pub",      "public",    "readonly",  "required",
              "return",   "short",     "signal",   "signed",    "sizeof",    "static",
              "struct",   "super",     "switch",   "template",  "this",      "throw",
              "true",     "try",       "type",     "typedef",   "typename",  "undefined",
              "union",    "unsigned",  "use",      "using",     "var",       "virtual",
              "void",     "volatile",  "while",    "yield",     "string",    "real",
              "co_await", "co_return", "co_yield", "include",   "define",    "pragma"})
            words.insert(QString::fromLatin1(word));
        return words;
    }();
    static const QSet<QString> python = [] {
        QSet<QString> words;
        for (const char* word :
             {"and",      "as",     "assert", "async",  "await",  "break",  "class",   "continue",
              "def",      "del",    "elif",   "else",   "except", "False",  "finally", "for",
              "from",     "global", "if",     "import", "in",     "is",     "lambda",  "None",
              "nonlocal", "not",    "or",     "pass",   "raise",  "return", "True",    "try",
              "while",    "with",   "yield",  "self"})
            words.insert(QString::fromLatin1(word));
        return words;
    }();
    static const QSet<QString> shell = [] {
        QSet<QString> words;
        for (const char* word :
             {"if",    "then",     "else",  "elif",  "fi",     "for",      "in",     "do",
              "done",  "while",    "until", "case",  "esac",   "function", "return", "export",
              "local", "readonly", "set",   "unset", "source", "exit",     "cd",     "echo"})
            words.insert(QString::fromLatin1(word));
        return words;
    }();
    static const QSet<QString> cmake = [] {
        QSet<QString> words;
        for (const char* word : {"if",
                                 "elseif",
                                 "else",
                                 "endif",
                                 "foreach",
                                 "endforeach",
                                 "while",
                                 "endwhile",
                                 "function",
                                 "endfunction",
                                 "macro",
                                 "endmacro",
                                 "return",
                                 "set",
                                 "unset",
                                 "option",
                                 "project",
                                 "include",
                                 "find_package",
                                 "add_library",
                                 "add_executable",
                                 "target_link_libraries",
                                 "target_include_directories",
                                 "target_compile_definitions",
                                 "target_sources",
                                 "install",
                                 "message",
                                 "list",
                                 "string",
                                 "file",
                                 "FetchContent_Declare",
                                 "FetchContent_MakeAvailable",
                                 "cmake_minimum_required"})
            words.insert(QString::fromLatin1(word));
        return words;
    }();
    static const QSet<QString> literals = [] {
        QSet<QString> words;
        for (const char* word : {"true", "false", "null", "yes", "no", "on", "off"})
            words.insert(QString::fromLatin1(word));
        return words;
    }();
    switch (family) {
    case Family::CLike:
        return cLike;
    case Family::Python:
        return python;
    case Family::Shell:
        return shell;
    case Family::CMake:
        return cmake;
    case Family::Json:
    case Family::Yaml:
        return literals;
    case Family::None:
        break;
    }
    return none;
}

bool hashComments(Family family) {
    return family == Family::Python || family == Family::Shell || family == Family::CMake ||
           family == Family::Yaml;
}

bool isWordChar(QChar c) { return c.isLetterOrNumber() || c == u'_'; }

// Words, numbers and types inside a stretch of code that is not a string or comment.
void scanCode(QStringView line, qsizetype from, qsizetype to, Family family,
              QList<SyntaxSpan>& spans) {
    const QSet<QString>& keywords = keywordsOf(family);
    qsizetype i = from;
    while (i < to) {
        const QChar c = line.at(i);
        const bool wordStart = i == 0 || !isWordChar(line.at(i - 1));
        if (wordStart && c.isDigit()) {
            qsizetype end = i + 1;
            while (end < to && (line.at(end).isLetterOrNumber() || line.at(end) == u'.' ||
                                line.at(end) == u'_'))
                ++end;
            spans.append({i, end - i, SyntaxRole::Number});
            i = end;
            continue;
        }
        if (wordStart && (c.isLetter() || c == u'_')) {
            qsizetype end = i + 1;
            while (end < to && isWordChar(line.at(end)))
                ++end;
            const QString word = line.sliced(i, end - i).toString();
            if (keywords.contains(word)) {
                spans.append({i, end - i, SyntaxRole::Keyword});
            } else if (family == Family::CLike && c.isUpper() && word.size() > 1) {
                spans.append({i, end - i, SyntaxRole::Type});
            }
            i = end;
            continue;
        }
        if (family == Family::Shell && wordStart && c == u'-' && i + 1 < to &&
            (line.at(i + 1).isLetter() || line.at(i + 1) == u'-')) {
            qsizetype end = i + 1;
            while (end < to && (isWordChar(line.at(end)) || line.at(end) == u'-'))
                ++end;
            spans.append({i, end - i, SyntaxRole::Attribute});
            i = end;
            continue;
        }
        if (family == Family::Shell && c == u'$' && i + 1 < to) {
            qsizetype end = i + 1;
            if (line.at(end) == u'{') {
                while (end < to && line.at(end) != u'}')
                    ++end;
                end = qMin(to, end + 1);
            } else {
                while (end < to && isWordChar(line.at(end)))
                    ++end;
            }
            if (end > i + 1)
                spans.append({i, end - i, SyntaxRole::Type});
            i = end;
            continue;
        }
        ++i;
    }
}

} // namespace

SyntaxRecipe syntaxRecipe(Theme theme) {
    SyntaxRecipe recipe;
    recipe.keyword = Tokens::color(u"accent-text", theme);
    recipe.string = Tokens::color(u"chart-2", theme);
    // chart-3 reads at 3.8:1 on a light panel; warning keeps the amber at 5.3:1 or more.
    recipe.number = Tokens::color(u"warning", theme);
    recipe.type = Tokens::color(u"chart-5", theme);
    recipe.comment = Tokens::color(u"text-muted", theme);
    recipe.attribute = Tokens::color(u"chart-4", theme);
    return recipe;
}

QColor syntaxColor(const SyntaxRecipe& recipe, SyntaxRole role) {
    switch (role) {
    case SyntaxRole::Keyword:
        return recipe.keyword;
    case SyntaxRole::String:
        return recipe.string;
    case SyntaxRole::Number:
        return recipe.number;
    case SyntaxRole::Type:
        return recipe.type;
    case SyntaxRole::Comment:
        return recipe.comment;
    case SyntaxRole::Attribute:
        return recipe.attribute;
    }
    return recipe.keyword;
}

QList<SyntaxSpan> highlightLine(QStringView line, QStringView language, int& state) {
    QList<SyntaxSpan> spans;
    const Family family = familyOf(language);
    if (family == Family::None) {
        state = 0;
        return spans;
    }
    const qsizetype size = line.size();
    qsizetype i = 0;
    qsizetype code = 0; // start of the pending code stretch

    // An open /* comment from the previous line.
    if (state == 1) {
        const qsizetype close = line.indexOf(u"*/");
        if (close < 0) {
            if (size)
                spans.append({0, size, SyntaxRole::Comment});
            return spans;
        }
        spans.append({0, close + 2, SyntaxRole::Comment});
        i = code = close + 2;
        state = 0;
    }

    // Keys: `name:` at the start of a QML or YAML line.
    if (family == Family::Yaml || (family == Family::CLike && language == u"qml")) {
        static const QRegularExpression key(QStringLiteral("^\\s*([A-Za-z_][\\w.-]*)\\s*:"));
        if (const auto match = key.matchView(line, i);
            match.hasMatch() && match.capturedStart(1) >= i) {
            scanCode(line, code, match.capturedStart(1), family, spans);
            spans.append({match.capturedStart(1), match.capturedLength(1), SyntaxRole::Attribute});
            i = code = match.capturedEnd(1);
        }
    }

    while (i < size) {
        const QChar c = line.at(i);
        const bool lineComment =
            (family == Family::CLike && c == u'/' && i + 1 < size && line.at(i + 1) == u'/') ||
            (hashComments(family) && c == u'#' && (i == 0 || line.at(i - 1).isSpace()));
        if (lineComment) {
            scanCode(line, code, i, family, spans);
            spans.append({i, size - i, SyntaxRole::Comment});
            return spans;
        }
        if (family == Family::CLike && c == u'/' && i + 1 < size && line.at(i + 1) == u'*') {
            scanCode(line, code, i, family, spans);
            const qsizetype close = line.indexOf(u"*/", i + 2);
            if (close < 0) {
                spans.append({i, size - i, SyntaxRole::Comment});
                state = 1;
                return spans;
            }
            spans.append({i, close + 2 - i, SyntaxRole::Comment});
            i = code = close + 2;
            continue;
        }
        const bool quote = c == u'"' || (c == u'\'' && family != Family::Json) ||
                           (c == u'`' && (family == Family::CLike || family == Family::Shell));
        if (quote) {
            scanCode(line, code, i, family, spans);
            qsizetype end = i + 1;
            while (end < size && line.at(end) != c) {
                if (line.at(end) == u'\\')
                    ++end;
                ++end;
            }
            end = qMin(size, end + 1);
            // A JSON string followed by a colon is a key.
            qsizetype after = end;
            while (after < size && line.at(after).isSpace())
                ++after;
            const bool key = family == Family::Json && after < size && line.at(after) == u':';
            spans.append({i, end - i, key ? SyntaxRole::Attribute : SyntaxRole::String});
            i = code = end;
            continue;
        }
        ++i;
    }
    scanCode(line, code, size, family, spans);
    return spans;
}

QList<SyntaxSpan> highlightCode(QStringView code, QStringView language) {
    QList<SyntaxSpan> spans;
    int state = 0;
    qsizetype offset = 0;
    for (const QStringView line : code.tokenize(u'\n')) {
        for (SyntaxSpan span : highlightLine(line, language, state)) {
            span.start += offset;
            spans.append(span);
        }
        offset += line.size() + 1;
    }
    return spans;
}

QStringList highlightedLines(QStringView code, QStringView language, const SyntaxRecipe& recipe) {
    const auto escape = [](QStringView text) {
        return text.toString().toHtmlEscaped().replace(u' ', QStringLiteral("&nbsp;"));
    };
    QStringList lines;
    int state = 0;
    for (const QStringView line : code.tokenize(u'\n')) {
        QString html;
        qsizetype at = 0;
        for (const SyntaxSpan& span : highlightLine(line, language, state)) {
            html += escape(line.sliced(at, span.start - at));
            html += QStringLiteral("<font color='%1'>%2</font>")
                        .arg(syntaxColor(recipe, span.role).name(QColor::HexArgb),
                             escape(line.sliced(span.start, span.length)));
            at = span.start + span.length;
        }
        html += escape(line.sliced(at));
        lines.append(html);
    }
    return lines;
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document) {}

void SyntaxHighlighter::setLanguage(const QString& language) {
    if (language_ == language)
        return;
    language_ = language;
    rehighlight();
}

void SyntaxHighlighter::setRecipe(const SyntaxRecipe& recipe) {
    recipe_ = recipe;
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    int state = qMax(0, previousBlockState());
    for (const SyntaxSpan& span : highlightLine(text, language_, state)) {
        QTextCharFormat format;
        format.setForeground(syntaxColor(recipe_, span.role));
        setFormat(int(span.start), int(span.length), format);
    }
    setCurrentBlockState(state);
}

} // namespace ket
