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

#include "alterationofpacedialog.h"
#include "ui_alterationofpacedialog.h"

AlterationOfPaceDialog::AlterationOfPaceDialog(
    QWidget *parent, const TempoMarker *current_marker)
    : QDialog(parent), ui(new Ui::AlterationOfPaceDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->typeComboBox->addItem(QStringLiteral("Accelerando (accel.)"),
                              TempoMarker::Accelerando);
    ui->typeComboBox->addItem(QStringLiteral("Ritardando (rit.)"),
                              TempoMarker::Ritardando);

    if (current_marker)
    {
        ui->typeComboBox->setCurrentIndex(
            current_marker->getAlterationOfPace() == TempoMarker::Ritardando
                ? 1 : 0);
    }
}

AlterationOfPaceDialog::~AlterationOfPaceDialog()
{
    delete ui;
}

TempoMarker::AlterationOfPaceType
AlterationOfPaceDialog::getAlterationOfPaceType() const
{
    return static_cast<TempoMarker::AlterationOfPaceType>(
        ui->typeComboBox->itemData(ui->typeComboBox->currentIndex()).toInt());
}
