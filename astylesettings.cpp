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

// The plugin formats C++ only. AStyle calls that mode "c" - it covers C, C++
// and C++/CLI - and it is what every file is passed with, no matter what its
// suffix is. The suffix only decides whether a file is formatted at all.
static const char MODE_OPTION[] = "--mode=c";

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
    indentWidth.setToolTip(Tr::tr("Number of spaces per indentation step. Artistic Style accepts "
                                  "values from 2 to 20."));
    indentWidth.setRange(2, 20);
    indentWidth.setDefaultValue(4);

    indentForceTabX.setSettingsKey("indentForceTabX");
    indentForceTabX.setLabelText(Tr::tr("Tab width for forced tabs:"));
    indentForceTabX.setToolTip(optionHelp("--indent=force-tab-x="));
    indentForceTabX.setRange(0, 20);
    indentForceTabX.setSpecialValueText(Tr::tr("same as the indent width"));
    indentForceTabX.setDefaultValue(0);

    indentContinuation.setSettingsKey("indentContinuation");
    indentContinuation.setLabelText(Tr::tr("Continuation indent:"));
    indentContinuation.setToolTip(optionHelp("--indent-continuation="));
    indentContinuation.setRange(0, 4);
    // AStyle's own default is 1; 0 here means "do not pass the option at all".
    indentContinuation.setSpecialValueText(Tr::tr("default (1)"));
    indentContinuation.setDefaultValue(0);

    minConditionalIndent.setSettingsKey("minConditionalIndent");
    minConditionalIndent.setLabelText(Tr::tr("Minimum conditional indent:"));
    minConditionalIndent.setToolTip(optionHelp("--min-conditional-indent="));
    minConditionalIndent.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    minConditionalIndent.setUseDataAsSavedValue();
    minConditionalIndent.addOption(SelectionAspect::Option(Tr::tr("0 - align with the parenthesis"),
                                                          {}, "0"));
    minConditionalIndent.addOption(SelectionAspect::Option(Tr::tr("1 - one indent"), {}, "1"));
    minConditionalIndent.addOption(SelectionAspect::Option(Tr::tr("2 - two indents (default)"),
                                                          {}, "2"));
    minConditionalIndent.addOption(SelectionAspect::Option(Tr::tr("3 - half an indent"), {}, "3"));
    minConditionalIndent.setDefaultValue(int(minConditionalIndent.indexForItemValue("2")));

    maxContinuationIndent.setSettingsKey("maxContinuationIndent");
    maxContinuationIndent.setLabelText(Tr::tr("Maximum continuation indent:"));
    maxContinuationIndent.setToolTip(optionHelp("--max-continuation-indent="));
    maxContinuationIndent.setRange(0, 120);
    maxContinuationIndent.setSpecialValueText(Tr::tr("default (40)"));
    maxContinuationIndent.setDefaultValue(0);

    maxCodeLength.setSettingsKey("maxCodeLength");
    maxCodeLength.setLabelText(Tr::tr("Maximum code length:"));
    maxCodeLength.setToolTip(Tr::tr("Break lines that are longer than this. The library accepts "
                                    "50 to 200 characters, 0 keeps the lines as they are."));
    maxCodeLength.setRange(0, 200);
    maxCodeLength.setSpecialValueText(Tr::tr("unlimited"));
    maxCodeLength.setDefaultValue(0);

    maxCodeLengthMode.setSettingsKey("maxCodeLengthMode");
    maxCodeLengthMode.setLabelText(Tr::tr("Measure the code length by:"));
    maxCodeLengthMode.setToolTip(optionHelp("--max-code-length-mode="));
    maxCodeLengthMode.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    maxCodeLengthMode.setUseDataAsSavedValue();
    maxCodeLengthMode.addOption(SelectionAspect::Option(Tr::tr("Code only"),
                                                       Tr::tr("Ignore the indentation."), "code"));
    maxCodeLengthMode.addOption(SelectionAspect::Option(Tr::tr("Whole line"),
                                                       Tr::tr("Include the indentation."),
                                                       "total"));
    maxCodeLengthMode.addOption(SelectionAspect::Option(Tr::tr("Ignore side comments"),
                                                       Tr::tr("Do not count trailing comments."),
                                                       "ignore-side-comments"));
    maxCodeLengthMode.setDefaultValue(int(maxCodeLengthMode.indexForItemValue("code")));

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
    addToggle(indentAfterParens, "indentAfterParens",
              Tr::tr("Indent after parentheses instead of aligning"),
              "--indent-after-parens");
    addToggle(indentLabels, "indentLabels", Tr::tr("Indent labels"),
              "--indent-labels");
    addToggle(indentCol1Comments, "indentCol1Comments", Tr::tr("Indent comments at column one"),
              "--indent-col1-comments");
    addToggle(indentPreprocBlock, "indentPreprocBlock", Tr::tr("Indent preprocessor blocks"),
              "--indent-preproc-block");
    addToggle(indentPreprocDefine, "indentPreprocDefine", Tr::tr("Indent define directives"),
              "--indent-preproc-define");
    addToggle(indentPreprocCond, "indentPreprocCond", Tr::tr("Indent conditional directives"),
              "--indent-preproc-cond");
    addToggle(convertTabs, "convertTabs", Tr::tr("Convert tabs to spaces"),
              "--convert-tabs");

    addToggle(padOper, "padOper", Tr::tr("Pad operators"), "--pad-oper");
    addToggle(padComma, "padComma", Tr::tr("Pad commas"), "--pad-comma");
    addToggle(padParenOut, "padParenOut", Tr::tr("Pad parentheses outside"),
              "--pad-paren-out");
    addToggle(padParenIn, "padParenIn", Tr::tr("Pad parentheses inside"),
              "--pad-paren-in");
    addToggle(padFirstParenOut, "padFirstParenOut", Tr::tr("Pad first parenthesis outside"),
              "--pad-first-paren-out");
    addToggle(padEmptyParen, "padEmptyParen", Tr::tr("Pad empty parentheses"),
              "--pad-empty-paren");
    addToggle(padHeader, "padHeader", Tr::tr("Pad headers"), "--pad-header");
    addToggle(unpadParen, "unpadParen", Tr::tr("Remove extra parentheses padding"),
              "--unpad-paren");
    addToggle(padSemicolon, "padSemicolon", Tr::tr("Remove padding before semicolons"),
              "--pad-semicolon=none");

    padInclude.setSettingsKey("padInclude");
    padInclude.setLabelText(Tr::tr("Include directives:"));
    padInclude.setToolTip(Tr::tr("Add or remove the space between '#include' and the header "
                                 "name. Hover an entry for a sample."));
    padInclude.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    padInclude.setUseDataAsSavedValue();
    padInclude.addOption(SelectionAspect::Option(Tr::tr("Unchanged"), {}, QString()));
    padInclude.addOption(SelectionAspect::Option(Tr::tr("Pad after #include"),
                                                 optionHelp("--pad-include"), "--pad-include"));
    padInclude.addOption(SelectionAspect::Option(Tr::tr("Remove padding"),
                                                 optionHelp("--pad-include=none"),
                                                 "--pad-include=none"));
    padInclude.setDefaultValue(0);

    padNegation.setSettingsKey("padNegation");
    padNegation.setLabelText(Tr::tr("Negations:"));
    padNegation.setToolTip(Tr::tr("Pad the '!' operator, one sided or on both sides. Hover an "
                                  "entry for a sample."));
    padNegation.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    padNegation.setUseDataAsSavedValue();
    padNegation.addOption(SelectionAspect::Option(Tr::tr("Unchanged"), {}, QString()));
    padNegation.addOption(SelectionAspect::Option(Tr::tr("Pad after '!'"),
                                                  optionHelp("--pad-negation"),
                                                  "--pad-negation"));
    padNegation.addOption(SelectionAspect::Option(Tr::tr("Pad before and after '!'"),
                                                  optionHelp("--pad-negation=before"),
                                                  "--pad-negation=before"));
    padNegation.setDefaultValue(0);

    addToggle(attachNamespaces, "attachNamespaces", Tr::tr("Attach namespace braces"),
              "--attach-namespaces");
    addToggle(attachClasses, "attachClasses", Tr::tr("Attach class braces"),
              "--attach-classes");
    addToggle(attachInlines, "attachInlines", Tr::tr("Attach inline braces"),
              "--attach-inlines");
    addToggle(attachExternC, "attachExternC", Tr::tr("Attach extern \"C\" braces"),
              "--attach-extern-c");
    addToggle(attachClosingWhile, "attachClosingWhile",
              Tr::tr("Attach the closing 'while' of a do-while loop"),
              "--attach-closing-while");
    addToggle(breakClosingBraces, "breakClosingBraces", Tr::tr("Break closing header braces"),
              "--break-closing-braces");
    addToggle(addBraces, "addBraces", Tr::tr("Add braces"), "--add-braces");
    addToggle(removeBraces, "removeBraces", Tr::tr("Remove braces"), "--remove-braces");
    addToggle(addOneLineBraces, "addOneLineBraces", Tr::tr("Add one line braces"),
              "--add-one-line-braces");
    addToggle(attachReturnType, "attachReturnType",
              Tr::tr("Attach return type to the function name"), "--attach-return-type");
    addToggle(attachReturnTypeDecl, "attachReturnTypeDecl",
              Tr::tr("Attach return type in declarations"), "--attach-return-type=decl");
    addToggle(breakReturnType, "breakReturnType",
              Tr::tr("Break return type from the function name"), "--break-return-type");
    addToggle(breakReturnTypeDecl, "breakReturnTypeDecl",
              Tr::tr("Break return type in declarations"), "--break-return-type=decl");

    addToggle(breakBlocks, "breakBlocks", Tr::tr("Break blocks"),
              "--break-blocks");
    addToggle(breakBlocksAll, "breakBlocksAll", Tr::tr("Break blocks everywhere"),
              "--break-blocks=all");
    addToggle(breakOneLineHeaders, "breakOneLineHeaders", Tr::tr("Break one line headers"),
              "--break-one-line-headers");
    addToggle(breakAfterLogical, "breakAfterLogical", Tr::tr("Break after logical operators"),
              "--break-after-logical");
    addToggle(keepOneLineBlocks, "keepOneLineBlocks", Tr::tr("Keep one line blocks"),
              "--keep-one-line-blocks");
    addToggle(keepOneLineStatements, "keepOneLineStatements",
              Tr::tr("Keep one line statements"), "--keep-one-line-statements");

    breakElseIfs.setSettingsKey("breakElseIfs");
    breakElseIfs.setLabelText(Tr::tr("Break \"else if\":"));
    breakElseIfs.setToolTip(optionHelp("--break-elseifs"));
    breakElseIfs.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    breakElseIfs.setUseDataAsSavedValue();
    breakElseIfs.addOption(SelectionAspect::Option(Tr::tr("Keep on one line"), {}, QString()));
    breakElseIfs.addOption(SelectionAspect::Option(Tr::tr("Break into separate lines"),
                                                   optionHelp("--break-elseifs"),
                                                   "--break-elseifs"));
    breakElseIfs.addOption(SelectionAspect::Option(Tr::tr("Break, without the extra indent"),
                                                   optionHelp("--break-elseifs=no-indent"),
                                                   "--break-elseifs=no-indent"));
    breakElseIfs.setDefaultValue(0);

    lineBetweenMembers.setSettingsKey("lineBetweenMembers");
    lineBetweenMembers.setLabelText(Tr::tr("Empty lines between class members:"));
    lineBetweenMembers.setToolTip(optionHelp("--line-between-members"));
    lineBetweenMembers.setDisplayStyle(SelectionAspect::DisplayStyle::ComboBox);
    lineBetweenMembers.setUseDataAsSavedValue();
    lineBetweenMembers.addOption(SelectionAspect::Option(Tr::tr("Unchanged"), {}, QString()));
    lineBetweenMembers.addOption(SelectionAspect::Option(Tr::tr("Between functions and members"),
                                                         optionHelp("--line-between-members"),
                                                         "--line-between-members"));
    lineBetweenMembers.addOption(SelectionAspect::Option(Tr::tr("Between all members"),
                                                         optionHelp("--line-between-members=all"),
                                                         "--line-between-members=all"));
    lineBetweenMembers.setDefaultValue(0);

    addToggle(deleteEmptyLines, "deleteEmptyLines",
              Tr::tr("Delete empty lines inside functions"), "--delete-empty-lines");
    addToggle(fillEmptyLines, "fillEmptyLines",
              Tr::tr("Fill empty lines with the indentation"), "--fill-empty-lines");
    addToggle(closeTemplates, "closeTemplates", Tr::tr("Close template angle brackets"),
              "--close-templates");
    addToggle(removeCommentPrefix, "removeCommentPrefix",
              Tr::tr("Remove the '*' prefix of block comments"), "--remove-comment-prefix");

    squeezeLines.setSettingsKey("squeezeLines");
    squeezeLines.setLabelText(Tr::tr("Maximum empty lines in a row:"));
    squeezeLines.setToolTip(optionHelp("--squeeze-lines="));
    squeezeLines.setRange(0, 10);
    squeezeLines.setSpecialValueText(Tr::tr("keep all"));
    squeezeLines.setDefaultValue(0);

    // --- Files and configuration ------------------------------------------
    fileExtensions.setSettingsKey("fileExtensions");
    fileExtensions.setLabelText(Tr::tr("File extensions:"));
    fileExtensions.setToolTip(Tr::tr("Space separated list of file extensions that are formatted "
                                     "when formatting a project. Only C and C++ files make sense "
                                     "here, the plugin always formats in C++ mode."));
    fileExtensions.setDisplayStyle(StringAspect::LineEditDisplay);
    fileExtensions.setDefaultValue("h hpp hxx h++ hh inl ipp tpp c cpp cxx cc c++ ino");

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
            TabWidget {
                Tab {
                    Tr::tr("General"),
                    Column {
                        Group {
                            title(Tr::tr("Braces and Indentation")),
                            Form {
                                style, br,
                                indentMode, br,
                                indentWidth, br,
                                indentForceTabX, br,
                                indentContinuation,
                            }
                        },
                        Group {
                            title(Tr::tr("Line Continuation")),
                            Form {
                                minConditionalIndent, br,
                                maxContinuationIndent,
                            }
                        },
                        Group {
                            title(Tr::tr("Long Lines")),
                            Form {
                                maxCodeLength, br,
                                maxCodeLengthMode,
                            }
                        },
                        Group {
                            title(Tr::tr("Pointers and References")),
                            Form {
                                alignPointer, br,
                                alignReference,
                            }
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Indentation"),
                    Column {
                        Grid {
                            indentClasses, indentModifiers, br,
                            indentSwitches, indentCases, br,
                            indentNamespaces, indentAfterParens, br,
                            indentLabels, indentCol1Comments, br,
                            indentPreprocBlock, indentPreprocDefine, br,
                            indentPreprocCond, convertTabs,
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Spacing"),
                    Column {
                        Grid {
                            padOper, padComma, br,
                            padParenOut, padParenIn, br,
                            padFirstParenOut, padEmptyParen, br,
                            padHeader, unpadParen, br,
                            padSemicolon,
                        },
                        Form {
                            padInclude, br,
                            padNegation,
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Braces"),
                    Column {
                        Group {
                            title(Tr::tr("Attach Braces")),
                            Grid {
                                attachNamespaces, attachClasses, br,
                                attachInlines, attachExternC, br,
                                attachClosingWhile, breakClosingBraces,
                            }
                        },
                        Group {
                            title(Tr::tr("Add or Remove Braces")),
                            Grid {
                                addBraces, addOneLineBraces, br,
                                removeBraces,
                            }
                        },
                        Group {
                            title(Tr::tr("Return Types")),
                            Grid {
                                attachReturnType, attachReturnTypeDecl, br,
                                breakReturnType, breakReturnTypeDecl,
                            }
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Line Breaks"),
                    Column {
                        Grid {
                            breakBlocks, breakBlocksAll, br,
                            breakOneLineHeaders, breakAfterLogical, br,
                            keepOneLineBlocks, keepOneLineStatements,
                        },
                        Form {
                            breakElseIfs, br,
                            lineBetweenMembers,
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Blank Lines and Misc"),
                    Column {
                        Grid {
                            deleteEmptyLines, fillEmptyLines, br,
                            closeTemplates, removeCommentPrefix,
                        },
                        Form {
                            squeezeLines,
                        },
                        st
                    }
                },
                Tab {
                    Tr::tr("Files and Configuration"),
                    Column {
                        Form {
                            fileExtensions, br,
                            useAstyleRc, br,
                            Row { useSpecificConfigFile, specificConfigFile }, br,
                            extraOptions,
                        },
                        st
                    }
                },
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

    // A separate tab width only exists for forced tabs.
    if (indentKind == "force-tab" && indentForceTabX() > 0)
        options.append(QStringLiteral("--indent=force-tab-x=%1").arg(indentForceTabX()));

    if (indentContinuation() > 0)
        options.append(QStringLiteral("--indent-continuation=%1").arg(indentContinuation()));

    options.append(QStringLiteral("--min-conditional-indent=%1")
                       .arg(minConditionalIndent.itemValue().toString()));

    // The library rejects values below 40, the spin box allows 0 as "default".
    if (maxContinuationIndent() > 0) {
        options.append(QStringLiteral("--max-continuation-indent=%1")
                           .arg(qMax(qint64(40), maxContinuationIndent())));
    }

    if (maxCodeLength() > 0) {
        // The library rejects values outside 50..200, the spin box allows 0 as "off".
        options.append(QStringLiteral("--max-code-length=%1")
                           .arg(qBound(qint64(50), maxCodeLength(), qint64(200))));
        const QString mode = maxCodeLengthMode.itemValue().toString();
        if (!mode.isEmpty())
            options.append("--max-code-length-mode=" + mode);
    }

    for (const ConditionalSwitch &conditionalSwitch : std::as_const(m_conditionalSwitches)) {
        if ((*conditionalSwitch.aspect)())
            options.append(conditionalSwitch.option);
    }

    const auto addSelection = [&options](const SelectionAspect &aspect) {
        const QString option = aspect.itemValue().toString();
        if (!option.isEmpty())
            options.append(option);
    };
    addSelection(padInclude);
    addSelection(padNegation);
    addSelection(breakElseIfs);
    addSelection(lineBetweenMembers);

    addSelection(alignPointer);
    addSelection(alignReference);

    if (squeezeLines() > 0)
        options.append(QStringLiteral("--squeeze-lines=%1").arg(squeezeLines()));

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
    options.append(QLatin1String(MODE_OPTION));

    if (const FilePath configFile = configFileFor(filePath); !configFile.isEmpty()) {
        const QString configuration = contentsOf(configFile);
        if (!configuration.isEmpty())
            options.append(configuration);
    }

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
