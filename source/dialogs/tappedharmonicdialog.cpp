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
  
#include "tappedharmonicdialog.h"

#include <powertabdocument/note.h>
#include <powertabdocument/harmonics.h>

#include <boost/foreach.hpp>

#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QComboBox>

TappedHarmonicDialog::TappedHarmonicDialog(const Note* note, uint8_t& tappedFret) :
    note(note),
    tappedFret(tappedFret)
{
    setWindowTitle(tr("Tapped Harmonic"));
    setModal(true);

    QFormLayout* formLayout = new QFormLayout;

    QLabel* frettedNote = new QLabel(QString::number(note->GetFretNumber()));

    formLayout->addRow(tr("Fretted Note:"), frettedNote);

    tappedFretSelector = new QComboBox;
    initTappedFrets();

    formLayout->addRow(tr("Tapped Fret:"), tappedFretSelector);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

/// Populates the drop-down menu with the available fret numbers for the tapped note
void TappedHarmonicDialog::initTappedFrets()
{
    const uint8_t currentFret = note->GetFretNumber();
    std::vector<uint8_t> fretOffsets = Harmonics::getFretOffsets();

    BOOST_FOREACH(uint8_t fretOffset, fretOffsets)
    {
        const uint8_t tappedFret = fretOffset + currentFret;

        // ensure that the tapped fret is actually possible to play
        if (Note::IsValidFretNumber(tappedFret))
        {
            tappedFretSelector->addItem(QString::number(fretOffset + currentFret));
        }
    }
}

void TappedHarmonicDialog::accept()
{
    tappedFret = tappedFretSelector->currentText().toUShort();
    done(Accepted);
}
