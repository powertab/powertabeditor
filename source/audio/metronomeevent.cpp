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
  
#include "metronomeevent.h"

#include <powertabdocument/generalmidi.h>
#include <audio/rtmidiwrapper.h>
#include <QSettings>
#include <app/settings.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

const uint8_t MetronomeEvent::METRONOME_PITCH = midi::MIDI_NOTE_MIDDLE_C;

MetronomeEvent::MetronomeEvent(uint8_t channel, double startTime, double duration,
                               uint32_t positionIndex, uint32_t systemIndex, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex),
    velocity(velocity)
{
}

void MetronomeEvent::performEvent(RtMidiWrapper& sequencer) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Metronome: " << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    // check if the metronome has been disabled
    QSettings settings;

    VelocityType actualVelocity = velocity;
    if (settings.value(Settings::MIDI_METRONOME_ENABLED,
                       Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool() == false)
    {
        actualVelocity = METRONOME_OFF;
    }

    sequencer.setPatch(myChannel, settings.value(Settings::MIDI_METRONOME_PRESET,
            Settings::MIDI_METRONOME_PRESET_DEFAULT).toInt());
    sequencer.setChannelMaxVolume(myChannel, midi::MAX_MIDI_CHANNEL_VOLUME);
    sequencer.playNote(myChannel, METRONOME_PITCH, actualVelocity);
}
