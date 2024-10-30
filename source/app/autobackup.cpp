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

#include "autobackup.h"

#include "documentmanager.h"
#include "paths.h"

#include <actions/undomanager.h>
#include <formats/powertab/powertabexporter.h>

#include <QTimer>

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

namespace
{
struct BackupItem
{
    /// Saves the file to the backup folder.
    void save() const;

    Score myScore;
    std::filesystem::path myOrigPath;
};

void
BackupItem::save() const
{
    PowerTabExporter exporter;

    auto backup_dir = Paths::getBackupDir();
    std::filesystem::create_directories(backup_dir);

    auto filename = myOrigPath.filename();
    filename.replace_extension(".pt2");
    
    auto path = backup_dir / filename;    

    try
    {
        exporter.save(path, myScore);
        std::cerr << "Saved backup to: " << path << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to save backup file: " << path << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
} // namespace

static std::mutex theLock;
static std::condition_variable theCV;
static std::vector<BackupItem> theBackupItems;
static bool theFinishedFlag = false;

/// Background thread for saving backup files without blocking the UI.
static void
backupThread()
{
    while (true)
    {
        std::vector<BackupItem> backup_items;
        
        // Wait for the next set of backup files.
        {
            std::unique_lock lock(theLock);
            theCV.wait(lock, [] { return theFinishedFlag || !theBackupItems.empty(); });

            if (theFinishedFlag)
                break;

            // Move to a local copy so we can release the lock before saving to disk.
            backup_items = std::move(theBackupItems);
        }

        for (const BackupItem &item : backup_items)
            item.save();
    }
}

AutoBackup::AutoBackup(const DocumentManager &document_manager, const UndoManager &undo_manager,
                       QObject *parent)
    : QObject(parent),
      myDocumentManager(document_manager),
      myUndoManager(undo_manager),
      myTimer(std::make_unique<QTimer>(this)),
      myWorkerThread(backupThread)
{
    connect(myTimer.get(), &QTimer::timeout, this, &AutoBackup::startBackup);

    static constexpr int interval_ms = 5000; // TODO - configure via preferences.
    myTimer->start(interval_ms);
}

AutoBackup::~AutoBackup()
{
    // Notify the worker thread and wait for it to finish.
    {
        std::unique_lock lock(theLock);
        theFinishedFlag = true;
    }
    theCV.notify_one();
    
    myWorkerThread.join();
}

void
AutoBackup::startBackup()
{
    std::cerr << "Starting backup..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<BackupItem> items_to_backup;

    for (int i = 0, n = myDocumentManager.getNumDocuments(); i < n; ++i)
    {
        // Only consider files with unsaved changes.
        if (myUndoManager.stacks()[i]->isClean())
            continue;
        
        const Document &doc = myDocumentManager.getDocument(i);
        
        BackupItem item;
        // Note we make a copy of the score, so that it can be safely saved from the background
        // thread.
        item.myScore = doc.getScore();

        if (doc.hasFilename())
            item.myOrigPath = doc.getFilename();
        else
            item.myOrigPath = "Untitled_" + std::to_string(i);

        std::cerr << "Added to backup: " << item.myOrigPath << std::endl;
        
        items_to_backup.push_back(std::move(item));
    }

    if (items_to_backup.empty())
    {
        std::cerr << "No files for backup" << std::endl;
        return;
    }

    // Send the documents to the worker thread to be saved to disk.
    {
        std::unique_lock lock(theLock);
        theBackupItems = std::move(items_to_backup);
    }
    theCV.notify_one();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << "Prepared documents for backup in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"
              << std::endl;
}
