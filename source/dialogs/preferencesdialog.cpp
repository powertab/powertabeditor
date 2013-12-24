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
#include <audio/midioutputdevice.h>
#include <boost/lexical_cast.hpp>
#include <dialogs/tuningdialog.h>
#include <QSettings>
#include <score/generalmidi.h>

typedef std::pair<int, int> MidiApiAndPort;
Q_DECLARE_METATYPE(MidiApiAndPort)

PreferencesDialog::PreferencesDialog(
    QWidget *parent, std::shared_ptr<SettingsPubSub> settingsPubsub,
    const TuningDictionary &dictionary)
    : QDialog(parent),
      ui(new Ui::PreferencesDialog),
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
    {
        ui->metronomePresetComboBox->addItem(QString::fromStdString(name));
        ui->defaultPresetComboBox->addItem(QString::fromStdString(name));
    }

    ui->vibratoStrengthSpinBox->setRange(1, 127);
    ui->wideVibratoStrengthSpinBox->setRange(1, 127);

    loadCurrentSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::loadCurrentSettings()
{
    QSettings settings;

    const int api = settings.value(Settings::MIDI_PREFERRED_API,
                              Settings::MIDI_PREFERRED_API_DEFAULT).toInt();
    const int port = settings.value(Settings::MIDI_PREFERRED_PORT,
                              Settings::MIDI_PREFERRED_PORT_DEFAULT).toInt();

    // Find the preferred midi port in the combo box.
    MidiOutputDevice device;
    ui->midiPortComboBox->setCurrentIndex(ui->midiPortComboBox->findText(
                QString::fromStdString(device.getPortName(api, port))));

    ui->metronomeEnabledCheckBox->setChecked(settings.value(Settings::MIDI_METRONOME_ENABLED,
                                                            Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool());

    ui->metronomePresetComboBox->setCurrentIndex(settings.value(
            Settings::MIDI_METRONOME_PRESET,
            Settings::MIDI_METRONOME_PRESET_DEFAULT).toInt());

    ui->vibratoStrengthSpinBox->setValue(settings.value(Settings::MIDI_VIBRATO_LEVEL,
                                                        Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt());

    ui->wideVibratoStrengthSpinBox->setValue(settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                                                            Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt());

    ui->defaultInstrumentNameLineEdit->setText(settings.value(
            Settings::DEFAULT_INSTRUMENT_NAME,
            Settings::DEFAULT_INSTRUMENT_NAME_DEFAULT).toString());
    ui->defaultPresetComboBox->setCurrentIndex(settings.value(
            Settings::DEFAULT_INSTRUMENT_PRESET,
            Settings::DEFAULT_INSTRUMENT_PRESET_DEFAULT).toInt());

    ui->defaultTuningClickButton->setToolTip(tr("Click to adjust tuning."));
    myDefaultTuning = settings.value(
            Settings::DEFAULT_INSTRUMENT_TUNING,
            QVariant::fromValue(Settings::DEFAULT_INSTRUMENT_TUNING_DEFAULT)).value<Tuning>();
    ui->defaultTuningClickButton->setText(QString::fromStdString(
            boost::lexical_cast<std::string>(myDefaultTuning)));
    connect(ui->defaultTuningClickButton, SIGNAL(clicked()), this, SLOT(editTuning()));
}

/// Save the new settings
void PreferencesDialog::accept()
{
    QSettings settings;

    MidiApiAndPort apiAndPort = ui->midiPortComboBox->itemData(
                ui->midiPortComboBox->currentIndex()).value<MidiApiAndPort>();
    settings.setValue(Settings::MIDI_PREFERRED_API, apiAndPort.first);
    settings.setValue(Settings::MIDI_PREFERRED_PORT, apiAndPort.second);

    settings.setValue(Settings::MIDI_METRONOME_ENABLED,
                      ui->metronomeEnabledCheckBox->isChecked());

    settings.setValue(Settings::MIDI_METRONOME_PRESET,
                      ui->metronomePresetComboBox->currentIndex());

    settings.setValue(Settings::MIDI_VIBRATO_LEVEL,
                      ui->vibratoStrengthSpinBox->value());

    settings.setValue(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                      ui->wideVibratoStrengthSpinBox->value());

    settings.setValue(Settings::DEFAULT_INSTRUMENT_NAME,
                      ui->defaultInstrumentNameLineEdit->text());

    settings.setValue(Settings::DEFAULT_INSTRUMENT_PRESET,
                      ui->defaultPresetComboBox->currentIndex());

    settings.setValue(Settings::DEFAULT_INSTRUMENT_TUNING,
                      QVariant::fromValue(myDefaultTuning));

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
