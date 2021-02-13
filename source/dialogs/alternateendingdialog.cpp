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

#include <algorithm>
#include <functional>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QVBoxLayout>

AlternateEndingDialog::AlternateEndingDialog(
    QWidget *parent, const AlternateEnding *current_ending)
    : QDialog(parent)
{
    const int LAYOUT_SPACING = 15;

    setWindowTitle(tr("Repeat Ending"));
    setModal(true);

    auto mainLayout = new QVBoxLayout(this);

    std::vector<QHBoxLayout *> rowLayouts;
    for (int i = AlternateEnding::MIN_NUMBER; i <= AlternateEnding::MAX_NUMBER; ++i)
    {
        // Group checkboxes into rows of 4.
        if ((i - 1) % 4 == 0)
            rowLayouts.push_back(new QHBoxLayout());

        QCheckBox *checkBox = new QCheckBox(QString::number(i), this);
        myCheckBoxes.push_back(checkBox);
        rowLayouts.back()->addWidget(checkBox);

        if (current_ending)
        {
            auto &&numbers = current_ending->getNumbers();
            if (std::binary_search(numbers.begin(), numbers.end(), i))
                checkBox->setChecked(true);
        }
    }

    // Set up the Da Capo, Dal Segno, and Dal Segno Segno checkboxes.
    myDaCapoCheckbox = new QCheckBox(tr("D.C."), this);
    myDaCapoCheckbox->setChecked(current_ending ? current_ending->hasDaCapo()
                                                : false);
    myDalSegnoCheckbox = new QCheckBox(tr("D.S."), this);
    myDalSegnoCheckbox->setChecked(
        current_ending ? current_ending->hasDalSegno() : false);
    myDalSegnoSegnoCheckbox = new QCheckBox(tr("D.S.S."), this);
    myDalSegnoSegnoCheckbox->setChecked(
        current_ending ? current_ending->hasDalSegnoSegno() : false);
    auto layout = new QHBoxLayout();
    layout->addWidget(myDaCapoCheckbox);
    layout->addWidget(myDalSegnoCheckbox);
    layout->addWidget(myDalSegnoSegnoCheckbox);
    rowLayouts.push_back(layout);

    for (QHBoxLayout *layout : rowLayouts)
    {
        layout->setSpacing(LAYOUT_SPACING);
        mainLayout->addLayout(layout);
    }

    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
    mainLayout->setSpacing(LAYOUT_SPACING);
    setLayout(mainLayout);
}

AlternateEnding AlternateEndingDialog::getAlternateEnding() const
{
    AlternateEnding ending;

    for (size_t i = 0; i < myCheckBoxes.size(); ++i)
    {
        if (myCheckBoxes[i]->isChecked())
            ending.addNumber(static_cast<int>(i) + 1);
    }

    ending.setDaCapo(myDaCapoCheckbox->isChecked());
    ending.setDalSegno(myDalSegnoCheckbox->isChecked());
    ending.setDalSegnoSegno(myDalSegnoSegnoCheckbox->isChecked());

    return ending;
}

void AlternateEndingDialog::accept()
{
    // Check that at least one repeat number was selected.
    if (std::find_if(myCheckBoxes.begin(), myCheckBoxes.end(),
                     [](const QCheckBox *checkbox) {
                         return checkbox->isChecked();
                     }) == myCheckBoxes.end() &&
        !myDaCapoCheckbox->isChecked() && !myDalSegnoCheckbox->isChecked() &&
        !myDalSegnoSegnoCheckbox->isChecked())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Repeat Ending"));
        msgBox.setText(tr("You must select at least one repeat number."));
        msgBox.exec();
    }
    else
        done(Accepted);
}
