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

#include <boost/foreach.hpp>
#include <QComboBox>
#include <QLabel>
#include <score/score.h>

PlayerChangeDialog::PlayerChangeDialog(QWidget *parent, const Score &score,
                                       const System &system) :
    QDialog(parent),
    ui(new Ui::PlayerChangeDialog)
{
    ui->setupUi(this);

    const int spacing = 12;

    // Set up title row.
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel(tr("Staff")));
    titleLayout->addWidget(new QLabel(tr("Instrument")));
    titleLayout->setSpacing(spacing);
    ui->formLayout->addRow(tr("Player"), titleLayout);

    BOOST_FOREACH(const Player &player, score.getPlayers())
    {
        const int numStrings = player.getTuning().getStringCount();

        QHBoxLayout *layout = new QHBoxLayout();
        layout->setSpacing(spacing);
        layout->addWidget(getStaffComboBox(numStrings, system));
        layout->addWidget(getInstrumentComboBox(score));

        ui->formLayout->addRow(
                    QString::fromStdString(player.getDescription() + ":"),
                    layout);
    }

    ui->formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
}

PlayerChangeDialog::~PlayerChangeDialog()
{
    delete ui;
}

QComboBox *PlayerChangeDialog::getStaffComboBox(int numStrings,
                                                const System &system)
{
    QComboBox *list = new QComboBox(this);

    int i = 0;
    BOOST_FOREACH(const Staff &staff, system.getStaves())
    {
        if (staff.getStringCount() == numStrings)
            list->addItem(QString::number(i + 1), i);
        ++i;
    }

    return list;
}

QComboBox *PlayerChangeDialog::getInstrumentComboBox(const Score &score)
{
    QComboBox *list = new QComboBox(this);

    int i = 0;
    BOOST_FOREACH(const Instrument &instrument, score.getInstruments())
    {
        list->addItem(QString::fromStdString(instrument.getDescription()), i);
        ++i;
    }

    return list;
}
