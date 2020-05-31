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

#include "bulkconverterdialog.h"
#include "ui_bulkconverterdialog.h"

#include "formats/powertab/common.h"

#include <app/paths.h>
#include <score/score.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QThread>

std::optional<std::string> convertFile(const boost::filesystem::path& src,
                                       const boost::filesystem::path& dst,
                                       std::unique_ptr<FileFormatManager>& ffm)
{
    QFileInfo fileInfo(QString::fromStdString(src.string()));
    std::optional<FileFormat> format = ffm->findFormat(
                fileInfo.suffix().toStdString());

    if (!format) return "bad format";

    Score score;
    try
    {
        ffm->importFile(score, src, *format);
    }
    catch (const std::exception &e)
    {
        return "could not import file: " + src.string();
    }

    try
    {
      ffm->exportFile(score, dst, FileFormat(getPowerTabFileFormat()));
    }
    catch (const std::exception &e)
    {
        return "could not export file: " + dst.string();
    }

    return std::nullopt;
}

BulkConverterWorker::BulkConverterWorker(boost::filesystem::path& source,
                                         boost::filesystem::path& destination,
                                         bool dryRun,
                                         std::unique_ptr<FileFormatManager>& fileFormatManager)
  : QObject(), mySrc(source), myDst(destination), myDryRun(dryRun),
    myFileCount(0), myFileFormatManager(fileFormatManager)
{
}

BulkConverterWorker::~BulkConverterWorker()
{
}

void BulkConverterWorker::walkAndConvert()
{
    std::vector<boost::filesystem::path> children;
    const std::string basePath = mySrc.string();

    children.emplace_back(mySrc);

    do {
        boost::filesystem::path p = children.back();
        children.pop_back();

        if (!boost::filesystem::is_directory(p)) continue;

        auto dir_it = boost::filesystem::directory_iterator(p);

        for(auto& entry : boost::make_iterator_range(dir_it, {})) {
            if (boost::filesystem::is_directory(entry)) {
               children.emplace_back(entry);
               continue;
            }

            // TODO: this should probably handle any supported format.
            //   check to see if there is a list of file formats
            //   like this
            if (!(boost::filesystem::is_regular_file(entry) &&
                  boost::filesystem::extension(entry) == ".ptb"))
                continue;

            myFileCount++;

            auto filePath = entry.path().string();
            const size_t pos = filePath.find(basePath);
            if (pos != std::string::npos)
                filePath.erase(pos, basePath.length());

            auto toPath = myDst / boost::filesystem::path(filePath);
            toPath.replace_extension("pt2");

            if (myDryRun) continue;

            auto destBaseDir = toPath.parent_path();
            if (!boost::filesystem::exists(destBaseDir))
                boost::filesystem::create_directory(destBaseDir);

            std::optional<std::string> error = convertFile(entry, toPath, myFileFormatManager);
            if (error != std::nullopt) {
                QString err = QString::fromStdString(error.value());
                emit errorMessage("error processing file: " + err);
            }

            emit progress((int) myFileCount);
        }
    } while (!children.empty());

    emit done();
}

BulkConverterDialog::BulkConverterDialog(QWidget *parent,
                                         std::unique_ptr<FileFormatManager>& manager)
  : QDialog(parent), ui(new Ui::BulkConverterDialog),
    myBulkWorkerThread(nullptr), myBulkConverterWorker(nullptr),
    myFileFormatManager(manager)
{
    ui->setupUi(this);

    auto flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);

    connect(ui->fileLocatorSource, &QAbstractButton::clicked, [=]() {
        setPath(ui->sourcePathEdit);
    });

    connect(ui->fileLocatorDestination, &QAbstractButton::clicked, [=]() {
        setPath(ui->destinationPathEdit);
    });

    connect(ui->convertButton, &QAbstractButton::clicked, [=](){ convert(); });

    connect(ui->exitButton, &QAbstractButton::clicked, this, &QDialog::reject);
}

BulkConverterDialog::~BulkConverterDialog()
{
    if (myBulkWorkerThread) {
        myBulkWorkerThread->quit();
        myBulkWorkerThread->wait();
    }
    delete ui;
}

void BulkConverterDialog::setPath(QLineEdit *edit)
{
    auto dialogOptions = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;
    QString path = QFileDialog::getExistingDirectory(
        this, tr("Directory To Convert"), QDir::currentPath(), dialogOptions);

    edit->setText(path);
    QString src = ui->sourcePathEdit->text();
    QString dst = ui->destinationPathEdit->text();
    const bool bothEditsHaveValues = !src.isEmpty() && !dst.isEmpty();
    ui->convertButton->setEnabled(bothEditsHaveValues);
}

void BulkConverterDialog::convert()
{
    ui->convertButton->setEnabled(false);

    boost::filesystem::path src = Paths::fromQString(ui->sourcePathEdit->text());
    boost::filesystem::path dst = Paths::fromQString(ui->destinationPathEdit->text());

    {
        // set default max.
        BulkConverterWorker bcw(src, dst, true, myFileFormatManager);
        bcw.walkAndConvert();
        const std::size_t fileCountToConvert = bcw.fileCount();
        ui->progressBar->setMaximum((int) fileCountToConvert);
        ui->progressBar->setValue(0);
    }

    // create thread to offload work in the background, such that we can
    // update the progress bar with each step informing the user of the
    // current state of bulk conversion.
    myBulkWorkerThread = new QThread;
    myBulkConverterWorker = new BulkConverterWorker(src, dst, false, myFileFormatManager);
    myBulkConverterWorker->moveToThread(myBulkWorkerThread);

    connect(myBulkWorkerThread, &QThread::started,
            myBulkConverterWorker, &BulkConverterWorker::walkAndConvert);

    connect(myBulkWorkerThread, &QThread::finished,
            myBulkWorkerThread, &QObject::deleteLater);

    connect(myBulkConverterWorker, &BulkConverterWorker::progress,
            this, &BulkConverterDialog::progress);

    connect(myBulkConverterWorker, &BulkConverterWorker::done,
            myBulkConverterWorker, &QObject::deleteLater);

    connect(myBulkConverterWorker, &BulkConverterWorker::done,
            this, &BulkConverterDialog::enableConvertButton);

    connect(myBulkConverterWorker, &BulkConverterWorker::errorMessage,
            this, &BulkConverterDialog::consumeErrorMessage);

    myBulkWorkerThread->start();
}

void BulkConverterDialog::progress(int pos)
{
    ui->progressBar->setValue(pos);
}

void BulkConverterDialog::enableConvertButton(void)
{
    ui->convertButton->setEnabled(true);
}

void BulkConverterDialog::consumeErrorMessage(QString error)
{
    ui->logger->appendPlainText(error);
}
