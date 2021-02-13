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

#include <QButtonGroup>

DynamicDialog::DynamicDialog(QWidget *parent, const Dynamic *current_dynamic)
    : QDialog(parent),
      ui(new Ui::DynamicDialog),
      myVolumeLevels(new QButtonGroup(this))
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Add each button to a QButtonGroup (to ensure that only one is ever
    // checked), and associate each with the corresponding value from the
    // VolumeLevel class.
    myVolumeLevels->addButton(ui->volumeOffButton,
                              static_cast<uint8_t>(VolumeLevel::Off));
    myVolumeLevels->addButton(ui->pppButton,
                              static_cast<uint8_t>(VolumeLevel::ppp));
    myVolumeLevels->addButton(ui->ppButton,
                              static_cast<uint8_t>(VolumeLevel::pp));
    myVolumeLevels->addButton(ui->pButton,
                              static_cast<uint8_t>(VolumeLevel::p));
    myVolumeLevels->addButton(ui->mpButton,
                              static_cast<uint8_t>(VolumeLevel::mp));
    myVolumeLevels->addButton(ui->mfButton,
                              static_cast<uint8_t>(VolumeLevel::mf));
    myVolumeLevels->addButton(ui->fButton,
                              static_cast<uint8_t>(VolumeLevel::f));
    myVolumeLevels->addButton(ui->ffButton,
                              static_cast<uint8_t>(VolumeLevel::ff));
    myVolumeLevels->addButton(ui->fffButton,
                              static_cast<uint8_t>(VolumeLevel::fff));

    auto default_button = myVolumeLevels->button(static_cast<uint8_t>(
        current_dynamic ? current_dynamic->getVolume() : VolumeLevel::f));
    default_button->setChecked(true);
}

DynamicDialog::~DynamicDialog()
{
    delete ui;
}

VolumeLevel DynamicDialog::getVolumeLevel() const
{
    return static_cast<VolumeLevel>(myVolumeLevels->checkedId());
}
