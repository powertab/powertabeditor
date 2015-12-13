/*
  * Copyright (C) 2012 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "recentfiles.h"

#include <app/settings.h>
#include <app/settingsmanager.h>
#include <QMenu>
#ifdef _WIN32
#include <QDir>
#include <shlobj.h>
#endif

RecentFiles::RecentFiles(SettingsManager &settings_manager,
                         QMenu *recent_files_menu, QObject *parent)
    : QObject(parent),
      mySettingsManager(settings_manager),
      myRecentFilesMenu(recent_files_menu)
{
    Q_ASSERT(recent_files_menu);

    /// Load recently used files from previous sessions.
    auto settings = mySettingsManager.getReadHandle();
    auto recent_files = settings->get(Settings::RecentFiles);
    for (const std::string &file : recent_files)
        myRecentFiles.append(QString::fromStdString(file));

    updateMenu();
}

RecentFiles::~RecentFiles()
{
}

void RecentFiles::save()
{
    auto settings = mySettingsManager.getWriteHandle();
    std::vector<std::string> recent_files;
    for (const QString &file : myRecentFiles)
        recent_files.push_back(file.toStdString());

    settings->set(Settings::RecentFiles, recent_files);
}

void RecentFiles::add(const QString &fileName)
{
    // If the filename is already in the list, move it to the front.
    myRecentFiles.removeOne(fileName);

    myRecentFiles.prepend(fileName);

    if (myRecentFiles.length() > MAX_RECENT_FILES)
        myRecentFiles.pop_back();

    save();
    updateMenu();

    // Add the file to the recent files jump list.
#ifdef _WIN32
    SHAddToRecentDocs(SHARD_PATHW, QDir::toNativeSeparators(fileName).utf16());
#endif
}

void RecentFiles::updateMenu()
{
    myRecentFilesMenu->clear();

    for (const QString& fileName : myRecentFiles)
    {
        auto fileAction = new QAction(fileName, myRecentFilesMenu);
        myRecentFilesMenu->addAction(fileAction);

        connect(fileAction, &QAction::triggered, [=]() {
            handleFileSelection(fileName);
        });
    }

    if (!myRecentFiles.isEmpty())
    {
        myRecentFilesMenu->addSeparator();

        QAction *clearRecentFiles = new QAction(tr("Clear Recent Files"),
                                                myRecentFilesMenu);
        connect(clearRecentFiles, &QAction::triggered, this,
                &RecentFiles::clear);
        myRecentFilesMenu->addAction(clearRecentFiles);
    }
}

void RecentFiles::clear()
{
    myRecentFiles.clear();
    save();
    updateMenu();
}

void RecentFiles::handleFileSelection(const QString &fileName)
{
    emit fileSelected(fileName);
}
