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

ArtificialHarmonicDialog::ArtificialHarmonicDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ArtificialHarmonicDialog),
      myKeyGroup(new QButtonGroup(this)),
      myAccidentalGroup(new QButtonGroup(this)),
      myOctaveGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    myKeyGroup->addButton(ui->keyCButton, ChordName::C);
    myKeyGroup->addButton(ui->keyDButton, ChordName::D);
    myKeyGroup->addButton(ui->keyEButton, ChordName::E);
    myKeyGroup->addButton(ui->keyFButton, ChordName::F);
    myKeyGroup->addButton(ui->keyGButton, ChordName::G);
    myKeyGroup->addButton(ui->keyAButton, ChordName::A);
    myKeyGroup->addButton(ui->keyBButton, ChordName::B);

    myAccidentalGroup->addButton(ui->sharpButton);
    myAccidentalGroup->addButton(ui->flatButton);
    myAccidentalGroup->addButton(ui->doubleSharpButton);
    myAccidentalGroup->addButton(ui->doubleFlatButton);

    myOctaveGroup->addButton(ui->octaveLocoButton,
        static_cast<int>(ArtificialHarmonic::Octave::Loco));
    myOctaveGroup->addButton(ui->octave8vaButton,
        static_cast<int>(ArtificialHarmonic::Octave::Octave8va));
    myOctaveGroup->addButton(ui->octave15maButton,
        static_cast<int>(ArtificialHarmonic::Octave::Octave15ma));

    ui->keyCButton->setChecked(true);
    ui->octave8vaButton->setChecked(true);

    // Since we want only 0 or 1 of the buttons to be checked at any time, we
    // need to handle this ourselves.
    myAccidentalGroup->setExclusive(false);
    connect(myAccidentalGroup,
            qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this,
            &ArtificialHarmonicDialog::onAccidentalButtonClicked);
}

ArtificialHarmonicDialog::~ArtificialHarmonicDialog()
{
    delete ui;
}

ArtificialHarmonic ArtificialHarmonicDialog::getHarmonic() const
{
    // QButtonGroup doesn't like having button ids of -1, so we can't easily
    // use checkedId() here.
    auto variation = ChordName::NoVariation;
    if (myAccidentalGroup->checkedButton() == ui->sharpButton)
        variation = ChordName::Sharp;
    else if (myAccidentalGroup->checkedButton() == ui->doubleSharpButton)
        variation = ChordName::DoubleSharp;
    else if (myAccidentalGroup->checkedButton() == ui->flatButton)
        variation = ChordName::Flat;
    else if (myAccidentalGroup->checkedButton() == ui->doubleFlatButton)
        variation = ChordName::DoubleFlat;

    return ArtificialHarmonic(
        static_cast<ChordName::Key>(myKeyGroup->checkedId()), variation,
        static_cast<ArtificialHarmonic::Octave>(myOctaveGroup->checkedId()));
}

void ArtificialHarmonicDialog::onAccidentalButtonClicked(
    QAbstractButton *clickedButton)
{
    for (auto &button : myAccidentalGroup->buttons())
    {
        if (button != clickedButton)
            button->setChecked(false);
    }
}
