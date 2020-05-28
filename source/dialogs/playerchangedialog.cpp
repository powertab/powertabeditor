/*
  * Copyright (C) 2013 Cameron White
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

#include "playerchangedialog.h"
#include "ui_playerchangedialog.h"

#include <QComboBox>
#include <QLabel>
#include <score/score.h>

PlayerChangeDialog::PlayerChangeDialog(QWidget *parent, const Score &score,
                                       const System &system,
                                       const PlayerChange *currentPlayers)
    : QDialog(parent),
      ui(new Ui::PlayerChangeDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    const int spacing = 12;

    // Set up title row.
    auto titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel(tr("Staff")));
    titleLayout->addWidget(new QLabel(tr("Instrument")));
    titleLayout->setSpacing(spacing * 4);
    ui->formLayout->addRow(tr("Player"), titleLayout);

    for (const Player &player : score.getPlayers())
    {
        const int numStrings = player.getTuning().getStringCount();

        auto layout = new QHBoxLayout();
        layout->setSpacing(spacing);
        layout->addWidget(getStaffComboBox(numStrings, system));
        layout->addWidget(getInstrumentComboBox(score));

        ui->formLayout->addRow(
                    QString::fromStdString(player.getDescription() + ":"),
                    layout);
    }

    // Initialize the dialog with the current staff/instrument for each player.
    if (currentPlayers)
    {
        for (unsigned int staff = 0; staff < system.getStaves().size(); ++staff)
        {
            std::vector<ActivePlayer> players =
                    currentPlayers->getActivePlayers(staff);

            for (const ActivePlayer &player : players)
            {
                const int i = player.getPlayerNumber();
                const int idx = myStaffComboBoxes.at(i)->findData(staff);
                myStaffComboBoxes.at(i)->setCurrentIndex(idx < 0 ? 0 : idx);
                myInstrumentComboBoxes.at(i)->setCurrentIndex(
                            player.getInstrumentNumber());
            }
        }
    }

    ui->formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
}

PlayerChangeDialog::~PlayerChangeDialog()
{
    delete ui;
}

PlayerChange PlayerChangeDialog::getPlayerChange() const
{
    PlayerChange change;

    for (size_t i = 0; i < myStaffComboBoxes.size(); ++i)
    {
        if (myStaffComboBoxes[i]->currentIndex() < 0)
            continue;

        const int staff = myStaffComboBoxes[i]->itemData(
                    myStaffComboBoxes[i]->currentIndex()).toInt();
        if (staff >= 0)
        {
            change.insertActivePlayer(
                staff, ActivePlayer(static_cast<int>(i),
                                    myInstrumentComboBoxes[i]->currentIndex()));
        }
    }

    return change;
}

QComboBox *PlayerChangeDialog::getStaffComboBox(int numStrings,
                                                const System &system)
{
    auto list = new QComboBox(this);
    myStaffComboBoxes.push_back(list);

    list->addItem(tr("None"), -1);

    int i = 0;
    for (const Staff &staff : system.getStaves())
    {
        if (staff.getStringCount() == numStrings)
            list->addItem(QString::number(i + 1), i);
        ++i;
    }

    return list;
}

QComboBox *PlayerChangeDialog::getInstrumentComboBox(const Score &score)
{
    auto list = new QComboBox(this);
    myInstrumentComboBoxes.push_back(list);

    int i = 0;
    for (const Instrument &instrument : score.getInstruments())
    {
        list->addItem(QString::fromStdString(instrument.getDescription()), i);
        ++i;
    }

    return list;
}
