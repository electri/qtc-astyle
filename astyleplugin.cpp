// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "astyleconstants.h"
#include "astyleformatter.h"
#include "astyleprojectdialog.h"
#include "astylesettings.h"
#include "astyletr.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/dialogs/ioptionspage.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>

#include <extensionsystem/iplugin.h>

#include <projectexplorer/project.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/projecttree.h>

#include <texteditor/formattexteditor.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditor.h>

#include <utils/fileutils.h>

#include <QAction>

using namespace Utils;

namespace Astyle::Internal {

class AstylePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Astyle.json")

public:
    AstylePlugin() = default;
    ~AstylePlugin() final = default;

    void initialize() final;
    void extensionsInitialized() final;

private:
    void formatCurrentFile();
    void formatProject();
    void updateActionStates();
    void reportOptionProblems(const QStringList &problems);

    QAction *m_formatFileAction = nullptr;
};

void AstylePlugin::initialize()
{
    Core::IOptionsPage::registerCategory(Constants::OPTION_CATEGORY,
                                         Tr::tr("Artistic Style"),
                                         ":/astyle/images/settingscategory_astyle.png");

    Core::MenuBuilder(Constants::MENU_ID)
        .setTitle(Tr::tr("&Artistic Style"))
        .setOnAllDisabledBehavior(Core::ActionContainer::Show)
        .addToContainer(Core::Constants::M_TOOLS);

    Core::ActionBuilder(this, Constants::ACTION_FORMAT_FILE)
        .setText(Tr::tr("Format &Current File"))
        .setContext(Core::Constants::C_GLOBAL)
        .bindContextAction(&m_formatFileAction)
        .addToContainer(Constants::MENU_ID)
        .addOnTriggered(this, &AstylePlugin::formatCurrentFile);

    Core::ActionBuilder(this, Constants::ACTION_FORMAT_PROJECT)
        .setText(Tr::tr("Format Current &Project..."))
        .setContext(Core::Constants::C_GLOBAL)
        .addToContainer(Constants::MENU_ID)
        .addOnTriggered(this, &AstylePlugin::formatProject);

    Core::ActionBuilder(this, Constants::ACTION_OPTIONS)
        .setText(Tr::tr("Artistic Style &Options..."))
        .setContext(Core::Constants::C_GLOBAL)
        .addToContainer(Constants::MENU_ID)
        .addOnTriggered(this, [] {
            Core::ICore::showSettings(Constants::OPTION_PAGE_ID);
        });

    if (Core::Command *command = Core::ActionManager::command(Constants::ACTION_FORMAT_FILE))
        command->setDefaultKeySequence(QKeySequence("Ctrl+Alt+F"));
}

void AstylePlugin::extensionsInitialized()
{
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &AstylePlugin::updateActionStates);
    updateActionStates();
}

void AstylePlugin::updateActionStates()
{
    const Core::IEditor *editor = Core::EditorManager::currentEditor();
    const bool applicable = editor
            && settings().isSupportedFile(editor->document()->filePath());
    m_formatFileAction->setEnabled(applicable);
}

void AstylePlugin::reportOptionProblems(const QStringList &problems)
{
    if (problems.isEmpty())
        return;

    QStringList message;
    message << Tr::tr("Artistic Style does not understand the configured options, nothing was "
                      "formatted:");
    for (const QString &problem : problems)
        message << QStringLiteral("    ") + problem;
    message << Tr::tr("Fix the options in Edit > Preferences > Artistic Style.");
    Core::MessageManager::writeDisrupting(message);
}

void AstylePlugin::formatCurrentFile()
{
    TextEditor::TextEditorWidget *widget = TextEditor::TextEditorWidget::currentTextEditorWidget();
    if (!widget) {
        Core::MessageManager::writeFlashing(Tr::tr("No editor is open."));
        return;
    }

    const FilePath filePath = widget->textDocument()->filePath();
    const QString options = settings().optionsForFile(filePath);

    // Give precise feedback instead of silently doing nothing.
    if (const QStringList problems = validateOptions(options); !problems.isEmpty()) {
        reportOptionProblems(problems);
        return;
    }

    const QString source = widget->toPlainText();
    const FormatResult formatted = formatBytes(source.toUtf8(), options);
    if (!formatted.ok) {
        reportOptionProblems(formatted.errors);
        return;
    }

    const QString formattedText = QString::fromUtf8(formatted.text);
    if (formattedText == source) {
        Core::MessageManager::writeFlashing(Tr::tr("\"%1\" is already formatted.")
                                                .arg(filePath.fileName()));
        return;
    }

    TextEditor::updateEditorText(widget, formattedText);
    Core::MessageManager::writeFlashing(Tr::tr("Formatted \"%1\".").arg(filePath.fileName()));
}

void AstylePlugin::formatProject()
{
    const QList<ProjectExplorer::Project *> projects = ProjectExplorer::ProjectManager::projects();
    if (projects.isEmpty()) {
        Core::MessageManager::writeFlashing(Tr::tr("No project is open."));
        return;
    }

    // The dialog offers a project selector when more than one project is open,
    // and preselects the one the project tree currently points at.
    ProjectExplorer::Project *current = ProjectExplorer::ProjectTree::currentProject();
    if (!current)
        current = projects.first();

    AstyleProjectDialog dialog(projects, current, Core::ICore::mainWindow());
    dialog.exec();
}

} // namespace Astyle::Internal

#include "astyleplugin.moc"
