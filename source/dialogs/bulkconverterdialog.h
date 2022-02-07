/*
  * Copyright (C) 2020 Simon Symeonidis
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

#ifndef DIALOGS_BULKCONVERTERDIALOG_H
#define DIALOGS_BULKCONVERTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QObject>
#include <QThread>

#include <filesystem>
#include <boost/range/iterator_range.hpp>

#include <formats/fileformatmanager.h>

#include <cstddef>

namespace Ui {
    class BulkConverterDialog;
}

class BulkConverterWorker : public QObject
{
    Q_OBJECT

public:
    explicit BulkConverterWorker(std::filesystem::path& source,
                                 std::filesystem::path& destination,
                                 bool dryRun,
                                 const FileFormat &export_format,
                                 std::unique_ptr<FileFormatManager>& fileFormatManager);
    ~BulkConverterWorker();

    inline void setFileCount(std::size_t f) { myFileCount = f; }
    inline std::size_t fileCount() { return myFileCount; }

public slots:
    void walkAndConvert();

signals:
    void progress(int pos);
    void done(void);
    void message(QString error);

private:
    std::filesystem::path mySrc;
    std::filesystem::path myDst;
    bool myDryRun;
    std::size_t myFileCount;
    FileFormat myExportFormat;
    std::unique_ptr<FileFormatManager>& myFileFormatManager;
};

class BulkConverterDialog : public QDialog {
    Q_OBJECT

public:
    explicit BulkConverterDialog(QWidget *parent,
                                 std::unique_ptr<FileFormatManager>& fileFormatManager);
    ~BulkConverterDialog();

public slots:
    void progress(int pos);
    void consumeMessage(QString message);

private slots:
    void setPath(QLineEdit*);
    void convert();
    void enableConvertButton(void);

private:
    QPushButton *convertButton();

    Ui::BulkConverterDialog *ui;
    QThread* myBulkWorkerThread;
    BulkConverterWorker* myBulkConverterWorker;
    std::unique_ptr<FileFormatManager>& myFileFormatManager;
};

#endif
