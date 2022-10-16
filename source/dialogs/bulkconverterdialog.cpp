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
#include <QDebug>
#include <QThread>

/// Returns the file extension without the leading '.'
static std::string
getExtension(const std::filesystem::path &path)
{
    std::string extension = path.extension().string();
    extension.erase(std::remove(extension.begin(), extension.end(), '.'),
                    extension.end());
    return extension;
}

static std::optional<std::string>
convertFile(const std::filesystem::path &src, const std::filesystem::path &dst,
            const FileFormat &export_format,
            std::unique_ptr<FileFormatManager> &ffm)
{
    std::optional<FileFormat> format = ffm->findFormat(getExtension(src));
    // Should already have a valid import extension if this method is called.
    assert(format);

    Score score;
    try
    {
        ffm->importFile(score, src, *format);
    }
    catch (const std::exception &)
    {
        return "could not import file: " + src.string();
    }

    try
    {
        ffm->exportFile(score, dst, export_format);
    }
    catch (const std::exception &)
    {
        return "could not export file: " + dst.string();
    }

    return std::nullopt;
}

BulkConverterWorker::BulkConverterWorker(
    std::filesystem::path &source, std::filesystem::path &destination,
    bool dryRun, const FileFormat &export_format,
    std::unique_ptr<FileFormatManager> &fileFormatManager)
    : QObject(),
      mySrc(source),
      myDst(destination),
      myDryRun(dryRun),
      myFileCount(0),
      myExportFormat(export_format),
      myFileFormatManager(fileFormatManager)
{
}

BulkConverterWorker::~BulkConverterWorker()
{
}

void BulkConverterWorker::walkAndConvert()
{
    std::vector<std::filesystem::path> children;

    children.emplace_back(mySrc);

    do {
        std::filesystem::path p = children.back();
        children.pop_back();

        if (!std::filesystem::is_directory(p)) continue;

        auto dir_it = std::filesystem::directory_iterator(p);

        for(auto& entry : boost::make_iterator_range(dir_it, {}))
        {
            if (std::filesystem::is_directory(entry)) {
               children.emplace_back(entry);
               continue;
            }

            const bool isRegularFile = std::filesystem::is_regular_file(entry);
            std::string extension = getExtension(entry.path());
            const bool isSupportedFormat = myFileFormatManager->extensionImportSupported(extension);

            if (!(isRegularFile && isSupportedFormat)) {
                QString errorMsg = QString::fromStdString(std::string("ignoring unsupported extension: ") + extension);
                emit message(errorMsg);
                continue;
            }

            myFileCount++;

            auto toPath = myDst / entry.path().lexically_relative(mySrc);
            toPath.replace_extension(myExportFormat.primaryExtension());

            if (myDryRun) continue;

            auto destBaseDir = toPath.parent_path();
            if (!std::filesystem::exists(destBaseDir))
                std::filesystem::create_directories(destBaseDir);

            std::optional<std::string> error =
                convertFile(entry, toPath, myExportFormat, myFileFormatManager);
            if (error != std::nullopt) {
                QString err = QString::fromStdString(error.value());
                emit message("error processing file: " + err);
            }

            emit progress((int) myFileCount);
        }
    } while (!children.empty());

    emit message("DONE!");
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

    for (const FileFormat &format : myFileFormatManager->exportFormats())
    {
        ui->convertToComboBox->addItem(
            QString::fromStdString(format.fileFilter()));
    }

    convertButton()->setEnabled(false);
    convertButton()->setText(tr("Convert"));
    connect(convertButton(), &QAbstractButton::clicked, [=](){ convert(); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
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
    convertButton()->setEnabled(bothEditsHaveValues);
}

void BulkConverterDialog::convert()
{
    convertButton()->setEnabled(false);

    const int format_idx = ui->convertToComboBox->currentIndex();
    FileFormat export_format = myFileFormatManager->exportFormats()[format_idx];

    std::filesystem::path src = Paths::fromQString(ui->sourcePathEdit->text());
    std::filesystem::path dst = Paths::fromQString(ui->destinationPathEdit->text());

    {
        // set default max.
        BulkConverterWorker bcw(src, dst, true, export_format,
                                myFileFormatManager);
        bcw.walkAndConvert();
        const std::size_t fileCountToConvert = bcw.fileCount();
        ui->progressBar->setMaximum((int) fileCountToConvert);
        ui->progressBar->setValue(0);
    }

    // create thread to offload work in the background, such that we can
    // update the progress bar with each step informing the user of the
    // current state of bulk conversion.
    myBulkWorkerThread = new QThread;
    myBulkConverterWorker = new BulkConverterWorker(
        src, dst, false, export_format, myFileFormatManager);
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

    connect(myBulkConverterWorker, &BulkConverterWorker::message,
            this, &BulkConverterDialog::consumeMessage);

    myBulkWorkerThread->start();
}

void BulkConverterDialog::progress(int pos)
{
    ui->progressBar->setValue(pos);
}

void BulkConverterDialog::enableConvertButton(void)
{
    convertButton()->setEnabled(true);
}

void BulkConverterDialog::consumeMessage(QString message)
{
    ui->logger->appendPlainText(message);
}

QPushButton *
BulkConverterDialog::convertButton()
{
    return ui->buttonBox->button(QDialogButtonBox::Apply);
}
