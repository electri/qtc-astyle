// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// Generated from the Artistic Style manual (https://astyle.sourceforge.net/astyle.html)
// by tools/gen_option_help.py. Do not edit the description texts by hand; they are
// quoted from the manual. The <pre> examples are the ones printed there under
// "becomes:". The manual wording is wrapped in QT_TRANSLATE_NOOP so that the
// translations can translate it; option names and code samples stay untouched.
//
// The plugin formats C++ only, so the manual's Java, C#, Objective-C and scripting
// options are not listed here.

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
    { "--max-code-length-mode=",
      "--max-code-length-mode=####",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option determines how line length is measured. Valid options are: code - code only, excluding indentation (default) total - entire line, including indentation (experimental) ignore-side-comments - ignore trailing side comments (experimental)"),
      "",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--min-conditional-indent=",
      "--min-conditional-indent=# / -m#",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Set the minimal indent that is added when a header is built of multiple lines. This indent helps to easily separate the header from the command statements that follow. The value for # indicates a number of indents and is a minimum value. The indent may be greater to align with the data on the previous line. The valid values are: 0 - no minimal indent. The lines will be aligned with the paren on the preceding line. 1 - indent at least one additional indent. 2 - indent at least two additional indents. 3 - indent at least one-half an additional indent. This is intended for large indents (e.g. 8). The default value is 2, two additional indents."),
      "// default setting makes this non-braced code clear\nif (a < b\n        || c > d)\n    foo++;\n\n// but creates an exaggerated indent in this braced code\nif (a < b\n        || c > d)\n{\n    foo++;\n}",
      "// setting makes this non-braced code less clear\nif (a < b\n    || c > d)\n    foo++;\n\n// but makes this braced code clearer\nif (a < b\n    || c > d)\n{\n    foo++;\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (when setting --min-conditional-indent=0):") },
    { "--max-continuation-indent=",
      "--max-continuation-indent=# / -M#",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Set the maximum of # spaces to indent a continuation line. The # indicates a number of columns and must not be less than 40 or greater than 120. If no value is set, the default value of 40 will be used. This option will prevent continuation lines from extending too far to the right. Setting a larger value will allow the code to be extended further to the right."),
      "",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--indent=force-tab-x=",
      "--indent=force-tab-x=# / -xT / -xT#",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This force-tab option allows the tab length to be set to a length that is different than the indent length. This may cause the indentation to be a mix of both tabs and spaces. Tabs will be used to indent, if possible. If a tab indent cannot be used, spaces will be used instead. This option sets the tab length. Treat each tab as # spaces (e.g. -xT6 / --indent=force-tab-x=6. # must be between 2 and 20. If no # is set, treats tabs as 8 spaces. To change the indent length from the default of 4 spaces the option \"indent=force-tab\" must also be used."),
      "",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--indent-continuation=",
      "--indent-continuation=# / -xt#",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Set the continuation indent for a line that ends with an opening paren '(' or an assignment '='. This includes function definitions and declarations. It will also modify the previous indent-after-paren option. The value for # indicates a number of indents. The valid values are the integer values from 0 thru 4. If this option is not used, the default value of 1 is used."),
      "isLongVariable =\n    foo1 ||\n    foo2;\n\nisLongFunction(\n    bar1,\n    bar2);",
      "isLongVariable =\n            foo1 ||\n            foo2;\n\nisLongFunction(\n            bar1,\n            bar2);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with indent-continuation=3):") },
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
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
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
    { "--indent-after-parens",
      "--indent-after-parens / -xU",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Indent, instead of align, continuation lines following lines that contain an opening paren '(' or an assignment '='. This includes function definitions and declarations and return statements. The indentation can be modified by using the following indent-continuation option. This option may be preferred for editors displaying proportional fonts."),
      "void Foo(bool bar1,\n         bool bar2)\n{\n    isLongFunction(bar1,\n                   bar2);\n\n    isLongVariable = foo1\n                     || foo2;\n}",
      "void Foo(bool bar1,\n    bool bar2)\n{\n    isLongFunction(bar1,\n        bar2);\n\n    isLongVariable = foo1\n        || foo2;\n}",
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
    { "--convert-tabs",
      "--convert-tabs / -c",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Converts tabs into spaces in the non-indentation part of the line. The number of spaces inserted will maintain the spacing of the tab. The current setting for spaces per tab is used. It may not produce the expected results if convert-tabs is used when changing spaces per tab. Tabs are not replaced within quotes."),
      "",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
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
    { "--pad-empty-paren",
      "--pad-empty-paren / -xo",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Apply padding to empty pairs of parentheses; combine with other parentheses padding options."),
      "if (isFoo())\n    bar(a, b);",
      "if ( isFoo () )\n    bar( a, b );",
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
    { "--pad-semicolon=none",
      "--pad-semicolon=none",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove space padding before a semicolon. An empty statement on a line of its own keeps its indent, and the empty expressions of a for header are left unchanged."),
      "void Foo()\n{\n    bar() ;\n    for (i = 0 ; i < 10 ; i++)\n        baz() ;\n}",
      "void Foo()\n{\n    bar();\n    for (i = 0; i < 10; i++)\n        baz();\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-include",
      "--pad-include",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding after include directives and removes superfluous whitespace."),
      "#include<stdio>\n#include   <iostream>",
      "#include <stdio>\n#include <iostream>",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-include=none",
      "--pad-include=none",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove space padding after include directives."),
      "#include<stdio>\n#include   <iostream>",
      "#include<stdio>\n#include<iostream>",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-negation",
      "--pad-negation",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding after negations."),
      "if (!isFoo(a,b))\n    bar(a,b);",
      "if (! isFoo(a, b))\n    bar(a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--pad-negation=before",
      "--pad-negation=before",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Insert space padding also before negations."),
      "if (!isFoo(a,b))\n    bar(a,b);",
      "if ( ! isFoo(a, b))\n    bar(a, b);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
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
    { "--attach-closing-while",
      "--attach-closing-while / -xV",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach the closing 'while' of a 'do-while' statement to the closing brace. This has precedence over both the brace style and the break closing braces option."),
      "do\n{\n    bar();\n    ++x;\n}\nwhile x == 1;",
      "do\n{\n    bar();\n    ++x;\n} while x == 1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-closing-braces",
      "--break-closing-braces / -y",
      QT_TRANSLATE_NOOP("QtC::Astyle", "When used with --style=java, --style=kr, --style=stroustrup, --style=linux, or --style=1tbs, this breaks closing headers (e.g. 'else', 'catch', ...) from their immediately preceding closing braces. Closing header braces are always broken with the other styles."),
      "void Foo(bool isFoo) {\n    if (isFoo) {\n        bar();\n    } else {\n        anotherBar();\n    }\n}",
      "void Foo(bool isFoo) {\n    if (isFoo) {\n        bar();\n    }\n    else {\n        anotherBar();\n    }\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (a broken 'else'):") },
    { "--add-braces",
      "--add-braces / -j",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Add braces to unbraced one line conditional statements (e.g. 'if', 'for', 'while'...). The statement must be on a single line. The braces will be added according to the requested brace style. If no style is requested the braces will be attached. Braces will NOT be added to a multi-statement line if keep-one-line-statements is requested. Braces will NOT be added to a one line block if keep-one-line-blocks is requested. If used with --add-one-line-braces, the result will be one line braces."),
      "if (isFoo)\n    isFoo = false;",
      "if (isFoo) {\n    isFoo = false;\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--add-one-line-braces",
      "--add-one-line-braces / -J",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Add one line braces to unbraced one line conditional statements (e.g. 'if', 'for', 'while'...). The statement must be on a single line. The option implies --keep-one-line-blocks and will not break the one line blocks."),
      "if (isFoo)\n    isFoo = false;",
      "if (isFoo)\n    { isFoo = false; }",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--remove-braces",
      "--remove-braces / -xj",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove braces from conditional statements (e.g. 'if', 'for', 'while'...). The statement must be a single statement on a single line. If --add-braces or --add-one-line-braces is also used the result will be to add braces. Braces will not be removed from \"One True Brace Style\", --style=1tbs."),
      "if (isFoo)\n{\n    isFoo = false;\n}",
      "if (isFoo)\n    isFoo = false;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--attach-return-type",
      "--attach-return-type / -xf  ",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach the return type to the function name. The two options are for the function definitions (-xf), and the function declarations or signatures (-xh). They are intended to undo the --break-return-type options. If used with --break-return-type, the result will be to break the return type. This option has no effect on Objective-C functions."),
      "void\nFoo(bool isFoo);",
      "void Foo(bool isFoo);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--attach-return-type=decl",
      "--attach-return-type=decl / -xh",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Attach the return type to the function name. The two options are for the function definitions (-xf), and the function declarations or signatures (-xh). They are intended to undo the --break-return-type options. If used with --break-return-type, the result will be to break the return type. This option has no effect on Objective-C functions."),
      "void\nFoo(bool isFoo);",
      "void Foo(bool isFoo);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-return-type",
      "--break-return-type / -xB  ",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Break the return type from the function name. The two options are for the function definitions (-xB), and the function declarations or signatures (-xD). If used with --attach-return-type, the result will be to break the return type. This option has no effect on Objective-C functions."),
      "void Foo(bool isFoo);",
      "void\nFoo(bool isFoo);",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-return-type=decl",
      "--break-return-type=decl / -xD",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Break the return type from the function name. The two options are for the function definitions (-xB), and the function declarations or signatures (-xD). If used with --attach-return-type, the result will be to break the return type. This option has no effect on Objective-C functions."),
      "void Foo(bool isFoo);",
      "void\nFoo(bool isFoo);",
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
    { "--break-elseifs=no-indent",
      "--break-elseifs=no-indent",
      QT_TRANSLATE_NOOP("QtC::Astyle", "This option removes extra indentation made by default when an \"if\" statement comes on the next line after \"else\"."),
      "if (x)\n{\n}\nelse\n  if (y)\n  {\n  }\n  else\n    if (z)\n    {\n    }",
      "if (x)\n{\n}\nelse\nif (y)\n{\n}\nelse\nif (z)\n{\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--break-after-logical",
      "--break-after-logical / -xL",
      QT_TRANSLATE_NOOP("QtC::Astyle", "By default logical conditionals will be placed first in the new line. This option will cause the logical conditionals to be placed last on the previous line. This option has no effect without --max-code-length."),
      "if (thisVariable1 == thatVariable1\n        || thisVariable2 == thatVariable2\n        || thisVariable3 == thatVariable3)\n    bar();",
      "if (thisVariable1 == thatVariable1 ||\n        thisVariable2 == thatVariable2 ||\n        thisVariable3 == thatVariable3)\n    bar();",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes (with break-after-logical):") },
    { "--line-between-members",
      "--line-between-members  ",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Ensures that there is at least one empty line between class members and, in C/C++, between functions. When the \"all\" parameter is omitted, two or more class fields that are not separated by an empty line stay this way (so, multiple fields can be grouped in a compact way). If the \"all\" parameter is specified, an empty line is enforced for any combinations of class members. Existing empty lines are kept intact."),
      "int i;\nint j;\nvoid foo()\n{\n}\nvoid bar()\n{\n}",
      "int i;\nint j;\n\nvoid foo()\n{\n}\n\nvoid bar()\n{\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--line-between-members=all",
      "--line-between-members=all",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Ensures that there is at least one empty line between class members and, in C/C++, between functions. When the \"all\" parameter is omitted, two or more class fields that are not separated by an empty line stay this way (so, multiple fields can be grouped in a compact way). If the \"all\" parameter is specified, an empty line is enforced for any combinations of class members. Existing empty lines are kept intact."),
      "int i;\nint j;\nvoid foo()\n{\n}\nvoid bar()\n{\n}",
      "int i;\nint j;\n\nvoid foo()\n{\n}\n\nvoid bar()\n{\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--keep-one-line-blocks",
      "--keep-one-line-blocks / -O",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Don't break one-line blocks."),
      "if (isFoo)\n{ isFoo = false; cout << isFoo << endl; }",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--keep-one-line-statements",
      "--keep-one-line-statements / -o",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Don't break complex statements and multiple statements residing on a single line."),
      "if (isFoo)\n{\n    isFoo = false; cout << isFoo << endl;\n}",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--delete-empty-lines",
      "--delete-empty-lines / -xe",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Eliminates unnecessary empty lines from within functions and methods. Empty lines outside of nested functions and methods, as well as lines specified by break-block options, are kept intact."),
      "void Foo()\n{\n\n    foo1 = 1;\n\n    foo2 = 2;\n\n}",
      "void Foo()\n{\n    foo1 = 1;\n    foo2 = 2;\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--fill-empty-lines",
      "--fill-empty-lines / -E",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Fill empty lines with the white space of the previous line."),
      "",
      nullptr,
      QT_TRANSLATE_NOOP("QtC::Astyle", "") },
    { "--squeeze-lines=",
      "--squeeze-lines=#",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove superfluous empty lines exceeding the given number."),
      "void Foo()\n{\n    foo1 = 1;\n}\n\n\n\n\nvoid Foo2()\n{\n    foo1 = 1;\n}",
      "void Foo()\n{\n    foo1 = 1;\n}\n\n\nvoid Foo2()\n{\n    foo1 = 1;\n}",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--close-templates",
      "--close-templates / -xy",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Closes whitespace between the ending angle brackets of template definitions. Closing the ending angle brackets is now allowed by the C++11 standard. Be sure your compiler supports this before making the changes."),
      "Stack< int, List< int > > stack1;",
      "Stack< int, List< int >> stack1;",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
    { "--remove-comment-prefix",
      "--remove-comment-prefix / -xp",
      QT_TRANSLATE_NOOP("QtC::Astyle", "Remove the preceding '*' in a multi-line comment that begins a line. A trailing '*', if present, is also removed. Text that is less than one indent is indented to one indent. Text greater than one indent is not changed. Multi-line comments that begin a line, but without the preceding '*', are indented to one indent for consistency. This can slightly modify the indentation of commented out blocks of code. Lines containing all '*' are left unchanged. Extra spacing is removed from the comment close '*/'."),
      "/*\n * comment line 1\n * comment line 2\n */",
      "/*\n    comment line 1\n    comment line 2\n*/",
      QT_TRANSLATE_NOOP("QtC::Astyle", "becomes:") },
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
