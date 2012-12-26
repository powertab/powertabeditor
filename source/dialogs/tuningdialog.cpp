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
  
#include "tuningdialog.h"
#include "ui_tuningdialog.h"

#include <app/tuningdictionary.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/tuning.h>

#include <algorithm>
#include <boost/bind.hpp>

Q_DECLARE_METATYPE(boost::shared_ptr<Tuning>);

/// If no guitar is provided, the guitar-specific options such as the capo will
/// be disabled.
TuningDialog::TuningDialog(QWidget *parent,
                           boost::shared_ptr<const Guitar> guitar,
                           const Tuning& currentTuning,
                           boost::shared_ptr<TuningDictionary> tuningDictionary) :
    QDialog(parent),
    ui(new Ui::TuningDialog),
    tuningDictionary(tuningDictionary)
{
    ui->setupUi(this);

    ui->tuningNameEdit->setText(QString::fromStdString(currentTuning.GetName()));

    ui->sharpsCheckBox->setChecked(currentTuning.UsesSharps());
    connect(ui->sharpsCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleSharps(bool)));

    if (guitar)
    {
        ui->capoSpinBox->setMinimum(Guitar::MIN_CAPO);
        ui->capoSpinBox->setMaximum(Guitar::MAX_CAPO);
        ui->capoSpinBox->setValue(guitar->GetCapo());
    }
    else
    {
        ui->capoLabel->hide();
        ui->capoSpinBox->hide();
    }
    
    ui->notationOffsetSpinBox->setMinimum(Tuning::MIN_MUSIC_NOTATION_OFFSET);
    ui->notationOffsetSpinBox->setMaximum(Tuning::MAX_MUSIC_NOTATION_OFFSET);
    ui->notationOffsetSpinBox->setValue(currentTuning.GetMusicNotationOffset());

    ui->numStringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->numStringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);
    ui->numStringsSpinBox->setValue(currentTuning.GetStringCount());
    connect(ui->numStringsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateEnabledStrings(int)));
    connect(ui->numStringsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTuningDictionary(int)));

    connect(ui->presetComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(loadPreset()));

    stringSelectors.push_back(ui->string1);
    stringSelectors.push_back(ui->string2);
    stringSelectors.push_back(ui->string3);
    stringSelectors.push_back(ui->string4);
    stringSelectors.push_back(ui->string5);
    stringSelectors.push_back(ui->string6);
    stringSelectors.push_back(ui->string7);
    stringSelectors.push_back(ui->string8);

    updateTuningDictionary(currentTuning.GetStringCount());
    generateNoteNames(currentTuning.UsesSharps());
    initStringSelectors(currentTuning);
    updateEnabledStrings(currentTuning.GetStringCount());
}

TuningDialog::~TuningDialog()
{
    delete ui;
}

void TuningDialog::initStringSelectors(const Tuning& currentTuning)
{
    for (size_t i = 0; i < stringSelectors.size(); i++)
    {
        QComboBox* selector = stringSelectors[i];

        selector->addItems(noteNames);
        
        if (currentTuning.IsValidString(i))
        {
            selector->setCurrentIndex(currentTuning.GetNote(i, false));
        }
    }
}

/// Generates a list of note names, to be used for selecting the pitch of a string
void TuningDialog::generateNoteNames(bool usesSharps)
{
    noteNames.clear();
    
    for (uint8_t note = midi::MIN_MIDI_NOTE; note < midi::MAX_MIDI_NOTE; note++)
    {
        noteNames << QString::fromStdString(
                         midi::GetMidiNoteTextSimple(note,usesSharps)) +
                     QString().setNum(midi::GetMidiNoteOctave(note));
    }
}

/// Switches the note names available for each string
void TuningDialog::toggleSharps(bool usesSharps)
{
    generateNoteNames(usesSharps);
    
    for (uint8_t i = 0; i < Tuning::MAX_STRING_COUNT; i++)
    {
        QComboBox* selector = stringSelectors.at(i);
        const int selectedIndex = selector->currentIndex();
        selector->clear();
        selector->addItems(noteNames);
        selector->setCurrentIndex(selectedIndex);
    }
}

/// Updates which string selection boxes are enabled, based on the number of strings selected
void TuningDialog::updateEnabledStrings(int numStrings)
{
    Q_ASSERT(numStrings <= (int)Tuning::MAX_STRING_COUNT && numStrings >= 0);
    
    std::for_each(stringSelectors.begin(), stringSelectors.begin() + numStrings,
                  boost::bind(&QComboBox::setEnabled, _1, true));
    
    std::for_each(stringSelectors.begin() + numStrings, stringSelectors.end(),
                  boost::bind(&QComboBox::setEnabled, _1, false));
}

/// Load all tuning presets for the specified number of strings.
void TuningDialog::updateTuningDictionary(int numStrings)
{
    std::vector<boost::shared_ptr<Tuning> > tunings;
    tuningDictionary->findTunings(tunings, numStrings);

    ui->presetComboBox->clear();

    for (size_t i = 0; i < tunings.size(); ++i)
    {
        boost::shared_ptr<Tuning> tuning = tunings[i];
        ui->presetComboBox->addItem(QString("%1 - %2").arg(
                    QString::fromStdString(tuning->GetName()),
                    QString::fromStdString(tuning->GetSpelling())),
                QVariant::fromValue(tuning));
    }
}

/// Load the currently-selected tuning preset.
void TuningDialog::loadPreset()
{
    if (ui->presetComboBox->currentIndex() < 0)
        return;

    boost::shared_ptr<Tuning> tuning = ui->presetComboBox->itemData(
                ui->presetComboBox->currentIndex()).value<boost::shared_ptr<Tuning> >();
    ui->sharpsCheckBox->setChecked(tuning->UsesSharps());
    initStringSelectors(*tuning);
}

Tuning TuningDialog::getNewTuning() const
{
    // create a new tuning with the specified settings
    Tuning newTuning;
    newTuning.SetName(ui->tuningNameEdit->text().toStdString());
    newTuning.SetMusicNotationOffset(ui->notationOffsetSpinBox->value());
    newTuning.SetSharps(ui->sharpsCheckBox->isChecked());

    // grab the selected tuning notes
    std::vector<uint8_t> tuningNotes(ui->numStringsSpinBox->value());
    std::transform(stringSelectors.begin(), stringSelectors.begin() + ui->numStringsSpinBox->value(),
                   tuningNotes.begin(), std::mem_fun(&QComboBox::currentIndex));

    newTuning.SetTuningNotes(tuningNotes);

    return newTuning;
}

uint8_t TuningDialog::getNewCapo() const
{
    return ui->capoSpinBox->value();
}
