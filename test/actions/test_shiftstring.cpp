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
#include <app/appinfo.h>
#include <formats/powertab/powertabimporter.h>
#include <score/score.h>

TEST_CASE("Actions/ShiftString")
{
    Score score;

    PowerTabImporter importer;
    importer.load(AppInfo::getAbsolutePath("data/test_shiftstring.pt2"), score);

    SECTION("Single shift up")
    {
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

            REQUIRE(!note0.hasProperty(Note::HammerOnOrPullOff));
            REQUIRE(!note1.hasProperty(Note::HammerOnOrPullOff));
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

            REQUIRE(note0.hasProperty(Note::HammerOnOrPullOff));
            REQUIRE(note1.hasProperty(Note::HammerOnOrPullOff));
        }
    }

    SECTION("Simple shift down (two notes)")
    {
        ScoreLocation location(score, 0, 0, 2, 0, 2);
        location.setSelectionStart(1);

        ShiftString action(location, false);
        action.redo();
        {
            Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
            const Note &note1 = voice.getPositions()[1].getNotes()[0];
            const Note &note2 = voice.getPositions()[2].getNotes()[0];

            REQUIRE(note1.getString() == 3);
            REQUIRE(note1.getFretNumber() == 14);
            // Hammer-on within shifted section should be maintained.
            REQUIRE(note1.hasProperty(Note::HammerOnOrPullOff));

            REQUIRE(note2.getString() == 3);
            REQUIRE(note2.getFretNumber() == 13);
            REQUIRE(!note2.hasProperty(Note::HammerOnOrPullOff));
        }
    }

    SECTION("Shift up (notes on adjacent strings)")
    {
        ScoreLocation location(score, 0, 0, 8, 0, 0);
        location.setSelectionStart(7);

        ShiftString action(location, true);
        action.redo();
        {
            Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
            const Note &note1 = voice.getPositions()[5].getNotes()[0];
            const Note &note2 = voice.getPositions()[5].getNotes()[1];

            REQUIRE(note1.getString() == 3);
            REQUIRE(note1.getFretNumber() == 4);
            REQUIRE(note2.getString() == 4);
            REQUIRE(note2.getFretNumber() == 4);
        }
    }

    SECTION("Shift down (notes on adjacent strings)")
    {
        ScoreLocation location(score, 0, 0, 6, 0, 0);
        location.setSelectionStart(5);

        ShiftString action(location, false);
        action.redo();
        {
            Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
            const Note &note1 = voice.getPositions()[4].getNotes()[0];
            const Note &note2 = voice.getPositions()[4].getNotes()[1];

            REQUIRE(note1.getString() == 1);
            REQUIRE(note1.getFretNumber() == 5);
            REQUIRE(note2.getString() == 2);
            REQUIRE(note2.getFretNumber() == 4);
        }
    }

    SECTION("Invalid shift (string)")
    {
        ScoreLocation location(score, 0, 0, 6, 0, 0);
        location.setSelectionStart(5);

        ShiftString action(location, true);
        action.redo();
        {
            Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
            const Note &note1 = voice.getPositions()[4].getNotes()[0];
            const Note &note2 = voice.getPositions()[4].getNotes()[1];

            REQUIRE(note1.getString() == 0);
            REQUIRE(note1.getFretNumber() == 0);
            REQUIRE(note2.getString() == 1);
            REQUIRE(note2.getFretNumber() == 0);
        }
    }

    SECTION("Invalid shift (fret)")
    {
        ScoreLocation location(score, 0, 0, 0, 0, 2);

        ShiftString action(location, true);
        action.redo();
        {
            Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
            const Note &note1 = voice.getPositions()[0].getNotes()[0];

            REQUIRE(note1.getString() == 2);
            REQUIRE(note1.getFretNumber() == 3);
        }
    }
}
