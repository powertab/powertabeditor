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

#include "volumeswelldialog.h"
#include "ui_volumeswelldialog.h"

#include <score/position.h>

#include <QButtonGroup>

VolumeSwellDialog::VolumeSwellDialog(QWidget *parent,
                                     const VolumeSwell *current_swell)
    : QDialog(parent),
      ui(new Ui::VolumeSwellDialog),
      myStartVolumeLevels(new QButtonGroup(this)),
      myEndVolumeLevels(new QButtonGroup(this))
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->overCurNoteOpt, &QRadioButton::toggled, ui->numNotesSpinBox,
            &QSpinBox::setDisabled);

    ui->numNotesSpinBox->setMinimum(1);
    ui->numNotesSpinBox->setMaximum(128);

    // Add each button to a QButtonGroup (to ensure that only one is ever
    // checked), and associate each with the corresponding value from the
    // VolumeLevel class.
    myStartVolumeLevels->addButton(ui->volumeOffButtonStart,
                                   static_cast<int>(VolumeLevel::Off));
    myStartVolumeLevels->addButton(ui->pppButtonStart,
                                   static_cast<int>(VolumeLevel::ppp));
    myStartVolumeLevels->addButton(ui->ppButtonStart,
                                   static_cast<int>(VolumeLevel::pp));
    myStartVolumeLevels->addButton(ui->pButtonStart,
                                   static_cast<int>(VolumeLevel::p));
    myStartVolumeLevels->addButton(ui->mpButtonStart,
                                   static_cast<int>(VolumeLevel::mp));
    myStartVolumeLevels->addButton(ui->mfButtonStart,
                                   static_cast<int>(VolumeLevel::mf));
    myStartVolumeLevels->addButton(ui->fButtonStart,
                                   static_cast<int>(VolumeLevel::f));
    myStartVolumeLevels->addButton(ui->ffButtonStart,
                                   static_cast<int>(VolumeLevel::ff));
    myStartVolumeLevels->addButton(ui->fffButtonStart,
                                   static_cast<int>(VolumeLevel::fff));
    if (current_swell)
    {
        myStartVolumeLevels
            ->button(static_cast<int>(current_swell->getStartVolume()))
            ->setChecked(true);
    }
    else
        ui->volumeOffButtonStart->setChecked(true);

    myEndVolumeLevels->addButton(ui->volumeOffButtonEnd,
                                 static_cast<int>(VolumeLevel::Off));
    myEndVolumeLevels->addButton(ui->pppButtonEnd,
                                 static_cast<int>(VolumeLevel::ppp));
    myEndVolumeLevels->addButton(ui->ppButtonEnd,
                                 static_cast<int>(VolumeLevel::pp));
    myEndVolumeLevels->addButton(ui->pButtonEnd,
                                 static_cast<int>(VolumeLevel::p));
    myEndVolumeLevels->addButton(ui->mpButtonEnd,
                                 static_cast<int>(VolumeLevel::mp));
    myEndVolumeLevels->addButton(ui->mfButtonEnd,
                                 static_cast<int>(VolumeLevel::mf));
    myEndVolumeLevels->addButton(ui->fButtonEnd,
                                 static_cast<int>(VolumeLevel::f));
    myEndVolumeLevels->addButton(ui->ffButtonEnd,
                                 static_cast<int>(VolumeLevel::ff));
    myEndVolumeLevels->addButton(ui->fffButtonEnd,
                                 static_cast<int>(VolumeLevel::fff));
    if (current_swell)
    {
        myEndVolumeLevels
            ->button(static_cast<int>(current_swell->getEndVolume()))
            ->setChecked(true);
    }
    else
        ui->fffButtonEnd->setChecked(true);

    ui->overCurNoteOpt->setChecked(true);
    if (current_swell)
    {
        ui->overFollowingNotesOpt->setChecked(current_swell->getDuration() > 0);
        ui->numNotesSpinBox->setValue(current_swell->getDuration());
    }

    ui->buttonBox->setFocus();
}

VolumeSwellDialog::~VolumeSwellDialog() = default;

VolumeSwell
VolumeSwellDialog::getVolumeSwell() const
{
    return VolumeSwell(
        static_cast<VolumeLevel>(myStartVolumeLevels->checkedId()),
        static_cast<VolumeLevel>(myEndVolumeLevels->checkedId()),
        ui->overCurNoteOpt->isChecked() ? 0 : ui->numNotesSpinBox->value());
}
