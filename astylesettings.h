// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <utils/aspects.h>
#include <utils/filepath.h>

#include <QStringList>

#include <vector>

namespace Astyle::Internal {

class AstyleSettings final : public Utils::AspectContainer
{
public:
    AstyleSettings();

    // Switches derived from the options shown on the settings page.
    QStringList uiOptions() const;

    // Complete AStyle option text for one file: page options, language mode,
    // configuration files and the free form extra options.
    QString optionsForFile(const Utils::FilePath &filePath) const;

    // Same, but for a bare file suffix such as "cpp".
    QString optionsForSuffix(const QString &suffix) const;

    bool isSupportedFile(const Utils::FilePath &filePath) const;
    QStringList supportedSuffixes() const;
    Utils::FilePaths filteredFiles(const Utils::FilePaths &files) const;

    // The configuration file that would be used for @p filePath, empty if none.
    Utils::FilePath configFileFor(const Utils::FilePath &filePath) const;

    Utils::SelectionAspect style{this};
    Utils::SelectionAspect indentMode{this};
    Utils::IntegerAspect indentWidth{this};
    Utils::IntegerAspect indentContinuation{this};
    Utils::IntegerAspect maxCodeLength{this};

    Utils::BoolAspect indentClasses{this};
    Utils::BoolAspect indentModifiers{this};
    Utils::BoolAspect indentSwitches{this};
    Utils::BoolAspect indentCases{this};
    Utils::BoolAspect indentNamespaces{this};
    Utils::BoolAspect indentLabels{this};
    Utils::BoolAspect indentPreprocBlock{this};
    Utils::BoolAspect indentPreprocDefine{this};
    Utils::BoolAspect indentPreprocCond{this};
    Utils::BoolAspect indentCol1Comments{this};

    Utils::BoolAspect padOper{this};
    Utils::BoolAspect padComma{this};
    Utils::BoolAspect padParenOut{this};
    Utils::BoolAspect padParenIn{this};
    Utils::BoolAspect padFirstParenOut{this};
    Utils::BoolAspect padHeader{this};
    Utils::BoolAspect unpadParen{this};
    Utils::BoolAspect padInclude{this};
    Utils::SelectionAspect alignPointer{this};
    Utils::SelectionAspect alignReference{this};

    Utils::BoolAspect breakBlocks{this};
    Utils::BoolAspect breakBlocksAll{this};
    Utils::BoolAspect breakOneLineHeaders{this};
    Utils::BoolAspect breakElseIfs{this};
    Utils::BoolAspect keepOneLineBlocks{this};
    Utils::BoolAspect keepOneLineStatements{this};

    Utils::BoolAspect attachNamespaces{this};
    Utils::BoolAspect attachClasses{this};
    Utils::BoolAspect attachInlines{this};
    Utils::BoolAspect attachExternC{this};
    Utils::BoolAspect convertTabs{this};
    Utils::BoolAspect deleteEmptyLines{this};

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
