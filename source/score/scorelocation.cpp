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

#include <boost/foreach.hpp>
#include <score/score.h>
#include <score/utils.h>

ScoreLocation::ScoreLocation(const Score &score, int system, int staff,
                             int position, int voice, int string)
    : myScore(score),
      myWriteableScore(0),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPositionIndex(position),
      mySelectionStart(position),
      myVoice(voice),
      myString(string)
{
}

ScoreLocation::ScoreLocation(Score &score, int system, int staff, int position,
                             int voice, int string)
    : myScore(score),
      myWriteableScore(&score),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPositionIndex(position),
      mySelectionStart(position),
      myVoice(voice),
      myString(string)
{
}

Score &ScoreLocation::getScore()
{
    return *myWriteableScore;
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

const Position *ScoreLocation::getPosition() const
{
    BOOST_FOREACH(const Position &pos, getStaff().getVoice(myVoice))
    {
        if (pos.getPosition() == myPositionIndex)
            return &pos;
    }

    return NULL;
}

Position *ScoreLocation::getPosition()
{
    return const_cast<Position *>(
                const_cast<const ScoreLocation &>(*this).getPosition());
}

int ScoreLocation::getSelectionStart() const
{
    return mySelectionStart;
}

void ScoreLocation::setSelectionStart(int position)
{
    mySelectionStart = position;
}

bool ScoreLocation::hasSelection() const
{
    return mySelectionStart != myPositionIndex;
}

std::vector<Position *> ScoreLocation::getSelectedPositions()
{
    std::vector<Position *> positions;
    const int min = std::min(myPositionIndex, mySelectionStart);
    const int max = std::max(myPositionIndex, mySelectionStart);

    BOOST_FOREACH(Position &pos, getStaff().getVoice(myVoice))
    {
        if (pos.getPosition() >= min && pos.getPosition() <= max)
            positions.push_back(&pos);
    }

    return positions;
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

Staff &ScoreLocation::getStaff()
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

System &ScoreLocation::getSystem()
{
    if (!myWriteableScore)
        throw std::logic_error("Cannot write to score");

    return myWriteableScore->getSystems()[mySystemIndex];
}

const Barline *ScoreLocation::getBarline() const
{
    return ScoreUtils::findByPosition(getSystem().getBarlines(),
                                      getPositionIndex());
}

Barline *ScoreLocation::getBarline()
{
    return ScoreUtils::findByPosition(getSystem().getBarlines(),
                                      getPositionIndex());
}

std::vector<Barline *> ScoreLocation::getSelectedBarlines()
{
    std::vector<Barline *> bars;
    const int min = std::min(myPositionIndex, mySelectionStart);
    const int max = std::max(myPositionIndex, mySelectionStart);

    BOOST_FOREACH(Barline &bar, getSystem().getBarlines())
    {
        const int position = bar.getPosition();
        if (position > 0 && position >= min && position <= max)
            bars.push_back(&bar);
    }

    return bars;
}

int ScoreLocation::getString() const
{
    return myString;
}

void ScoreLocation::setString(int string)
{
    myString = string;
}

const Note *ScoreLocation::getNote() const
{
    const Position *position = getPosition();
    return position ? Utils::findByString(*position, myString) : NULL;
}

Note *ScoreLocation::getNote()
{
    return const_cast<Note *>(
                const_cast<const ScoreLocation &>(*this).getNote());
}

std::vector<Note *> ScoreLocation::getSelectedNotes()
{
    std::vector<Note *> notes;

    if (!hasSelection())
    {
        if (getNote())
            notes.push_back(getNote());
    }
    else
    {
        std::vector<Position *> selectedPositions(getSelectedPositions());
        BOOST_FOREACH(Position *pos, selectedPositions)
        {
            BOOST_FOREACH(Note &note, pos->getNotes())
            {
                notes.push_back(&note);
            }
        }
    }

    return notes;
}

int ScoreLocation::getVoice() const
{
    return myVoice;
}

void ScoreLocation::setVoice(int voice)
{
    myVoice = voice;
}

std::ostream &operator <<(std::ostream &os, const ScoreLocation &location)
{
    os << "System: " << location.getSystemIndex() + 1;
    os << ", Staff: " << location.getStaffIndex() + 1;
    os << ", Posiiton: " << location.getPositionIndex() + 1;
    os << ", String: " << location.getString() + 1;
    return os;
}
