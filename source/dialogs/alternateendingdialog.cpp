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
  
#include "alternateendingdialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QMessageBox>

#include <powertabdocument/alternateending.h>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>

AlternateEndingDialog::AlternateEndingDialog(std::shared_ptr<AlternateEnding> altEnding) :
    altEnding(altEnding)
{
    const int SPACING = 15;

    setWindowTitle(tr("Repeat Ending"));
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout;

    std::vector<QHBoxLayout*> horizLayouts;
    for (uint8_t i = 1; i <= AlternateEnding::dalSegnoSegno; i++)
    {
        if ((i - 1) % 4 == 0) // group checkboxes into rows of 4
        {
            horizLayouts.push_back(new QHBoxLayout);
        }

        QCheckBox* checkBox = new QCheckBox(QString::fromStdString(AlternateEnding::GetNumberText(i)));

        if (altEnding->IsNumberSet(i))
        {
            checkBox->setChecked(true);
        }
        checkBoxes.push_back(checkBox);
        horizLayouts.back()->addWidget(checkBox);
    }

    BOOST_FOREACH(QHBoxLayout* horizLayout, horizLayouts)
    {
        horizLayout->setSpacing(SPACING);
        mainLayout->addLayout(horizLayout);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    mainLayout->setSpacing(SPACING);
    setLayout(mainLayout);
}

void AlternateEndingDialog::accept()
{
    // check that at least one repeat number was selected
    if (std::find_if(checkBoxes.begin(), checkBoxes.end(),
                     std::mem_fun(&QCheckBox::isChecked)) == checkBoxes.end())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Repeat Ending"));
        msgBox.setText(tr("You must select at least one repeat number."));
        msgBox.exec();
    }
    else
    {
        // update alternate ending with the selected checkboxes
        for(uint8_t i = 1; i <= checkBoxes.size(); i++)
        {
            if (checkBoxes[i-1]->isChecked())
            {
                altEnding->SetNumber(i);
            }
            else
            {
                altEnding->ClearNumber(i);
            }
        }

        done(Accepted);
    }
}
