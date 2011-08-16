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

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

#include <algorithm>
#include <boost/bind.hpp>

TuningDialog::TuningDialog(const Tuning& currentTuning, QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Tuning"));
    setModal(true);

    QFormLayout* formLayout = new QFormLayout;
    
    tuningNameEditor = new QLineEdit;
    tuningNameEditor->setText(QString().fromStdString(currentTuning.GetName()));
    formLayout->addRow(tr("Name:"), tuningNameEditor);
    
    usesSharpsSelector = new QCheckBox;
    usesSharpsSelector->setChecked(currentTuning.UsesSharps());
    connect(usesSharpsSelector, SIGNAL(toggled(bool)), this, SLOT(toggleSharps(bool)));
    formLayout->addRow(tr("Sharps:"), usesSharpsSelector);
    
    notationOffsetSelector = new QSpinBox;
    notationOffsetSelector->setMinimum(Tuning::MIN_MUSIC_NOTATION_OFFSET);
    notationOffsetSelector->setMaximum(Tuning::MAX_MUSIC_NOTATION_OFFSET);
    notationOffsetSelector->setValue(currentTuning.GetMusicNotationOffset());
    formLayout->addRow(tr("Music Notation Offset:"), notationOffsetSelector);
    
    numStringsSelector = new QSpinBox;
    numStringsSelector->setMinimum(Tuning::MIN_STRING_COUNT);
    numStringsSelector->setMaximum(Tuning::MAX_STRING_COUNT);
    numStringsSelector->setValue(currentTuning.GetStringCount());

    formLayout->addRow(tr("Number of Strings:"), numStringsSelector);
    connect(numStringsSelector, SIGNAL(valueChanged(int)), this, SLOT(updateEnabledStrings(int)));

    generateNoteNames(currentTuning.UsesSharps());
    initStringSelectors(currentTuning);
    updateEnabledStrings(currentTuning.GetStringCount());

    QHBoxLayout* tuningSelectorsLayout = new QHBoxLayout;
    
    foreach (QComboBox* selector, stringSelectors)
    {
        tuningSelectorsLayout->addWidget(selector);
    }
    
    formLayout->addRow(tr("Tuning:"), tuningSelectorsLayout);
    
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void TuningDialog::initStringSelectors(const Tuning& currentTuning)
{
    for (uint8_t i = 0; i < Tuning::MAX_STRING_COUNT; i++)
    {
        QComboBox* selector = new QComboBox;
        selector->addItems(noteNames);
        
        if (currentTuning.IsValidString(i))
        {
            selector->setCurrentIndex(currentTuning.GetNote(i, false));
        }

        stringSelectors.push_back(selector);
    }
}

/// Generates a list of note names, to be used for selecting the pitch of a string
void TuningDialog::generateNoteNames(bool usesSharps)
{
    noteNames.clear();
    
    for (uint8_t note = midi::MIN_MIDI_NOTE; note < midi::MAX_MIDI_NOTE; note++)
    {
        noteNames << QString().fromStdString(midi::GetMidiNoteText(note, usesSharps)) +
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

Tuning TuningDialog::getNewTuning() const
{
    // create a new tuning with the specified settings
    Tuning newTuning;
    newTuning.SetName(tuningNameEditor->text().toStdString());
    newTuning.SetMusicNotationOffset(notationOffsetSelector->value());
    newTuning.SetSharps(usesSharpsSelector->isChecked());

    // grab the selected tuning notes
    std::vector<uint8_t> tuningNotes(numStringsSelector->value());
    std::transform(stringSelectors.begin(), stringSelectors.begin() + numStringsSelector->value(),
                   tuningNotes.begin(), std::mem_fun(&QComboBox::currentIndex));

    newTuning.SetTuningNotes(tuningNotes);

    return newTuning;
}
