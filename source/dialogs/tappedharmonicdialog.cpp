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
#include "ui_tappedharmonicdialog.h"

#include <score/note.h>

TappedHarmonicDialog::TappedHarmonicDialog(QWidget *parent, int originalFret)
    : QDialog(parent),
      ui(new Ui::TappedHarmonicDialog()),
      myOriginalFret(originalFret)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->currentFretSpinBox->setValue(myOriginalFret);

    // Populate the list of available frets.
    for (int offset : Harmonics::getValidFretOffsets())
    {
        const int tappedFret = myOriginalFret + offset;

        if (tappedFret <= Note::MAX_FRET_NUMBER)
            ui->tappedFretComboBox->addItem(QString::number(tappedFret));
    }
}

TappedHarmonicDialog::~TappedHarmonicDialog()
{
    delete ui;
}

int TappedHarmonicDialog::getTappedFret() const
{
    return ui->tappedFretComboBox->currentText().toInt();
}

void TappedHarmonicDialog::accept()
{
    done(Accepted);
}
