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
  
#include "playnoteevent.h"

#include <audio/midioutputdevice.h>
#include <score/generalmidi.h>
#include <score/instrument.h>
#include <score/player.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

PlayNoteEvent::PlayNoteEvent(int channel, double startTime, double duration,
                             uint8_t pitch, int position, int system,
                             const Player &player, const Instrument &instrument,
                             PlayNoteEvent::VelocityType velocity)
    : MidiEvent(channel, startTime, duration, position, system),
      myPitch(pitch),
      myPlayer(player),
      myInstrument(instrument),
      myVelocity(velocity)
{
}

void PlayNoteEvent::performEvent(MidiOutputDevice &device) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Play Note: " << mySystem << ", " << myPosition << " at " <<
                myStartTime;
#endif

    // Grab the patch/pan/volume immediately before playback to allow for
    // real-time mixing.
    device.setPatch(myChannel, myInstrument.getMidiPreset());

    device.setPan(myChannel, myPlayer.getPan());
    device.setChannelMaxVolume(myChannel, myPlayer.getMaxVolume());
    device.playNote(myChannel, myPitch, myVelocity);
}
