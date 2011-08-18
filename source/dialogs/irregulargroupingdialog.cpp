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

#include <powertabdocument/position.h>

IrregularGroupingDialog::IrregularGroupingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IrregularGroupingDialog)
{
    ui->setupUi(this);

    ui->notesPlayedSpinBox->setMinimum(Position::MIN_IRREGULAR_GROUPING_NOTES_PLAYED);
    ui->notesPlayedSpinBox->setMaximum(Position::MAX_IRREGULAR_GROUPING_NOTES_PLAYED);

    ui->notesPlayedOverSpinBox->setMinimum(Position::MIN_IRREGULAR_GROUPING_NOTES_PLAYED_OVER);
    ui->notesPlayedOverSpinBox->setMaximum(Position::MAX_IRREGULAR_GROUPING_NOTES_PLAYED_OVER);

    ui->notesPlayedSpinBox->setValue(6);
    ui->notesPlayedOverSpinBox->setValue(4);
}

IrregularGroupingDialog::~IrregularGroupingDialog()
{
    delete ui;
}

uint8_t IrregularGroupingDialog::notesPlayed() const
{
    return ui->notesPlayedSpinBox->value();
}

uint8_t IrregularGroupingDialog::notesPlayedOver() const
{
    return ui->notesPlayedOverSpinBox->value();
}
