// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// Generated from the Artistic Style 3.6.20 manual (doc/astyle.html) that ships
// with the sources in 3rdparty/astyle. Do not edit the description texts by hand;
// they are quoted from the manual. The <pre> examples are the ones printed there
// under "becomes:".

#include "astyleoptionhelp.h"

#include <QCoreApplication>
#include <QHash>
#include <QString>

namespace Astyle::Internal {

struct OptionDoc
{
    const char *option;
    const char *title;
    const char *description;
    const char *before;
    const char *after;
    const char *caption;
};

static const OptionDoc kOptionDocs[] = {
    { "--indent-classes",
      "--indent-classes / -C",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent 'class' and 'struct' blocks so that the entire block is indented. The struct blocks are indented only if an access modifier, 'public:', 'protected:' or 'private:', is declared somewhere in the struct. This option is effective for C++ files only."),
      "class Foo\n{\npublic:\n    Foo();\n    virtual ~Foo();\n};",
      "class Foo\n{\n    public:\n        Foo();\n        virtual ~Foo();\n};",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-modifiers",
      "--indent-modifiers / -xG",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent 'class ' and 'struct' access modifiers, 'public:', 'protected:' and 'private:', one half indent. The rest of the class is not indented. This option is effective for C++ files only. If used with indent-classes this option will be ignored."),
      "class Foo\n{\npublic:\n    Foo();\n    virtual ~Foo();\n};",
      "class Foo\n{\n  public:\n    Foo();\n    virtual ~Foo();\n};",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-switches",
      "--indent-switches / -S",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent 'switch' blocks so that the 'case X:' statements are indented in the switch block. The entire case block is indented."),
      "switch (foo)\n{\ncase 1:\n    a += 1;\n    break;\n\ncase 2:\n{\n    a += 2;\n    break;\n}\n}",
      "switch (foo)\n{\n    case 1:\n        a += 1;\n        break;\n\n    case 2:\n    {\n        a += 2;\n        break;\n    }\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-cases",
      "--indent-cases / -K",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent 'case X:' blocks from the 'case X:' headers. Case statements not enclosed in blocks are NOT indented."),
      "switch (foo)\n{\n    case 1:\n        a += 1;\n        break;\n\n    case 2:\n    {\n        a += 2;\n        break;\n    }\n}",
      "switch (foo)\n{\n    case 1:\n        a += 1;\n        break;\n\n    case 2:\n        {\n            a += 2;\n            break;\n        }\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-namespaces",
      "--indent-namespaces / -N",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Add extra indentation to namespace blocks. This option has no effect on Java files. It will also indent CORBA IDL module statements."),
      "namespace foospace\n{\nclass Foo\n{\n    public:\n        Foo();\n        virtual ~Foo();\n};\n}",
      "namespace foospace\n{\n    class Foo\n    {\n        public:\n            Foo();\n            virtual ~Foo();\n    };\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-labels",
      "--indent-labels / -L",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Add extra indentation to labels so they appear 1 indent less than the current indentation, rather than being flushed to the left (the default)."),
      "void Foo() {\n    while (isFoo) {\n        if (isFoo)\n            goto error;\n        ...\nerror:\n        ...\n        }\n}",
      "void Foo() {\n    while (isFoo) {\n        if (isFoo)\n            goto error;\n        ...\n    error:\n        ...\n        }\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with indented 'error:'):") },
    { "--indent-preproc-block",
      "--indent-preproc-block / -xW",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent preprocessor blocks at brace level zero and immediately within a namespace. There are restrictions on what will be indented. Blocks within methods, classes, arrays, etc., will not be indented. Blocks containing braces or multi-line define statements will not be indented. Without this option the preprocessor block is not indented."),
      "#ifdef _WIN32\n#include <windows.h>\n#ifndef NO_EXPORT\n#define EXPORT\n#endif\n#endif",
      "#ifdef _WIN32\n    #include <windows.h>\n    #ifndef NO_EXPORT\n        #define EXPORT\n    #endif\n#endif",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-preproc-define",
      "--indent-preproc-define / -w",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent multi-line preprocessor definitions ending with a backslash. Should be used with --convert-tabs for proper results. Does a pretty good job, but cannot perform miracles in obfuscated preprocessor definitions. Without this option the preprocessor statements remain unchanged."),
      "#define Is_Bar(arg,a,b) \\\n(Is_Foo((arg), (a)) \\\n|| Is_Foo((arg), (b)))",
      "#define Is_Bar(arg,a,b) \\\n    (Is_Foo((arg), (a)) \\\n     || Is_Foo((arg), (b)))",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-preproc-cond",
      "--indent-preproc-cond / -xw",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent preprocessor conditional statements to the same level as the source code."),
      "        isFoo = true;\n#ifdef UNICODE\n        text = wideBuff;\n#else\n        text = buff;\n#endif",
      "isFoo = true;\n#ifdef UNICODE\ntext = wideBuff;\n#else\ntext = buff;\n#endif",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--indent-col1-comments",
      "--indent-col1-comments / -Y",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent C++ comments beginning in column one. By default C++ comments beginning in column one are assumed to be commented-out code and not indented. This option will allow the comments to be indented with the code."),
      "void Foo()\\n\"\n{\n// comment\n    if (isFoo)\n        bar();\n}",
      "void Foo()\\n\"\n{\n    // comment\n    if (isFoo)\n        bar();\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-oper",
      "--pad-oper / -p",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding around operators. This will also pad commas. Any end of line comments will remain in the original column, if possible. Note that there is no option to unpad. Once padded, they stay padded."),
      "if (foo==2)\n    a=bar((b-c)*a,d--);",
      "if (foo == 2)\n    a = bar((b - c) * a, d--);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-comma",
      "--pad-comma / -xg",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding after commas. This is not needed if pad-oper is used. Any end of line comments will remain in the original column, if possible. Note that there is no option to unpad. Once padded, they stay padded."),
      "if (isFoo(a,b))\n    bar(a,b);",
      "if (isFoo(a, b))\n    bar(a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-paren-out",
      "--pad-paren=out / -d",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding around parens on the outside only. Parens that are empty will not be padded. Any end of line comments will remain in the original column, if possible. This can be used with pad-paren=none below to remove unwanted spaces."),
      "if (isFoo((a+2), b))\n    bar(a, b);",
      "if (isFoo ( (a+2), b) )\n    bar (a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-paren-in",
      "--pad-paren=in / -D",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding around paren on the inside only. Any end of line comments will remain in the original column, if possible. This can be used with pad-paren=none below to remove unwanted spaces."),
      "if (isFoo((a+2), b))\n    bar(a, b);",
      "if ( isFoo( ( a+2 ), b ) )\n    bar( a, b );",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-first-paren-out",
      "--pad-first-paren-out / -xd",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding around the first paren in a series on the outside only. Parens that are empty will not be padded. Any end of line comments will remain in the original column, if possible. This can be used with pad-paren=none below to remove unwanted spaces. If used with pad-paren or pad-paren=out, this option will be ignored. If used with pad-paren=in, the result will be the pad-paren."),
      "if (isFoo((a+2), b))\n    bar(a, b);",
      "if (isFoo ((a+2), b))\n    bar (a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-header",
      "--pad-header / -H",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding between a header (e.g. 'if', 'for', 'while'...) and the following paren. Any end of line comments will remain in the original column, if possible. This can be used with pad-paren=none to remove unwanted spaces."),
      "if(isFoo((a+2), b))\n    bar(a, b);",
      "if (isFoo((a+2), b))\n    bar(a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--unpad-paren",
      "--pad-paren=none / -U",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove extra space padding around parens on the inside and outside. Any end of line comments will remain in the original column, if possible. This option can be used in combination with the paren padding options pad-paren, pad-paren=out, pad-paren=in, and pad-header above. Only padding that has not been requested by other options will be removed. For example, if a source has parens padded on both the inside and outside, and you want inside only. You need to use pad-paren=none to remove the outside padding, and pad-paren=in to retain the inside padding. Using only pad-paren=in would not remove the outside padding."),
      "if ( isFoo( ( a+2 ), b ) )\n    bar ( a, b );",
      "if(isFoo((a+2), b))\n    bar(a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with no padding option requested):") },
    { "--pad-include",
      "--pad-include",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding after include directives and removes superfluous whitespace."),
      "#include<stdio>\n#include   <iostream>",
      "#include <stdio>\n#include <iostream>",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-blocks",
      "--break-blocks / -f",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...)."),
      "isFoo = true;\nif (isFoo) {\n    bar();\n} else {\n    anotherBar();\n}\nisBar = false;",
      "isFoo = true;\n\nif (isFoo) {\n    bar();\n} else {\n    anotherBar();\n}\n\nisBar = false;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-blocks=all",
      "--break-blocks=all / -F",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...). Treat closing header blocks (e.g. 'else', 'catch'), structs and functions as stand-alone blocks."),
      "isFoo = true;\nif (isFoo) {\n    bar();\n} else {\n    anotherBar();\n}\nisBar = false;",
      "isFoo = true;\n\nif (isFoo) {\n    bar();\n\n} else {\n    anotherBar();\n}\n\nisBar = false;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-one-line-headers",
      "--break-one-line-headers / -xb",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Break one line headers (e.g. 'if', 'while', 'else', ...) from a statement residing on the same line. If the statement is enclosed in braces, the braces will be formatted according to the requested brace style. A multi-statement line will NOT be broken if keep-one-line-statements is requested. One line blocks will NOT be broken if keep-one-line-blocks is requested and the header is enclosed in the block."),
      "void Foo(bool isFoo)\n{\n    if (isFoo1) bar1();\n\n    if (isFoo2) { bar2(); }\n}",
      "void Foo(bool isFoo)\n{\n    if (isFoo1)\n        bar1();\n\n    if (isFoo2) {\n        bar2();\n    }\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-elseifs",
      "--break-elseifs / -e",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Break \"else if\" header combinations into separate lines. This option has no effect if keep-one-line-statements is used, the \"else if\" statements will remain as they are. If this option is NOT used, \"else if\" header combinations will be placed on a single line."),
      "if (isFoo) {\n    bar();\n}\nelse if (isFoo1()) {\n    bar1();\n}\nelse if (isFoo2()) {\n    bar2();\n}",
      "if (isFoo) {\n    bar();\n}\nelse\n    if (isFoo1()) {\n        bar1();\n    }\n    else\n        if (isFoo2()) {\n            bar2();\n        }",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--keep-one-line-blocks",
      "--keep-one-line-blocks / -O",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Don't break one-line blocks."),
      "if (isFoo)\n{ isFoo = false; cout << isFoo << endl; }",
      nullptr,
      "" },
    { "--keep-one-line-statements",
      "--keep-one-line-statements / -o",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Don't break complex statements and multiple statements residing on a single line."),
      "if (isFoo)\n{\n    isFoo = false; cout << isFoo << endl;\n}",
      nullptr,
      "" },
    { "--attach-namespaces",
      "--attach-namespaces / -xn",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach braces to a namespace statement. This is done regardless of the brace style being used. It will also attach braces to CORBA IDL module statements."),
      "namespace FooName {\n...\n}",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "the brace is always attached to a namespace statement:") },
    { "--attach-classes",
      "--attach-classes / -xc",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach braces to a class statement. This is done regardless of the brace style being used."),
      "class FooClass {\n...\n};",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "the brace is always attached to a class statement:") },
    { "--attach-inlines",
      "--attach-inlines / -xl",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach braces to class and struct inline function definitions. This option has precedence for all styles except Horstmann and Pico (run-in styles). It is effective for C++ files only."),
      "class FooClass\n{\n    void Foo() {\n    ...\n    }\n};",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "all braces are attached to class and struct inline method definitions:") },
    { "--attach-extern-c",
      "--attach-extern-c / -xk",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach braces to a braced extern \"C\" statement. This is done regardless of the brace style being used. This option is effective for C++ files only. An extern \"C\" statement that is part of a function definition is formatted according to the requested brace style. Braced extern \"C\" statements are unaffected by the brace style and this option is the only way to change them."),
      "#ifdef __cplusplus\nextern \"C\" {\n#endif",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "this option attaches braces to a braced extern \"C\" statement:") },
    { "--convert-tabs",
      "--convert-tabs / -c",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Converts tabs into spaces in the non-indentation part of the line. The number of spaces inserted will maintain the spacing of the tab. The current setting for spaces per tab is used. It may not produce the expected results if convert-tabs is used when changing spaces per tab. Tabs are not replaced within quotes."),
      "",
      nullptr,
      "" },
    { "--delete-empty-lines",
      "--delete-empty-lines / -xe",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Eliminates unnecessary empty lines from within functions and methods. Empty lines outside of nested functions and methods, as well as lines specified by break-block options, are kept intact."),
      "void Foo()\n{\n\n    foo1 = 1;\n\n    foo2 = 2;\n\n}",
      "void Foo()\n{\n    foo1 = 1;\n    foo2 = 2;\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--align-pointer=type",
      "--align-pointer=type / -k1",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach a pointer or reference operator (*, &, or ^) to either the variable type (left) or variable name (right), or place it between the type and name (middle). The spacing between the type and name will be preserved, if possible. This option is for C/C++, C++/CLI, and C# files. To format references separately, use the following align-reference option."),
      "char* foo1;\nchar & foo2;\nstring ^s1;",
      "char* foo1;\nchar& foo2;\nstring^ s1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-pointer=type):") },
    { "--align-pointer=middle",
      "--align-pointer=middle / -k2",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach a pointer or reference operator (*, &, or ^) to either the variable type (left) or variable name (right), or place it between the type and name (middle). The spacing between the type and name will be preserved, if possible. This option is for C/C++, C++/CLI, and C# files. To format references separately, use the following align-reference option."),
      "char* foo1;\nchar & foo2;\nstring ^s1;",
      "char * foo1;\nchar & foo2;\nstring ^ s1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-pointer=middle):") },
    { "--align-pointer=name",
      "--align-pointer=name / -k3",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach a pointer or reference operator (*, &, or ^) to either the variable type (left) or variable name (right), or place it between the type and name (middle). The spacing between the type and name will be preserved, if possible. This option is for C/C++, C++/CLI, and C# files. To format references separately, use the following align-reference option."),
      "char* foo1;\nchar & foo2;\nstring ^s1;",
      "char *foo1;\nchar &foo2;\nstring ^s1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-pointer=name):") },
    { "--align-reference=none",
      "--align-reference=none / -W0",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option will align references separate from pointers. Pointers are not changed by this option. If pointers and references are to be aligned the same, use the previous align-pointer option. The option align-reference=none will not change the reference alignment. The other options are the same as for align-pointer. This option is for C/C++, C++/CLI, and C# files."),
      "",
      nullptr,
      "" },
    { "--align-reference=type",
      "--align-reference=type / -W1",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option will align references separate from pointers. Pointers are not changed by this option. If pointers and references are to be aligned the same, use the previous align-pointer option. The option align-reference=none will not change the reference alignment. The other options are the same as for align-pointer. This option is for C/C++, C++/CLI, and C# files."),
      "char &foo1;",
      "char& foo1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-reference=type):") },
    { "--align-reference=middle",
      "--align-reference=middle / -W2",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option will align references separate from pointers. Pointers are not changed by this option. If pointers and references are to be aligned the same, use the previous align-pointer option. The option align-reference=none will not change the reference alignment. The other options are the same as for align-pointer. This option is for C/C++, C++/CLI, and C# files."),
      "char& foo2;",
      "char & foo2;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-reference=middle):") },
    { "--align-reference=name",
      "--align-reference=name / -W3",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option will align references separate from pointers. Pointers are not changed by this option. If pointers and references are to be aligned the same, use the previous align-pointer option. The option align-reference=none will not change the reference alignment. The other options are the same as for align-pointer. This option is for C/C++, C++/CLI, and C# files."),
      "char& foo3;",
      "char &foo3;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with align-reference=name):") },
};

static QString escape(const QString &text)
{
    return text.toHtmlEscaped();
}

// Puts the "before" and "after" snippets next to each other so that the effect of
// the option can be compared at a glance. Monospace font keeps the columns aligned.
static QString sideBySide(const QString &before, const QString &after)
{
    const QStringList leftLines = before.split('\n');
    const QStringList rightLines = after.split('\n');

    int leftWidth = 0;
    for (const QString &line : leftLines)
        leftWidth = qMax(leftWidth, int(line.size()));
    leftWidth += 4;

    QString result = QStringLiteral("Before").leftJustified(leftWidth, ' ');
    result += QStringLiteral("After\n");

    const qsizetype count = qMax(leftLines.size(), rightLines.size());
    for (qsizetype i = 0; i < count; ++i) {
        const QString left = i < leftLines.size() ? leftLines.at(i) : QString();
        const QString right = i < rightLines.size() ? rightLines.at(i) : QString();
        result += left.leftJustified(leftWidth, ' ') + right + QLatin1Char('\n');
    }
    return result;
}

QString optionHelp(const QString &option)
{
    static const QHash<QString, const OptionDoc *> docs = [] {
        QHash<QString, const OptionDoc *> map;
        for (const OptionDoc &doc : kOptionDocs)
            map.insert(QString::fromLatin1(doc.option), &doc);
        return map;
    }();

    const auto it = docs.constFind(option);
    if (it == docs.constEnd())
        return QString();
    const OptionDoc &doc = **it;

    QString html = QStringLiteral("<b>%1</b>").arg(escape(QString::fromUtf8(doc.title)));
    html += QStringLiteral("<p>%1</p>").arg(escape(
        QCoreApplication::translate("QtC::Astyle", doc.description)));

    if (doc.after) {
        html += QStringLiteral("<pre>%1</pre>")
                    .arg(sideBySide(QString::fromUtf8(doc.before),
                                   QString::fromUtf8(doc.after))
                             .toHtmlEscaped());
    } else if (doc.before && *doc.before) {
        const QString caption = doc.caption && *doc.caption
                                    ? escape(QCoreApplication::translate(
                                          "QtC::Astyle", doc.caption))
                                    : escape(QCoreApplication::translate(
                                          "QtC::Astyle", "Result:"));
        html += QStringLiteral("<p><i>%1</i></p>").arg(caption);
        html += QStringLiteral("<pre>%1</pre>")
                    .arg(escape(QString::fromUtf8(doc.before)));
    }

    return html;
}

} // namespace Astyle::Internal
