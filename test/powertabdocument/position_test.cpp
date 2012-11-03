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

#include <catch.hpp>

#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

#include "tuning_fixtures.h"
#include "serialization_test.h"

struct PositionFixture
{
    PositionFixture()
    {
        pos = new Position(2, 4, 1);

        note1 = new Note(1, 0);
        note2 = new Note(3, 5);
        note3 = new Note(5, 4);
        note4 = new Note(0, 0);
        pos->InsertNote(note1);
        pos->InsertNote(note2);
        pos->InsertNote(note3);
        pos->InsertNote(note4);

        pos->SetArpeggioUp(true);
        pos->SetPickStrokeDown(true);
        pos->SetTremoloBar(Position::dip, 4, 12);
    }

    ~PositionFixture()
    {
        delete pos;
    }

    Note *note1, *note2, *note3, *note4;
    Position* pos;
};

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/GetNoteByString", "")
{
    REQUIRE(pos->GetNoteByString(2) == NULL);
    REQUIRE(pos->GetNoteByString(1) == note1);
    REQUIRE(pos->GetNoteByString(3) == note2);
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/AddNote/NoNoteAtString", "")
{
    Note* newNote = new Note(2, 0);
    pos->InsertNote(newNote);

    REQUIRE(pos->GetNoteCount() == 5u);
    REQUIRE(pos->GetNoteByString(2) == newNote);
}

TEST_CASE("PowerTabDocument/Position/AddNote/NoteAlreadyAtString", "")
{
    REQUIRE_THROWS_AS(PositionFixture().pos->InsertNote(new Note(0, 0)),
                      std::logic_error);
}

TEST_CASE("PowerTabDocument/Position/RemoveNote/NoNoteAtString", "")
{
    REQUIRE_THROWS_AS(PositionFixture().pos->RemoveNote(2), std::logic_error);
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/RemoveNote/NoteAlreadyAtString", "")
{
    pos->RemoveNote(0);

    REQUIRE(pos->GetNoteCount() == 3u);
    REQUIRE(pos->GetNoteByString(0) == NULL);
}

TEST_CASE("PowerTabDocument/Position/GetStringBounds/MultipleStrings", "")
{
    REQUIRE(PositionFixture().pos->GetStringBounds() == (std::pair<uint8_t, uint8_t>(0, 5)));
}

TEST_CASE("PowerTabDocument/Position/GetStringBounds/SingleString", "")
{
    Position pos;
    pos.InsertNote(new Note(4, 13));

    REQUIRE(pos.GetStringBounds() == (std::pair<uint8_t, uint8_t>(4, 4)));
}

TEST_CASE("PowerTabDocument/Position/Equality", "")
{
    PositionFixture pos1, pos2, pos3;

    REQUIRE(*(pos1.pos) == *(pos2.pos));
    pos2.pos->GetNote(0)->SetTrill(5);
    REQUIRE(*(pos1.pos) != *(pos2.pos));

    REQUIRE(*(pos1.pos) == *(pos3.pos));
    pos3.pos->SetTremoloBar(Position::diveAndHold, 4, 4);
    REQUIRE(*(pos1.pos) != *(pos3.pos));
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/CloneObject", "")
{
    Position* pos2 = pos->CloneObject();

    REQUIRE(pos != pos2);
    REQUIRE(*pos == *pos2);

    delete pos2;
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/Assignment", "")
{
    Position* pos2 = new Position;

    REQUIRE(*pos != *pos2);

    *pos2 = *pos;
    REQUIRE(*pos == *pos2);

    pos2->GetNote(0)->SetFretNumber(24);
    REQUIRE(*pos != *pos2);

    delete pos2;
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/CanShiftTabNumber", "")
{
    const uint8_t numStrings = 6;
    StandardTuningFixture stdTuning;
    Tuning tuning = stdTuning.tuning;

    // can't shift up since we are at the high E string
    REQUIRE(!pos->CanShiftTabNumber(note4, Position::SHIFT_UP, numStrings, tuning));
    // can't shift down since there is already a note at string # 1
    REQUIRE(!pos->CanShiftTabNumber(note4, Position::SHIFT_DOWN, numStrings, tuning));

    // at fret 5 on the D string, so can shift up or down
    REQUIRE(pos->CanShiftTabNumber(note2, Position::SHIFT_DOWN, numStrings, tuning));
    REQUIRE(pos->CanShiftTabNumber(note2, Position::SHIFT_UP, numStrings, tuning));

    // at fret 4, so we can't shift up to a higher string
    REQUIRE(!pos->CanShiftTabNumber(note3, Position::SHIFT_UP, numStrings, tuning));
    // can't shift down from bottom string
    REQUIRE(!pos->CanShiftTabNumber(note3, Position::SHIFT_DOWN, numStrings, tuning));
}

TEST_CASE("PowerTabDocument/Position/IrregularNoteText/NoGrouping", "")
{
    Position pos;
    REQUIRE(pos.GetIrregularGroupingText().empty());
}

TEST_CASE("PowerTabDocument/Position/IrregularNoteText/Triplet", "")
{
    Position pos;
    pos.SetIrregularGroupingTiming(3, 2);
    REQUIRE(pos.GetIrregularGroupingText() == "3");
}

TEST_CASE("PowerTabDocument/Position/IrregularNoteText/Irregular", "")
{
    Position pos;
    pos.SetIrregularGroupingTiming(7, 5);
    REQUIRE(pos.GetIrregularGroupingText() == "7:5");
}

TEST_CASE("PowerTabDocument/Position/TremoloBarText", "")
{
    Position pos;

    pos.SetTremoloBar(Position::dip, 1, 3);
    REQUIRE(pos.GetTremoloBarText() == "3/4");

    pos.SetTremoloBar(Position::dip, 1, 4);
    REQUIRE(pos.GetTremoloBarText() == "1");

    pos.SetTremoloBar(Position::dip, 1, 8);
    REQUIRE(pos.GetTremoloBarText() == "2");

    pos.SetTremoloBar(Position::dip, 1, 6);
    REQUIRE(pos.GetTremoloBarText() == "1 1/2");

    pos.SetTremoloBar(Position::dip, 1, 9);
    REQUIRE(pos.GetTremoloBarText() == "2 1/4");
}

TEST_CASE_METHOD(PositionFixture, "PowerTabDocument/Position/Serialization", "")
{
    testSerialization(*pos);
}
