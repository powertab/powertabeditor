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

#include <QSettings>
#include <app/settings.h>
#include <app/pubsub/settingspubsub.h>

PlaybackWidget::PlaybackWidget(boost::shared_ptr<SettingsPubSub> pubsub,
                               QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PlaybackWidget), pubsub(pubsub)
{
    ui->setupUi(this);

    ui->speedSpinner->setMinimum(50);
    ui->speedSpinner->setMaximum(125);
    ui->speedSpinner->setSuffix("%");
    ui->speedSpinner->setValue(100);

    ui->rewindToStartButton->setIcon(style()->standardIcon(
                                         QStyle::SP_MediaSkipBackward));
    ui->playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->metronomeToggleButton->setIcon(style()->standardIcon(
                                           QStyle::SP_MediaVolumeMuted));

    updateMetronomeButton();

    connect(ui->speedSpinner, SIGNAL(valueChanged(int)),
            this, SIGNAL(playbackSpeedChanged(int)));
    connect(ui->playPauseButton, SIGNAL(clicked()), this,
            SIGNAL(playbackButtonToggled()));
    connect(ui->metronomeToggleButton, SIGNAL(toggled(bool)),
            this, SLOT(onMetronomeButtonToggled(bool)));
    connect(ui->rewindToStartButton, SIGNAL(clicked()),
            this, SIGNAL(rewindToStartClicked()));
    connect(ui->scoreSelector, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(scoreSelected(int)));

    connection = pubsub->subscribe(
                boost::bind(&PlaybackWidget::onSettingChanged, this, _1));
}

PlaybackWidget::~PlaybackWidget()
{
    delete ui;
    connection.disconnect();
}

int PlaybackWidget::playbackSpeed() const
{
    return ui->speedSpinner->value();
}

/// Toggles the play/pause button.
void PlaybackWidget::setPlaybackMode(bool isPlaying)
{
    if (isPlaying)
    {
        ui->playPauseButton->setIcon(style()->standardIcon(
                                         QStyle::SP_MediaPause));
    }
    else
    {
        ui->playPauseButton->setIcon(style()->standardIcon(
                                         QStyle::SP_MediaPlay));
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
    pubsub->publish(Settings::MIDI_METRONOME_ENABLED);
}

/// Listens for changes to the metronome setting.
void PlaybackWidget::onSettingChanged(const std::string &setting)
{
    if (setting == Settings::MIDI_METRONOME_ENABLED)
    {
        updateMetronomeButton();
    }
}

void PlaybackWidget::onDocumentUpdated(QStringList& scores)
{
    ui->scoreSelector->clear();
    ui->scoreSelector->addItems(scores);
}

void PlaybackWidget::updateLocationLabel(const QString &location)
{
    ui->locationLabel->setText(location);
}
