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

#include "playbackwidget.h"
#include "ui_playbackwidget.h"

#include <app/documentmanager.h>
#include <score/staff.h>
#include <score/score.h>
#include <widgets/common.h>

static QString getShortcutHint(const QAction &action)
{
    if (!action.shortcut().isEmpty())
        return QString(" (%1)").arg(action.shortcut().toString());
    else
        return "";
}

PlaybackWidget::PlaybackWidget(const QAction &playPauseCommand,
                               const QAction &rewindCommand,
                               const QAction &metronomeCommand, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlaybackWidget),
      myVoices(new QButtonGroup(this))
{
    ui->setupUi(this);

    myVoices->addButton(ui->voice1Button, 0);
    myVoices->addButton(ui->voice2Button, 1);
    Q_ASSERT(myVoices->buttons().length() == Staff::NUM_VOICES);
    ui->voice1Button->setChecked(true);

    ui->speedSpinner->setMinimum(50);
    ui->speedSpinner->setMaximum(125);
    ui->speedSpinner->setSuffix("%");
    ui->speedSpinner->setValue(100);

    ui->rewindToStartButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(&rewindCommand, &QAction::changed, [&]() {
        ui->rewindToStartButton->setToolTip(
            tr("Click to move playback to the beginning of the score%1.")
                .arg(getShortcutHint(rewindCommand)));
    });

    ui->playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(&playPauseCommand, &QAction::changed, [&]() {
        ui->playPauseButton->setToolTip(
            tr("Click to start or stop playback%1.")
                .arg(getShortcutHint(playPauseCommand)));
    });

    ui->metronomeToggleButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaVolume));
    connect(&metronomeCommand, &QAction::changed, [&]() {
        ui->metronomeToggleButton->setToolTip(
            tr("Click to toggle whether the metronome is turned on%1.")
                .arg(getShortcutHint(metronomeCommand)));
    });

    connect(myVoices, SIGNAL(buttonClicked(int)), this,
            SIGNAL(activeVoiceChanged(int)));
    connect(ui->speedSpinner, SIGNAL(valueChanged(int)), this,
            SIGNAL(playbackSpeedChanged(int)));
    connect(ui->filterComboBox, SIGNAL(currentIndexChanged(int)), this,
            SIGNAL(activeFilterChanged(int)));
    connectButtonToAction(ui->playPauseButton, &playPauseCommand);
    connectButtonToAction(ui->metronomeToggleButton, &metronomeCommand);
    connectButtonToAction(ui->rewindToStartButton, &rewindCommand);
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
}

void PlaybackWidget::reset(const Document &doc)
{
    ui->filterComboBox->blockSignals(true);

    // Rebuild the filter list.
    ui->filterComboBox->clear();
    for (const ViewFilter &filter : doc.getScore().getViewFilters())
    {
        ui->filterComboBox->addItem(
            QString::fromStdString(filter.getDescription()));
    }

    // Update the selected filter.
    if (doc.getViewOptions().getFilter())
        ui->filterComboBox->setCurrentIndex(*doc.getViewOptions().getFilter());

    // Update the selected voice.
    myVoices->button(doc.getCaret().getLocation().getVoiceIndex())->setChecked(true);

    ui->filterComboBox->blockSignals(false);
}

int PlaybackWidget::getPlaybackSpeed() const
{
    return ui->speedSpinner->value();
}

void PlaybackWidget::setPlaybackMode(bool isPlaying)
{
    if (isPlaying)
    {
        ui->playPauseButton->setIcon(
            style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        ui->playPauseButton->setIcon(
            style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void PlaybackWidget::updateLocationLabel(const std::string &location)
{
    ui->locationLabel->setText(QString::fromStdString(location));
}
