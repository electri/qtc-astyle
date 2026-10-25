// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <utils/filepath.h>
#include <utils/fancylineedit.h>

#include <QByteArray>
#include <QDialog>
#include <QFutureWatcher>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace ProjectExplorer {
class Project;
}

namespace Astyle::Internal {

// Result of formatting a single file, filled in on a worker thread.
struct FileOutcome
{
    enum Code { Unchanged, Changed, Failed };

    Code code = Unchanged;
    QByteArray contents;
    QString error;
};

// One scheduled file: the path plus the AStyle option text computed for it on
// the UI thread (option lookup touches the settings object and is not thread
// safe).
struct FileJob
{
    Utils::FilePath filePath;
    QString options;
};

// Lists the source files of a project, lets the user pick the subset to format
// and runs the formatting with per file progress and result feedback.
class AstyleProjectDialog final : public QDialog
{
    Q_OBJECT

public:
    AstyleProjectDialog(const QList<ProjectExplorer::Project *> &projects,
                        ProjectExplorer::Project *currentProject,
                        QWidget *parent = nullptr);

    void reject() override;

private:
    void setupUi();
    void populateFiles();
    QTreeWidgetItem *addGroup(const QString &title);
    void addFile(QTreeWidgetItem *group, const Utils::FilePath &filePath);
    void onItemChanged(QTreeWidgetItem *item, int column);
    QList<QTreeWidgetItem *> checkedFileItems() const;
    void applyFilter(const QString &text);
    void updateSummary();

    void setStatus(QTreeWidgetItem *item, const QString &text);
    void startFormatting();
    void stopFormatting();
    void onResultReady(int index);
    void onFinished();
    bool handleUnsavedFiles(QList<QTreeWidgetItem *> *items);

    QComboBox *m_projectCombo = nullptr;
    Utils::FancyLineEdit *m_filterEdit = nullptr;
    QTreeWidget *m_tree = nullptr;
    QLabel *m_summaryLabel = nullptr;
    QProgressBar *m_progress = nullptr;
    QPushButton *m_formatButton = nullptr;
    QPushButton *m_stopButton = nullptr;
    QPushButton *m_closeButton = nullptr;

    QList<ProjectExplorer::Project *> m_projects;
    ProjectExplorer::Project *m_currentProject = nullptr;

    QFutureWatcher<FileOutcome> *m_watcher = nullptr;
    QVector<FileJob> m_jobs;
    QVector<QTreeWidgetItem *> m_jobItems;
    QStringList m_messages;
    int m_changed = 0;
    int m_unchanged = 0;
    int m_skipped = 0;
    int m_failed = 0;
    bool m_updatingChecks = false;
    bool m_running = false;
};

} // namespace Astyle::Internal
