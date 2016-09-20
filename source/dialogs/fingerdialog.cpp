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

#include "fingerdialog.h"
#include "ui_fingerdialog.h"

#include <QMessageBox>

#include <score/note.h>

FingerDialog::FingerDialog(QWidget *parent, int finger)
: QDialog(parent),
ui(new Ui::FingerDialog),
myFinger(finger)
{
    ui->setupUi(this);
    
    ui->fingerSpinBox->setValue(finger);
    
    ui->fingerSpinBox->setMinimum(0);
    ui->fingerSpinBox->setMaximum(4);
    ui->fingerSpinBox->setValue(0);
    
    ui->fingerSpinBox->selectAll();
}

FingerDialog::~FingerDialog()
{
    delete ui;
}

int fingerDialog::getFinger() const
{
    return ui->fingerSpinBox->value();
}

void FingerDialog::accept()
{
    if (getFinger() == myOriginalFinger)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Finger"));
        msgBox.setText(tr("The trilled fret number cannot be the same as the original fret number."));
        msgBox.exec();
        ui->FingerSpinBox->setFocus();
    }
    else
    {
        done(Accepted);
    }
}
