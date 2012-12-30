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

#include "trilldialog.h"
#include "ui_trilldialog.h"

#include <QMessageBox>

#include <powertabdocument/note.h>

TrillDialog::TrillDialog(QWidget* parent, const Note* note) :
    QDialog(parent),
    ui(new Ui::TrillDialog),
    note(note)
{
    ui->setupUi(this);

    ui->currentFretSpinBox->setValue(note->GetFretNumber());

    ui->trillFretSpinBox->setMinimum(Note::MIN_FRET_NUMBER);
    ui->trillFretSpinBox->setMaximum(Note::MAX_FRET_NUMBER);
    ui->trillFretSpinBox->setValue(0);

    ui->trillFretSpinBox->selectAll();
}

TrillDialog::~TrillDialog()
{
    delete ui;
}

uint8_t TrillDialog::getTrill() const
{
    return ui->trillFretSpinBox->value();
}

void TrillDialog::accept()
{
    const int selectedFret = ui->trillFretSpinBox->value();

    if (!note->IsValidTrill(selectedFret))
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Trill"));
        msgBox.setText(tr("The trilled fret number cannot be the same as the original fret number."));
        msgBox.exec();
        ui->trillFretSpinBox->setFocus();
    }
    else
    {
        done(Accepted);
    }
}
