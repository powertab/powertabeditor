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

#include "mixer.h"

#include "mixeritem.h"
#include <QVBoxLayout>
#include <score/score.h>

Mixer::Mixer(QWidget *parent, const TuningDictionary &dictionary,
             const PlayerPubSub &pubsub)
    : QWidget(parent), myDictionary(dictionary), myPubSub(pubsub)
{
    myLayout = new QVBoxLayout(this);
    myLayout->setSpacing(0);
    myLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(myLayout);
}

void Mixer::update(const Score &score)
{
    clear();

    for (int i = 0; i < score.getPlayers().size(); ++i)
    {
        myLayout->addWidget(new MixerItem(this, i, score.getPlayers()[i],
                                          myDictionary, myPubSub));
    }
}

void Mixer::clear()
{
    while (QLayoutItem *item = myLayout->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
}
