#!/usr/bin/env python3
"""Generate astyleoptionhelp.cpp from the official Artistic Style manual.

Source: https://astyle.sourceforge.net/astyle.html, the manual of the version
bundled in 3rdparty/astyle. The manual is slightly ahead of the library, so the
option *spellings* written by this script are always the ones the bundled
library accepts (see ASOptions::parseOption() in astyle_main.cpp); only the
explanatory text comes from the manual.

Run:    python gen_option_help.py <path to astyle.html> <output .cpp>
"""

import html
import io
import re
import sys

# Translation context of the plugin, same as in astyletr.h.
TR_CONTEXT = "QtC::Astyle"

# Options shown in the settings page, in the order they appear there.
# The plugin formats C++ only, so the Java/C#/Objective-C/scripting options of
# the manual are deliberately absent.
TOGGLES = [
    # -- General ------------------------------------------------------------
    "--max-code-length-mode=",
    "--min-conditional-indent=",
    "--max-continuation-indent=",
    "--indent=force-tab-x=",
    "--indent-continuation=",
    # align-pointer / align-reference are combo boxes, but their entries get the same
    # "before / after" help as the check boxes.
    "--align-pointer=type", "--align-pointer=middle", "--align-pointer=name",
    "--align-reference=none", "--align-reference=type", "--align-reference=middle",
    "--align-reference=name",
    # -- Indentation --------------------------------------------------------
    "--indent-classes", "--indent-modifiers", "--indent-switches", "--indent-cases",
    "--indent-namespaces", "--indent-after-parens", "--indent-labels",
    "--indent-preproc-block", "--indent-preproc-define", "--indent-preproc-cond",
    "--indent-col1-comments", "--convert-tabs",
    # -- Spacing ------------------------------------------------------------
    "--pad-oper", "--pad-comma",
    "--pad-paren-out", "--pad-paren-in", "--pad-first-paren-out", "--pad-empty-paren",
    "--pad-header", "--unpad-paren", "--pad-semicolon=none",
    "--pad-include", "--pad-include=none", "--pad-negation", "--pad-negation=before",
    # -- Braces -------------------------------------------------------------
    "--attach-namespaces", "--attach-classes", "--attach-inlines", "--attach-extern-c",
    "--attach-closing-while", "--break-closing-braces",
    "--add-braces", "--add-one-line-braces", "--remove-braces",
    "--attach-return-type", "--attach-return-type=decl",
    "--break-return-type", "--break-return-type=decl",
    # -- Line breaks --------------------------------------------------------
    "--break-blocks", "--break-blocks=all", "--break-one-line-headers",
    "--break-elseifs", "--break-elseifs=no-indent", "--break-after-logical",
    "--line-between-members", "--line-between-members=all",
    "--keep-one-line-blocks", "--keep-one-line-statements",
    # -- Blank lines and miscellaneous --------------------------------------
    "--delete-empty-lines", "--fill-empty-lines", "--squeeze-lines=",
    "--close-templates", "--remove-comment-prefix",
]

# Options whose manual section is filed under a different name.
SECTION_ALIAS = {
    "--break-elseifs=no-indent": "no-indent-if-after-else",
    "--indent=force-tab-x=": "indent=force-tab-x",
}

# Options the manual documents under a name the bundled library does not accept.
# Only the heading is replaced, the wording still comes from the manual.
TITLE_OVERRIDE = {
    "--break-elseifs=no-indent": "--break-elseifs=no-indent",
}

# Manual sections that head several options at once do not say which option a
# sample belongs to. For those the sample is picked here.
EXAMPLE_CAPTION = {
    "--break-after-logical": "becomes (with break-after-logical):",
}

# The manual files --break-after-logical under --max-code-length and shares one
# description for both, which reads badly for a separate check box. The wording
# below is the paragraph the manual dedicates to the option.
HAND_WRITTEN = {
    "--break-after-logical": (
        "--break-after-logical / -xL",
        "By default logical conditionals will be placed first in the new line. "
        "This option will cause the logical conditionals to be placed last on the "
        "previous line. This option has no effect without --max-code-length.",
        "if (thisVariable1 == thatVariable1\n        || thisVariable2 == thatVariable2\n"
        "        || thisVariable3 == thatVariable3)\n    bar();",
        "if (thisVariable1 == thatVariable1 ||\n        thisVariable2 == thatVariable2 ||\n"
        "        thisVariable3 == thatVariable3)\n    bar();",
        "becomes (with break-after-logical):",
    ),
}


def strip_tags(s):
    s = re.sub(r"<span[^>]*>|</span>", "", s)
    s = re.sub(r"<br\s*/?>", "\n", s)
    s = re.sub(r"<[^>]+>", "", s)
    return html.unescape(s)


def norm_text(s):
    return re.sub(r"\s+", " ", strip_tags(s)).strip()


def norm_code(s):
    lines = [line.rstrip() for line in strip_tags(s).splitlines()]
    while lines and not lines[0].strip():
        lines.pop(0)
    while lines and not lines[-1].strip():
        lines.pop()
    indent = min((len(l) - len(l.lstrip()) for l in lines if l.strip()), default=0)
    return "\n".join(line[indent:].replace("\t", "    ") for line in lines)


def split_chunks(section):
    """Split one <p id="_..."> region into the blocks that each introduce an
    option. A region may document several options, sometimes in one paragraph
    ("--line-between-members / --line-between-members=all") and sometimes in
    separate paragraphs with their own description ("--pad-include" and
    "--pad-include=none"). Both cases must keep their own text."""
    starts = []
    for m in re.finditer(r'<code class="title">', section):
        open_p = section.rfind("<p", 0, m.start())
        start = open_p if open_p >= 0 else m.start()
        if not starts or starts[-1] != start:
            starts.append(start)
    return [section[start:starts[i + 1] if i + 1 < len(starts) else len(section)]
            for i, start in enumerate(starts)]


def load_sections(doc):
    src = io.open(doc, encoding="utf-8", errors="replace").read()
    marks = [(m.start(), m.group(1)) for m in re.finditer(r'<p id="_([^"]+)">', src)]
    sections = {}
    for i, (pos, name) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(src)
        sections.setdefault(name, []).extend(split_chunks(src[pos:end]))
    return sections


def parse_chunk(chunk):
    # Every heading of the block, then the shared description.
    titles = [norm_text(t) for t in
              re.findall(r'<code class="title">(.*?)</code>', chunk, re.S)]
    title = "   ".join(titles)
    div = chunk.find('<div class="code">')
    head = chunk[:div] if div >= 0 else chunk
    desc = norm_text(re.sub(r'<code class="title">.*?</code>', "", head, flags=re.S))

    seq = []
    if div >= 0:
        rest = chunk[div:]
        pos = 0
        for m in re.finditer(r"<pre>(.*?)</pre>", rest, re.S):
            gap = rest[pos:m.start()]
            if gap:
                seq.append(("text", norm_text(gap)))
            seq.append(("code", norm_code(m.group(1))))
            pos = m.end()
        if rest[pos:]:
            seq.append(("text", norm_text(rest[pos:])))

    examples = []  # (before, after, caption)
    i = 0
    while i < len(seq):
        kind, value = seq[i]
        if kind != "code":
            i += 1
            continue
        if (i + 2 < len(seq) and seq[i + 1][0] == "text"
                and "becom" in seq[i + 1][1] and seq[i + 2][0] == "code"):
            examples.append((value, seq[i + 2][1], seq[i + 1][1]))
            i += 3
        else:
            caption = seq[i - 1][1] if i > 0 and seq[i - 1][0] == "text" else ""
            examples.append((value, None, caption))
            i += 1
    return titles, title, desc, examples


def short_title(option, title):
    """Some blocks head several values at once, e.g.
    "--align-pointer=type / -k1   --align-pointer=middle / -k2   ...".
    Keep only the part that belongs to @p option."""
    index = title.find(option)
    if index < 0:
        return title
    rest = title[index:]
    following = rest.find(" --")
    return rest[:following] if following > 0 else rest


def section_key(option, sections):
    if option in SECTION_ALIAS:
        return SECTION_ALIAS[option]
    key = option[2:]
    for candidate in (key, key.replace("-", "_"), key.split("=")[0]):
        if candidate in sections:
            return candidate
    return key.split("=")[0]


def find_chunk(option, sections):
    """Pick the block of the manual that documents @p option. Sections that
    document several options are told apart by their heading."""
    chunks = sections.get(section_key(option, sections))
    if not chunks:
        return None
    for chunk in chunks:
        if norm_text(chunk.split("</p>", 1)[0].replace('<code class="title">', "\n")
                     .replace("</code>", "")) == option:
            return chunk
    wanted = option[2:]
    for chunk in chunks:
        if wanted in norm_text(chunk.split("</p>", 1)[0]):
            return chunk
    return chunks[0]


def pick_example(option, examples):
    """For --foo=bar prefer the example whose caption mentions foo=bar.

    Sections that document several values at once label their snippets with
    "becomes (with foo=bar):".  If none of the labels matches, the manual has no
    example for this value, and showing the first one would be wrong.
    """
    if not examples:
        return "", None, ""
    if option in EXAMPLE_CAPTION:
        for before, after, caption in examples:
            if (caption or "") == EXAMPLE_CAPTION[option]:
                return before, after, caption
    if "=" in option:
        wanted = option[2:]
        for before, after, caption in examples:
            if wanted in (caption or ""):
                return before, after, caption
        if any("with " in (caption or "") for _, _, caption in examples):
            return "", None, ""
    return examples[0]


def c_string(text):
    if text is None:
        return "nullptr"
    out = text.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")
    return '"%s"' % out


def c_translatable(text):
    """Same literal, but marked for lupdate so that the manual wording can be
    translated. The option names and the code samples never go through this:
    they must stay exactly as AStyle prints them."""
    if text is None:
        return "nullptr"
    return 'QT_TRANSLATE_NOOP("%s", %s)' % (TR_CONTEXT, c_string(text))


def main():
    doc, out = sys.argv[1], sys.argv[2]
    sections = load_sections(doc)

    entries = []
    used = set()
    missing = []
    for option in TOGGLES:
        if option in HAND_WRITTEN:
            entries.append((option,) + HAND_WRITTEN[option])
            continue
        chunk = find_chunk(option, sections)
        if chunk is None:
            missing.append(option)
            continue
        used.add(section_key(option, sections))
        titles, title, desc, examples = parse_chunk(chunk)
        before, after, caption = pick_example(option, examples)
        heading = TITLE_OVERRIDE.get(option) or short_title(option, title)
        entries.append((option, heading, desc, before, after, caption))

    if missing:
        sys.exit("no manual section for: %s" % ", ".join(missing))

    lines = []
    lines.append("// Copyright (C) 2026 The Astyle Plugin Authors")
    lines.append("// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0")
    lines.append("")
    lines.append("// Generated from the Artistic Style manual (https://astyle.sourceforge.net/astyle.html)")
    lines.append("// by tools/gen_option_help.py. Do not edit the description texts by hand; they are")
    lines.append("// quoted from the manual. The <pre> examples are the ones printed there under")
    lines.append("// \"becomes:\". The manual wording is wrapped in QT_TRANSLATE_NOOP so that the")
    lines.append("// translations can translate it; option names and code samples stay untouched.")
    lines.append("//")
    lines.append("// The plugin formats C++ only, so the manual's Java, C#, Objective-C and scripting")
    lines.append("// options are not listed here.")
    lines.append("")
    lines.append('#include "astyleoptionhelp.h"')
    lines.append("")
    lines.append("#include <QCoreApplication>")
    lines.append("#include <QHash>")
    lines.append("#include <QString>")
    lines.append("")
    lines.append("namespace Astyle::Internal {")
    lines.append("")
    lines.append("struct OptionDoc")
    lines.append("{")
    lines.append("    const char *option;")
    lines.append("    const char *title;")
    lines.append("    const char *description;")
    lines.append("    const char *before;")
    lines.append("    const char *after;")
    lines.append("    const char *caption;")
    lines.append("};")
    lines.append("")
    lines.append("static const OptionDoc kOptionDocs[] = {")
    for option, title, desc, before, after, caption in entries:
        lines.append("    { %s," % c_string(option))
        lines.append("      %s," % c_string(title))
        lines.append("      %s," % c_translatable(desc))
        lines.append("      %s," % c_string(before))
        lines.append("      %s," % c_string(after))
        lines.append("      %s }," % c_translatable(caption))
    lines.append("};")
    lines.append("")
    lines.append("static QString escape(const QString &text)")
    lines.append("{")
    lines.append("    return text.toHtmlEscaped();")
    lines.append("}")
    lines.append("")
    lines.append("// Puts the \"before\" and \"after\" snippets next to each other so that the effect of")
    lines.append("// the option can be compared at a glance. Monospace font keeps the columns aligned.")
    lines.append("static QString sideBySide(const QString &before, const QString &after)")
    lines.append("{")
    lines.append("    const QStringList leftLines = before.split('\\n');")
    lines.append("    const QStringList rightLines = after.split('\\n');")
    lines.append("")
    lines.append("    int leftWidth = 0;")
    lines.append("    for (const QString &line : leftLines)")
    lines.append("        leftWidth = qMax(leftWidth, int(line.size()));")
    lines.append("    leftWidth += 4;")
    lines.append("")
    lines.append("    QString result = QStringLiteral(\"Before\").leftJustified(leftWidth, ' ');")
    lines.append("    result += QStringLiteral(\"After\\n\");")
    lines.append("")
    lines.append("    const qsizetype count = qMax(leftLines.size(), rightLines.size());")
    lines.append("    for (qsizetype i = 0; i < count; ++i) {")
    lines.append("        const QString left = i < leftLines.size() ? leftLines.at(i) : QString();")
    lines.append("        const QString right = i < rightLines.size() ? rightLines.at(i) : QString();")
    lines.append("        result += left.leftJustified(leftWidth, ' ') + right + QLatin1Char('\\n');")
    lines.append("    }")
    lines.append("    return result;")
    lines.append("}")
    lines.append("")
    lines.append("QString optionHelp(const QString &option)")
    lines.append("{")
    lines.append("    static const QHash<QString, const OptionDoc *> docs = [] {")
    lines.append("        QHash<QString, const OptionDoc *> map;")
    lines.append("        for (const OptionDoc &doc : kOptionDocs)")
    lines.append("            map.insert(QString::fromLatin1(doc.option), &doc);")
    lines.append("        return map;")
    lines.append("    }();")
    lines.append("")
    lines.append("    const auto it = docs.constFind(option);")
    lines.append("    if (it == docs.constEnd())")
    lines.append("        return QString();")
    lines.append("    const OptionDoc &doc = **it;")
    lines.append("")
    lines.append("    QString html = QStringLiteral(\"<b>%1</b>\").arg(escape(QString::fromUtf8(doc.title)));")
    lines.append("    html += QStringLiteral(\"<p>%1</p>\").arg(escape(")
    lines.append("        QCoreApplication::translate(\"%s\", doc.description)));" % TR_CONTEXT)
    lines.append("")
    lines.append("    if (doc.after) {")
    lines.append("        html += QStringLiteral(\"<pre>%1</pre>\")")
    lines.append("                    .arg(sideBySide(QString::fromUtf8(doc.before),")
    lines.append("                                   QString::fromUtf8(doc.after))")
    lines.append("                             .toHtmlEscaped());")
    lines.append("    } else if (doc.before && *doc.before) {")
    lines.append("        const QString caption = doc.caption && *doc.caption")
    lines.append("                                    ? escape(QCoreApplication::translate(")
    lines.append("                                          \"%s\", doc.caption))" % TR_CONTEXT)
    lines.append("                                    : escape(QCoreApplication::translate(")
    lines.append("                                          \"%s\", \"Result:\"));" % TR_CONTEXT)
    lines.append("        html += QStringLiteral(\"<p><i>%1</i></p>\").arg(caption);")
    lines.append("        html += QStringLiteral(\"<pre>%1</pre>\")")
    lines.append("                    .arg(escape(QString::fromUtf8(doc.before)));")
    lines.append("    }")
    lines.append("")
    lines.append("    return html;")
    lines.append("}")
    lines.append("")
    lines.append("} // namespace Astyle::Internal")
    lines.append("")

    io.open(out, "w", encoding="utf-8", newline="\n").write("\n".join(lines))
    print("wrote", out, "with", len(entries), "entries; sections used:", len(used))


if __name__ == "__main__":
    main()
