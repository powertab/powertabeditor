#include "recentfiles.h"

#include <QMenu>
#include <QSettings>

#include <app/settings.h>

#include <sigfwd/sigfwd.hpp>
#include <boost/bind.hpp>

RecentFiles::RecentFiles(QMenu* recentFilesMenu, QObject* parent) :
    QObject(parent),
    recentFilesMenu(recentFilesMenu)
{
    Q_ASSERT(recentFilesMenu);

    // load recently used files from previous sessions
    QSettings settings;
    recentFiles = settings.value(Settings::APP_RECENT_FILES).toStringList();
    updateMenu();
}

/// Save the recent files list to disk
RecentFiles::~RecentFiles()
{
    QSettings settings;
    settings.setValue(Settings::APP_RECENT_FILES, recentFiles);
}

/// Adds a file to the recent documents list
void RecentFiles::add(const QString &fileName)
{
    // if the filename is already in the list, move it to the front
    recentFiles.removeOne(fileName);
    recentFiles.prepend(fileName);

    updateMenu();
}

void RecentFiles::updateMenu()
{
    recentFilesMenu->clear();

    foreach (const QString& fileName, recentFiles)
    {
        QAction* fileAction = new QAction(fileName, recentFilesMenu);
        recentFilesMenu->addAction(fileAction);

        sigfwd::connect(fileAction, SIGNAL(triggered()),
                        boost::bind(&RecentFiles::handleFileSelection, this, fileName));
    }

    if (!recentFiles.isEmpty())
    {
        recentFilesMenu->addSeparator();

        QAction* clearRecentFiles = new QAction(tr("Clear Recent Files"), recentFilesMenu);
        connect(clearRecentFiles, SIGNAL(triggered()), this, SLOT(clear()));
        recentFilesMenu->addAction(clearRecentFiles);
    }
}

/// Clear the recent files list
void RecentFiles::clear()
{
    recentFiles.clear();
    updateMenu();
}

void RecentFiles::handleFileSelection(const QString &fileName)
{
    emit fileSelected(fileName);
}
