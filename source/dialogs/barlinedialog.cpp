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

BarlineDialog::BarlineDialog(QWidget *parent, Barline::BarType type,
                             int repeats, bool isStartBar, bool isEndBar)
    : QDialog(parent),
      ui(new Ui::BarlineDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ui->barlineTypeComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &BarlineDialog::onBarlineTypeChanged);

    ui->barlineTypeComboBox->addItem(tr("Single"), Barline::SingleBar);
    ui->barlineTypeComboBox->addItem(tr("Double"), Barline::DoubleBar);
    ui->barlineTypeComboBox->addItem(tr("Free Time"), Barline::FreeTimeBar);

    if (!isEndBar)
        ui->barlineTypeComboBox->addItem(tr("Repeat Start"),
                                         Barline::RepeatStart);

    if (!isStartBar)
    {
        ui->barlineTypeComboBox->addItem(tr("Repeat End"), Barline::RepeatEnd);
        ui->barlineTypeComboBox->addItem(tr("Double Bar Fine"),
                                         Barline::DoubleBarFine);
    }

    ui->barlineTypeComboBox->setCurrentIndex(
                ui->barlineTypeComboBox->findData(type));

    // Set limits for repeat count.
    ui->repeatCountSpinBox->setMinimum(Barline::MIN_REPEAT_COUNT);
    ui->repeatCountSpinBox->setValue(repeats);
}

BarlineDialog::~BarlineDialog()
{
    delete ui;
}

Barline::BarType BarlineDialog::getBarType() const
{
    return static_cast<Barline::BarType>(ui->barlineTypeComboBox->itemData(
                ui->barlineTypeComboBox->currentIndex()).toInt());
}

int BarlineDialog::getRepeatCount() const
{
    return ui->repeatCountSpinBox->value();
}

void BarlineDialog::onBarlineTypeChanged(int index)
{
    int newBarlineType = ui->barlineTypeComboBox->itemData(index).toInt();
    ui->repeatCountSpinBox->setEnabled(newBarlineType == Barline::RepeatEnd);
}
