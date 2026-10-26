// Standalone smoke test for the Astyle plugin formatting core.
// Compiles the plugin's astyleformatter.cpp unchanged and checks
//  - every option switch the settings page can emit is understood by AStyle,
//  - line endings, byte order marks and legacy encodings survive formatting.

#include "astyleformatter.h"

#include <QByteArray>
#include <QString>
#include <QStringList>

#include <cstdio>

using namespace Astyle::Internal;

static int g_failures = 0;

static void check(bool ok, const QString &what, const QString &detail = {})
{
    if (ok) {
        std::printf("  ok   %s\n", qPrintable(what));
        return;
    }
    ++g_failures;
    std::printf("  FAIL %s%s%s\n", qPrintable(what),
                detail.isEmpty() ? "" : " -- ", qPrintable(detail));
}

static void checkSwitches()
{
    std::printf("[switches]\n");

    // Everything the settings page can emit. The plugin formats C++ only, so
    // --mode=c is the sole mode; the Java, C#, Objective-C and scripting modes
    // and options are not reachable from the page any more.
    const QStringList switches = {
        "--style=allman", "--style=java", "--style=kr", "--style=stroustrup",
        "--style=whitesmith", "--style=vtk", "--style=ratliff", "--style=gnu",
        "--style=linux", "--style=horstmann", "--style=1tbs", "--style=google",
        "--style=mozilla", "--style=webkit", "--style=pico", "--style=lisp",
        "--indent=spaces=4", "--indent=tab=4", "--indent=force-tab=4",
        "--indent=force-tab-x=8",
        "--indent-continuation=2",
        "--min-conditional-indent=0", "--min-conditional-indent=1",
        "--min-conditional-indent=2", "--min-conditional-indent=3",
        "--max-continuation-indent=40", "--max-continuation-indent=120",
        "--max-code-length=50", "--max-code-length=200",
        "--max-code-length-mode=code", "--max-code-length-mode=total",
        "--max-code-length-mode=ignore-side-comments",
        "--indent-classes", "--indent-modifiers", "--indent-switches", "--indent-cases",
        "--indent-namespaces", "--indent-after-parens", "--indent-labels",
        "--indent-preproc-block", "--indent-preproc-define", "--indent-preproc-cond",
        "--indent-col1-comments", "--convert-tabs",
        "--pad-oper", "--pad-comma", "--pad-paren-out", "--pad-paren-in",
        "--pad-first-paren-out", "--pad-empty-paren", "--pad-header", "--unpad-paren",
        "--pad-semicolon=none", "--pad-include", "--pad-include=none",
        "--pad-negation", "--pad-negation=before",
        "--align-pointer=type", "--align-pointer=middle", "--align-pointer=name",
        "--align-reference=none", "--align-reference=type", "--align-reference=middle",
        "--align-reference=name",
        "--attach-namespaces", "--attach-classes", "--attach-inlines",
        "--attach-extern-c", "--attach-closing-while", "--break-closing-braces",
        "--add-braces", "--add-one-line-braces", "--remove-braces",
        "--attach-return-type", "--attach-return-type=decl",
        "--break-return-type", "--break-return-type=decl",
        "--break-blocks", "--break-blocks=all", "--break-one-line-headers",
        "--break-elseifs", "--break-elseifs=no-indent", "--break-after-logical",
        "--line-between-members", "--line-between-members=all",
        "--keep-one-line-blocks", "--keep-one-line-statements",
        "--delete-empty-lines", "--fill-empty-lines", "--squeeze-lines=1",
        "--close-templates", "--remove-comment-prefix",
        "--mode=c",
    };

    for (const QString &option : switches) {
        const QStringList problems = validateOptions(option);
        check(problems.isEmpty(), "understood: " + option, problems.join("; "));
    }

    const QStringList bogus = validateOptions("--this-option-does-not-exist");
    check(!bogus.isEmpty(), "unknown option is reported", bogus.join("; "));
}

static void checkLineEndings()
{
    std::printf("[line endings]\n");

    const QByteArray crlf = "int  main()\r\n{\r\nreturn 0;\r\n}\r\n";
    const FormatResult result = formatBytes(crlf, "--style=allman");
    check(result.ok, "crlf: formatting succeeded", result.errors.join("; "));
    check(result.text.contains("\r\n"), "crlf: kept carriage returns");
    QByteArray withoutCrlf = result.text;
    withoutCrlf.replace("\r\n", "");
    check(!withoutCrlf.contains('\n'), "crlf: no bare newlines introduced");
    check(result.text.contains("{\r\n"), "crlf: allman brace placement");

    const QByteArray lf = "int main()\n{\nreturn 0;\n}\n";
    const FormatResult lfResult = formatBytes(lf, "--style=allman");
    check(lfResult.ok && !lfResult.text.contains('\r'), "lf: stays lf");

    const QByteArray noFinalNewline = "int main()\r\n{\r\nreturn 0;\r\n}";
    const FormatResult noFinalResult = formatBytes(noFinalNewline, "--style=allman");
    check(noFinalResult.ok && !noFinalResult.text.endsWith('\n'),
          "crlf: missing final newline is not added");
}

static void checkEncodings()
{
    std::printf("[encodings]\n");

    // GBK: "// 中文" is 2F 2F 20 D6 D0 CE C4
    QByteArray gbk = "// ";
    gbk.append("\xd6\xd0\xce\xc4", 4);
    gbk.append("\nint  main()\n{\nreturn 0;\n}\n");

    const FormatResult gbkResult = formatBytes(gbk, "--style=allman");
    check(gbkResult.ok, "gbk: formatting succeeded", gbkResult.errors.join("; "));
    check(gbkResult.text.contains(QByteArray("\xd6\xd0\xce\xc4", 4)),
          "gbk: chinese bytes untouched");

    // UTF-8
    QByteArray utf8 = QString::fromUtf8("// \xe4\xb8\xad\xe6\x96\x87\nint  main()\n{\nreturn 0;\n}\n").toUtf8();
    const FormatResult utf8Result = formatBytes(utf8, "--style=allman");
    check(utf8Result.ok && utf8Result.text.contains("\xe4\xb8\xad\xe6\x96\x87"),
          "utf8: chinese bytes untouched");

    // UTF-8 with BOM
    QByteArray bomUtf8("\xef\xbb\xbf", 3);
    const QByteArray utf8Body = "int  main()\n{\nreturn 0;\n}\n";
    const FormatResult bomResult = formatBytes(bomUtf8 + utf8Body, "--style=allman");
    check(bomResult.ok, "utf8 bom: formatting succeeded", bomResult.errors.join("; "));
    check(bomResult.text.startsWith("\xef\xbb\xbf"), "utf8 bom: bom preserved");
    check(bomResult.text.count("\xef\xbb\xbf") == 1, "utf8 bom: bom not duplicated");

    // UTF-16 LE with BOM
    const QString utf16Source = QStringLiteral("int  main()\n{\nreturn 0;\n}\n");
    QByteArray utf16("\xff\xfe", 2);
    for (const QChar c : utf16Source)
        utf16.append(c.unicode() <= 0xff ? char(c.unicode())
                                         : char(0));  // ascii only in this sample
    for (const QChar c : utf16Source)
        utf16.append(char(0));
    // The loop above is intentionally replaced by a correct encoder below.
    utf16.truncate(2);
    for (const QChar c : utf16Source) {
        utf16.append(char(ushort(c.unicode()) & 0xff));
        utf16.append(char((ushort(c.unicode()) >> 8) & 0xff));
    }

    const FormatResult utf16Result = formatBytes(utf16, "--style=allman");
    check(utf16Result.ok, "utf16le: formatting succeeded", utf16Result.errors.join("; "));
    check(utf16Result.text.startsWith("\xff\xfe"), "utf16le: bom preserved");
    check(utf16Result.text.size() % 2 == 0, "utf16le: even byte count");
    check(utf16Result.text.contains(QByteArray("{\x00", 2)), "utf16le: content is utf16");
}

static void checkIdempotency()
{
    std::printf("[stability]\n");

    const QByteArray source =
        "class Foo{public:\n"
        "void  bar(int*a,int  b){if(a){*a=b;}}\n"
        "};\n";
    const QStringList optionSets = {
        "--style=allman --pad-oper --align-pointer=name --indent=spaces=4",
        "--style=kr --pad-paren-out --indent=spaces=2 --indent-switches",
        "--style=google --max-code-length=40 --break-blocks",
    };

    for (const QString &options : optionSets) {
        const FormatResult first = formatBytes(source, options);
        check(first.ok, "first pass ok: " + options, first.errors.join("; "));
        const FormatResult second = formatBytes(first.text, options);
        check(second.ok && second.text == first.text,
              "second pass is a no-op: " + options);
    }
}

int main()
{
    std::printf("Artistic Style plugin core test, library version %s\n",
                qPrintable(libraryVersion()));

    checkSwitches();
    checkLineEndings();
    checkEncodings();
    checkIdempotency();

    std::printf("\n%s (%d failure(s))\n", g_failures == 0 ? "PASSED" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
