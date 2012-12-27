/*
  * Copyright (C) 2012 Cameron White
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

#include <catch.hpp>

#include <actions/removeartificialharmonic.h>
#include <powertabdocument/chordname.h>
#include <powertabdocument/note.h>

TEST_CASE("Actions/RemoveArtificialHarmonic", "")
{
    Note note;
    const uint8_t originalKey = ChordName::D;
    const uint8_t originalKeyVariation = ChordName::variationUp;
    const uint8_t originalOctave = Note::artificialHarmonicOctave8va;
    note.SetArtificialHarmonic(originalKey, originalKeyVariation,
                               originalOctave);

    uint8_t key = 0;
    uint8_t keyVariation = 0;
    uint8_t octave = 0;

    RemoveArtificialHarmonic action(&note);

    action.redo();
    REQUIRE(!note.HasArtificialHarmonic());

    action.undo();
    REQUIRE(note.HasArtificialHarmonic());

    note.GetArtificialHarmonic(key, keyVariation, octave);
    REQUIRE(key == originalKey);
    REQUIRE(keyVariation == originalKeyVariation);
    REQUIRE(octave == originalOctave);
}
