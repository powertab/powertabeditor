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

#include <app/settings.h>
#include <app/settingsmanager.h>
#include <audio/midioutputdevice.h>
#include <audio/settings.h>
#include <dialogs/tuningdialog.h>
#include <score/generalmidi.h>
#include <util/tostring.h>

typedef std::pair<int, int> MidiApiAndPort;
Q_DECLARE_METATYPE(MidiApiAndPort)

PreferencesDialog::PreferencesDialog(QWidget *parent,
                                     SettingsManager &settings_manager,
                                     const TuningDictionary &dictionary)
    : QDialog(parent),
      ui(new Ui::PreferencesDialog),
      mySettingsManager(settings_manager),
      myDictionary(dictionary)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

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

    ui->systemSpacingSpinBox->setRange(0, 100);

    loadCurrentSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::loadCurrentSettings()
{
    auto settings = mySettingsManager.getReadHandle();

    const unsigned int api = settings->get(Settings::MidiApi);
    const unsigned int port = settings->get(Settings::MidiPort);

    // Find the preferred midi port in the combo box.
    MidiOutputDevice device;
    if (api < device.getApiCount() && port < device.getPortCount(api))
    {
        ui->midiPortComboBox->setCurrentIndex(ui->midiPortComboBox->findText(
            QString::fromStdString(device.getPortName(api, port))));
    }

    ui->vibratoStrengthSpinBox->setValue(
        settings->get(Settings::MidiVibratoLevel));

    ui->wideVibratoStrengthSpinBox->setValue(
        settings->get(Settings::MidiWideVibratoLevel));

    ui->playNotesWhileEditingCheckBox->setChecked(
        settings->get(Settings::PlayNotesWhileEditing));

    ui->metronomeEnabledCheckBox->setChecked(
        settings->get(Settings::MetronomeEnabled));

    ui->metronomePresetComboBox->setCurrentIndex(
        settings->get(Settings::MetronomePreset));

    ui->strongAccentVolumeSpinBox->setValue(
        settings->get(Settings::MetronomeStrongAccent));

    ui->weakAccentVolumeSpinBox->setValue(
        settings->get(Settings::MetronomeWeakAccent));

    ui->countInEnabledCheckBox->setChecked(
        settings->get(Settings::CountInEnabled));

    ui->countInPresetComboBox->setCurrentIndex(
        settings->get(Settings::CountInPreset));

    ui->countInVolumeSpinBox->setValue(settings->get(Settings::CountInVolume));

    ui->scoreThemeComboBox->setCurrentIndex(
        static_cast<int>(settings->get(Settings::Theme)));

    ui->openInNewWindowCheckBox->setChecked(
        settings->get(Settings::OpenFilesInNewWindow));

    ui->systemSpacingSpinBox->setValue(
      settings->get(Settings::SystemSpacing));

    ui->defaultInstrumentNameLineEdit->setText(
        QString::fromStdString(settings->get(Settings::DefaultInstrumentName)));
    ui->defaultPresetComboBox->setCurrentIndex(
        settings->get(Settings::DefaultInstrumentPreset));

    ui->defaultTuningClickButton->setToolTip(tr("Click to adjust tuning."));
    myDefaultTuning = settings->get(Settings::DefaultTuning);
    ui->defaultTuningClickButton->setText(
        QString::fromStdString(Util::toString(myDefaultTuning)));
    connect(ui->defaultTuningClickButton, &ClickableLabel::clicked, this,
            &PreferencesDialog::editTuning);
}

/// Save the new settings
void PreferencesDialog::accept()
{
    auto settings = mySettingsManager.getWriteHandle();

    MidiApiAndPort apiAndPort = ui->midiPortComboBox->itemData(
                ui->midiPortComboBox->currentIndex()).value<MidiApiAndPort>();
    settings->set(Settings::MidiApi, apiAndPort.first);
    settings->set(Settings::MidiPort, apiAndPort.second);

    settings->set(Settings::MidiVibratoLevel,
                  ui->vibratoStrengthSpinBox->value());

    settings->set(Settings::MidiWideVibratoLevel,
                  ui->wideVibratoStrengthSpinBox->value());

    settings->set(Settings::PlayNotesWhileEditing,
                  ui->playNotesWhileEditingCheckBox->isChecked());

    settings->set(Settings::MetronomeEnabled,
                  ui->metronomeEnabledCheckBox->isChecked());

    settings->set(Settings::MetronomePreset,
                  ui->metronomePresetComboBox->currentIndex());

    settings->set(Settings::MetronomeStrongAccent,
                  ui->strongAccentVolumeSpinBox->value());

    settings->set(Settings::MetronomeWeakAccent,
                  ui->weakAccentVolumeSpinBox->value());

    settings->set(Settings::CountInEnabled,
                  ui->countInEnabledCheckBox->isChecked());

    settings->set(Settings::CountInPreset,
                  ui->countInPresetComboBox->currentIndex());

    settings->set(Settings::CountInVolume, ui->countInVolumeSpinBox->value());

    settings->set(Settings::Theme, static_cast<ScoreTheme>(
                                       ui->scoreThemeComboBox->currentIndex()));

    settings->set(Settings::OpenFilesInNewWindow,
                  ui->openInNewWindowCheckBox->isChecked());

    settings->set(Settings::SystemSpacing,
                  ui->systemSpacingSpinBox->value());

    settings->set(Settings::DefaultInstrumentName,
                  ui->defaultInstrumentNameLineEdit->text().toStdString());

    settings->set(Settings::DefaultInstrumentPreset,
                  ui->defaultPresetComboBox->currentIndex());

    settings->set(Settings::DefaultTuning, myDefaultTuning);

    done(Accepted);
}

void PreferencesDialog::editTuning()
{
    TuningDialog dialog(this, myDefaultTuning, myDictionary);

    if (dialog.exec() == QDialog::Accepted)
    {
        myDefaultTuning = dialog.getTuning();
        ui->defaultTuningClickButton->setText(
            QString::fromStdString(Util::toString(myDefaultTuning)));
    }
}
