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

#ifndef SCORE_INSTRUMENT_H
#define SCORE_INSTRUMENT_H

#include <cstdint>
#include "fileversion.h"
#include <string>

class Instrument
{
public:
    Instrument();

    bool operator==(const Instrument &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns a description of the instrument (e.g. "Clean Guitar").
    const std::string &getDescription() const;
    /// Sets the description of the instrument.
    void setDescription(const std::string &description);

    /// Returns the MIDI preset for the instrument.
    uint8_t getMidiPreset() const;
    /// Sets the MIDI preset for the instrument.
    void setMidiPreset(uint8_t preset);

private:
    std::string myDescription;
    uint8_t myMidiPreset;
};

template <class Archive>
void Instrument::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("description", myDescription);
    ar("midi_preset", myMidiPreset);
}

#endif
