# Artistic Style (astyle) plugin for Qt Creator 20.0.2

**English** | [简体中文](README-zh.md)

A plugin for Qt Creator that formats source files with
[Artistic Style](https://astyle.sourceforge.net/).

Artistic Style (AStyle below) is embedded as a **static library**, so there is no external
executable, no temporary file and no child process: the formatter runs in process on the
**current file** or on **all source files of the current project**.

- Original project: <https://gitee.com/zhouliqiu/qtc_astyle> (AStyle 2.04, written for the
  Qt Creator 8 era, no longer compiles against current versions)
- AStyle upstream: <https://gitlab.com/saalen/astyle>
- Embedded version: **AStyle 3.6.20** (MIT), see `3rdparty/astyle/VERSION.txt`
- Repository: <https://github.com/electri/qtc-astyle>

> **The repository root is `src/plugins/astyle/` of a Qt Creator source tree.**
> The plugin has to be built together with the IDE (the `add_qtc_plugin` macro, the same Qt,
> the same compiler), so the way to use it is to drop it into a source tree:

```bash
cd <qtcreator-source>
git clone https://github.com/electri/qtc-astyle.git src/plugins/astyle

# Make sure src/plugins/CMakeLists.txt contains this line (inserted in alphabetical order)
grep -q 'add_subdirectory(astyle)' src/plugins/CMakeLists.txt \
  || sed -i '/add_subdirectory(autotest)/i add_subdirectory(astyle)' src/plugins/CMakeLists.txt
```

Then reconfigure with CMake and build the `Astyle` target, as described in section 2.

---

## 1. Layout

```
(repository root = src/plugins/astyle/ of the source tree)
├── CMakeLists.txt                     # add_qtc_plugin(Astyle ...)
├── Astyle.json.in                     # plugin metadata (version and dependencies filled in by CMake)
├── astyle.qrc                         # settings category icon
├── astyleconstants.h                  # menu, action and settings page ids
├── astyletr.h
├── astyleformatter.h/.cpp             # core: calls the AStyle library, BOM/encoding/line ending handling, option validation
├── astyleoptionhelp.h/.cpp            # tooltip for every option (generated from the official manual, with Before/After)
├── astylesettings.h/.cpp              # options page (AspectContainer) + .astylerc lookup + option assembly
├── astyleplugin.cpp                   # IPlugin: menu, actions, single file formatting
├── astyleprojectdialog.h/.cpp         # project wide dialog: file list, checkboxes, progress, per file result
├── images/settingscategory_astyle.png
├── tools/gen_option_help.py           # regenerates the tooltips above from the official AStyle manual
├── tools/fill_translations.py         # translation table + writes the translations back into the .ts
├── tools/merge_into_qtcreator_ts.py   # merges the plugin .ts context into qtcreator_zh_CN.ts
├── translations/astyle_zh_CN.ts       # Simplified Chinese texts (context QtC::Astyle, 282 messages)
├── tests/standalone/                  # standalone core unit test (not part of the Qt Creator build)
└── 3rdparty/astyle/                   # AStyle sources (static library with ASTYLE_LIB, target AstyleLib)
```

---

## 2. Building

The plugin is built inside a Qt Creator source tree, with the same Qt and the same compiler as
the IDE. That is the only reliable way to ship it.

```bash
# from an already configured build directory
cmake --build <build-dir> --target Astyle
```

On Windows an MSVC environment is required: run the command above from the
**x64 Native Tools Command Prompt for VS**, or `call "<VS>\VC\Auxiliary\Build\vcvars64.bat"`
in a normal command prompt first.

Output:

```
<build-dir>/lib/qtcreator/plugins/Astyle.dll
```

Installation (overwriting the copy of an installed IDE works as well):

```
<QtCreator>/lib/qtcreator/plugins/Astyle.dll
```

After startup the plugin appears as "Artistic Style" in *Help > About Plugins*.

> **The plugin id has to be lowercase**: `"Id"` in `Astyle.json.in` is `astyle` (the target name
> and the DLL file name remain `Astyle`; the two do not have to match). `PluginSpec` rejects a
> plugin whose `id.isLower()` check fails and shows `Plugin id "Astyle" must be lowercase`.
>
> Also note that `Q_PLUGIN_METADATA(FILE "Astyle.json")` only re-reads the json when **moc runs
> again**. After editing `Astyle.json.in` alone you must `touch astyleplugin.cpp` before building,
> otherwise ninja considers the target up to date.

### Core unit test (optional)

```bash
cmake -S tests/standalone -B build/astest -G Ninja \
      -DCMAKE_PREFIX_PATH=<Qt 6 prefix>
cmake --build build/astest
build/astest/astyle-core-test
```

The test compiles `astyleformatter.cpp` directly and covers:

- **Every** AStyle switch the options page can produce is accepted by the library (so a typo
  cannot fail silently at run time)
- CRLF / LF / missing final newline, and preservation of the line ending style
- GBK, UTF-8, UTF-8 BOM and UTF-16 LE BOM stay byte identical across formatting
- Formatting twice is idempotent (the result does not change on a second pass)

---

## 3. Usage

Menu **Tools > Artistic Style**:

| Menu entry | Description | Shortcut |
| --- | --- | --- |
| Format Current File | Formats the whole file in the current editor, **keeping the cursor, the folded blocks and the scroll position; undoable with Ctrl+Z** | `Ctrl+Alt+F` |
| Format Current Project... | Opens the **project file list dialog**, formats the checked files and writes them to disk | — |
| Artistic Style Options... | Opens the options page of this plugin | — |

The options page lives at **Edit > Preferences > Artistic Style** (a separate category on the left).

### Project wide dialog

`Format Current Project...` no longer formats everything behind a single confirmation box. It
first lists every source file of the project whose suffix matches:

```
┌───────────────────────────────────────────────────────────────┐
│ Project: [ plugin.pro         ▼ ]   ← only shown for several projects
│ Filter:  [                     ]   ← filters by file name or directory
├───────────────────────────────────────────────────────────────┤
│ ☑ **Header files (12)**                                       │
│    ☑ astyleformatter.h     src/plugins/astyle                 │
│    ☑ astyleoptionhelp.h    src/plugins/astyle    has unsaved changes
│ ☑ **Source files (9)**                                        │
│    ☑ astyleplugin.cpp      src/plugins/astyle                 │
│ ☐ **Other files (2)**                                         │
│    ☐ astyle.qrc            src/plugins/astyle                 │
├───────────────────────────────────────────────────────────────┤
│ [██████░░░░░░]                    21 of 23 files selected     │
│                              [Format] [Stop] [Close]           │
└───────────────────────────────────────────────────────────────┘
```

- Files are grouped into **header / source / other**, and a group checkbox is tri-state
  (`Qt::PartiallyChecked`); clicking the group selects or deselects all of its files.
- The third column shows the result per file as it happens: `changed` / `unchanged` /
  `skipped: <reason>` / `failed: <reason>` / `not processed`.
- A running job can be stopped (`QFuture::cancel()`; results already produced are kept), and
  `Close` is disabled while it runs.
- Files with unsaved changes are marked in the list first. Pressing *Format* then asks whether to
  **save and format**, **skip those files** or **cancel**; skipped files are deselected
  automatically, so unsaved editor content is never overwritten.
- The directory column shows paths **relative to the project root** and falls back to absolute
  paths for files outside the project.

The design follows `CDlgFormatProject` from `gitee.com/zhouliqiu/qtc_astyle` (groups,
checkboxes, status column, progress), but the file list comes from the IDE
(`Project::files(Project::SourceFiles)`), so **no qmake private API is involved** and CMake,
qmake and Meson projects all work. Progress and status are read straight from
`QFutureWatcher::resultReadyAt()` instead of a separate worker class.

### Options page

The plugin **only formats C++**, so the options page keeps only the switches that mean something
for C++: every file is handed to the library with `--mode=c` (the C/C++ mode of AStyle), and the
Java / C# / Objective-C / JS modes and their dedicated options (`--mode=java`,
`--pad-method-colon`, `--join-lines` and friends) are gone. The page is split into seven tabs:

| Tab | Contents |
| --- | --- |
| General | Predefined style (`--style=`), indentation kind and width, forced tab width, continuation indent, minimal conditional indent, maximal continuation indent, maximal line length and how it is measured, pointer/reference alignment |
| Indentation | Indentation of classes, access modifiers, switch, case, namespaces, after parentheses, labels, column 1 comments, preprocessor blocks/defines/conditionals; convert tabs to spaces |
| Spacing | Padding of operators, commas, inner/outer/first/empty parentheses, function headers; removing padding before parentheses and semicolons; padding of `#include` and `!` |
| Braces | Brace attachment for namespace/class/inline/extern "C"/do-while, breaking closing braces, adding and removing braces, attaching and breaking return types |
| Line Breaks | Empty lines between blocks, breaking one line headers, breaking `else if` (including "no extra indent"), breaking after logical operators, lines between class members, keeping one line blocks/statements |
| Blank Lines and Misc | Deleting empty lines inside functions, filling empty lines with indentation, maximal consecutive empty lines, closing template brackets, removing the `*` prefix of block comments |
| Files and Configuration | Suffixes that take part in project formatting, `.astylerc` support, an explicit configuration file, free form additional options |

In total it covers **65 switches that are valid for C++** in library mode. Section 7 lists the
switches that are not exposed and why.

### Tooltips

The tooltip of every checkbox (and of every entry of `Align pointers` / `Align references`)
contains:

1. The option name with its short form, for example `--indent-switches / -S`.
2. The **wording of the official Artistic Style manual** describing what the option does.
3. The **before/after code sample** from the manual, side by side, so the effect of ticking the
   box is visible at a glance.

For example the tooltip of `Indent switch blocks`:

```
--indent-switches / -S
Indent 'switch' blocks so that the 'case X:' statements are indented in the
switch block. The entire case block is indented.

Before                         After
switch (foo)                   switch (foo)
{                              {
case 1:                            case 1:
    a += 1;                            a += 1;
    break;                             break;
...
```

The text is not written by hand: `tools/gen_option_help.py` parses the **official AStyle manual**
and generates `astyleoptionhelp.cpp`. The manual is slightly newer than the embedded 3.6.20, so
the script only takes its **explanations and samples** while the option spellings always follow
what `ASOptions::parseOption()` of the embedded library really accepts (for instance the manual's
`--no-indent-if-after-else` is still `--break-elseifs=no-indent` here). To upgrade AStyle or the
manual, run it again:

```bash
curl -sSLo /tmp/astyle.html https://astyle.sourceforge.net/astyle.html
python tools/gen_option_help.py /tmp/astyle.html astyleoptionhelp.cpp
```

### Order in which options are applied (later wins)

1. The switches on the options page.
2. The language mode: **`--mode=c` is always appended** (the plugin only does C++; the suffix
   only decides which files are formatted, not the mode).
3. `.astylerc`: searched from the **directory of the target file upwards** through all parents
   (including the drive root and the home directory).
4. The explicit configuration file (when "Use specific configuration file" is ticked).
5. Switches typed into "Additional options".

A `.astylerc` in a project therefore overrides the global preferences, and
`Additional options` always has the last word.

---

## 4. Implementation notes

**Why not `ASConsole` or an external process?**
Built with `ASTYLE_LIB`, AStyle keeps only the library API: `ASFormatter`, `ASBeautifier`,
`ASEnhancer`, `ASOptions`. The console front end (`main()`, `ASConsole`, file management options
such as `--suffix` and `--recursive`) is compiled out. The plugin calls the official C entry
point `AStyleMain()` and lets `ASOptions` parse the option string, which gives **exactly the
command line option syntax**, including the comment and whitespace rules of `.astylerc`.

**Encoding and line endings**
- Every file on disk is passed to the library as **raw bytes** and taken back unchanged, so
  double byte encodings such as GBK are safe (AStyle only inserts and removes ASCII whitespace
  and line breaks; even a GBK character whose trailing byte happens to be `}` (0x7D) or `\`
  (0x5C) is not mistaken for syntax, and `tests/standalone` has a case for that).
- UTF-8 BOM: stripped before formatting and restored afterwards, so the BOM does not take part
  in the first line indentation.
- UTF-16 LE/BE with BOM: converted to UTF-8 for formatting and written back in the original byte
  order (`AStyleMain` only takes narrow strings).
- The line ending style follows the input file by default (AStyle's `LINEEND_DEFAULT`, which
  copies the ending of each input line), so a CRLF project is not turned into LF.

**Single file formatting**
`TextEditor::updateEditorText()` performs a differential replacement instead of resetting the
whole document, so the cursor, the folded blocks and the scroll position survive and the change
lands on the undo stack.

**Project wide formatting**
- The file list dialog comes first (see above) and only the checked files are touched.
- The options of every file are assembled and **validated up front** on the UI thread (one
  validation per distinct configuration). A problem aborts the run immediately, so half a project
  is never formatted.
- `QtConcurrent::mapped` formats in parallel (the AStyle library has no shared mutable state and
  is thread safe). Every result returns to the UI thread through
  `QFutureWatcher::resultReadyAt(int)`, which updates the status column and the progress bar.
- Writing back:
  - File open in an editor: the result is decoded with the **encoding of the document**
    (`TextDocument::format().decode()`), then `updateEditorText` + `save()`, so the editor does
    not complain about an external modification.
  - The document encoding cannot decode the bytes, or the format carries a UTF-16/32 BOM
    (`QStringEncoder` does not write the BOM back): `FileSaver` writes the bytes untouched and
    `TextDocument::reload()` picks them up, **without transcoding at all**.
  - File not open: `FileSaver` writes atomically, pure bytes, safe by construction.
  - File with **unsaved changes**: skipped and counted, never overwritten.
- When the run ends, General Messages reports `N formatted, M unchanged, K skipped, J failed`
  together with the failure reasons.

  Note: this path used to decode the formatter output with `QString::fromUtf8()`. Every Chinese
  character of a GBK source became U+FFFD and was saved back like that, so a single
  "format current project" could destroy the comments of an entire project.

---

## 5. Differences to the old qtc_astyle plugin

| Aspect | qtc_astyle (gitee) | This plugin |
| --- | --- | --- |
| Qt Creator | 8.x, the `QTextCodec` era | **20.0.2**, Qt 6.11, no deprecated API |
| AStyle | 2.04 (vendored) | **3.6.20** (current, MIT) |
| Options page | several `.ui` forms plus `QSettings` | `AspectContainer` + `Layouting`, native look, immediate apply/cancel with dirty tracking |
| Passing options | the plugin concatenates a command line string itself | parsed by the official `ASOptions`, validated before the run, errors reported right away |
| Project wide | `QThread` plus the qmake private API to read the `.pro` file list, own `CDlgFormatProject` | `QtConcurrent` in parallel; the file list comes from `Project::files()`; a dialog in the same spirit (groups, filter, status column, stop) |
| Encoding | `QStringConverter::System` (local encoding only) | byte level pass-through plus dedicated BOM handling (GBK, UTF-8 and UTF-16 all work) |
| Single file | resets the whole document | differential replacement, keeps cursor and folding, undoable |
| Build | standalone project with `find_package(QtCreator)` | `add_qtc_plugin` inside the source tree, same Qt and compiler as the IDE |

---

## 6. Simplified Chinese translation

Qt Creator has no notion of one `.qm` per plugin: the texts of all plugins and libraries end up
in `share/qtcreator/translations/qtcreator_<lang>.ts`, which `lrelease` compiles into
`qtcreator_<lang>.qm` during the build. `main.cpp` loads it from
`resourcePath()/translations` at startup according to the interface language. This plugin hooks
into that mechanism, so it neither installs a `QTranslator` itself nor changes the plugin
metadata.

| File | Purpose |
| --- | --- |
| `translations/astyle_zh_CN.ts` | the 282 texts of the plugin (context `QtC::Astyle`), reviewable on its own |
| `tools/fill_translations.py` | translation table (source text to Chinese) plus the script that writes them back |
| `tools/merge_into_qtcreator_ts.py` | merges the context above into `qtcreator_zh_CN.ts` |

Regenerating after changing a `tr()` in the sources:

```bash
cd src/plugins/astyle

# 1. Extract from the sources and update the plugin .ts. The source files must come
#    before -ts, otherwise lupdate treats them as output files and reports
#    "has no recognized extension"
lupdate -locations relative -no-ui-lines *.cpp *.h -ts translations/astyle_zh_CN.ts

# 2. Fill in the translations; the script reports missing entries
#    (messages=282 missing=0 means complete)
python tools/fill_translations.py translations/astyle_zh_CN.ts translations/astyle_zh_CN.ts

# 3. Merge into the main .ts (replaces by context name and inserts in lupdate order,
#    leaves every other byte alone, safe to repeat)
python tools/merge_into_qtcreator_ts.py translations/astyle_zh_CN.ts \
       ../../share/qtcreator/translations/qtcreator_zh_CN.ts

# 4. Rebuild the .qm into the build directory (loaded at IDE startup)
lrelease ../../share/qtcreator/translations/qtcreator_zh_CN.ts \
         -qm ../../build/qt611_vs2022_release/share/qtcreator/translations/qtcreator_zh_CN.qm
```

What is translated:

- Menus and actions (`Artistic Style`, `Format Current File`, `Format Current Project...`), all
  labels and groups of the options page, every message, the progress and result counters.
- The **manual description** of the 65 option tooltips and their comparison captions. The
  descriptions are marked with `QT_TRANSLATE_NOOP("QtC::Astyle", ...)` in `astyleoptionhelp.cpp`
  and looked up at run time through `QCoreApplication::translate()`.
- **Not translated**: option names (`--indent-classes / -C`), the before/after code samples, and
  the `Before`/`After` column headers. The first two have to match the AStyle output, the last
  two take part in the column alignment of the monospaced font.

Switching the language: *Edit > Preferences > Environment > Interface language*, or the
`-ui-language zh_CN` command line argument.

> If only `Astyle.dll` is copied into another Qt Creator installation, that installation's
> `share/qtcreator/translations/qtcreator_zh_CN.qm` does not contain these 282 messages and the
> interface falls back to English. The `.qm` over there has to be updated as well (or regenerated
> with the steps above).

---

## 7. Known limitations

- Only whole files can be formatted (no selection or range formatting).
- There is no format on save (it could be added through `EditorManager::aboutToSave`, similar to
  the Beautifier plugin).
- No qbs build script is provided (the Qt Creator 20 mainline uses CMake).
- Only **C++** is formatted (which includes C sources, `--mode=c` of AStyle covers both). Java,
  C#, Objective-C, JS and GHC modes and their dedicated options (`--mode=java`, `--mode=cs`,
  `--mode=objc`, `--pad-method-colon`, `--align-method-colon`, `--pad-param-type` and others)
  have been removed from the interface, and the plugin never switches the language by suffix.
- In AStyle 3.6 `--pad-brackets`, `--indent-lambda`, `--squeeze-ws`, `--preserve-ws`,
  `--lineend` and `--suffix` are **console only** options (they sit behind the
  `#ifdef ASTYLE_LIB` cut in `astyle_main.cpp`), so library mode always rejects them and they are
  not exposed.
- `--indent=none` and `--remove-braces=one-line` are **accepted** by the library, but the official
  manual has no section for them, so a tooltip with the same "description plus before/after"
  shape cannot be produced. They are not exposed either; write them into "Additional options" if
  you need them.
- File management options in `.astylerc` such as `--options=`, `--suffix=` and `--recursive` have
  no effect in library mode.

---

## 8. License

- The plugin itself follows Qt Creator: **GPL-3.0-only WITH Qt-GPL-exception-1.0**, full text in
  `LICENSES/LICENSE.GPL3-EXCEPT`.
- `3rdparty/astyle/` contains the upstream Artistic Style sources under the **MIT** license, see
  `3rdparty/astyle/LICENSE.md` (copy in `LICENSES/MIT.txt`).
- This plugin is not affiliated with, or endorsed by, The Qt Company.
