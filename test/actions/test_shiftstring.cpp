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
    score.insertPlayer(Player());
    score.insertInstrument(Instrument());

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

        // Insert a player change so there is a tuning to work with.
        PlayerChange change(0);
        change.insertActivePlayer(0, ActivePlayer(0, 0));
        system.insertPlayerChange(change);

        score.insertSystem(system);
    }

    ScoreLocation location(score, 0, 0, 1, 0, 2);

    ShiftString action(location, true);

    action.redo();
    {
        Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
        const Note &note0 = voice.getPositions()[0].getNotes()[0];
        const Note &note1 = voice.getPositions()[1].getNotes()[0];

        REQUIRE(note0.getString() == 2);
        REQUIRE(note0.getFretNumber() == 3);

        REQUIRE(note1.getString() == 1);
        REQUIRE(note1.getFretNumber() == 5);

        // TODO - enable once supported.
#if 0
        REQUIRE(!note0.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(!note1.hasProperty(Note::HammerOnOrPullOff));
#endif
    }

    action.undo();
    {
        Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
        const Note &note0 = voice.getPositions()[0].getNotes()[0];
        const Note &note1 = voice.getPositions()[1].getNotes()[0];

        REQUIRE(note0.getString() == 2);
        REQUIRE(note0.getFretNumber() == 3);

        REQUIRE(note1.getString() == 2);
        REQUIRE(note1.getFretNumber() == 9);

        // TODO - enable once supported.
#if 0
        REQUIRE(note0.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(note1.hasProperty(Note::HammerOnOrPullOff));
#endif
    }
}
