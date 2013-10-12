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

#include "mixeritem.h"
#include "ui_mixeritem.h"

#include <boost/lexical_cast.hpp>
#include <score/player.h>

MixerItem::MixerItem(QWidget *parent, int playerIndex, const Player &player)
    : QWidget(parent), ui(new Ui::MixerItem)
{
    ui->setupUi(this);

    ui->playerIndexLabel->setText(QString("%1.").arg(playerIndex + 1));
    ui->playerNameLabel->setText(
        QString::fromStdString(player.getDescription()));
    ui->playerVolume->setValue(player.getMaxVolume());
    ui->playerPan->setValue(player.getPan());
    ui->playerTuning->setText(QString::fromStdString(
        boost::lexical_cast<std::string>(player.getTuning())));

    ui->playerNameEdit->hide();
}

MixerItem::~MixerItem()
{
    delete ui;
}
