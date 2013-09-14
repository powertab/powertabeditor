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

#ifndef TEST_ACTIONTESTS_H
#define TEST_ACTIONTESTS_H

#include <score/score.h>
#include <score/scorelocation.h>

struct ActionFixture
{
    ActionFixture()
        : myLocation(myScore)
    {
        System system;
        Staff staff(6);
        Position position(42);
        position.insertNote(Note(2, 3));
        position.insertNote(Note(5, 1));
        staff.insertPosition(0, position);

        system.insertStaff(staff);
        myScore.insertSystem(system);

        myLocation.setPositionIndex(42);
        myLocation.setSelectionStart(42);
        myLocation.setString(2);
    }

    Score myScore;
    ScoreLocation myLocation;
};

#endif
