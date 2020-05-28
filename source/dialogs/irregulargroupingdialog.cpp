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

#include "irregulargroupingdialog.h"
#include "ui_irregulargroupingdialog.h"

IrregularGroupingDialog::IrregularGroupingDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::IrregularGroupingDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->notesPlayedSpinBox->setMinimum(2);
    ui->notesPlayedSpinBox->setMaximum(16);

    ui->notesPlayedOverSpinBox->setMinimum(2);
    ui->notesPlayedOverSpinBox->setMaximum(8);

    ui->notesPlayedSpinBox->setValue(6);
    ui->notesPlayedOverSpinBox->setValue(4);
}

IrregularGroupingDialog::~IrregularGroupingDialog()
{
    delete ui;
}

int IrregularGroupingDialog::getNotesPlayed() const
{
    return ui->notesPlayedSpinBox->value();
}

int IrregularGroupingDialog::getNotesPlayedOver() const
{
    return ui->notesPlayedOverSpinBox->value();
}
