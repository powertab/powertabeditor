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

#include <audio/rtmidiwrapper.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/guitar.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

#include <qglobal.h>

using boost::shared_ptr;

PlayNoteEvent::PlayNoteEvent(uint8_t channel, double startTime, double duration, uint8_t pitch, uint32_t positionIndex,
                             uint32_t systemIndex, shared_ptr<const Guitar> guitar, bool isMuted, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex),
    pitch(pitch),
    guitar(guitar),
    isMuted(isMuted),
    velocity(velocity)
{
}

void PlayNoteEvent::performEvent(RtMidiWrapper& sequencer) const
{
    Q_ASSERT(guitar);

#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Play Note: " << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    // grab the patch/pan/volume immediately before playback to allow for real-time mixing
    if (isMuted)
    {
        sequencer.setPatch(channel, midi::MIDI_PRESET_ELECTRIC_GUITAR_MUTED);
    }
    else
    {
        sequencer.setPatch(channel, guitar->GetPreset());
    }

    sequencer.setPan(channel, guitar->GetPan());
    sequencer.setChannelMaxVolume(channel, guitar->GetInitialVolume());

    sequencer.playNote(channel, pitch, velocity);
}
