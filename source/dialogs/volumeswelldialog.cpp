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

#include <powertabdocument/position.h>
#include <powertabdocument/dynamic.h>

VolumeSwellDialog::VolumeSwellDialog(const Position* position, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeSwellDialog)
{
    ui->setupUi(this);

    connect(ui->overCurNoteOpt, SIGNAL(toggled(bool)),
            ui->numNotesSpinBox, SLOT(setDisabled(bool)));

    ui->numNotesSpinBox->setMinimum(1);
    ui->numNotesSpinBox->setMaximum(Position::MAX_VOLUME_SWELL_DURATION);

    // put the volume buttons in a QButtonGroup so that only one can be
    // checked at a time
    startVolumeLevels = new QButtonGroup(this);
    startVolumeLevels->addButton(ui->volumeOffButtonStart, Dynamic::off);
    startVolumeLevels->addButton(ui->pppButtonStart, Dynamic::ppp);
    startVolumeLevels->addButton(ui->ppButtonStart, Dynamic::pp);
    startVolumeLevels->addButton(ui->pButtonStart, Dynamic::p);
    startVolumeLevels->addButton(ui->mpButtonStart, Dynamic::mp);
    startVolumeLevels->addButton(ui->mfButtonStart, Dynamic::mf);
    startVolumeLevels->addButton(ui->fButtonStart, Dynamic::f);
    startVolumeLevels->addButton(ui->ffButtonStart, Dynamic::ff);
    startVolumeLevels->addButton(ui->fffButtonStart, Dynamic::fff);

    endVolumeLevels = new QButtonGroup(this);
    endVolumeLevels->addButton(ui->volumeOffButtonEnd, Dynamic::off);
    endVolumeLevels->addButton(ui->pppButtonEnd, Dynamic::ppp);
    endVolumeLevels->addButton(ui->ppButtonEnd, Dynamic::pp);
    endVolumeLevels->addButton(ui->pButtonEnd, Dynamic::p);
    endVolumeLevels->addButton(ui->mpButtonEnd, Dynamic::mp);
    endVolumeLevels->addButton(ui->mfButtonEnd, Dynamic::mf);
    endVolumeLevels->addButton(ui->fButtonEnd, Dynamic::f);
    endVolumeLevels->addButton(ui->ffButtonEnd, Dynamic::ff);
    endVolumeLevels->addButton(ui->fffButtonEnd, Dynamic::fff);

    // initialize with existing values
    if (position->HasVolumeSwell())
    {
        uint8_t startVolume = 0, endVolume = 0, duration = 0;
        position->GetVolumeSwell(startVolume, endVolume, duration);

        startVolumeLevels->button(startVolume)->setChecked(true);
        endVolumeLevels->button(endVolume)->setChecked(true);

        if (duration == 0)
            ui->overCurNoteOpt->setChecked(true);
        else
            ui->overFollowingNotesOpt->setChecked(true);
    }
    else
    {
        ui->volumeOffButtonStart->setChecked(true);
        ui->fffButtonEnd->setChecked(true);
        ui->overCurNoteOpt->setChecked(true);
    }

    ui->buttonBox->setFocus();
}

VolumeSwellDialog::~VolumeSwellDialog()
{
    delete ui;
}

uint8_t VolumeSwellDialog::getNewStartVolume() const
{
    return newStartVolume;
}

uint8_t VolumeSwellDialog::getNewEndVolume() const
{
    return newEndVolume;
}

uint8_t VolumeSwellDialog::getNewDuration() const
{
    return newDuration;
}

void VolumeSwellDialog::accept()
{
    newDuration = ui->overCurNoteOpt->isChecked() ? 0 : ui->numNotesSpinBox->value();
    newStartVolume = startVolumeLevels->checkedId();
    newEndVolume = endVolumeLevels->checkedId();

    done(QDialog::Accepted);
}
