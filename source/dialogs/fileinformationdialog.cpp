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

    ui->songTypeButtonGroup->setId(ui->songButton, PowerTabFileHeader::FILETYPE_SONG);
    ui->songTypeButtonGroup->setId(ui->lessonButton, PowerTabFileHeader::FILETYPE_LESSON);
    connect(ui->songTypeButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(handleSongTypeButtonClick(QAbstractButton*)));

    connect(ui->releaseTypeList, SIGNAL(currentRowChanged(int)),
            this, SLOT(handleReleaseTypeChanged(int)));

    connect(ui->traditionalSongValue, SIGNAL(toggled(bool)),
            this, SLOT(handleAuthorTypeChanged(bool)));

    ui->lessonLevelButtonGroup->setId(ui->beginnerLevelButton,
                                      PowerTabFileHeader::LESSONLEVEL_BEGINNER);
    ui->lessonLevelButtonGroup->setId(ui->intermediateLevelButton,
                                      PowerTabFileHeader::LESSONLEVEL_INTERMEDIATE);
    ui->lessonLevelButtonGroup->setId(ui->advancedLevelButton,
                                      PowerTabFileHeader::LESSONLEVEL_ADVANCED);

    const PowerTabFileHeader& header = doc->GetHeader();

    // Select either the song or lesson button.
    QAbstractButton *button = ui->songTypeButtonGroup->button(header.GetFileType());
    Q_ASSERT(button != NULL);
    button->click();

    // Initialize song information.
    ui->songTitleValue->setText(QString::fromStdString(header.GetSongTitle()));
    ui->songArtistValue->setText(QString::fromStdString(header.GetSongArtist()));

    ui->releaseTypeList->setCurrentRow(header.GetSongReleaseType());

    ui->albumTitleValue->setText(QString::fromStdString(header.GetSongAudioReleaseTitle()));
    ui->albumTypeValue->setCurrentIndex(header.GetSongAudioReleaseType());
    ui->albumYearValue->setValue(header.GetSongAudioReleaseYear());
    ui->audioLiveRecordingValue->setChecked(header.IsSongAudioReleaseLive());

    ui->videoTitleValue->setText(QString::fromStdString(header.GetSongVideoReleaseTitle()));
    ui->videoLiveRecordingValue->setChecked(header.IsSongVideoReleaseLive());

    ui->bootlegTitleValue->setText(QString::fromStdString(header.GetSongBootlegTitle()));

    const boost::gregorian::date bootlegDate = header.GetSongBootlegDate();
    ui->bootlegDateValue->setDate(QDate(bootlegDate.year(), bootlegDate.month(), bootlegDate.day()));

    ui->traditionalSongValue->setChecked(header.GetSongAuthorType() == PowerTabFileHeader::AUTHORTYPE_TRADITIONAL);
    ui->composerValue->setText(QString::fromStdString(header.GetSongComposer()));
    ui->lyricistValue->setText(QString::fromStdString(header.GetSongLyricist()));
    ui->arrangerValue->setText(QString::fromStdString(header.GetSongArranger()));
    ui->guitarTranscriberValue->setText(QString::fromStdString(header.GetSongGuitarScoreTranscriber()));
    ui->bassTranscriberValue->setText(QString::fromStdString(header.GetSongBassScoreTranscriber()));
    ui->songCopyrightValue->setText(QString::fromStdString(header.GetSongCopyright()));

    ui->lessonTitleValue->setText(QString::fromStdString(header.GetLessonTitle()));
    ui->lessonSubtitleValue->setText(QString::fromStdString(header.GetLessonSubtitle()));

    ui->lessonStyleValue->setCurrentIndex(header.GetLessonMusicStyle());

    QAbstractButton *levelButton = ui->lessonLevelButtonGroup->button(header.GetLessonLevel());
    Q_ASSERT(levelButton != NULL);
    levelButton->click();

    ui->lessonAuthorValue->setText(QString::fromStdString(header.GetLessonAuthor()));
    ui->lessonCopyrightValue->setText(QString::fromStdString(header.GetLessonCopyright()));

    // Initialize performance notes.
    ui->guitarNotesValue->setPlainText(QString::fromStdString(header.GetSongGuitarScoreNotes()));
    ui->bassNotesValue->setPlainText(QString::fromStdString(header.GetSongBassScoreNotes()));

    // Initialize file properties.
    const QString filePath = QString::fromStdString(doc->GetFileName());
    const QFileInfo fileInfo(filePath);
    QLocale locale;

    ui->fileNameValue->setText(fileInfo.fileName());
    ui->fileTypeValue->setText(tr("Power Tab Version %1").arg(
                                   getFileVersionString(header)));
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

void FileInformationDialog::handleSongTypeButtonClick(QAbstractButton *button)
{
    const int id = ui->songTypeButtonGroup->id(button);
    Q_ASSERT(id != -1);
    ui->songTypeStack->setCurrentIndex(id);
}

void FileInformationDialog::handleReleaseTypeChanged(int index)
{
    ui->releaseInfoStack->setCurrentIndex(index);
}

void FileInformationDialog::handleAuthorTypeChanged(bool traditional)
{
    ui->composerValue->setEnabled(!traditional);
    ui->lyricistValue->setEnabled(!traditional);
}
