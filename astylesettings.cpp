// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "astylesettings.h"

#include "astyleconstants.h"
#include "astyleoptionhelp.h"
#include "astyletr.h"

#include <coreplugin/dialogs/ioptionspage.h>

#include <utils/fileutils.h>
#include <utils/layoutbuilder.h>

#include <QRegularExpression>

#include <utility>

using namespace Utils;

namespace Astyle::Internal {

AstyleSettings &settings()
{
    static AstyleSettings theSettings;
    return theSettings;
}

static QString modeOptionForSuffix(const QString &suffix)
{
    const QString suffixLower = suffix.toLower();
    if (suffixLower == "java")
        return "--mode=java";
    if (suffixLower == "cs")
        return "--mode=cs";
    if (suffixLower == "m" || suffixLower == "mm")
        return "--mode=objc";
    return "--mode=c";
}

AstyleSettings::AstyleSettings()
{
    setAutoApply(false);
    setSettingsGroups(Constants::SETTINGS_GROUP, Constants::SETTINGS_SUBGROUP);

    // --- Style -------------------------------------------------------------
    style.setSettingsKey("style");
    style.setLabelText(Tr::tr("Style:"));
    style.setToolTip(Tr::tr("Predefined formatting style, the equivalent of --style=<name>."));
    style.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    style.setUseDataAsSavedValue();
    const auto addStyle = [this](const QString &name, const QString &option, const QString &tip) {
        style.addOption(SelectionAspect::Option(name, tip, option));
    };
    addStyle(Tr::tr("Allman"), "--style=allman",
             Tr::tr("Braces on their own line (BSD style)."));
    addStyle(Tr::tr("Java"), "--style=java",
             Tr::tr("Braces attached to the header of the block."));
    addStyle(Tr::tr("K&R"), "--style=kr",
             Tr::tr("Braces attached to the header, but function braces on their own line."));
    addStyle(Tr::tr("Stroustrup"), "--style=stroustrup",
             Tr::tr("Like K&R, but closing braces get their own line."));
    addStyle(Tr::tr("Whitesmith"), "--style=whitesmith",
             Tr::tr("Indented, with braces on their own line."));
    addStyle(Tr::tr("VTK"), "--style=vtk",
             Tr::tr("Whitesmith with indented braces and switches."));
    addStyle(Tr::tr("Ratliff"), "--style=ratliff",
             Tr::tr("Braceless style where the braces are indented."));
    addStyle(Tr::tr("GNU"), "--style=gnu",
             Tr::tr("Indented braces, extra indentation for blocks."));
    addStyle(Tr::tr("Linux"), "--style=linux",
             Tr::tr("Like K&R with eight spaces indentation."));
    addStyle(Tr::tr("Horstmann"), "--style=horstmann",
             Tr::tr("Run-in braces, indented bodies."));
    addStyle(Tr::tr("1TBS"), "--style=1tbs",
             Tr::tr("The One True Brace Style."));
    addStyle(Tr::tr("Google"), "--style=google",
             Tr::tr("Google C++ style."));
    addStyle(Tr::tr("Mozilla"), "--style=mozilla",
             Tr::tr("Mozilla style."));
    addStyle(Tr::tr("WebKit"), "--style=webkit",
             Tr::tr("WebKit style."));
    addStyle(Tr::tr("Pico"), "--style=pico",
             Tr::tr("Run-in braces, opening brace follows the first statement."));
    addStyle(Tr::tr("Lisp"), "--style=lisp",
             Tr::tr("Run-in braces with closing braces at the end of the statement."));
    addStyle(Tr::tr("No style"), "",
             Tr::tr("Do not change the brace style."));
    style.setDefaultValue(int(style.indexForItemValue("--style=allman")));

    indentMode.setSettingsKey("indentMode");
    indentMode.setLabelText(Tr::tr("Indent with:"));
    indentMode.setToolTip(Tr::tr("The equivalent of --indent=spaces, --indent=tab or "
                                 "--indent=force-tab."));
    indentMode.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    indentMode.setUseDataAsSavedValue();
    indentMode.addOption(SelectionAspect::Option(Tr::tr("Spaces"), Tr::tr("Indent with spaces."),
                                                 "spaces"));
    indentMode.addOption(SelectionAspect::Option(Tr::tr("Tabs"), Tr::tr("Indent with tabs, but use "
                                                                      "spaces for continuation lines."),
                                                 "tab"));
    indentMode.addOption(SelectionAspect::Option(Tr::tr("Force tabs"),
                                                 Tr::tr("Indent with tabs everywhere possible."),
                                                 "force-tab"));
    indentMode.setDefaultValue(int(indentMode.indexForItemValue("spaces")));

    indentWidth.setSettingsKey("indentWidth");
    indentWidth.setLabelText(Tr::tr("Indent width:"));
    indentWidth.setToolTip(Tr::tr("Number of spaces per indentation step."));
    indentWidth.setRange(1, 20);
    indentWidth.setDefaultValue(4);

    indentContinuation.setSettingsKey("indentContinuation");
    indentContinuation.setLabelText(Tr::tr("Continuation indent:"));
    indentContinuation.setToolTip(Tr::tr("Indent for continuation lines inside parentheses."));
    indentContinuation.setRange(0, 4);
    indentContinuation.setSpecialValueText(Tr::tr("unchanged"));
    indentContinuation.setDefaultValue(0);

    maxCodeLength.setSettingsKey("maxCodeLength");
    maxCodeLength.setLabelText(Tr::tr("Maximum code length:"));
    maxCodeLength.setToolTip(Tr::tr("Break lines that are longer than this. 0 keeps the lines."));
    maxCodeLength.setRange(0, 250);
    maxCodeLength.setSpecialValueText(Tr::tr("unlimited"));
    maxCodeLength.setDefaultValue(0);

    // --- Toggles -----------------------------------------------------------
    // The tool tip is generated from the Artistic Style manual: the option name, what
    // it does and - where the manual has one - a before/after sample of the effect.
    const auto addToggle = [this](BoolAspect &aspect, const QString &key,
                                  const QString &label, const QString &option) {
        aspect.setSettingsKey(Key(key.toUtf8()));
        aspect.setLabelText(label);
        aspect.setToolTip(optionHelp(option));
        aspect.setDefaultValue(false);
        m_conditionalSwitches.push_back({&aspect, option});
    };

    addToggle(indentClasses, "indentClasses", Tr::tr("Indent class and struct blocks"),
              "--indent-classes");
    addToggle(indentModifiers, "indentModifiers", Tr::tr("Indent access modifiers"),
              "--indent-modifiers");
    addToggle(indentSwitches, "indentSwitches", Tr::tr("Indent switch blocks"),
              "--indent-switches");
    addToggle(indentCases, "indentCases", Tr::tr("Indent case blocks"),
              "--indent-cases");
    addToggle(indentNamespaces, "indentNamespaces", Tr::tr("Indent namespace blocks"),
              "--indent-namespaces");
    addToggle(indentLabels, "indentLabels", Tr::tr("Indent labels"),
              "--indent-labels");
    addToggle(indentPreprocBlock, "indentPreprocBlock", Tr::tr("Indent preprocessor blocks"),
              "--indent-preproc-block");
    addToggle(indentPreprocDefine, "indentPreprocDefine", Tr::tr("Indent define directives"),
              "--indent-preproc-define");
    addToggle(indentPreprocCond, "indentPreprocCond", Tr::tr("Indent conditional directives"),
              "--indent-preproc-cond");
    addToggle(indentCol1Comments, "indentCol1Comments", Tr::tr("Indent comments at column 1"),
              "--indent-col1-comments");

    addToggle(padOper, "padOper", Tr::tr("Pad operators"), "--pad-oper");
    addToggle(padComma, "padComma", Tr::tr("Pad commas"), "--pad-comma");
    addToggle(padParenOut, "padParenOut", Tr::tr("Pad parentheses outside"),
              "--pad-paren-out");
    addToggle(padParenIn, "padParenIn", Tr::tr("Pad parentheses inside"),
              "--pad-paren-in");
    addToggle(padFirstParenOut, "padFirstParenOut", Tr::tr("Pad first parenthesis outside"),
              "--pad-first-paren-out");
    addToggle(padHeader, "padHeader", Tr::tr("Pad headers"), "--pad-header");
    addToggle(unpadParen, "unpadParen", Tr::tr("Remove extra parentheses padding"),
              "--unpad-paren");
    addToggle(padInclude, "padInclude", Tr::tr("Pad include directives"), "--pad-include");

    addToggle(breakBlocks, "breakBlocks", Tr::tr("Break blocks"),
              "--break-blocks");
    addToggle(breakBlocksAll, "breakBlocksAll", Tr::tr("Break blocks everywhere"),
              "--break-blocks=all");
    addToggle(breakOneLineHeaders, "breakOneLineHeaders", Tr::tr("Break one line headers"),
              "--break-one-line-headers");
    addToggle(breakElseIfs, "breakElseIfs", Tr::tr("Break else if"),
              "--break-elseifs");
    addToggle(keepOneLineBlocks, "keepOneLineBlocks", Tr::tr("Keep one line blocks"),
              "--keep-one-line-blocks");
    addToggle(keepOneLineStatements, "keepOneLineStatements",
              Tr::tr("Keep one line statements"), "--keep-one-line-statements");

    addToggle(attachNamespaces, "attachNamespaces", Tr::tr("Attach namespace braces"),
              "--attach-namespaces");
    addToggle(attachClasses, "attachClasses", Tr::tr("Attach class braces"),
              "--attach-classes");
    addToggle(attachInlines, "attachInlines", Tr::tr("Attach inline braces"),
              "--attach-inlines");
    addToggle(attachExternC, "attachExternC", Tr::tr("Attach extern \"C\" braces"),
              "--attach-extern-c");
    addToggle(convertTabs, "convertTabs", Tr::tr("Convert tabs to spaces"),
              "--convert-tabs");
    addToggle(deleteEmptyLines, "deleteEmptyLines", Tr::tr("Delete empty lines"),
              "--delete-empty-lines");

    alignPointer.setSettingsKey("alignPointer");
    alignPointer.setLabelText(Tr::tr("Align pointers:"));
    alignPointer.setToolTip(Tr::tr("Attach '*' and '&' to the type, to the name, or put them "
                                   "between both. Hover an entry for a sample."));
    alignPointer.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    alignPointer.setUseDataAsSavedValue();
    alignPointer.addOption(SelectionAspect::Option(Tr::tr("Unchanged"),
                                                   Tr::tr("Leave the pointer and reference "
                                                          "alignment untouched."), QString()));
    alignPointer.addOption(SelectionAspect::Option(Tr::tr("Type"),
                                                   optionHelp("--align-pointer=type"),
                                                   "--align-pointer=type"));
    alignPointer.addOption(SelectionAspect::Option(Tr::tr("Middle"),
                                                   optionHelp("--align-pointer=middle"),
                                                   "--align-pointer=middle"));
    alignPointer.addOption(SelectionAspect::Option(Tr::tr("Name"),
                                                   optionHelp("--align-pointer=name"),
                                                   "--align-pointer=name"));
    alignPointer.setDefaultValue(0);

    alignReference.setSettingsKey("alignReference");
    alignReference.setLabelText(Tr::tr("Align references:"));
    alignReference.setToolTip(Tr::tr("Align '&' separately from pointers. The entries behave "
                                     "like the ones above."));
    alignReference.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    alignReference.setUseDataAsSavedValue();
    alignReference.addOption(SelectionAspect::Option(Tr::tr("Follow pointer"), {},
                                                     QString()));
    alignReference.addOption(SelectionAspect::Option(Tr::tr("None"),
                                                     optionHelp("--align-reference=none"),
                                                     "--align-reference=none"));
    alignReference.addOption(SelectionAspect::Option(Tr::tr("Type"),
                                                     optionHelp("--align-reference=type"),
                                                     "--align-reference=type"));
    alignReference.addOption(SelectionAspect::Option(Tr::tr("Middle"),
                                                     optionHelp("--align-reference=middle"),
                                                     "--align-reference=middle"));
    alignReference.addOption(SelectionAspect::Option(Tr::tr("Name"),
                                                     optionHelp("--align-reference=name"),
                                                     "--align-reference=name"));
    alignReference.setDefaultValue(0);

    // --- Files and configuration ------------------------------------------
    fileExtensions.setSettingsKey("fileExtensions");
    fileExtensions.setLabelText(Tr::tr("File extensions:"));
    fileExtensions.setToolTip(Tr::tr("Space separated list of file extensions that are formatted "
                                     "when formatting a project."));
    fileExtensions.setDisplayStyle(StringAspect::LineEditDisplay);
    fileExtensions.setDefaultValue("h hpp hxx h++ c cpp cxx cc c++ m mm ino cs java");

    useAstyleRc.setSettingsKey("useAstyleRc");
    useAstyleRc.setLabelText(Tr::tr("Use .astylerc files found next to the file or in the home "
                                    "directory"));
    useAstyleRc.setToolTip(Tr::tr("Options from such a file are applied after the options above, "
                                  "so they can override them."));
    useAstyleRc.setDefaultValue(true);

    useSpecificConfigFile.setSettingsKey("useSpecificConfigFile");
    useSpecificConfigFile.setLabelText(Tr::tr("Use specific configuration file:"));

    specificConfigFile.setSettingsKey("specificConfigFile");
    specificConfigFile.setExpectedKind(PathChooser::File);
    specificConfigFile.setPromptDialogFilter(Tr::tr("Artistic Style options (*.astylerc *.rc);;"
                                                    "All files (*)"));
    specificConfigFile.setEnabler(&useSpecificConfigFile);

    extraOptions.setSettingsKey("extraOptions");
    extraOptions.setLabelText(Tr::tr("Additional options:"));
    extraOptions.setToolTip(Tr::tr("Free form AStyle options, one or more per line. They are "
                                   "appended last and therefore win over everything above."));
    extraOptions.setDisplayStyle(StringAspect::TextEditDisplay);
    extraOptions.setPlaceHolderText("--indent-preproc-block\n--max-code-length=100");

    setLayouter([this] {
        using namespace Layouting;
        return Column {
            Group {
                title(Tr::tr("Formatting Style")),
                Form {
                    style, br,
                    indentMode, br,
                    indentWidth, br,
                    indentContinuation, br,
                    maxCodeLength,
                }
            },
            Group {
                title(Tr::tr("Indentation")),
                Grid {
                    indentClasses, indentModifiers, br,
                    indentSwitches, indentCases, br,
                    indentNamespaces, indentLabels, br,
                    indentPreprocBlock, indentPreprocDefine, br,
                    indentPreprocCond, indentCol1Comments,
                }
            },
            Group {
                title(Tr::tr("Spacing and Alignment")),
                Grid {
                    padOper, padComma, br,
                    padParenOut, padParenIn, br,
                    padFirstParenOut, padHeader, br,
                    unpadParen, padInclude,
                },
                Form {
                    alignPointer, br,
                    alignReference,
                }
            },
            Group {
                title(Tr::tr("Line Breaking")),
                Grid {
                    breakBlocks, breakBlocksAll, br,
                    breakOneLineHeaders, breakElseIfs, br,
                    keepOneLineBlocks, keepOneLineStatements,
                }
            },
            Group {
                title(Tr::tr("Other")),
                Grid {
                    attachNamespaces, attachClasses, br,
                    attachInlines, attachExternC, br,
                    convertTabs, deleteEmptyLines,
                }
            },
            Group {
                title(Tr::tr("Files and Configuration")),
                Form {
                    fileExtensions, br,
                    useAstyleRc, br,
                    Row { useSpecificConfigFile, specificConfigFile }, br,
                    extraOptions,
                }
            },
            st
        };
    });

    readSettings();
}

QStringList AstyleSettings::supportedSuffixes() const
{
    QStringList suffixes;
    const QStringList raw = fileExtensions().split(QRegularExpression("[\\s,;]+"),
                                                   Qt::SkipEmptyParts);
    for (QString suffix : raw) {
        suffix = suffix.trimmed().toLower();
        if (suffix.startsWith("*."))
            suffix.remove(0, 2);
        else if (suffix.startsWith('*'))
            suffix.remove(0, 1);
        if (suffix.startsWith('.'))
            suffix.remove(0, 1);
        if (!suffix.isEmpty() && !suffixes.contains(suffix))
            suffixes.append(suffix);
    }
    return suffixes;
}

bool AstyleSettings::isSupportedFile(const FilePath &filePath) const
{
    return supportedSuffixes().contains(filePath.suffix().toLower());
}

FilePaths AstyleSettings::filteredFiles(const FilePaths &files) const
{
    const QStringList suffixes = supportedSuffixes();
    FilePaths result;
    for (const FilePath &file : files) {
        if (suffixes.contains(file.suffix().toLower()))
            result.append(file);
    }
    return result;
}

QStringList AstyleSettings::uiOptions() const
{
    QStringList options;

    const QString styleOption = style.itemValue().toString();
    if (!styleOption.isEmpty())
        options.append(styleOption);

    QString indentKind = indentMode.itemValue().toString();
    if (indentKind.isEmpty())
        indentKind = "spaces";
    options.append(QStringLiteral("--indent=%1=%2").arg(indentKind).arg(indentWidth()));

    if (indentContinuation() > 0)
        options.append(QStringLiteral("--indent-continuation=%1").arg(indentContinuation()));

    if (maxCodeLength() > 0)
        options.append(QStringLiteral("--max-code-length=%1").arg(maxCodeLength()));

    for (const ConditionalSwitch &conditionalSwitch : std::as_const(m_conditionalSwitches)) {
        if ((*conditionalSwitch.aspect)())
            options.append(conditionalSwitch.option);
    }

    const QString pointerOption = alignPointer.itemValue().toString();
    if (!pointerOption.isEmpty())
        options.append(pointerOption);

    const QString referenceOption = alignReference.itemValue().toString();
    if (!referenceOption.isEmpty())
        options.append(referenceOption);

    return options;
}

FilePath AstyleSettings::configFileFor(const FilePath &filePath) const
{
    if (useSpecificConfigFile() && specificConfigFile().isReadableFile())
        return specificConfigFile();

    if (!useAstyleRc())
        return {};

    // Look for a configuration file in the directory of the file and all its
    // parents, the way .clang-format or .editorconfig are looked up.
    FilePath directory = filePath.parentDir();
    while (!directory.isEmpty()) {
        const FilePath candidates[] = {directory / ".astylerc", directory / "astylerc"};
        for (const FilePath &candidate : candidates) {
            if (candidate.isReadableFile())
                return candidate;
        }
        const FilePath parent = directory.parentDir();
        if (parent == directory)
            break;
        directory = parent;
    }

    return {};
}

static QString contentsOf(const FilePath &filePath)
{
    const Result<QByteArray> contents = filePath.fileContents();
    if (!contents)
        return {};
    return QString::fromUtf8(*contents);
}

QString AstyleSettings::optionsForFile(const FilePath &filePath) const
{
    QStringList options = uiOptions();
    options.append(modeOptionForSuffix(filePath.suffix()));

    if (const FilePath configFile = configFileFor(filePath); !configFile.isEmpty()) {
        const QString configuration = contentsOf(configFile);
        if (!configuration.isEmpty())
            options.append(configuration);
    }

    options.append(extraOptions());
    return options.join('\n');
}

QString AstyleSettings::optionsForSuffix(const QString &suffix) const
{
    QStringList options = uiOptions();
    options.append(modeOptionForSuffix(suffix));
    options.append(extraOptions());
    return options.join('\n');
}

class AstyleSettingsPage final : public Core::IOptionsPage
{
public:
    AstyleSettingsPage()
    {
        setId(Constants::OPTION_PAGE_ID);
        setDisplayName(Tr::tr("Artistic Style"));
        setCategory(Constants::OPTION_CATEGORY);
        setSettingsProvider([] { return &settings(); });
    }
};

const AstyleSettingsPage theSettingsPage;

} // namespace Astyle::Internal
