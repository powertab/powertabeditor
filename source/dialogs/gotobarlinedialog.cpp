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

#include "gotobarlinedialog.h"
#include "ui_gotobarlinedialog.h"

#include <powertabdocument/barline.h>
#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

GoToBarlineDialog::GoToBarlineDialog(QWidget* parent, const Score* score) :
    QDialog(parent),
    ui(new Ui::GoToBarlineDialog),
    score(score)
{
    ui->setupUi(this);

    for (size_t i = 0; i < score->GetSystemCount(); ++i)
    {
        Score::SystemConstPtr system(score->GetSystem(i));
        std::vector<System::BarlineConstPtr> barlines;
        system->GetBarlines(barlines);

        // Index all barlines except for the end bar.
        for (size_t j = 0; j < barlines.size() - 1; ++j)
        {
            const uint32_t position = barlines[j]->GetPosition();
            barlineLocations.push_back(SystemLocation(i, position));
        }
    }

    ui->barlineSpinBox->setValue(1);
    ui->barlineSpinBox->setMinimum(1);
    ui->barlineSpinBox->setMaximum(barlineLocations.size());
}

GoToBarlineDialog::~GoToBarlineDialog()
{
    delete ui;
}

/// Returns the location of the selected barline.
SystemLocation GoToBarlineDialog::getLocation() const
{
    const int index = ui->barlineSpinBox->value();
    return barlineLocations.at(index - 1);
}
