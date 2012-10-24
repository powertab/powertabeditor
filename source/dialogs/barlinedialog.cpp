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

#include "barlinedialog.h"
#include "ui_barlinedialog.h"

#include <powertabdocument/barline.h>

BarlineDialog::BarlineDialog(QWidget* parent, uint8_t barType, uint8_t repeats) :
    QDialog(parent),
    ui(new Ui::BarlineDialog)
{
    ui->setupUi(this);

    ui->barlineTypeComboBox->addItem(tr("Single"));
    ui->barlineTypeComboBox->addItem(tr("Double"));
    ui->barlineTypeComboBox->addItem(tr("Free Time"));
    ui->barlineTypeComboBox->addItem(tr("Repeat Start"));
    ui->barlineTypeComboBox->addItem(tr("Repeat End"));
    ui->barlineTypeComboBox->addItem(tr("Double Bar Fine"));

    connect(ui->barlineTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onBarlineTypeChanged(int)));
    ui->barlineTypeComboBox->setCurrentIndex(barType);
    onBarlineTypeChanged(barType);

    // Set limits for repeat count.
    ui->repeatCountSpinBox->setMinimum(Barline::MIN_REPEAT_COUNT);
    ui->repeatCountSpinBox->setMaximum(Barline::MAX_REPEAT_COUNT);
    ui->repeatCountSpinBox->setValue(repeats);
}

BarlineDialog::~BarlineDialog()
{
    delete ui;
}

/// Returns the barline type that was selected.
uint8_t BarlineDialog::barlineType() const
{
    return ui->barlineTypeComboBox->currentIndex();
}

/// Returns the repeat count that was selected.
uint8_t BarlineDialog::repeatCount() const
{
    return ui->repeatCountSpinBox->value();
}

void BarlineDialog::onBarlineTypeChanged(int newBarlineType)
{
    if (newBarlineType == Barline::repeatEnd)
    {
        ui->repeatCountSpinBox->setEnabled(true);
    }
    else
    {
        ui->repeatCountSpinBox->setEnabled(false);
    }
}
