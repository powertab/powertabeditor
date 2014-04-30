/*
  * Copyright (C) 2014 Cameron White
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

#include "benddialog.h"
#include "ui_benddialog.h"

#include <score/note.h>

BendDialog::BendDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BendDialog)
{
    ui->setupUi(this);

    ui->bendTypeComboBox->addItems({
        tr("Bend"), tr("Bend and Release"), tr("Bend and Hold"),
        tr("Pre-Bend"), tr("Pre-Bend and Release"), tr("Pre-Bend and Hold"),
        tr("Gradual Release"), tr("Immediate Release")
    });

    initBendPitches();

    ui->bendDurationComboBox->addItems({
        tr("Default"), tr("Current Note Duration")
    });
    for (int i = 1; i <= 8; ++i)
    {
        ui->bendDurationComboBox->addItem(
            tr("Current Note Duration + Next %n Note(s)", 0, i));
    }

    initDrawPoints(ui->vertStartingPointComboBox);
    initDrawPoints(ui->vertEndingPointComboBox);

    // TODO - enable/disable fields based on current bend type.
}

BendDialog::~BendDialog() { delete ui; }

void BendDialog::initBendPitches()
{
    for (int i = 0; i <= 12; ++i)
    {
        const QString text = QString::fromStdString(Bend::getPitchText(i));

        if (i != 0)
            ui->bentPitchComboBox->addItem(text, i);
        ui->releasePitchComboBox->addItem(text, i);
    }

    // Default bend pitch is "Full".
    ui->bentPitchComboBox->setCurrentIndex(3);
}

void BendDialog::initDrawPoints(QComboBox *c)
{
    c->addItem(tr("High"));
    c->addItem(tr("Middle"));
    c->addItem(tr("Low (Tablature Staff Line)"));
}
