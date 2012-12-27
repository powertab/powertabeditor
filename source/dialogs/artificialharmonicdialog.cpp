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

#include "artificialharmonicdialog.h"
#include "ui_artificialharmonicdialog.h"

#include <QButtonGroup>

#include <powertabdocument/chordname.h>
#include <powertabdocument/note.h>

ArtificialHarmonicDialog::ArtificialHarmonicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArtificialHarmonicDialog),
    keyGroup(new QButtonGroup(this)),
    accidentalGroup(new QButtonGroup(this)),
    octaveGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    keyGroup->addButton(ui->keyCButton, ChordName::C);
    keyGroup->addButton(ui->keyDButton, ChordName::D);
    keyGroup->addButton(ui->keyEButton, ChordName::E);
    keyGroup->addButton(ui->keyFButton, ChordName::F);
    keyGroup->addButton(ui->keyGButton, ChordName::G);
    keyGroup->addButton(ui->keyAButton, ChordName::A);
    keyGroup->addButton(ui->keyBButton, ChordName::B);

    accidentalGroup->addButton(ui->sharpButton);
    accidentalGroup->addButton(ui->flatButton);
    accidentalGroup->addButton(ui->doubleSharpButton);
    accidentalGroup->addButton(ui->doubleFlatButton);

    octaveGroup->addButton(ui->octaveLocoButton,
                           Note::artificialHarmonicOctaveLoco);
    octaveGroup->addButton(ui->octave8vaButton,
                           Note::artificialHarmonicOctave8va);
    octaveGroup->addButton(ui->octave15maButton,
                           Note::artificialHarmonicOctave15ma);

    ui->keyCButton->setChecked(true);
    ui->octave8vaButton->setChecked(true);

    // Since we want only 0 or 1 of the buttons to be checked at any time, we
    // need to handle this ourselves.
    accidentalGroup->setExclusive(false);
    connect(accidentalGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(onAccidentalButtonClicked(QAbstractButton*)));
}

ArtificialHarmonicDialog::~ArtificialHarmonicDialog()
{
    delete ui;
}

uint8_t ArtificialHarmonicDialog::getKey() const
{
    uint8_t key = 0;
    uint8_t keyVariation = 0;
    ChordName::ComputeKeyAndVariation(key, keyVariation, keyGroup->checkedId(),
            ui->sharpButton->isChecked(), ui->doubleSharpButton->isChecked(),
            ui->flatButton->isChecked(), ui->doubleFlatButton->isChecked());
    return key;
}

uint8_t ArtificialHarmonicDialog::getKeyVariation() const
{
    uint8_t key = 0;
    uint8_t keyVariation = 0;
    ChordName::ComputeKeyAndVariation(key, keyVariation, keyGroup->checkedId(),
            ui->sharpButton->isChecked(), ui->doubleSharpButton->isChecked(),
            ui->flatButton->isChecked(), ui->doubleFlatButton->isChecked());
    return keyVariation;
}

uint8_t ArtificialHarmonicDialog::getOctave() const
{
    return octaveGroup->checkedId();
}

void ArtificialHarmonicDialog::onAccidentalButtonClicked(
        QAbstractButton *clickedButton)
{
    foreach (QAbstractButton *button, accidentalGroup->buttons())
    {
        if (button != clickedButton)
        {
            button->setChecked(false);
        }
    }
}
