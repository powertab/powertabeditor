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

#include "multibarrestdialog.h"
#include "ui_multibarrestdialog.h"

MultiBarRestDialog::MultiBarRestDialog(QWidget *parent, int initial_value)
    : QDialog(parent), ui(new Ui::MultiBarRestDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->numberOfBarsSpinBox->setMinimum(2);
    ui->numberOfBarsSpinBox->setValue(initial_value);
}

MultiBarRestDialog::~MultiBarRestDialog()
{
    delete ui;
}

int MultiBarRestDialog::getBarCount() const
{
    return ui->numberOfBarsSpinBox->value();
}
