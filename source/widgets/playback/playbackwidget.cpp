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

#include <app/pubsub/settingspubsub.h>
#include <app/settings.h>
#include <QSettings>
#include <score/staff.h>

PlaybackWidget::PlaybackWidget(std::shared_ptr<SettingsPubSub> pubsub,
                               QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlaybackWidget),
      myVoices(new QButtonGroup(this)),
      myPubsub(pubsub)
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
    ui->playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->metronomeToggleButton->setIcon(
        style()->standardIcon(QStyle::SP_MediaVolumeMuted));

    updateMetronomeButton();

    connect(myVoices, SIGNAL(buttonClicked(int)), this,
            SIGNAL(activeVoiceChanged(int)));
    connect(ui->speedSpinner, SIGNAL(valueChanged(int)), this,
            SIGNAL(playbackSpeedChanged(int)));
    connect(ui->playPauseButton, SIGNAL(clicked()), this,
            SIGNAL(playbackButtonToggled()));
    connect(ui->metronomeToggleButton, SIGNAL(toggled(bool)), this,
            SLOT(onMetronomeButtonToggled(bool)));
    connect(ui->rewindToStartButton, SIGNAL(clicked()), this,
            SIGNAL(rewindToStartClicked()));

    myConnection = myPubsub->subscribe(
        boost::bind(&PlaybackWidget::onSettingChanged, this, _1));
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
    myConnection.disconnect();
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

void PlaybackWidget::updateMetronomeButton()
{
    QSettings settings;
    ui->metronomeToggleButton->setChecked(
        settings.value(Settings::MIDI_METRONOME_ENABLED,
                       Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool());
}

/// When the metronome button is toggled, update the setting.
void PlaybackWidget::onMetronomeButtonToggled(bool enable)
{
    QSettings settings;
    settings.setValue(Settings::MIDI_METRONOME_ENABLED, enable);
    myPubsub->publish(Settings::MIDI_METRONOME_ENABLED);
}

/// Listens for changes to the metronome setting.
void PlaybackWidget::onSettingChanged(const std::string &setting)
{
    if (setting == Settings::MIDI_METRONOME_ENABLED)
        updateMetronomeButton();
}

void PlaybackWidget::updateLocationLabel(const std::string &location)
{
    ui->locationLabel->setText(QString::fromStdString(location));
}
