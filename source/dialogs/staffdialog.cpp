/*
  * Copyright (C) 2013 Cameron White
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

#include "staffdialog.h"
#include "ui_staffdialog.h"

#include <score/tuning.h>

StaffDialog::StaffDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::StaffDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->numberOfStringsSpinBox->setMinimum(Tuning::MIN_STRING_COUNT);
    ui->numberOfStringsSpinBox->setMaximum(Tuning::MAX_STRING_COUNT);
    ui->numberOfStringsSpinBox->setValue(6);

    ui->clefTypeComboBox->addItem(QStringLiteral("Treble"));
    ui->clefTypeComboBox->addItem(QStringLiteral("Bass"));
}

StaffDialog::~StaffDialog()
{
    delete ui;
}

int StaffDialog::getStringCount() const
{
    return ui->numberOfStringsSpinBox->value();
}

void StaffDialog::setStringCount(int strings)
{
    ui->numberOfStringsSpinBox->setValue(strings);
}

Staff::ClefType StaffDialog::getClefType() const
{
    return static_cast<Staff::ClefType>(ui->clefTypeComboBox->currentIndex());
}

void StaffDialog::setClefType(Staff::ClefType type)
{
    ui->clefTypeComboBox->setCurrentIndex(static_cast<int>(type));
}
