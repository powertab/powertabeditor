#include "fileinformationdialog.h"
#include "ui_fileinformationdialog.h"

#include <QFileInfo>
#include <QDateTime>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/powertabfileheader.h>

FileInformationDialog::FileInformationDialog(boost::shared_ptr<PowerTabDocument> doc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileInformationDialog)
{
    ui->setupUi(this);

    const QString filePath = QString::fromStdString(doc->GetFileName());
    const QFileInfo fileInfo(filePath);
    QLocale locale;

    ui->fileNameValue->setText(fileInfo.fileName());
    ui->fileTypeValue->setText(tr("Power Tab Version %1").arg(
                                   getFileVersionString(doc->GetHeader())));
    ui->fileLocationValue->setText(filePath);
    ui->fileSizeValue->setText(tr("%1 bytes").arg(
                                   locale.toString(fileInfo.size())));
    ui->fileCreatedValue->setText(locale.toString(fileInfo.created()));
    ui->fileModifiedValue->setText(locale.toString(fileInfo.lastModified()));
    ui->fileAccessedValue->setText(locale.toString(fileInfo.lastRead()));
}

FileInformationDialog::~FileInformationDialog()
{
    delete ui;
}

QString FileInformationDialog::getFileVersionString(const PowerTabFileHeader& header)
{
    QString fileVersion;

    switch (header.GetVersion()) {
    case PowerTabFileHeader::Version_1_0:
        fileVersion = tr("1.0");
        break;
    case PowerTabFileHeader::Version_1_0_2:
        fileVersion = tr("1.0.2");
        break;
    case PowerTabFileHeader::Version_1_5:
        fileVersion = tr("1.5");
        break;
    case PowerTabFileHeader::Version_1_7:
        fileVersion = tr("1.7");
        break;
    case PowerTabFileHeader::Version_2_0:
        fileVersion = tr("2.0");
        break;
    default:
        fileVersion = "??";
        break;
    }

    return fileVersion;
}
