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
  
#ifndef TUNING_FIXTURES_H
#define TUNING_FIXTURES_H

#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

struct StandardTuningFixture
{
    StandardTuningFixture() :
        tuning("Tuning", 0, true, {midi::MIDI_NOTE_E4, midi::MIDI_NOTE_B3,
            midi::MIDI_NOTE_G3, midi::MIDI_NOTE_D3, midi::MIDI_NOTE_A2,
            midi::MIDI_NOTE_E2})
    {
    }

    ~StandardTuningFixture() {}

    Tuning tuning;
};

#endif // TUNING_FIXTURES_H
