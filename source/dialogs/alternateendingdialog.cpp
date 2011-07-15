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
