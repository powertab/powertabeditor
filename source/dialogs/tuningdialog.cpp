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
#include <score/generalmidi.h>
#include <score/tuning.h>
#include <util/tostring.h>

Q_DECLARE_METATYPE(const Tuning *)

TuningDialog::TuningDialog(QWidget *parent, const Tuning &currentTuning,
                           const TuningDictionary &dictionary)
    : QDialog(parent),
      ui(new Ui::TuningDialog),
      myDictionary(dictionary)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->tuningNameEdit->setText(QString::fromStdString(currentTuning.getName()));

    ui->sharpsCheckBox->setChecked(currentTuning.usesSharps());
    connect(ui->sharpsCheckBox, &QCheckBox::toggled, this,
            &TuningDialog::toggleSharps);
    connect(ui->sharpsCheckBox, &QCheckBox::clicked, this,
            &TuningDialog::updateCurrentPreset);

    ui->capoSpinBox->setMinimum(Tuning::MIN_CAPO);
    ui->capoSpinBox->setMaximum(Tuning::MAX_CAPO);
    ui->capoSpinBox->setValue(currentTuning.getCapo());
    connect(ui->capoSpinBox, qOverload<int>(&QSpinBox::valueChanged), this,
            &TuningDialog::updateCurrentPreset);

    ui->notationOffsetSpinBox->setMinimum(Tuning::MIN_MUSIC_NOTATION_OFFSET);
    ui->notationOffsetSpinBox->setMaximum(Tuning::MAX_MUSIC_NOTATION_OFFSET);
    ui->notationOffsetSpinBox->setValue(currentTuning.getMusicNotationOffset());
    connect(ui->notationOffsetSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &TuningDialog::updateCurrentPreset);

    ui->numStringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->numStringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);
    ui->numStringsSpinBox->setValue(currentTuning.getStringCount());
    connect(ui->numStringsSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &TuningDialog::updateEnabledStrings);
    connect(ui->numStringsSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &TuningDialog::updateTuningDictionary);

    myStringSelectors.push_back(ui->string1);
    myStringSelectors.push_back(ui->string2);
    myStringSelectors.push_back(ui->string3);
    myStringSelectors.push_back(ui->string4);
    myStringSelectors.push_back(ui->string5);
    myStringSelectors.push_back(ui->string6);
    myStringSelectors.push_back(ui->string7);
    myStringSelectors.push_back(ui->string8);

    for (QComboBox *selector : myStringSelectors)
    {
        connect(selector, qOverload<int>(&QComboBox::activated), this,
                &TuningDialog::updateCurrentPreset);
    }

    updateTuningDictionary(currentTuning.getStringCount());
    generateNoteNames(currentTuning.usesSharps());
    initStringSelectors(currentTuning);
    updateEnabledStrings(currentTuning.getStringCount());

    connect(ui->presetComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TuningDialog::loadPreset);

    updateCurrentPreset();
}

TuningDialog::~TuningDialog()
{
    delete ui;
}

void TuningDialog::initStringSelectors(const Tuning& currentTuning)
{
    for (int i = 0; i < static_cast<int>(myStringSelectors.size()); i++)
    {
        QComboBox *selector = myStringSelectors[i];

        selector->addItems(myNoteNames);
        
        if (i < currentTuning.getStringCount())
            selector->setCurrentIndex(currentTuning.getNote(i, false));
    }
}

void TuningDialog::generateNoteNames(bool usesSharps)
{
    myNoteNames.clear();
    
    for (uint8_t note = Midi::MIN_MIDI_NOTE; note < Midi::MAX_MIDI_NOTE; ++note)
    {
        myNoteNames << QString::fromStdString(
                         Midi::getMidiNoteTextSimple(note,usesSharps)) +
                     QString::number(Midi::getMidiNoteOctave(note));
    }
}

void TuningDialog::toggleSharps(bool usesSharps)
{
    generateNoteNames(usesSharps);
    
    for (uint8_t i = 0; i < Tuning::MAX_STRING_COUNT; i++)
    {
        QComboBox *selector = myStringSelectors.at(i);
        const int selectedIndex = selector->currentIndex();
        selector->clear();
        selector->addItems(myNoteNames);
        selector->setCurrentIndex(selectedIndex);
    }
}

void TuningDialog::updateEnabledStrings(int numStrings)
{
    Q_ASSERT(numStrings <= (int)Tuning::MAX_STRING_COUNT && numStrings >= 0);

    std::for_each(myStringSelectors.begin(),
                  myStringSelectors.begin() + numStrings, [](QComboBox * c)
    { c->setEnabled(true); });

    std::for_each(myStringSelectors.begin() + numStrings,
                  myStringSelectors.end(), [](QComboBox * c)
    { c->setEnabled(false); });
}

void TuningDialog::updateTuningDictionary(int numStrings)
{
    std::vector<const TuningDictionary::Entry *> entries;
    myDictionary.findTunings(numStrings, entries);

    ui->presetComboBox->clear();

    for (const TuningDictionary::Entry *entry : entries)
    {
        const Tuning *tuning = &entry->myTuning;
        ui->presetComboBox->addItem(
            QStringLiteral("%1 - %2").arg(
                QString::fromStdString(tuning->getName()),
                QString::fromStdString(Util::toString(*tuning))),
            QVariant::fromValue(tuning));
    }
}

void TuningDialog::loadPreset()
{
    if (ui->presetComboBox->currentIndex() < 0)
        return;

    const Tuning *tuning = ui->presetComboBox->itemData(
                ui->presetComboBox->currentIndex()).value<const Tuning *>();

    ui->sharpsCheckBox->setChecked(tuning->usesSharps());

    // Don't invalidate the preset.
    ui->capoSpinBox->blockSignals(true);
    ui->capoSpinBox->setValue(tuning->getCapo());
    ui->capoSpinBox->blockSignals(false);

    ui->notationOffsetSpinBox->blockSignals(true);
    ui->notationOffsetSpinBox->setValue(tuning->getMusicNotationOffset());
    ui->notationOffsetSpinBox->blockSignals(false);

    initStringSelectors(*tuning);
}

Tuning TuningDialog::getTuning() const
{
    // Create a new tuning with the specified settings.
    Tuning newTuning;
    newTuning.setName(ui->tuningNameEdit->text().toStdString());
    newTuning.setMusicNotationOffset(ui->notationOffsetSpinBox->value());
    newTuning.setSharps(ui->sharpsCheckBox->isChecked());
    newTuning.setCapo(ui->capoSpinBox->value());

    // Grab the selected tuning notes.
    std::vector<uint8_t> tuningNotes(ui->numStringsSpinBox->value());
    std::transform(myStringSelectors.begin(),
                   myStringSelectors.begin() + ui->numStringsSpinBox->value(),
                   tuningNotes.begin(),
                   [](const QComboBox *c) { return c->currentIndex(); });

    newTuning.setNotes(tuningNotes);

    return newTuning;
}

void TuningDialog::updateCurrentPreset()
{
    const Tuning currentTuning = getTuning();

    // Check if the current tuning matches one of the available presets.
    for (int i = 0; i < ui->presetComboBox->count(); ++i)
    {
        auto tuning = ui->presetComboBox->itemData(i).value<const Tuning *>();

        if (tuning->isSameTuning(currentTuning))
        {
            ui->presetComboBox->setCurrentIndex(i);
            return;
        }
    }

    // Otherwise, the tuning currently doesn't match any presets.
    ui->presetComboBox->setCurrentIndex(-1);
}
