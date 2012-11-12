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

#include <boost/assign.hpp>
#include "serialization_test.h"

#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>
#include <powertabdocument/tuning.h>
#include <powertabdocument/keysignature.h>
#include <powertabdocument/generalmidi.h>

// Provides a staff with some positions already inserted
struct StaffFixture
{
    StaffFixture()
    {
        highPositions.push_back(new Position(0, 4, 0));
        highPositions.push_back(new Position(2, 4, 0));
        highPositions.push_back(new Position(4, 4, 0));
        highPositions.push_back(new Position(5, 4, 0));
        highPositions.push_back(new Position(6, 4, 0));

        // put some sample notes at each position
        highPositions.at(0)->InsertNote(new Note(1, 4));
        highPositions.at(0)->InsertNote(new Note(2, 3));
        highPositions.at(1)->InsertNote(new Note(1, 5));
        highPositions.at(2)->InsertNote(new Note(1, 5));
        highPositions.at(3)->InsertNote(new Note(1, 6));
        highPositions.at(4)->InsertNote(new Note(1, 5));

        lowPositions.push_back(new Position(1, 4, 0));
        lowPositions.push_back(new Position(3, 8, 0));

        staff.positionArrays[0] = highPositions;

        staff.positionArrays[1] = lowPositions;
    }

    ~StaffFixture() {}

    Staff staff;
    std::vector<Position*> highPositions;
    std::vector<Position*> lowPositions;
};

TEST_CASE("PowerTabDocument/Staff/Clef/IsValidClef", "")
{
    REQUIRE(Staff::IsValidClef(Staff::BASS_CLEF));
    REQUIRE(Staff::IsValidClef(Staff::TREBLE_CLEF));
    REQUIRE(!Staff::IsValidClef(Staff::BASS_CLEF + 1));
}

TEST_CASE("PowerTabDocument/Staff/Clef/ModifyClef", "")
{
    Staff staff;
    staff.SetClef(Staff::BASS_CLEF);
    REQUIRE(staff.GetClef() == Staff::BASS_CLEF);

    staff.SetClef(Staff::TREBLE_CLEF);
    REQUIRE(staff.GetClef() == Staff::TREBLE_CLEF);
}

TEST_CASE("PowerTabDocument/Staff/GetLastPosition/EmptyStaff", "")
{
    Staff emptyStaff;
    REQUIRE(emptyStaff.GetLastPosition() == NULL);
}

TEST_CASE("PowerTabDocument/Staff/GetLastPosition/SinglePosition", "")
{
    Staff singleItemStaff;
    Position* pos1 = new Position();
    singleItemStaff.positionArrays[0].push_back(pos1);

    REQUIRE(singleItemStaff.GetLastPosition() == pos1);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetLastPosition/MultiplePositions", "")
{
    REQUIRE(staff.GetLastPosition() == highPositions.back());
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanHammerOn/LessThanNextFretOkay", "")
{
    Position* pos = highPositions.at(0);
    REQUIRE(staff.CanHammerOn(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanHammerOn/EqualFretsNotOkay", "")
{
    Position* pos = highPositions.at(1);
    REQUIRE(!staff.CanHammerOn(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanHammerOn/GreaterThanNextFretNotOkay", "")
{
    Position* pos = highPositions.at(3);
    REQUIRE(!staff.CanHammerOn(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanHammerOn/LastNoteCantBeHammeron", "")
{
    Position* pos = highPositions.back();
    REQUIRE(!staff.CanHammerOn(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanHammerOn/HammeronReplacesSlide", "")
{
    // Add a slide between two notes.
    Position* pos = highPositions.at(2);
    Note *note = pos->GetNote(0);
    note->SetSlideOutOf(1, 1);

    REQUIRE(note->HasSlideOutOf());

    REQUIRE(staff.CanHammerOn(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanPullOff/PulloffReplacesSlide", "")
{
    // Add a slide between two notes.
    Position* pos = highPositions.at(3);
    Note *note = pos->GetNote(0);
    note->SetSlideOutOf(1, 1);

    REQUIRE(note->HasSlideOutOf());

    REQUIRE(staff.CanPullOff(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanSlideBetweenNotes/DifferentFretOkay", "")
{
    Position* pos = highPositions.at(0);
    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));

    pos = highPositions.at(3);
    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanSlideBetweenNotes/EqualFretsNotOkay", "")
{
    Position* pos = highPositions.at(1);
    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanSlideBetweenNotes/LastNoteCantSlideToNext", "")
{
    Position* pos = highPositions.back();
    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanSlideBetweenNotes/SlideReplacesHammeron", "")
{
    // Add a hammeron between two notes.
    Position* pos = highPositions.at(3);
    Note *note = pos->GetNote(0);
    note->SetHammerOn(true);

    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == true);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CanSlideBetweenNotes/SlideReplacesPulloff", "")
{
    // Add a hammeron between two notes.
    Position* pos = highPositions.at(2);
    Note *note = pos->GetNote(0);
    note->SetPullOff(true);

    REQUIRE(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == true);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetSlideSteps/Calculation", "")
{
    Position* pos = highPositions.at(0);
    REQUIRE(staff.GetSlideSteps(pos, pos->GetNote(0)) == 1);

    pos = highPositions.at(1);
    REQUIRE(staff.GetSlideSteps(pos, pos->GetNote(0)) == 0);

    pos = highPositions.at(3);
    REQUIRE(staff.GetSlideSteps(pos, pos->GetNote(0)) == -1);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetSlideSteps/ThrowsWithNoAdjacentNote", "")
{
    Position* pos = highPositions.at(0);
    // use note on the second string, since the next position only has a note on the first string
    REQUIRE_THROWS_AS(staff.GetSlideSteps(pos, pos->GetNote(1)),
                      std::logic_error);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetPositionCount", "")
{
    REQUIRE(staff.GetPositionCount(0) == 5u);
    REQUIRE(staff.GetPositionCount(1) == 2u);

    REQUIRE_THROWS_AS(staff.GetPositionCount(2), std::out_of_range);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetPosition", "")
{
    REQUIRE(staff.GetPosition(0, 1) == highPositions.at(1));
    REQUIRE(staff.GetPosition(1, 1) == lowPositions.at(1));

    REQUIRE_THROWS_AS(staff.GetPosition(0, 99), std::out_of_range);
    REQUIRE_THROWS_AS(staff.GetPosition(3, 0), std::out_of_range);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetPositionByPosition/NullIfNoPositionFound", "")
{
    REQUIRE(staff.GetPositionByPosition(0, 1) == NULL);
    REQUIRE(staff.GetPositionByPosition(0, 99) == NULL);

    REQUIRE_THROWS_AS(staff.GetPositionByPosition(99, 0), std::out_of_range);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/GetPositionByPosition/PositionFoundCorrectly", "")
{
    REQUIRE(staff.GetPositionByPosition(0, 2) == highPositions.at(1));
    REQUIRE(staff.GetPositionByPosition(1, 1) == lowPositions.at(0));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/InsertPosition", "")
{
    const size_t originalPosCount = staff.GetPositionCount(0);
    Position* newPos = new Position(1, 4, 0);
    staff.InsertPosition(0, newPos);

    REQUIRE(staff.GetPositionCount(0) == originalPosCount + 1);
    // check that the position list was re-sorted by position index
    REQUIRE(staff.GetPosition(0, 1) == newPos);

    // cannot insert a position if there is already something at that position
    REQUIRE(staff.InsertPosition(0, newPos) == false);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/RemovePosition", "")
{
    Position* pos = staff.GetPositionByPosition(0, 2);
    staff.RemovePosition(0, 2);
    delete pos;

    // check that it was removed, and the positions were shifted accordingly
    REQUIRE(staff.GetPositionCount(0) == highPositions.size() - 1);
    REQUIRE(staff.GetPosition(0, 1) == highPositions.at(2));
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/AssignmentAndEquality", "")
{
    Staff newStaff;
    REQUIRE(newStaff != staff);

    newStaff = staff;
    REQUIRE(newStaff == staff);

    newStaff.GetPosition(0, 2)->SetArpeggioDown(true);
    REQUIRE(newStaff != staff);
}

TEST_CASE("PowerTabDocument/Staff/GetNoteLocation", "")
{
    Staff staff;

    KeySignature keySig;
    Tuning tuning;
    tuning.SetToStandard();

    Note note(0, 1);

    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == 0);

    note.SetFretNumber(0);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == 1);

    note.SetFretNumber(3);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == -1);

    note.SetFretNumber(2);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == 0);

    note.SetFretNumber(14);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == -7);

    note.SetFretNumber(21);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == -12);

    // Test some edge cases with B#.
    {
        KeySignature sevenSharps(KeySignature::majorKey, 7);
        note.SetString(4);
        note.SetFretNumber(1);
        REQUIRE(staff.GetNoteLocation(&note, sevenSharps, tuning) == 12);

        note.SetFretNumber(2);
        REQUIRE(staff.GetNoteLocation(&note, sevenSharps, tuning) == 11);

        note.SetFretNumber(3);
        REQUIRE(staff.GetNoteLocation(&note, sevenSharps, tuning) == 11);

        note.SetFretNumber(4);
        REQUIRE(staff.GetNoteLocation(&note, sevenSharps, tuning) == 10);
    }

    // Bass clef
    staff.SetClef(Staff::BASS_CLEF);
    note.SetString(5);
    note.SetFretNumber(5);

    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == 0);

    note.SetFretNumber(8);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == -2);

    note.SetOctave15ma(true);
    REQUIRE(staff.GetNoteLocation(&note, keySig, tuning) == 12);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/Serialization", "")
{
    testSerialization(staff);
}

TEST_CASE_METHOD(StaffFixture, "PowerTabDocument/Staff/CalculateClef", "")
{
    using namespace midi;

    Tuning tuning;
    std::vector<uint8_t> notes;

    // create a tuning with a very low 7th string
    using namespace boost::assign;
    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
            MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2, MIDI_NOTE_E1;
    tuning.SetTuningNotes(notes);

    staff.CalculateClef(tuning);

    // the fixture is only using high notes by default, so should have a treble clef
    REQUIRE(staff.GetClef() == Staff::TREBLE_CLEF);

    // add a low note, which should trigger a switch to the bass clef
    staff.GetPosition(0,0)->InsertNote(new Note(6, 0));
    staff.CalculateClef(tuning);

    REQUIRE(staff.GetClef() == Staff::BASS_CLEF);
}
