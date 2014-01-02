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

#include <app/settings.h>
#include <audio/midioutputdevice.h>
#include <QSettings>
#include <score/generalmidi.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

MetronomeEvent::MetronomeEvent(int channel, double startTime, double duration,
                               int position, int system, VelocityType velocity)
    : MidiEvent(channel, startTime, duration, position, system),
      myVelocity(velocity)
{
}

uint8_t MetronomeEvent::getMetronomePreset()
{
    QSettings settings;
    return Midi::MIDI_PERCUSSION_PRESET_OFFSET +
           settings.value(Settings::MIDI_METRONOME_PRESET,
                          Settings::MIDI_METRONOME_PRESET_DEFAULT).toUInt();
}

void MetronomeEvent::performEvent(MidiOutputDevice &device) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Metronome: " << mySystem << ", " << myPosition << " at " <<
                myStartTime;
#endif

    QSettings settings;
    uint8_t velocity;

    // Check if the metronome has been disabled.
    if (!settings.value(Settings::MIDI_METRONOME_ENABLED,
                        Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool())
    {
        velocity = 0;
    }
    else if (myVelocity == WeakAccent)
    {
        velocity = settings.value(Settings::MIDI_METRONOME_WEAK_ACCENT,
                                  Settings::MIDI_METRONOME_WEAK_ACCENT_DEFAULT).toUInt();
    }
    else
    {
        velocity =
            settings.value(Settings::MIDI_METRONOME_STRONG_ACCENT,
                           Settings::MIDI_METRONOME_STRONG_ACCENT_DEFAULT).toUInt();
    }

    // The metronome events use the percussion channel, so we don't need to perform
    // a patch change. The note determines whether we hear a cymbal, snare, etc.
    device.setChannelMaxVolume(myChannel, Midi::MAX_MIDI_CHANNEL_VOLUME);
    device.playNote(myChannel, getMetronomePreset(), velocity);
}
