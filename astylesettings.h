// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <utils/aspects.h>
#include <utils/filepath.h>

#include <QStringList>

#include <vector>

namespace Astyle::Internal {

// Artistic Style can format C, C++, C#, Java, Objective-C and a couple of
// scripting languages. This plugin formats C++ only, so only the options that
// affect C++ are offered here and every file is passed to the library with
// --mode=c. The Java, C#, Objective-C and scripting options of the manual are
// deliberately absent, they would only add noise.
class AstyleSettings final : public Utils::AspectContainer
{
public:
    AstyleSettings();

    // Switches derived from the options shown on the settings page.
    QStringList uiOptions() const;

    // Complete AStyle option text for one file: page options, the language
    // mode, configuration files and the free form extra options.
    QString optionsForFile(const Utils::FilePath &filePath) const;

    bool isSupportedFile(const Utils::FilePath &filePath) const;
    QStringList supportedSuffixes() const;
    Utils::FilePaths filteredFiles(const Utils::FilePaths &files) const;

    // The configuration file that would be used for @p filePath, empty if none.
    Utils::FilePath configFileFor(const Utils::FilePath &filePath) const;

    // --- General -----------------------------------------------------------
    Utils::SelectionAspect style{this};
    Utils::SelectionAspect indentMode{this};
    Utils::IntegerAspect indentWidth{this};
    Utils::IntegerAspect indentForceTabX{this};
    Utils::IntegerAspect indentContinuation{this};
    Utils::SelectionAspect minConditionalIndent{this};
    Utils::IntegerAspect maxContinuationIndent{this};
    Utils::IntegerAspect maxCodeLength{this};
    Utils::SelectionAspect maxCodeLengthMode{this};
    Utils::SelectionAspect alignPointer{this};
    Utils::SelectionAspect alignReference{this};

    // --- Indentation -------------------------------------------------------
    Utils::BoolAspect indentClasses{this};
    Utils::BoolAspect indentModifiers{this};
    Utils::BoolAspect indentSwitches{this};
    Utils::BoolAspect indentCases{this};
    Utils::BoolAspect indentNamespaces{this};
    Utils::BoolAspect indentAfterParens{this};
    Utils::BoolAspect indentLabels{this};
    Utils::BoolAspect indentCol1Comments{this};
    Utils::BoolAspect indentPreprocBlock{this};
    Utils::BoolAspect indentPreprocDefine{this};
    Utils::BoolAspect indentPreprocCond{this};
    Utils::BoolAspect convertTabs{this};

    // --- Spacing -----------------------------------------------------------
    Utils::BoolAspect padOper{this};
    Utils::BoolAspect padComma{this};
    Utils::BoolAspect padParenOut{this};
    Utils::BoolAspect padParenIn{this};
    Utils::BoolAspect padFirstParenOut{this};
    Utils::BoolAspect padEmptyParen{this};
    Utils::BoolAspect padHeader{this};
    Utils::BoolAspect unpadParen{this};
    Utils::BoolAspect padSemicolon{this};
    Utils::SelectionAspect padInclude{this};
    Utils::SelectionAspect padNegation{this};

    // --- Braces ------------------------------------------------------------
    Utils::BoolAspect attachNamespaces{this};
    Utils::BoolAspect attachClasses{this};
    Utils::BoolAspect attachInlines{this};
    Utils::BoolAspect attachExternC{this};
    Utils::BoolAspect attachClosingWhile{this};
    Utils::BoolAspect breakClosingBraces{this};
    Utils::BoolAspect addBraces{this};
    Utils::BoolAspect addOneLineBraces{this};
    Utils::BoolAspect removeBraces{this};
    Utils::BoolAspect attachReturnType{this};
    Utils::BoolAspect attachReturnTypeDecl{this};
    Utils::BoolAspect breakReturnType{this};
    Utils::BoolAspect breakReturnTypeDecl{this};

    // --- Line breaks -------------------------------------------------------
    Utils::BoolAspect breakBlocks{this};
    Utils::BoolAspect breakBlocksAll{this};
    Utils::BoolAspect breakOneLineHeaders{this};
    Utils::SelectionAspect breakElseIfs{this};
    Utils::BoolAspect breakAfterLogical{this};
    Utils::SelectionAspect lineBetweenMembers{this};
    Utils::BoolAspect keepOneLineBlocks{this};
    Utils::BoolAspect keepOneLineStatements{this};

    // --- Blank lines and miscellaneous -------------------------------------
    Utils::BoolAspect deleteEmptyLines{this};
    Utils::BoolAspect fillEmptyLines{this};
    Utils::IntegerAspect squeezeLines{this};
    Utils::BoolAspect closeTemplates{this};
    Utils::BoolAspect removeCommentPrefix{this};

    // --- Files and configuration -------------------------------------------
    Utils::StringAspect fileExtensions{this};
    Utils::BoolAspect useAstyleRc{this};
    Utils::BoolAspect useSpecificConfigFile{this};
    Utils::FilePathAspect specificConfigFile{this};
    Utils::StringAspect extraOptions{this};

private:
    struct ConditionalSwitch
    {
        Utils::BoolAspect *aspect;
        QString option;
    };

    std::vector<ConditionalSwitch> m_conditionalSwitches;
};

AstyleSettings &settings();

} // namespace Astyle::Internal
