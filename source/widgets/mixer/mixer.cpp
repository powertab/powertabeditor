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
  
#include "mixer.h"

#include <QVBoxLayout>

#include <app/skinmanager.h>
#include "mixerinstrument.h"

using boost::shared_ptr;

Mixer::Mixer(shared_ptr<SkinManager> skinManager, QFrame *parent) :
    QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel);

    setStyleSheet(skinManager->getMixerStyle());

    layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

void Mixer::addInstrument(shared_ptr<Guitar> guitar)
{
    boost::shared_ptr<MixerInstrument> channel(new MixerInstrument(guitar));
    layout->addWidget(channel.get());

    channelList.push_back(channel);
}

void Mixer::removeInstrument(size_t index)
{
    Q_ASSERT(index < channelList.size());

    layout->removeWidget(channelList.at(index).get());
    channelList.erase(channelList.begin() + index);
}

shared_ptr<Guitar> Mixer::getInstrument(size_t index) const
{
    return channelList.at(index)->getInstrument();
}

/// Triggers an update for each channel of the mixer
void Mixer::update()
{
    foreach (boost::shared_ptr<MixerInstrument> mixerInstrument, channelList)
    {
        mixerInstrument->update();        
    }
}
