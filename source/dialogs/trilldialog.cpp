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
  
#include "trilldialog.h"

#include <powertabdocument/note.h>

#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

TrillDialog::TrillDialog(Note* note, quint8& trillFret) :
    note(note),
    trillFret(trillFret)
{
    setWindowTitle(tr("Trill"));
    setModal(true);

    QFormLayout* formLayout = new QFormLayout;

    QSpinBox* mainNote = new QSpinBox;
    mainNote->setValue(note->GetFretNumber());
    mainNote->setDisabled(true);

    trillFretSelector = new QSpinBox;
    trillFretSelector->setValue(0);
    trillFretSelector->setMinimum(Note::MIN_FRET_NUMBER);
    trillFretSelector->setMaximum(Note::MAX_FRET_NUMBER);

    formLayout->addRow(tr("Current Fret:"), mainNote);
    formLayout->addRow(tr("Trill With Fret:"), trillFretSelector);

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

void TrillDialog::accept()
{
    const int selectedFret = trillFretSelector->value();

    if (!note->IsValidTrill(selectedFret))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Trill"));
        msgBox.setText(tr("The trilled fret number cannot be the same as the original fret number."));
        msgBox.exec();
        trillFretSelector->setFocus();
    }
    else
    {
        trillFret = selectedFret;
        done(Accepted);
    }
}
