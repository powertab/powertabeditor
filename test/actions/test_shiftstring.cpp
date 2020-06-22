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

#include <catch2/catch.hpp>

#include <actions/shiftstring.h>
#include <score/score.h>

TEST_CASE("Actions/ShiftString")
{
    Score score;

    {
        System system;
        Staff staff;
        Voice &voice = staff.getVoices()[0];

        {
            Position pos(0);
            Note note(2, 3);
            note.setProperty(Note::HammerOnOrPullOff);
            pos.insertNote(note);
            voice.insertPosition(pos);
        }
        {
            Position pos(1);
            Note note(2, 9);
            note.setProperty(Note::HammerOnOrPullOff);
            pos.insertNote(note);
            voice.insertPosition(pos);
        }
        {
            Position pos(2);
            Note note(2, 8);
            pos.insertNote(note);
            voice.insertPosition(pos);
        }

        system.insertStaff(staff);
        score.insertSystem(system);
    }

    ScoreLocation location(score);
    location.setPositionIndex(1);

    ShiftString action(location, true);

    action.redo();
    {
        Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
        const Note &note0 = voice.getPositions()[0].getNotes()[0];
        const Note &note1 = voice.getPositions()[1].getNotes()[0];

        REQUIRE(note1.getString() == 1);
        REQUIRE(note1.getFretNumber() == 4);

        REQUIRE(!note0.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(!note1.hasProperty(Note::HammerOnOrPullOff));
    }

    action.undo();
    {
        Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
        const Note &note0 = voice.getPositions()[0].getNotes()[0];
        const Note &note1 = voice.getPositions()[1].getNotes()[0];

        REQUIRE(note0.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(note1.hasProperty(Note::HammerOnOrPullOff));
    }
}
