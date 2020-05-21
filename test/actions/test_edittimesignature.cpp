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
  
#include <catch2/catch.hpp>

#include <actions/edittimesignature.h>
#include <score/score.h>

TEST_CASE("Actions/EditTimeSignature", "")
{
    Score score;
    System system;
    system.insertBarline(Barline(6, Barline::SingleBar));
    score.insertSystem(system);

    TimeSignature newTime;
    newTime.setBeatsPerMeasure(5);
    newTime.setBeatValue(8);

    ScoreLocation location(score, 0, 0, 6);
    EditTimeSignature action(location, newTime);

    action.redo();
    {
        const System &system = score.getSystems()[0];
        REQUIRE_FALSE(system.getBarlines()[0].getTimeSignature() == newTime);
        REQUIRE(system.getBarlines()[1].getTimeSignature() == newTime);
        REQUIRE(system.getBarlines()[2].getTimeSignature() == newTime);
        REQUIRE(!system.getBarlines()[2].getTimeSignature().isVisible());
    }

    action.undo();
    {
        const System &system = score.getSystems()[0];
        REQUIRE_FALSE(system.getBarlines()[1].getTimeSignature() == newTime);
        REQUIRE_FALSE(system.getBarlines()[2].getTimeSignature() == newTime);
    }
}
