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

#include "scorelocation.h"

#include <score/score.h>

ScoreLocation::ScoreLocation(const Score &score, int system, int staff,
                             int position, int voice, int string)
    : myScore(score),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPositionIndex(position),
      myVoice(voice),
      myString(string)
{
}

const Score &ScoreLocation::getScore() const
{
    return myScore;
}

int ScoreLocation::getPositionIndex() const
{
    return myPositionIndex;
}

void ScoreLocation::setPositionIndex(int position)
{
    myPositionIndex = position;
}

int ScoreLocation::getStaffIndex() const
{
    return myStaffIndex;
}

void ScoreLocation::setStaffIndex(int staff)
{
    myStaffIndex = staff;
}

const Staff &ScoreLocation::getStaff() const
{
    return getSystem().getStaves()[myStaffIndex];
}

int ScoreLocation::getSystemIndex() const
{
    return mySystemIndex;
}

void ScoreLocation::setSystemIndex(int system)
{
    mySystemIndex = system;
}

const System &ScoreLocation::getSystem() const
{
    return myScore.getSystems()[mySystemIndex];
}

int ScoreLocation::getString() const
{
    return myString;
}

void ScoreLocation::setString(int string)
{
    myString = string;
}

int ScoreLocation::getVoice() const
{
    return myVoice;
}

void ScoreLocation::setVoice(int voice)
{
    myVoice = voice;
}
