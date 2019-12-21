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
#include <score/player.h>
#include "test_serialization.h"

TEST_CASE("Score/Player/Serialization", "")
{
    Tuning tuning;
    std::vector<uint8_t> notes;
    notes.push_back(Midi::MIDI_NOTE_E4);
    notes.push_back(Midi::MIDI_NOTE_C2);
    notes.push_back(Midi::MIDI_NOTE_G3);
    tuning.setNotes(notes);
    tuning.setMusicNotationOffset(3);
    tuning.setSharps(false);
    tuning.setCapo(7);

    Player player;
    player.setDescription("My Description");
    player.setMaxVolume(42);
    player.setPan(123);

    Serialization::test("player", player);
}
