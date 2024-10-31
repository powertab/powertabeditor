/*
  * Copyright (C) 2024 Cameron White
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

#ifndef APP_AUTOBACKUP_H
#define APP_AUTOBACKUP_H

#include <QObject>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <thread>

class DocumentManager;
class QTimer;
class SettingsManager;
class UndoManager;

class AutoBackup : public QObject
{
public:
    AutoBackup(const DocumentManager &document_manager, const UndoManager &undo_manager,
               SettingsManager &settings_manager,
               QObject *parent = nullptr);
    ~AutoBackup();

private:
    void updateTimerSettings(const SettingsManager &settings_manager);
    void startBackup();
    
    const DocumentManager &myDocumentManager;
    const UndoManager &myUndoManager;

    std::unique_ptr<QTimer> myTimer;
    std::thread myWorkerThread;
    boost::signals2::scoped_connection mySettingsListener;
};

#endif
