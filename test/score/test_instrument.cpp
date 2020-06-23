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
  
#include <catch2/catch.hpp>

#include <score/generalmidi.h>
#include <score/instrument.h>
#include "test_serialization.h"

TEST_CASE("Score/Instrument/Serialization")
{
    Instrument instrument;
    instrument.setDescription("Clean Guitar");
    instrument.setMidiPreset(Midi::MIDI_PRESET_ELECTRIC_GUITAR_CLEAN);

    Serialization::test("instrument", instrument);
}
