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
  
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_SUITE(PositionTest)

    BOOST_FIXTURE_TEST_CASE(GetNoteByString, PositionFixture)
    {
        BOOST_CHECK_EQUAL(pos->GetNoteByString(2), (Note*)NULL);
        BOOST_CHECK_EQUAL(pos->GetNoteByString(1), note1);
        BOOST_CHECK_EQUAL(pos->GetNoteByString(3), note2);
    }

    BOOST_AUTO_TEST_SUITE(AddNote)

        BOOST_FIXTURE_TEST_CASE(NoNoteAtString, PositionFixture)
        {
            Note* newNote = new Note(2, 0);
            pos->InsertNote(newNote);

            BOOST_CHECK_EQUAL(pos->GetNoteCount(), 5u);
            BOOST_CHECK_EQUAL(pos->GetNoteByString(2), newNote);
        }

        BOOST_FIXTURE_TEST_CASE(NoteAlreadyAtString, PositionFixture)
        {
            BOOST_CHECK_THROW(pos->InsertNote(new Note(0, 0)), std::logic_error);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(RemoveNote)

        BOOST_FIXTURE_TEST_CASE(NoNoteAtString, PositionFixture)
        {
            BOOST_CHECK_THROW(pos->RemoveNote(2), std::logic_error);
        }

        BOOST_FIXTURE_TEST_CASE(NoteAlreadyAtString, PositionFixture)
        {
            pos->RemoveNote(0);

            BOOST_CHECK_EQUAL(pos->GetNoteCount(), 3u);
            BOOST_CHECK_EQUAL(pos->GetNoteByString(0), (Note*)NULL);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(GetStringBounds)

        BOOST_FIXTURE_TEST_CASE(MultipleStrings, PositionFixture)
        {
            BOOST_CHECK(pos->GetStringBounds() == (std::pair<uint8_t, uint8_t>(0, 5)));
        }

        BOOST_AUTO_TEST_CASE(SingleString)
        {
            Position pos;
            pos.InsertNote(new Note(4, 13));

            BOOST_CHECK(pos.GetStringBounds() == (std::pair<uint8_t, uint8_t>(4, 4)));
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(Equality)
    {
        PositionFixture pos1, pos2, pos3;

        BOOST_CHECK(*(pos1.pos) == *(pos2.pos));
        pos2.pos->GetNote(0)->SetTrill(5);
        BOOST_CHECK(*(pos1.pos) != *(pos2.pos));

        BOOST_CHECK(*(pos1.pos) == *(pos3.pos));
        pos3.pos->SetTremoloBar(Position::diveAndHold, 4, 4);
        BOOST_CHECK(*(pos1.pos) != *(pos3.pos));
    }

    BOOST_FIXTURE_TEST_CASE(CloneObject, PositionFixture)
    {
        Position* pos2 = pos->CloneObject();

        BOOST_CHECK_NE(pos, pos2);
        BOOST_CHECK(*pos == *pos2);

        delete pos2;
    }

    BOOST_FIXTURE_TEST_CASE(Assignment, PositionFixture)
    {
        Position* pos2 = new Position;

        BOOST_CHECK(*pos != *pos2);

        *pos2 = *pos;
        BOOST_CHECK(*pos == *pos2);

        pos2->GetNote(0)->SetFretNumber(24);
        BOOST_CHECK(*pos != *pos2);

        delete pos2;
    }

    BOOST_FIXTURE_TEST_SUITE(ShiftString, PositionFixture)

        BOOST_AUTO_TEST_CASE(CanShiftTabNumber)
        {
            const uint8_t numStrings = 6;
            StandardTuningFixture stdTuning;
            Tuning tuning = stdTuning.tuning;

            // can't shift up since we are at the high E string
            BOOST_CHECK(!pos->CanShiftTabNumber(note4, Position::SHIFT_UP, numStrings, tuning));
            // can't shift down since there is already a note at string # 1
            BOOST_CHECK(!pos->CanShiftTabNumber(note4, Position::SHIFT_DOWN, numStrings, tuning));

            // at fret 5 on the D string, so can shift up or down
            BOOST_CHECK(pos->CanShiftTabNumber(note2, Position::SHIFT_DOWN, numStrings, tuning));
            BOOST_CHECK(pos->CanShiftTabNumber(note2, Position::SHIFT_UP, numStrings, tuning));

            // at fret 4, so we can't shift up to a higher string
            BOOST_CHECK(!pos->CanShiftTabNumber(note3, Position::SHIFT_UP, numStrings, tuning));
            // can't shift down from bottom string
            BOOST_CHECK(!pos->CanShiftTabNumber(note3, Position::SHIFT_DOWN, numStrings, tuning));
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(IrregularNoteText)

        BOOST_AUTO_TEST_CASE(NoGrouping)
        {
            Position pos;
            BOOST_CHECK_EQUAL("", pos.GetIrregularGroupingText());
        }

        BOOST_AUTO_TEST_CASE(Triplet)
        {
            Position pos;
            pos.SetIrregularGroupingTiming(3, 2);
            BOOST_CHECK_EQUAL("3", pos.GetIrregularGroupingText());
        }

        BOOST_AUTO_TEST_CASE(Irregular)
        {
            Position pos;
            pos.SetIrregularGroupingTiming(7, 5);
            BOOST_CHECK_EQUAL("7:5", pos.GetIrregularGroupingText());
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_CASE(TremoloBarText)
    {
        Position pos;

        pos.SetTremoloBar(Position::dip, 1, 3);
        BOOST_CHECK_EQUAL("3/4", pos.GetTremoloBarText());

        pos.SetTremoloBar(Position::dip, 1, 4);
        BOOST_CHECK_EQUAL("1", pos.GetTremoloBarText());

        pos.SetTremoloBar(Position::dip, 1, 8);
        BOOST_CHECK_EQUAL("2", pos.GetTremoloBarText());

        pos.SetTremoloBar(Position::dip, 1, 6);
        BOOST_CHECK_EQUAL("1 1/2", pos.GetTremoloBarText());

        pos.SetTremoloBar(Position::dip, 1, 9);
        BOOST_CHECK_EQUAL("2 1/4", pos.GetTremoloBarText());
    }

    BOOST_FIXTURE_TEST_CASE(Serialization, PositionFixture)
    {
        testSerialization(*pos);
    }

BOOST_AUTO_TEST_SUITE_END()
