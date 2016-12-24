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

#include "fileinformationdialog.h"
#include "ui_fileinformationdialog.h"

#include <app/documentmanager.h>
#include <QFileInfo>

FileInformationDialog::FileInformationDialog(
    QWidget *parent, const Document &doc)
    : QDialog(parent), ui(new Ui::FileInformationDialog)
{
    ui->setupUi(this);

    ui->songTypeButtonGroup->setId(
        ui->songButton, static_cast<int>(ScoreInfo::ScoreType::Song));
    ui->songTypeButtonGroup->setId(
        ui->lessonButton, static_cast<int>(ScoreInfo::ScoreType::Lesson));

    connect(ui->songTypeButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(handleSongTypeButtonClick(QAbstractButton *)));

    connect(ui->releaseTypeList, SIGNAL(currentRowChanged(int)), this,
            SLOT(handleReleaseTypeChanged(int)));

    connect(ui->traditionalSongValue, SIGNAL(toggled(bool)), this,
            SLOT(handleAuthorTypeChanged(bool)));

    ui->lessonLevelButtonGroup->setId(
        ui->beginnerLevelButton,
        static_cast<int>(LessonData::DifficultyLevel::Beginner));
    ui->lessonLevelButtonGroup->setId(
        ui->intermediateLevelButton,
        static_cast<int>(LessonData::DifficultyLevel::Intermediate));
    ui->lessonLevelButtonGroup->setId(
        ui->advancedLevelButton,
        static_cast<int>(LessonData::DifficultyLevel::Advanced));

    const ScoreInfo &info = doc.getScore().getScoreInfo();

    // Select either the song or lesson button.
    QAbstractButton *button =
        ui->songTypeButtonGroup->button(static_cast<int>(info.getScoreType()));
    Q_ASSERT(button != NULL);
    button->click();

    if (info.getScoreType() == ScoreInfo::ScoreType::Song)
    {
        const SongData &song = info.getSongData();
        // Initialize song information.
        ui->songTitleValue->setText(QString::fromStdString(song.getTitle()));
        ui->songArtistValue->setText(
            QString::fromStdString(song.getArtist()));

        if (song.isAudioRelease())
        {
            ui->releaseTypeList->setCurrentRow(0);
            const SongData::AudioReleaseInfo &audioRelease =
                song.getAudioReleaseInfo();

            ui->albumTitleValue->setText(
                QString::fromStdString(audioRelease.getTitle()));
            ui->albumTypeValue->setCurrentIndex(
                static_cast<int>(audioRelease.getReleaseType()));
            ui->albumYearValue->setValue(audioRelease.getYear());
            ui->audioLiveRecordingValue->setChecked(audioRelease.isLive());
        }
        else if (song.isVideoRelease())
        {
            ui->releaseTypeList->setCurrentRow(1);
            const SongData::VideoReleaseInfo &videoRelease =
                song.getVideoReleaseInfo();

            ui->videoTitleValue->setText(
                QString::fromStdString(videoRelease.getTitle()));
            ui->videoLiveRecordingValue->setChecked(videoRelease.isLive());
        }
        else if (song.isBootleg())
        {
            ui->releaseTypeList->setCurrentRow(2);
            const SongData::BootlegInfo &bootleg = song.getBootlegInfo();

            ui->bootlegTitleValue->setText(
                QString::fromStdString(bootleg.getTitle()));

            const boost::gregorian::date date = bootleg.getDate();
            ui->bootlegDateValue->setDate(
                QDate(date.year(), date.month(), date.day()));
        }
        else if (song.isUnreleased())
            ui->releaseTypeList->setCurrentRow(3);

        ui->traditionalSongValue->setChecked(song.isTraditionalAuthor());

        if (!song.isTraditionalAuthor())
        {
            const SongData::AuthorInfo &authorInfo = song.getAuthorInfo();

            ui->composerValue->setText(
                QString::fromStdString(authorInfo.getComposer()));
            ui->lyricistValue->setText(
                QString::fromStdString(authorInfo.getLyricist()));
        }

        ui->arrangerValue->setText(QString::fromStdString(song.getArranger()));
        ui->transcriberValue->setText(
            QString::fromStdString(song.getTranscriber()));
        ui->songCopyrightValue->setText(
            QString::fromStdString(song.getCopyright()));

        // Initialize performance notes.
        ui->notesValue->setPlainText(
            QString::fromStdString(song.getPerformanceNotes()));

        // Initialize lyrics.
        ui->lyricsValue->setPlainText(QString::fromStdString(song.getLyrics()));
    }
    else
    {
        const LessonData &lesson = info.getLessonData();

        ui->lessonTitleValue->setText(
            QString::fromStdString(lesson.getTitle()));
        ui->lessonSubtitleValue->setText(
            QString::fromStdString(lesson.getSubtitle()));

        ui->lessonStyleValue->setCurrentIndex(
            static_cast<int>(lesson.getMusicStyle()));

        QAbstractButton *levelButton = ui->lessonLevelButtonGroup->button(
            static_cast<int>(lesson.getDifficultyLevel()));
        Q_ASSERT(levelButton != NULL);
        levelButton->click();

        ui->lessonAuthorValue->setText(
            QString::fromStdString(lesson.getAuthor()));
        ui->lessonCopyrightValue->setText(
            QString::fromStdString(lesson.getCopyright()));
    }

    if (!doc.hasFilename())
    {
        ui->fileNameValue->setText(tr("Untitled"));
        return;
    }

    // Initialize file properties.
    const QString filePath = QString::fromStdString(doc.getFilename());
    const QFileInfo fileInfo(filePath);
    QLocale locale;

    ui->fileNameValue->setText(fileInfo.fileName());
    ui->fileLocationValue->setText(filePath);
    ui->fileSizeValue->setText(
        tr("%1 bytes").arg(locale.toString(fileInfo.size())));
    ui->fileCreatedValue->setText(locale.toString(fileInfo.created()));
    ui->fileModifiedValue->setText(locale.toString(fileInfo.lastModified()));
    ui->fileAccessedValue->setText(locale.toString(fileInfo.lastRead()));
}

FileInformationDialog::~FileInformationDialog()
{
    delete ui;
}

ScoreInfo FileInformationDialog::getScoreInfo() const
{
    ScoreInfo info;

    if (ui->songTypeStack->currentIndex() == 0) // Song
    {
        SongData song;

        song.setTitle(ui->songTitleValue->text().toStdString());
        song.setArtist(ui->songArtistValue->text().toStdString());

        switch (ui->releaseInfoStack->currentIndex())
        {
            case 0:
                song.setAudioReleaseInfo(SongData::AudioReleaseInfo(
                    static_cast<SongData::AudioReleaseInfo::ReleaseType>(
                        ui->albumTypeValue->currentIndex()),
                    ui->albumTitleValue->text().toStdString(),
                    ui->albumYearValue->value(),
                    ui->audioLiveRecordingValue->isChecked()));
                break;
            case 1:
                song.setVideoReleaseInfo(SongData::VideoReleaseInfo(
                    ui->videoTitleValue->text().toStdString(),
                    ui->videoLiveRecordingValue->isChecked()));
                break;
            case 2:
            {
                const QDate date = ui->bootlegDateValue->date();
                song.setBootlegInfo(SongData::BootlegInfo(
                    ui->bootlegTitleValue->text().toStdString(),
                    boost::gregorian::date(date.year(), date.month(),
                                           date.day())));
                break;
            }
            case 3:
                song.setUnreleased();
                break;
        }

        if (ui->traditionalSongValue->isChecked())
            song.setTraditionalAuthor();
        else
        {
            song.setAuthorInfo(
                SongData::AuthorInfo(ui->composerValue->text().toStdString(),
                                     ui->lyricistValue->text().toStdString()));
        }

        song.setArranger(ui->arrangerValue->text().toStdString());
        song.setTranscriber(ui->transcriberValue->text().toStdString());
        song.setCopyright(ui->songCopyrightValue->text().toStdString());
        song.setPerformanceNotes(ui->notesValue->toPlainText().toStdString());
        song.setLyrics(ui->lyricsValue->toPlainText().toStdString());

        info.setSongData(song);
    }
    else // Lesson
    {
        LessonData lesson;

        lesson.setTitle(ui->lessonTitleValue->text().toStdString());
        lesson.setSubtitle(ui->lessonSubtitleValue->text().toStdString());
        lesson.setMusicStyle(static_cast<LessonData::MusicStyle>(
            ui->lessonStyleValue->currentIndex()));
        lesson.setDifficultyLevel(static_cast<LessonData::DifficultyLevel>(
            ui->lessonLevelButtonGroup->checkedId()));
        lesson.setAuthor(ui->lessonAuthorValue->text().toStdString());
        lesson.setCopyright(ui->lessonCopyrightValue->text().toStdString());

        info.setLessonData(lesson);
    }

    return info;
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
