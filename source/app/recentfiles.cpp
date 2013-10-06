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

#include <QMenu>
#include <QSettings>

#include <app/settings.h>
#include <sigfwd/sigfwd.hpp>

RecentFiles::RecentFiles(QMenu *recentFilesMenu, QObject *parent) :
    QObject(parent),
    myRecentFilesMenu(recentFilesMenu)
{
    Q_ASSERT(recentFilesMenu);

    // load recently used files from previous sessions
    QSettings settings;
    myRecentFiles = settings.value(Settings::APP_RECENT_FILES).toStringList();
    updateMenu();
}

RecentFiles::~RecentFiles()
{
    save();
}

void RecentFiles::save()
{
    QSettings settings;
    settings.setValue(Settings::APP_RECENT_FILES, myRecentFiles);
}

void RecentFiles::add(const QString &fileName)
{
    // if the filename is already in the list, move it to the front
    myRecentFiles.removeOne(fileName);

    myRecentFiles.prepend(fileName);

    if (myRecentFiles.length() > MAX_RECENT_FILES)
    {
        myRecentFiles.pop_back();
    }

    save();
    updateMenu();
}

void RecentFiles::updateMenu()
{
    myRecentFilesMenu->clear();

    foreach (const QString& fileName, myRecentFiles)
    {
        auto fileAction = new QAction(fileName, myRecentFilesMenu);
        myRecentFilesMenu->addAction(fileAction);

        sigfwd::connect(fileAction, SIGNAL(triggered()), [=]() {
            handleFileSelection(fileName);
        });
    }

    if (!myRecentFiles.isEmpty())
    {
        myRecentFilesMenu->addSeparator();

        QAction *clearRecentFiles = new QAction(tr("Clear Recent Files"),
                                                myRecentFilesMenu);
        connect(clearRecentFiles, SIGNAL(triggered()), this, SLOT(clear()));
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
