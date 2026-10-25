"""Merge the plugin's .ts contexts into Qt Creator's qtcreator_<lang>.ts.

Qt Creator has no per-plugin translation files: every plugin's strings live in
the single share/qtcreator/translations/qtcreator_<lang>.ts, which the build
compiles into qtcreator_<lang>.qm and the IDE loads at startup. So the contexts
of src/plugins/astyle/translations/astyle_zh_CN.ts have to be copied into the
big file to take effect.

The merge is deliberately conservative: existing contexts are reused/replaced
by name, the contexts stay sorted the way lupdate sorts them, and everything
else in the file is left byte for byte untouched (line endings included).

Usage: python merge_into_qtcreator_ts.py <plugin.ts> <qtcreator_xx.ts> [--check]
"""

import os
import re
import shutil
import sys

# Directory of the plugin sources as seen from share/qtcreator/translations/.
PLUGIN_SOURCE_DIR = "../../../src/plugins/astyle/"

CONTEXT_RE = re.compile(r"<context>.*?</context>\n?", re.S)
NAME_RE = re.compile(r"<name>(.*?)</name>")
LOCATION_RE = re.compile(r'(<location filename=")([^"]+)(" line=")\+(\d+)(")')


def parse_contexts(text):
    contexts = {}
    for match in CONTEXT_RE.finditer(text):
        block = match.group(0)
        name = NAME_RE.search(block).group(1)
        contexts[name] = block
    return contexts


def normalise_locations(block):
    """Point the locations at the real source files and make them absolute.

    Within one context lupdate writes the first location of a file with the
    file name and an absolute line, later ones as a delta. The generated block
    starts each file with a delta (relative to the plugin .ts), so the plus
    signs have to go once the block is moved to another .ts.
    """

    def replace(match):
        filename = os.path.basename(match.group(2))
        return match.group(1) + PLUGIN_SOURCE_DIR + filename + match.group(3) \
            + match.group(4) + match.group(5)

    # Locations without a file name are deltas by design and stay as they are.
    return LOCATION_RE.sub(replace, block)


def merge(target_text, plugin_text):
    newline = "\r\n" if "\r\n" in target_text else "\n"
    target = target_text.replace("\r\n", "\n")

    incoming = parse_contexts(plugin_text.replace("\r\n", "\n"))
    if not incoming:
        raise SystemExit("no <context> found in the plugin .ts")

    # Remember where every existing context starts and ends, so that the file
    # can be patched at exactly these offsets and everything else is preserved.
    spans = [(m.start(), m.end(), NAME_RE.search(m.group(0)).group(1))
             for m in CONTEXT_RE.finditer(target)]

    # Cut out the contexts we are going to (re-)insert.
    for start, end, name in reversed(spans):
        if name in incoming:
            target = target[:start] + target[end:]

    for name in sorted(incoming):
        block = normalise_locations(incoming[name])

        # Insert before the first context that sorts after the new one, which
        # is the order lupdate produces. Re-read the spans, they have shifted.
        offset = None
        for match in CONTEXT_RE.finditer(target):
            if NAME_RE.search(match.group(0)).group(1) > name:
                offset = match.start()
                break
        if offset is None:
            offset = target.index("</TS>")

        target = target[:offset] + block + target[offset:]

    return target.replace("\n", newline), sorted(incoming)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2

    plugin_path, target_path = argv[1], argv[2]
    check_only = "--check" in argv[3:]

    with open(plugin_path, "r", encoding="utf-8") as handle:
        plugin_text = handle.read()
    with open(target_path, "r", encoding="utf-8", newline="") as handle:
        target_text = handle.read()

    merged, names = merge(target_text, plugin_text)

    if merged == target_text:
        print("%s: already up to date (%s)" % (target_path, ", ".join(names)))
        return 0

    if check_only:
        print("%s: out of date, run without --check (%s)" % (target_path, ", ".join(names)))
        return 1

    backup_path = target_path + ".bak"
    shutil.copyfile(target_path, backup_path)
    with open(target_path, "w", encoding="utf-8", newline="") as handle:
        handle.write(merged)

    print("%s: merged %s (backup: %s)" % (target_path, ", ".join(names), backup_path))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
