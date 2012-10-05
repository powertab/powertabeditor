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

#include <QSettings>

#include <audio/rtmidiwrapper.h>
#include <app/settings.h>
#include "app/skinmanager.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(ui->useSkinCheckBox, SIGNAL(toggled(bool)),
            ui->skinComboBox, SLOT(setEnabled(bool)));

    // add available MIDI ports
    RtMidiWrapper rtMidiWrapper;
    for (size_t i = 0; i < rtMidiWrapper.getApiCount(); ++i)
    {
        for(uint32_t j = 0; j < rtMidiWrapper.getPortCount(i); j++)
        {
            std::string portName = rtMidiWrapper.getPortName(i, j);
            ui->midiPortComboBox->addItem(QString::fromStdString(portName));
        }
    }

    ui->vibratoStrengthSpinBox->setRange(1, 127);
    ui->wideVibratoStrengthSpinBox->setRange(1, 127);

    ui->skinComboBox->addItems(SkinManager::availableSkins());

    loadCurrentSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

/// Load the current preferences and initialize the widgets with those values
void PreferencesDialog::loadCurrentSettings()
{
    QSettings settings;

    ui->midiPortComboBox->setCurrentIndex(settings.value(Settings::MIDI_PREFERRED_PORT,
                                                         Settings::MIDI_PREFFERED_PORT_DEFAULT).toInt());

    ui->metronomeEnabledCheckBox->setChecked(settings.value(Settings::MIDI_METRONOME_ENABLED,
                                                            Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool());

    ui->vibratoStrengthSpinBox->setValue(settings.value(Settings::MIDI_VIBRATO_LEVEL,
                                                        Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt());

    ui->wideVibratoStrengthSpinBox->setValue(settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                                                            Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt());

    const int skinIndex = ui->skinComboBox->findText(settings.value(Settings::APPEARANCE_SKIN_NAME).toString());
    ui->skinComboBox->setCurrentIndex(skinIndex == -1 ? 0 : skinIndex);

    ui->useSkinCheckBox->setChecked(settings.value(Settings::APPEARANCE_USE_SKIN, false).toBool());
    ui->skinComboBox->setEnabled(ui->useSkinCheckBox->isChecked());
}

/// Save the new settings
void PreferencesDialog::accept()
{
    QSettings settings;
    settings.setValue(Settings::MIDI_PREFERRED_PORT,
                      ui->midiPortComboBox->currentIndex());

    settings.setValue(Settings::MIDI_METRONOME_ENABLED,
                      ui->metronomeEnabledCheckBox->isChecked());

    settings.setValue(Settings::MIDI_VIBRATO_LEVEL,
                      ui->vibratoStrengthSpinBox->value());

    settings.setValue(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                      ui->wideVibratoStrengthSpinBox->value());

    settings.setValue(Settings::APPEARANCE_USE_SKIN,
                      ui->useSkinCheckBox->isChecked());

    settings.setValue(Settings::APPEARANCE_SKIN_NAME,
                      ui->skinComboBox->currentText());

    settings.sync();

    done(Accepted);
}

