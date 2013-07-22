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

#include <actions/editkeysignature.h>
#include <score/score.h>

TEST_CASE("Actions/EditKeySignature", "")
{
    Score score;
    System system;
    system.insertBarline(Barline(6, Barline::SingleBar));
    score.insertSystem(system);

    const KeySignature newKey(KeySignature::Minor, 3, false);

    ScoreLocation location(score, 0, 0, 6);
    EditKeySignature action(location, newKey);

    action.redo();
    {
        const System &system = score.getSystems()[0];
        REQUIRE_FALSE(system.getBarlines()[0].getKeySignature() == newKey);
        REQUIRE(system.getBarlines()[1].getKeySignature() == newKey);
        REQUIRE(system.getBarlines()[2].getKeySignature() == newKey);
        REQUIRE(!system.getBarlines()[2].getKeySignature().isVisible());
    }

    action.undo();
    {
        const System &system = score.getSystems()[0];
        REQUIRE_FALSE(system.getBarlines()[1].getKeySignature() == newKey);
        REQUIRE_FALSE(system.getBarlines()[2].getKeySignature() == newKey);
    }
}
