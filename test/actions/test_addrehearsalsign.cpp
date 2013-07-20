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
  
#include <catch.hpp>

#include <actions/addrehearsalsign.h>
#include <score/score.h>

TEST_CASE("Actions/AddRehearsalSign", "")
{
    Score score;
    System system;
    system.insertBarline(Barline(6, Barline::SingleBar));
    system.getBarlines()[0].setRehearsalSign(RehearsalSign("A", "Intro"));
    score.insertSystem(system);

    ScoreLocation location(score, 0, 0, 6);
    AddRehearsalSign action(location, "Verse");

    action.redo();
    const Barline &barline = score.getSystems()[0].getBarlines()[1];
    REQUIRE(barline.hasRehearsalSign());
    const RehearsalSign &sign = barline.getRehearsalSign();
    REQUIRE(sign.getDescription() == "Verse");
    REQUIRE(sign.getLetters() == "B");

    action.undo();
    REQUIRE(!score.getSystems()[0].getBarlines()[1].hasRehearsalSign());
}
