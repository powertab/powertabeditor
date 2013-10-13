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

#include <app/pubsub/playerpubsub.h>
#include <boost/lexical_cast.hpp>
#include <score/player.h>
#include <sigfwd/sigfwd.hpp>

MixerItem::MixerItem(QWidget *parent, int playerIndex, const Player &player,
                     const PlayerPubSub &pubsub)
    : QWidget(parent),
      ui(new Ui::MixerItem),
      myPubSub(pubsub),
      myPlayerIndex(playerIndex),
      myTuning(player.getTuning())
{
    ui->setupUi(this);

    ui->playerIndexLabel->setText(QString("%1.").arg(playerIndex + 1));
    ui->playerNameLabel->setText(
        QString::fromStdString(player.getDescription()));
    ui->playerNameEdit->setText(ui->playerNameLabel->text());
    ui->playerVolume->setValue(player.getMaxVolume());
    ui->playerPan->setValue(player.getPan());
    ui->playerTuning->setText(QString::fromStdString(
        boost::lexical_cast<std::string>(player.getTuning())));

    ui->playerNameEdit->hide();

    connect(ui->playerNameLabel, SIGNAL(clicked()), ui->playerNameLabel,
            SLOT(hide()));
    connect(ui->playerNameLabel, SIGNAL(clicked()), ui->playerNameEdit,
            SLOT(show()));
    connect(ui->playerNameLabel, SIGNAL(clicked()), ui->playerNameEdit,
            SLOT(setFocus()));

    connect(ui->playerNameEdit, SIGNAL(editingFinished()), this,
            SLOT(onPlayerNameEdited()));

    sigfwd::connect(ui->playerVolume, SIGNAL(valueChanged(int)),
                    boost::bind(&MixerItem::onEdited, this, false));
    sigfwd::connect(ui->playerPan, SIGNAL(valueChanged(int)),
                    boost::bind(&MixerItem::onEdited, this, false));
}

MixerItem::~MixerItem()
{
    delete ui;
}

void MixerItem::onPlayerNameEdited()
{
    // Avoid sending another message when the editor becomes hidden.
    if (ui->playerNameEdit->isHidden())
        return;

    ui->playerNameLabel->setText(ui->playerNameEdit->text());
    ui->playerNameEdit->hide();
    ui->playerNameLabel->show();

    onEdited(true);
}

void MixerItem::onEdited(bool undoable)
{
    Player player;
    player.setDescription(ui->playerNameLabel->text().toStdString());
    player.setMaxVolume(ui->playerVolume->value());
    player.setPan(ui->playerPan->value());
    player.setTuning(myTuning);

    myPubSub.publish(myPlayerIndex, player, undoable);
}
