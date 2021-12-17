/*
  * Copyright (C) 2021 Cameron White
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

#include "tremolobardialog.h"
#include "ui_tremolobardialog.h"

#include <score/position.h>

TremoloBarDialog::TremoloBarDialog(QWidget *parent,
                                   const TremoloBar *current_trem)
    : QDialog(parent), ui(new Ui::TremoloBarDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->typeComboBox->addItems({ tr("Dip"), tr("Dive and Release"),
                                 tr("Dive and Hold"), tr("Release"),
                                 tr("Return and Release"),
                                 tr("Return and Hold"), tr("Inverted Dip") });

    initPitches();

    if (current_trem)
    {
        ui->typeComboBox->setCurrentIndex(
            static_cast<int>(current_trem->getType()));
        ui->pitchComboBox->setCurrentIndex(
            ui->pitchComboBox->findData(current_trem->getPitch()));
        ui->durationSpinBox->setValue(current_trem->getDuration());
    }

    connect(ui->typeComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TremoloBarDialog::handleTypeChanged);
    handleTypeChanged();
}

TremoloBarDialog::~TremoloBarDialog()
{
    delete ui;
}

TremoloBar
TremoloBarDialog::getTremoloBar() const
{
    return TremoloBar(
        static_cast<TremoloBar::Type>(ui->typeComboBox->currentIndex()),
        ui->pitchComboBox->itemData(ui->pitchComboBox->currentIndex()).toInt(),
        ui->durationSpinBox->value());
}

void
TremoloBarDialog::initPitches()
{
    for (int i = 0; i <= 28; ++i)
    {
        ui->pitchComboBox->addItem(
            QString::fromStdString(TremoloBar::getPitchText(i)), i);
    }

    // Default is a full bend.
    ui->pitchComboBox->setCurrentIndex(4);
}

void
TremoloBarDialog::handleTypeChanged()
{
    const auto type =
        static_cast<TremoloBar::Type>(ui->typeComboBox->currentIndex());

    ui->durationSpinBox->setEnabled(type != TremoloBar::Type::Dip &&
                                    type != TremoloBar::Type::InvertedDip &&
                                    type != TremoloBar::Type::Release);
    ui->pitchComboBox->setEnabled(type != TremoloBar::Type::Release);
}
