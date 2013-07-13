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

#include "instrument.h"

#include <stdexcept>
#include "generalmidi.h"

Instrument::Instrument()
    : myDescription(""),
      myMidiPreset(Midi::MIDI_PRESET_ACOUSTIC_GUITAR_STEEL)
{
}

bool Instrument::operator==(const Instrument &other) const
{
    return myDescription == other.myDescription &&
           myMidiPreset == other.myMidiPreset;
}

const std::string &Instrument::getDescription() const
{
    return myDescription;
}

void Instrument::setDescription(const std::string &description)
{
    myDescription = description;
}

uint8_t Instrument::getMidiPreset() const
{
    return myMidiPreset;
}

void Instrument::setMidiPreset(uint8_t preset)
{
    if (preset > Midi::LAST_MIDI_PRESET)
        throw std::out_of_range("Invalid MIDI preset");

    myMidiPreset = preset;
}

