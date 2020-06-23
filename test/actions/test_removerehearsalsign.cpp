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

#include <actions/removerehearsalsign.h>
#include <score/score.h>

TEST_CASE("Actions/RemoveRehearsalSign")
{
    Score score;
    System system;
    system.insertBarline(Barline(6, Barline::SingleBar));
    system.getBarlines()[0].setRehearsalSign(RehearsalSign("A", "Intro"));
    system.getBarlines()[1].setRehearsalSign(RehearsalSign("B", "Verse"));
    score.insertSystem(system);

    ScoreLocation location(score, 0, 0, 0);
    RemoveRehearsalSign action(location);

    action.redo();
    {
        const Barline &barline1 = score.getSystems()[0].getBarlines()[0];
        const Barline &barline2 = score.getSystems()[0].getBarlines()[1];
        REQUIRE(!barline1.hasRehearsalSign());
        REQUIRE(barline2.hasRehearsalSign());
        REQUIRE(barline2.getRehearsalSign().getLetters() == "A");
    }

    action.undo();
    {
        const Barline &barline1 = score.getSystems()[0].getBarlines()[0];
        const Barline &barline2 = score.getSystems()[0].getBarlines()[1];
        REQUIRE(barline1.hasRehearsalSign());
        REQUIRE(barline1.getRehearsalSign().getLetters() == "A");
        REQUIRE(barline2.hasRehearsalSign());
        REQUIRE(barline2.getRehearsalSign().getLetters() == "B");
    }
}
