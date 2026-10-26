// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "astyleprojectdialog.h"

#include "astyleformatter.h"
#include "astylesettings.h"
#include "astyletr.h"

#include <coreplugin/editormanager/documentmodel.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>

#include <projectexplorer/project.h>

#include <texteditor/formattexteditor.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditor.h>

#include <utils/fancylineedit.h>
#include <utils/fsengine/fileiconprovider.h>
#include <utils/fileutils.h>
#include <utils/textfileformat.h>

#include <QComboBox>
#include <QDir>
#include <QFont>
#include <QHBoxLayout>
#include <QHash>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSet>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

#include <algorithm>
#include <utility>

using namespace Utils;

namespace Astyle::Internal {

namespace {

constexpr int kStatusColumn = 2;

bool isHeaderSuffix(const QString &suffix)
{
    static const QStringList suffixes{"h", "hh", "hpp", "hxx", "h++", "inl", "ipp"};
    return suffixes.contains(suffix);
}

bool isSourceSuffix(const QString &suffix)
{
    static const QStringList suffixes{"c", "cc", "cpp", "cxx", "c++", "m", "mm"};
    return suffixes.contains(suffix);
}

FilePath pathOf(const QTreeWidgetItem *item)
{
    return item->data(0, Qt::UserRole).value<FilePath>();
}

Core::IDocument *documentOf(const FilePath &filePath)
{
    return Core::DocumentModel::documentForFilePath(filePath);
}

// Runs on a worker thread, must not touch any UI or settings.
FileOutcome formatOneFile(const FileJob &job)
{
    FileOutcome outcome;

    const Result<QByteArray> original = job.filePath.fileContents();
    if (!original) {
        outcome.code = FileOutcome::Failed;
        outcome.error = original.error();
        return outcome;
    }

    const FormatResult formatted = formatBytes(*original, job.options);
    if (!formatted.ok) {
        outcome.code = FileOutcome::Failed;
        outcome.error = formatted.errors.join("; ");
        return outcome;
    }

    if (formatted.text == *original) {
        outcome.code = FileOutcome::Unchanged;
        return outcome;
    }

    outcome.code = FileOutcome::Changed;
    outcome.contents = formatted.text;
    return outcome;
}

// Replaces the file with exactly the bytes handed in. No encoding is involved
// on this path, which is what makes it safe for GBK or UTF-16 sources.
bool writeBytes(const FilePath &filePath, const QByteArray &contents, QString *error)
{
    FileSaver saver(filePath);
    if (saver.hasError()) {
        *error = saver.errorString();
        return false;
    }
    if (!saver.write(contents)) {
        *error = saver.errorString();
        return false;
    }
    if (const Result<> res = saver.finalize(); !res) {
        *error = res.error();
        return false;
    }
    return true;
}

// QStringEncoder() does not write the byte order mark back out, and the
// document save path only restores the UTF-8 one. Documents in a UTF-16 or
// UTF-32 format therefore have to keep the byte for byte route.
bool hasWideBom(const TextEncoding &encoding)
{
    const QByteArray name = encoding.name();
    return name.startsWith("UTF-16") || name.startsWith("UTF-32");
}

// Writes the formatted text back, either into the open document (so that the
// editor does not have to reload the file) or directly to disk.
bool writeBack(const FileOutcome &outcome, const FilePath &filePath, QString *error)
{
    if (Core::IDocument *document = documentOf(filePath)) {
        auto textDocument = qobject_cast<TextEditor::TextDocument *>(document);
        if (!textDocument) {
            *error = Tr::tr("is not a text document");
            return false;
        }
        if (textDocument->isModified()) {
            *error = Tr::tr("has unsaved changes");
            return false;
        }

        // Artistic Style keeps the byte stream of the file, so the result is
        // still encoded like the file on disk and not in UTF-8. Decoding it
        // with the format the document was opened with is mandatory: reading
        // a GBK or UTF-16 file as UTF-8 turns every non-ASCII character into
        // U+FFFD, and saving that back destroys the file.
        const TextFileFormat format = textDocument->format();
        QString text;
        if (!hasWideBom(format.encoding()) && format.decode(outcome.contents, &text)) {
            text.replace("\r\n", "\n");

            const QList<Core::IEditor *> editors
                    = Core::DocumentModel::editorsForDocument(document);
            TextEditor::TextEditorWidget *widget
                    = editors.isEmpty()
                    ? nullptr
                    : TextEditor::TextEditorWidget::fromEditor(editors.first());
            if (widget) {
                TextEditor::updateEditorText(widget, text);
            } else if (const Result<> res = textDocument->setPlainText(text); !res) {
                *error = res.error();
                return false;
            }

            if (const Result<> res = textDocument->save(); !res) {
                *error = res.error();
                return false;
            }
            return true;
        }

        // Either the document was opened with an encoding that cannot
        // represent the bytes on disk, or its format carries a byte order mark
        // that a round trip would drop. Rewriting the whole file in that case
        // is not acceptable, so write the bytes untouched and let the document
        // pick them up again.
        if (!writeBytes(filePath, outcome.contents, error))
            return false;
        const Result<> res = textDocument->reload(Core::IDocument::FlagReload,
                                                  Core::IDocument::TypeContents);
        if (!res) {
            *error = res.error();
            return false;
        }
        return true;
    }

    return writeBytes(filePath, outcome.contents, error);
}

void reportOptionProblems(const QStringList &problems, QWidget *parent)
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

    QMessageBox::warning(parent, Tr::tr("Format Project"), problems.join(QLatin1Char('\n')));
}

} // namespace

AstyleProjectDialog::AstyleProjectDialog(const QList<ProjectExplorer::Project *> &projects,
                                         ProjectExplorer::Project *currentProject,
                                         QWidget *parent)
    : QDialog(parent)
    , m_projects(projects)
    , m_currentProject(currentProject)
    , m_watcher(new QFutureWatcher<FileOutcome>(this))
{
    setWindowTitle(Tr::tr("Format Project"));
    setObjectName("Astyle.FormatProject");
    resize(760, 520);

    setupUi();

    connect(m_watcher, &QFutureWatcher<FileOutcome>::resultReadyAt,
            this, &AstyleProjectDialog::onResultReady);
    connect(m_watcher, &QFutureWatcher<FileOutcome>::finished,
            this, &AstyleProjectDialog::onFinished);

    populateFiles();
}

void AstyleProjectDialog::setupUi()
{
    auto layout = new QVBoxLayout(this);

    if (m_projects.size() > 1) {
        auto projectRow = new QHBoxLayout;
        auto projectLabel = new QLabel(Tr::tr("&Project:"), this);
        m_projectCombo = new QComboBox(this);
        projectLabel->setBuddy(m_projectCombo);
        for (ProjectExplorer::Project *project : std::as_const(m_projects))
            m_projectCombo->addItem(project->displayName());
        const int current = int(m_projects.indexOf(m_currentProject));
        m_projectCombo->setCurrentIndex(current >= 0 ? current : 0);
        projectRow->addWidget(projectLabel);
        projectRow->addWidget(m_projectCombo, 1);
        layout->addLayout(projectRow);
        connect(m_projectCombo, &QComboBox::currentIndexChanged,
                this, &AstyleProjectDialog::populateFiles);
    }

    auto filterRow = new QHBoxLayout;
    auto filterLabel = new QLabel(Tr::tr("Fi&lter:"), this);
    m_filterEdit = new FancyLineEdit(this);
    m_filterEdit->setFiltering(true);
    m_filterEdit->setPlaceholderText(Tr::tr("Filter the file list"));
    filterLabel->setBuddy(m_filterEdit);
    filterRow->addWidget(filterLabel);
    filterRow->addWidget(m_filterEdit, 1);
    layout->addLayout(filterRow);

    m_tree = new QTreeWidget(this);
    m_tree->setColumnCount(3);
    m_tree->setHeaderLabels({Tr::tr("File"), Tr::tr("Directory"), Tr::tr("Status")});
    m_tree->setRootIsDecorated(true);
    m_tree->setUniformRowHeights(true);
    m_tree->setAllColumnsShowFocus(true);
    m_tree->setExpandsOnDoubleClick(true);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tree->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(kStatusColumn, QHeaderView::Interactive);
    m_tree->setColumnWidth(0, 340);
    m_tree->setColumnWidth(kStatusColumn, 170);
    layout->addWidget(m_tree, 1);

    auto progressRow = new QHBoxLayout;
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_progress->setTextVisible(false);
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setMinimumWidth(260);
    progressRow->addWidget(m_progress, 1);
    progressRow->addWidget(m_summaryLabel);
    layout->addLayout(progressRow);

    auto buttonRow = new QHBoxLayout;
    // No default button on purpose: pressing Return while typing in the filter
    // field must not start a whole project run.
    m_formatButton = new QPushButton(Tr::tr("&Format"), this);
    m_stopButton = new QPushButton(Tr::tr("&Stop"), this);
    m_stopButton->hide();
    m_closeButton = new QPushButton(Tr::tr("&Close"), this);
    buttonRow->addStretch(1);
    buttonRow->addWidget(m_formatButton);
    buttonRow->addWidget(m_stopButton);
    buttonRow->addWidget(m_closeButton);
    layout->addLayout(buttonRow);

    connect(m_filterEdit, &QLineEdit::textChanged, this, &AstyleProjectDialog::applyFilter);
    connect(m_tree, &QTreeWidget::itemChanged, this, &AstyleProjectDialog::onItemChanged);
    connect(m_formatButton, &QPushButton::clicked, this, &AstyleProjectDialog::startFormatting);
    connect(m_stopButton, &QPushButton::clicked, this, &AstyleProjectDialog::stopFormatting);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);
}

void AstyleProjectDialog::populateFiles()
{
    m_updatingChecks = true;
    m_tree->clear();
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_summaryLabel->clear();

    ProjectExplorer::Project *project = m_projectCombo
            ? m_projects.value(m_projectCombo->currentIndex())
            : m_currentProject;

    if (!project) {
        m_tree->setEnabled(false);
        m_summaryLabel->setText(Tr::tr("No project is open."));
        m_formatButton->setEnabled(false);
        m_updatingChecks = false;
        return;
    }

    // The project files arrive in build system order, deduplicate them and use a
    // stable alphabetical order so that the dialog does not reshuffle.
    FilePaths files;
    QSet<QString> seen;
    for (const FilePath &file : settings().filteredFiles(
                 project->files(ProjectExplorer::Project::SourceFiles))) {
        const QString key = file.toFSPathString();
        if (seen.contains(key))
            continue;
        seen.insert(key);
        files.append(file);
    }
    std::sort(files.begin(), files.end(), [](const FilePath &lhs, const FilePath &rhs) {
        return QString::compare(lhs.toFSPathString(), rhs.toFSPathString(), Qt::CaseInsensitive)
                < 0;
    });

    if (files.isEmpty()) {
        m_tree->setEnabled(false);
        m_summaryLabel->setText(
            Tr::tr("Project \"%1\" has no source files with the configured extensions.")
                .arg(project->displayName()));
        m_formatButton->setEnabled(false);
        m_updatingChecks = false;
        return;
    }

    QTreeWidgetItem *headers = addGroup(Tr::tr("Header Files"));
    QTreeWidgetItem *sources = addGroup(Tr::tr("Source Files"));
    QTreeWidgetItem *others = addGroup(Tr::tr("Other Files"));

    const FilePath projectDir = project->projectDirectory();
    QHash<QTreeWidgetItem *, int> counts;
    for (const FilePath &file : std::as_const(files)) {
        const QString suffix = file.suffix().toLower();
        QTreeWidgetItem *group = isHeaderSuffix(suffix) ? headers
                : isSourceSuffix(suffix)               ? sources
                                                       : others;
        addFile(group, file);

        // Directory relative to the project, so that the list stays readable.
        QTreeWidgetItem *item = group->child(group->childCount() - 1);
        item->setText(1, QDir::toNativeSeparators(file.relativePathFromDir(projectDir)));
        ++counts[group];
    }

    const QList<QTreeWidgetItem *> groups{headers, sources, others};
    for (QTreeWidgetItem *group : groups) {
        const int count = counts.value(group);
        group->setText(0, group->data(0, Qt::UserRole + 1).toString()
                              + QStringLiteral(" (%1)").arg(count));
        group->setHidden(count == 0);
        group->setCheckState(0, count > 0 ? Qt::Checked : Qt::Unchecked);
    }

    m_updatingChecks = false;

    m_tree->expandAll();
    m_tree->setEnabled(true);
    applyFilter(m_filterEdit->text());
    updateSummary();
}

QTreeWidgetItem *AstyleProjectDialog::addGroup(const QString &title)
{
    auto item = new QTreeWidgetItem(m_tree);
    item->setText(0, title);
    item->setData(0, Qt::UserRole + 1, title);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    item->setCheckState(0, Qt::Unchecked);
    QFont font = item->font(0);
    font.setBold(true);
    item->setFont(0, font);
    return item;
}

void AstyleProjectDialog::addFile(QTreeWidgetItem *group, const FilePath &filePath)
{
    auto item = new QTreeWidgetItem(group);
    item->setText(0, filePath.fileName());
    item->setIcon(0, Utils::FileIconProvider::icon(filePath));
    item->setToolTip(0, filePath.toUserOutput());
    item->setData(0, Qt::UserRole, QVariant::fromValue(filePath));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    item->setCheckState(0, Qt::Checked);

    // Files with unsaved changes cannot be written back without losing the
    // editor content, so warn about them right away.
    if (const Core::IDocument *document = documentOf(filePath); document && document->isModified())
        setStatus(item, Tr::tr("Unsaved changes"));
}

void AstyleProjectDialog::onItemChanged(QTreeWidgetItem *item, int column)
{
    if (m_updatingChecks || column != 0)
        return;

    m_updatingChecks = true;
    if (QTreeWidgetItem *group = item->parent()) {
        int checked = 0;
        const int total = group->childCount();
        for (int i = 0; i < total; ++i) {
            if (group->child(i)->checkState(0) == Qt::Checked)
                ++checked;
        }
        group->setCheckState(0, checked == total ? Qt::Checked
                                : checked == 0  ? Qt::Unchecked
                                                : Qt::PartiallyChecked);
    } else {
        // A click on a group check box applies to all of its files; a
        // partially checked group starts from "all checked".
        const Qt::CheckState state = item->checkState(0) == Qt::Checked ? Qt::Checked
                                                                       : Qt::Unchecked;
        for (int i = 0; i < item->childCount(); ++i)
            item->child(i)->setCheckState(0, state);
        item->setCheckState(0, state);
    }
    m_updatingChecks = false;

    updateSummary();
}

QList<QTreeWidgetItem *> AstyleProjectDialog::checkedFileItems() const
{
    QList<QTreeWidgetItem *> items;
    for (int g = 0; g < m_tree->topLevelItemCount(); ++g) {
        QTreeWidgetItem *group = m_tree->topLevelItem(g);
        for (int i = 0; i < group->childCount(); ++i) {
            QTreeWidgetItem *item = group->child(i);
            if (item->checkState(0) == Qt::Checked)
                items.append(item);
        }
    }
    return items;
}

void AstyleProjectDialog::applyFilter(const QString &text)
{
    const QString needle = text.trimmed();
    for (int g = 0; g < m_tree->topLevelItemCount(); ++g) {
        QTreeWidgetItem *group = m_tree->topLevelItem(g);
        int visible = 0;
        for (int i = 0; i < group->childCount(); ++i) {
            QTreeWidgetItem *item = group->child(i);
            const bool match = needle.isEmpty()
                    || item->text(0).contains(needle, Qt::CaseInsensitive)
                    || item->text(1).contains(needle, Qt::CaseInsensitive);
            item->setHidden(!match);
            if (match)
                ++visible;
        }
        group->setHidden(visible == 0);
        if (visible > 0)
            group->setExpanded(true);
    }
}

void AstyleProjectDialog::updateSummary()
{
    int total = 0;
    for (int g = 0; g < m_tree->topLevelItemCount(); ++g)
        total += m_tree->topLevelItem(g)->childCount();

    const int selected = int(checkedFileItems().size());
    m_summaryLabel->setText(Tr::tr("%1 of %2 files selected").arg(selected).arg(total));
    m_formatButton->setEnabled(!m_running && selected > 0 && m_tree->isEnabled());
}

void AstyleProjectDialog::setStatus(QTreeWidgetItem *item, const QString &text)
{
    item->setText(kStatusColumn, text);
    item->setToolTip(kStatusColumn, text);
}

void AstyleProjectDialog::startFormatting()
{
    QList<QTreeWidgetItem *> items = checkedFileItems();
    if (items.isEmpty() || m_running)
        return;

    if (!handleUnsavedFiles(&items))
        return;
    if (items.isEmpty())
        return;

    // Option texts are computed here, on the UI thread, and validated once per
    // distinct configuration so that a typo cannot ruin a whole project run.
    m_jobs.clear();
    m_jobItems.clear();
    m_messages.clear();
    m_changed = m_unchanged = m_skipped = m_failed = 0;

    QStringList problems;
    QSet<QString> validatedOptions;
    for (QTreeWidgetItem *item : std::as_const(items)) {
        FileJob job;
        job.filePath = pathOf(item);
        job.options = settings().optionsForFile(job.filePath);
        m_jobs.append(job);
        m_jobItems.append(item);
        setStatus(item, QString());

        if (!validatedOptions.contains(job.options)) {
            validatedOptions.insert(job.options);
            QString flattenedOptions = job.options;
            flattenedOptions.replace('\n', "; ");
            for (const QString &problem : validateOptions(job.options))
                problems.append(QStringLiteral("%1: %2").arg(flattenedOptions, problem));
        }
    }
    if (!problems.isEmpty()) {
        reportOptionProblems(problems, this);
        return;
    }

    m_summaryLabel->clear();
    m_progress->setRange(0, int(m_jobs.size()));
    m_progress->setValue(0);

    m_running = true;
    m_formatButton->hide();
    m_stopButton->show();
    m_stopButton->setEnabled(true);
    m_closeButton->setEnabled(false);
    m_tree->setEnabled(false);
    m_filterEdit->setEnabled(false);
    if (m_projectCombo)
        m_projectCombo->setEnabled(false);

    m_watcher->setFuture(QtConcurrent::mapped(m_jobs, &formatOneFile));
}

void AstyleProjectDialog::stopFormatting()
{
    if (!m_running)
        return;
    m_stopButton->setEnabled(false);
    m_stopButton->setText(Tr::tr("Stopping..."));
    m_watcher->cancel();
}

void AstyleProjectDialog::onResultReady(int index)
{
    if (index < 0 || index >= m_jobItems.size())
        return;

    const FileOutcome outcome = m_watcher->resultAt(index);
    const FilePath filePath = m_jobs.at(index).filePath;
    QTreeWidgetItem *item = m_jobItems.at(index);

    switch (outcome.code) {
    case FileOutcome::Unchanged:
        ++m_unchanged;
        setStatus(item, Tr::tr("Unchanged"));
        break;
    case FileOutcome::Failed:
        ++m_failed;
        setStatus(item, Tr::tr("Failed: %1").arg(outcome.error));
        m_messages << Tr::tr("Failed \"%1\": %2.").arg(filePath.fileName(), outcome.error);
        break;
    case FileOutcome::Changed: {
        QString error;
        if (writeBack(outcome, filePath, &error)) {
            ++m_changed;
            setStatus(item, Tr::tr("Changed"));
        } else {
            ++m_skipped;
            setStatus(item, Tr::tr("Skipped: %1").arg(error));
            m_messages << Tr::tr("Skipped \"%1\": %2.").arg(filePath.fileName(), error);
        }
        break;
    }
    }

    m_progress->setValue(m_progress->value() + 1);
    m_tree->scrollToItem(item);
}

void AstyleProjectDialog::onFinished()
{
    // Files that were still queued when the user pressed stop.
    for (QTreeWidgetItem *item : std::as_const(m_jobItems)) {
        if (item->text(kStatusColumn).isEmpty())
            setStatus(item, Tr::tr("Not processed"));
    }

    m_running = false;
    m_formatButton->show();
    m_stopButton->hide();
    m_stopButton->setText(Tr::tr("&Stop"));
    m_stopButton->setEnabled(true);
    m_closeButton->setEnabled(true);
    m_tree->setEnabled(true);
    m_filterEdit->setEnabled(true);
    if (m_projectCombo)
        m_projectCombo->setEnabled(true);

    QString summary = Tr::tr("Artistic Style: %1 formatted, %2 unchanged, %3 skipped, %4 failed.")
                          .arg(m_changed)
                          .arg(m_unchanged)
                          .arg(m_skipped)
                          .arg(m_failed);
    if (m_watcher->isCanceled())
        summary += QLatin1Char(' ') + Tr::tr("The run was canceled.");
    m_summaryLabel->setText(summary);

    if (m_failed == 0 && m_skipped == 0)
        Core::MessageManager::writeFlashing(summary);
    else
        Core::MessageManager::writeDisrupting(QStringList{summary} + m_messages);

    m_jobs.clear();
    m_jobItems.clear();
    m_messages.clear();
    updateSummary();
}

bool AstyleProjectDialog::handleUnsavedFiles(QList<QTreeWidgetItem *> *items)
{
    int modified = 0;
    for (const QTreeWidgetItem *item : std::as_const(*items)) {
        if (const Core::IDocument *document = documentOf(pathOf(item));
            document && document->isModified()) {
            ++modified;
        }
    }
    if (modified == 0)
        return true;

    QMessageBox box(QMessageBox::Question,
                    Tr::tr("Unsaved Changes"),
                    Tr::tr("%1 of the selected files are open with unsaved changes.")
                        .arg(modified),
                    QMessageBox::NoButton,
                    this);
    box.setInformativeText(Tr::tr("Save them first to include them in the run, otherwise they "
                                  "are skipped."));
    QPushButton *saveButton = box.addButton(Tr::tr("&Save and Format"), QMessageBox::AcceptRole);
    QPushButton *skipButton = box.addButton(Tr::tr("&Skip These Files"), QMessageBox::AcceptRole);
    box.addButton(QMessageBox::Cancel);
    box.setDefaultButton(saveButton);
    box.exec();

    if (box.clickedButton() != saveButton && box.clickedButton() != skipButton)
        return false;

    if (box.clickedButton() == saveButton) {
        QSet<Core::IDocument *> saved;
        for (const QTreeWidgetItem *item : std::as_const(*items)) {
            Core::IDocument *document = documentOf(pathOf(item));
            if (document && document->isModified() && !saved.contains(document)) {
                saved.insert(document);
                Core::EditorManager::saveDocument(document);
            }
        }
    }

    // Everything that is still modified is dropped from the run and unchecked,
    // so that the list shows what will not be touched.
    QList<QTreeWidgetItem *> remaining;
    for (QTreeWidgetItem *item : std::as_const(*items)) {
        const Core::IDocument *document = documentOf(pathOf(item));
        if (document && document->isModified()) {
            item->setCheckState(0, Qt::Unchecked);
            setStatus(item, Tr::tr("Skipped: unsaved changes"));
            continue;
        }
        remaining.append(item);
    }
    *items = remaining;
    return !items->isEmpty();
}

void AstyleProjectDialog::reject()
{
    if (m_watcher->isRunning()) {
        m_watcher->cancel();
        m_watcher->waitForFinished();
    }
    QDialog::reject();
}

} // namespace Astyle::Internal
