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
  
#include <doctest/doctest.h>

#include <score/staff.h>
#include <score/utils.h>
#include <score/voiceutils.h>
#include "test_serialization.h"

TEST_CASE("Score/Staff/Clef")
{
    Staff staff;

    REQUIRE(staff.getClefType() == Staff::TrebleClef);
    staff.setClefType(Staff::BassClef);
    REQUIRE(staff.getClefType() == Staff::BassClef);
}

TEST_CASE("Score/Staff/Positions")
{
    Staff staff;
    Voice &voice0 = staff.getVoices()[0];
    Voice &voice1 = staff.getVoices()[1];

    REQUIRE(voice0.getPositions().size() == 0);
    REQUIRE(voice1.getPositions().size() == 0);

    Position pos1(3), pos2(5), pos3(1);

    voice1.insertPosition(pos1);
    voice1.insertPosition(pos2);
    voice1.insertPosition(pos3);

    REQUIRE(voice1.getPositions().size() == 3);
    voice1.removePosition(pos1);

    REQUIRE(voice1.getPositions().size() == 2);
    REQUIRE(voice1.getPositions()[0] == pos3);
    REQUIRE(voice1.getPositions()[1] == pos2);
}

TEST_CASE("Score/Staff/Dynamics")
{
    Staff staff;

    REQUIRE(staff.getDynamics().size() == 0);

    Dynamic dynamic(3, Dynamic::mf);
    staff.insertDynamic(dynamic);
    staff.insertDynamic(Dynamic(1, Dynamic::pp));
    REQUIRE(staff.getDynamics().size() == 2);
    REQUIRE(staff.getDynamics()[1] == dynamic);

    staff.removeDynamic(dynamic);
    REQUIRE(staff.getDynamics().size() == 1);
}

TEST_CASE("Score/Staff/Serialization")
{
    Staff staff;
    staff.setClefType(Staff::BassClef);
    staff.getVoices()[1].insertPosition(Position(42));
    staff.insertDynamic(Dynamic(11, Dynamic::pp));
    staff.setStringCount(7);

    Serialization::test("staff", staff);
}

TEST_CASE("Score/Staff/GetPositionsInRange")
{
    Staff staff;
    Position pos1(1), pos4(4), pos6(6), pos7(7), pos8(8);
    Voice &voice = staff.getVoices()[0];
    voice.insertPosition(pos1);
    voice.insertPosition(pos4);
    voice.insertPosition(pos6);
    voice.insertPosition(pos7);
    voice.insertPosition(pos8);

    REQUIRE(std::distance(
                ScoreUtils::findInRange(voice.getPositions(), 9, 15).begin(),
                ScoreUtils::findInRange(voice.getPositions(), 9, 15).end()) ==
            0);
    REQUIRE(std::distance(
                ScoreUtils::findInRange(voice.getPositions(), 8, 10).begin(),
                ScoreUtils::findInRange(voice.getPositions(), 8, 10).end()) ==
            1);
    REQUIRE(std::distance(
                ScoreUtils::findInRange(voice.getPositions(), 4, 7).begin(),
                ScoreUtils::findInRange(voice.getPositions(), 4, 7).end()) ==
            3);
}

TEST_CASE("Score/Staff/GetNextNote")
{
    Staff staff;
    Position pos1(1), pos4(4), pos6(6), pos7(7), pos8(8);
    pos1.insertNote(Note(2, 1));
    pos4.insertNote(Note(3, 1));
    pos6.insertNote(Note(3, 0));
    pos7.insertNote(Note(4, 2));
    pos8.insertNote(Note(3, 2));

    Voice &voice = staff.getVoices()[0];
    voice.insertPosition(pos1);
    voice.insertPosition(pos4);
    voice.insertPosition(pos6);
    voice.insertPosition(pos7);
    voice.insertPosition(pos8);

    REQUIRE(!VoiceUtils::getNextNote(voice, 6, 5));
    REQUIRE(VoiceUtils::getNextNote(voice, 6, 4));
    REQUIRE(!VoiceUtils::getNextNote(voice, 6, 3));

    REQUIRE(!VoiceUtils::getPreviousNote(voice, 6, 2));
    REQUIRE(VoiceUtils::getPreviousNote(voice, 6, 3));
}
