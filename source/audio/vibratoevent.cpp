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

#include <audio/rtmidiwrapper.h>
#include <QSettings>
#include <app/settings.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

VibratoEvent::VibratoEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                           EventType eventType, VibratoType vibratoType) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    eventType(eventType),
    vibratoType(vibratoType)
{
}

void VibratoEvent::performEvent(RtMidiWrapper& sequencer) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Vibrato: " << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    if (eventType == VIBRATO_ON)
    {
        QSettings settings;
        uint8_t vibratoWidth = 0;

        if (vibratoType == NORMAL_VIBRATO)
        {
            vibratoWidth = settings.value(Settings::MIDI_VIBRATO_LEVEL,
                                          Settings::MIDI_VIBRATO_LEVEL_DEFAULT).toUInt();
        }
        else if (vibratoType == WIDE_VIBRATO)
        {
            vibratoWidth = settings.value(Settings::MIDI_WIDE_VIBRATO_LEVEL,
                                          Settings::MIDI_WIDE_VIBRATO_LEVEL_DEFAULT).toUInt();
        }

        sequencer.setVibrato(channel, vibratoWidth);
    }
    else // VIBRATO_OFF
    {
        sequencer.setVibrato(channel, 0);
    }

}
