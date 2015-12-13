/*
  * Copyright (C) 2011 Cameron White
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
  
#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <app/pubsub/settingspubsub.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <audio/midioutputdevice.h>
#include <boost/lexical_cast.hpp>
#include <dialogs/tuningdialog.h>
#include <QSettings>
#include <score/generalmidi.h>

typedef std::pair<int, int> MidiApiAndPort;
Q_DECLARE_METATYPE(MidiApiAndPort)

PreferencesDialog::PreferencesDialog(
    QWidget *parent, SettingsManager &settings_manager,
    std::shared_ptr<SettingsPubSub> settingsPubsub,
    const TuningDictionary &dictionary)
    : QDialog(parent),
      ui(new Ui::PreferencesDialog),
      mySettingsManager(settings_manager),
      mySettingsPubsub(settingsPubsub),
      myDictionary(dictionary)
{
    ui->setupUi(this);

    // Add available MIDI ports.
    MidiOutputDevice device;
    for (size_t i = 0; i < device.getApiCount(); ++i)
    {
        for(unsigned int j = 0; j < device.getPortCount(i); ++j)
        {
            std::string portName = device.getPortName(i, j);
            ui->midiPortComboBox->addItem(
                QString::fromStdString(portName),
                QVariant::fromValue(
                    std::make_pair(static_cast<int>(i), static_cast<int>(j))));
        }
    }

    for (const std::string &name : Midi::getPresetNames())
        ui->defaultPresetComboBox->addItem(QString::fromStdString(name));

    for (const std::string &name : Midi::getPercussionPresetNames())
    {
        ui->metronomePresetComboBox->addItem(QString::fromStdString(name));
        ui->countInPresetComboBox->addItem(QString::fromStdString(name));
    }

    ui->vibratoStrengthSpinBox->setRange(1, 127);
    ui->wideVibratoStrengthSpinBox->setRange(1, 127);

    ui->strongAccentVolumeSpinBox->setRange(0, 127);
    ui->weakAccentVolumeSpinBox->setRange(0, 127);

    ui->countInVolumeSpinBox->setRange(0, 127);

    loadCurrentSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::loadCurrentSettings()
{
    QSettings settings;
    // TODO - fully replace QSettings
    auto new_settings = mySettingsManager.getReadHandle();

    const unsigned int api =
        settings.value(Settings::MIDI_PREFERRED_API,
                       Settings::MIDI_PREFERRED_API_DEFAULT).toUInt();
    const unsigned int port =
        settings.value(Settings::MIDI_PREFERRED_PORT,
                       Settings::MIDI_PREFERRED_PORT_DEFAULT).toUInt();

    // Find the preferred midi port in the combo box.
    MidiOutputDevice device;
    if (api < device.getApiCount() && port < device.getPortCount(api))
    {
        ui->midiPortComboBox->setCurrentIndex(ui->midiPortComboBox->findText(
            QString::fromStdString(device.getPortName(api, port))));
    }

    ui->vibratoStrengthSpinBox->setValue(
        settings.value(Settings::MIDI_VIBRATO_LEVEL,
                       Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt());

    ui->wideVibratoStrengthSpinBox->setValue(
        settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                       Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt());

    ui->metronomeEnabledCheckBox->setChecked(
        settings.value(Settings::MIDI_METRONOME_ENABLED,
                       Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool());

    ui->metronomePresetComboBox->setCurrentIndex(
        settings.value(Settings::MIDI_METRONOME_PRESET,
                       Settings::MIDI_METRONOME_PRESET_DEFAULT).toInt());

    ui->strongAccentVolumeSpinBox->setValue(
        settings.value(Settings::MIDI_METRONOME_STRONG_ACCENT,
                       Settings::MIDI_METRONOME_STRONG_ACCENT_DEFAULT).toInt());

    ui->weakAccentVolumeSpinBox->setValue(
        settings.value(Settings::MIDI_METRONOME_WEAK_ACCENT,
                       Settings::MIDI_METRONOME_WEAK_ACCENT_DEFAULT).toInt());

    ui->countInEnabledCheckBox->setChecked(
        settings.value(Settings::MIDI_METRONOME_ENABLE_COUNTIN,
                       Settings::MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT).toBool());

    ui->countInPresetComboBox->setCurrentIndex(
        settings.value(Settings::MIDI_METRONOME_COUNTIN_PRESET,
                       Settings::MIDI_METRONOME_COUNTIN_PRESET_DEFAULT).toInt());

    ui->countInVolumeSpinBox->setValue(
        settings.value(Settings::MIDI_METRONOME_COUNTIN_VOLUME,
                       Settings::MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT).toInt());

    ui->openInNewWindowCheckBox->setChecked(
        new_settings->get(Settings::OpenFilesInNewWindow));

    ui->defaultInstrumentNameLineEdit->setText(QString::fromStdString(
        new_settings->get(Settings::DefaultInstrumentName)));
    ui->defaultPresetComboBox->setCurrentIndex(
        new_settings->get(Settings::DefaultInstrumentPreset));

    ui->defaultTuningClickButton->setToolTip(tr("Click to adjust tuning."));
    myDefaultTuning = new_settings->get(Settings::DefaultTuning);
    ui->defaultTuningClickButton->setText(QString::fromStdString(
            boost::lexical_cast<std::string>(myDefaultTuning)));
    connect(ui->defaultTuningClickButton, SIGNAL(clicked()), this, SLOT(editTuning()));
}

/// Save the new settings
void PreferencesDialog::accept()
{
    QSettings settings;
    // TODO - fully remove QSettings.
    auto new_settings = mySettingsManager.getWriteHandle();

    MidiApiAndPort apiAndPort = ui->midiPortComboBox->itemData(
                ui->midiPortComboBox->currentIndex()).value<MidiApiAndPort>();
    settings.setValue(Settings::MIDI_PREFERRED_API, apiAndPort.first);
    settings.setValue(Settings::MIDI_PREFERRED_PORT, apiAndPort.second);

    settings.setValue(Settings::MIDI_VIBRATO_LEVEL,
                      ui->vibratoStrengthSpinBox->value());

    settings.setValue(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                      ui->wideVibratoStrengthSpinBox->value());

    settings.setValue(Settings::MIDI_METRONOME_ENABLED,
                      ui->metronomeEnabledCheckBox->isChecked());

    settings.setValue(Settings::MIDI_METRONOME_PRESET,
                      ui->metronomePresetComboBox->currentIndex());

    settings.setValue(Settings::MIDI_METRONOME_STRONG_ACCENT,
                      ui->strongAccentVolumeSpinBox->value());

    settings.setValue(Settings::MIDI_METRONOME_WEAK_ACCENT,
                      ui->weakAccentVolumeSpinBox->value());

    settings.setValue(Settings::MIDI_METRONOME_ENABLE_COUNTIN,
                      ui->countInEnabledCheckBox->isChecked());

    settings.setValue(Settings::MIDI_METRONOME_COUNTIN_PRESET,
                      ui->countInPresetComboBox->currentIndex());

    settings.setValue(Settings::MIDI_METRONOME_COUNTIN_VOLUME,
                      ui->countInVolumeSpinBox->value());

    new_settings->set(Settings::OpenFilesInNewWindow,
                      ui->openInNewWindowCheckBox->isChecked());

    new_settings->set(Settings::DefaultInstrumentName,
                      ui->defaultInstrumentNameLineEdit->text().toStdString());

    new_settings->set(Settings::DefaultInstrumentPreset,
                      ui->defaultPresetComboBox->currentIndex());

    new_settings->set(Settings::DefaultTuning, myDefaultTuning);

    settings.sync();

    mySettingsPubsub->publish(Settings::MIDI_METRONOME_ENABLED);

    done(Accepted);
}

void PreferencesDialog::editTuning()
{
    TuningDialog dialog(this, myDefaultTuning, myDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        myDefaultTuning = dialog.getTuning();
        ui->defaultTuningClickButton->setText(QString::fromStdString(
                boost::lexical_cast<std::string>(myDefaultTuning)));
    }
}
