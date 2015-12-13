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

#ifndef APP_RECENTFILES_H
#define APP_RECENTFILES_H

#include <QObject>
#include <QStringList>

class QMenu;
class SettingsManager;

class RecentFiles : public QObject
{
    Q_OBJECT

public:
    RecentFiles(SettingsManager &settings_manager, QMenu *recent_files_menu,
                QObject *parent = nullptr);
    ~RecentFiles();

    /// Adds a file to the recent documents list.
    void add(const QString& fileName);

signals:
    void fileSelected(const QString& fileName);

private slots:
    /// Clears the recent files list.
    void clear();
    void handleFileSelection(const QString& fileName);

private:
    void updateMenu();

    /// Save the recent files list to disk.
    void save();

    static const int MAX_RECENT_FILES = 10;
    SettingsManager &mySettingsManager;
    QMenu *myRecentFilesMenu;
    QStringList myRecentFiles;
};

#endif
