#!/usr/bin/env python3
"""Generate astyleoptionhelp.cpp from the official Artistic Style manual.

Source: AStyle 3.6.20 doc/astyle.html (the same version bundled in 3rdparty/astyle).
Run:    python gen_option_help.py <path to astyle.html> <output .cpp>
"""

import html
import io
import re
import sys

# Translation context of the plugin, same as in astyletr.h.
TR_CONTEXT = "QtC::Astyle"

# Options shown in the settings page, in the order they appear there.
TOGGLES = [
    "--indent-classes", "--indent-modifiers", "--indent-switches", "--indent-cases",
    "--indent-namespaces", "--indent-labels", "--indent-preproc-block",
    "--indent-preproc-define", "--indent-preproc-cond", "--indent-col1-comments",
    "--pad-oper", "--pad-comma", "--pad-paren-out", "--pad-paren-in",
    "--pad-first-paren-out", "--pad-header", "--unpad-paren", "--pad-include",
    "--break-blocks", "--break-blocks=all", "--break-one-line-headers", "--break-elseifs",
    "--keep-one-line-blocks", "--keep-one-line-statements",
    "--attach-namespaces", "--attach-classes", "--attach-inlines", "--attach-extern-c",
    "--convert-tabs", "--delete-empty-lines",
    # align-pointer / align-reference are combo boxes, but their entries get the same
    # "before / after" help as the check boxes.
    "--align-pointer=type", "--align-pointer=middle", "--align-pointer=name",
    "--align-reference=none", "--align-reference=type", "--align-reference=middle",
    "--align-reference=name",
]


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


def load_sections(doc):
    src = io.open(doc, encoding="utf-8", errors="replace").read()
    marks = [(m.start(), m.group(1)) for m in re.finditer(r'<p id="_([^"]+)">', src)]
    sections = {}
    for i, (pos, name) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(src)
        sections[name] = src[pos:end]
    return sections


def parse_section(section):
    title_match = re.search(r'<code class="title">(.*?)</code>', section, re.S)
    title = norm_text(title_match.group(1)) if title_match else ""
    div = section.find('<div class="code">')
    head = section[:div] if div >= 0 else section
    desc = norm_text(re.sub(r'<code class="title">.*?</code>', "", head, flags=re.S))

    seq = []
    if div >= 0:
        rest = section[div:]
        pos = 0
        for m in re.finditer(r"<pre>(.*?)</pre>", rest, re.S):
            chunk = rest[pos:m.start()]
            if chunk:
                seq.append(("text", norm_text(chunk)))
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
    return title, desc, examples


def short_title(option, title):
    """Some sections head several values at once, e.g.
    "--align-pointer=type / -k1   --align-pointer=middle / -k2   ...".
    Keep only the part that belongs to @p option."""
    index = title.find(option)
    if index < 0:
        return title
    rest = title[index:]
    following = rest.find(" --")
    return rest[:following] if following > 0 else rest


def section_key(option, sections):
    key = option[2:]
    if key in sections:
        return key
    underscored = key.replace("-", "_")
    if underscored in sections:
        return underscored
    return key.split("=")[0]


def pick_example(option, examples):
    """For --foo=bar prefer the example whose caption mentions foo=bar.

    Sections that document several values at once (align-pointer, align-reference,
    ...) label their snippets with "becomes (with foo=bar):".  If none of the
    labels matches, the manual has no example for this value, and showing the
    first one would be wrong.
    """
    if not examples:
        return "", None, ""
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
    for option in TOGGLES:
        key = section_key(option, sections)
        used.add(key)
        title, desc, examples = parse_section(sections[key])
        before, after, caption = pick_example(option, examples)
        entries.append((option, short_title(option, title), desc, before, after, caption))

    lines = []
    lines.append("// Copyright (C) 2026 The Astyle Plugin Authors")
    lines.append("// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0")
    lines.append("")
    lines.append("// Generated from the Artistic Style 3.6.20 manual (doc/astyle.html) that ships")
    lines.append("// with the sources in 3rdparty/astyle. Do not edit the description texts by hand;")
    lines.append("// they are quoted from the manual. The <pre> examples are the ones printed there")
    lines.append("// under \"becomes:\". The manual wording is wrapped in QT_TRANSLATE_NOOP so that the")
    lines.append("// translations can translate it; option names and code samples stay untouched.")
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
