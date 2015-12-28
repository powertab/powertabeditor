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
  
#include "dynamicdialog.h"
#include "ui_dynamicdialog.h"

DynamicDialog::DynamicDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DynamicDialog),
      myVolumeLevels(new QButtonGroup(this))
{
    ui->setupUi(this);

    // Add each button to a QButtonGroup (to ensure that only one is ever
    // checked), and associate each with the corresponding value from the
    // Dynamic class.
    myVolumeLevels->addButton(ui->volumeOffButton, Dynamic::Off);
    myVolumeLevels->addButton(ui->pppButton, Dynamic::ppp);
    myVolumeLevels->addButton(ui->ppButton, Dynamic::pp);
    myVolumeLevels->addButton(ui->pButton, Dynamic::p);
    myVolumeLevels->addButton(ui->mpButton, Dynamic::mp);
    myVolumeLevels->addButton(ui->mfButton, Dynamic::mf);
    myVolumeLevels->addButton(ui->fButton, Dynamic::f);
    myVolumeLevels->addButton(ui->ffButton, Dynamic::ff);
    myVolumeLevels->addButton(ui->fffButton, Dynamic::fff);

    myVolumeLevels->button(Dynamic::f)->setChecked(true);
}

DynamicDialog::~DynamicDialog()
{
    delete ui;
}

Dynamic::VolumeLevel DynamicDialog::getVolumeLevel() const
{
    return static_cast<Dynamic::VolumeLevel>(myVolumeLevels->checkedId());
}
