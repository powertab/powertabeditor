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
  
#include "vibratoevent.h"

#include <app/settings.h>
#include <audio/midioutputdevice.h>
#include <QSettings>

#ifdef LOG_MIDI_EVENTS
#include <QDebug>
#endif

VibratoEvent::VibratoEvent(int channel, double startTime, int position,
                           int system, EventType eventType,
                           VibratoType vibratoType)
    : MidiEvent(channel, startTime, 0, position, system),
      myEventType(eventType),
      myVibratoType(vibratoType)
{
}

void VibratoEvent::performEvent(MidiOutputDevice &device) const
{
#ifdef LOG_MIDI_EVENTS
    qDebug() << "Vibrato: " << mySystem << ", " << myPosition << " at " <<
                myStartTime;
#endif

    switch (myEventType)
    {
    case VibratoEvent::VibratoOn:
    {
        QSettings settings;
        uint8_t vibratoWidth = 0;

        if (myVibratoType == NormalVibrato)
        {
            vibratoWidth = settings.value(Settings::MIDI_VIBRATO_LEVEL,
                                          Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt();
        }
        else if (myVibratoType == WideVibrato)
        {
            vibratoWidth = settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                                          Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt();
        }

        device.setVibrato(myChannel, vibratoWidth);
    }
        break;
    case VibratoEvent::VibratoOff:
        device.setVibrato(myChannel, 0);
        break;
    }
}
