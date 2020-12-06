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
#include <score/utils.h>

ConstScoreLocation::ConstScoreLocation(const Score &score, int system,
                                       int staff, int position, int voice,
                                       int string)
    : myScore(score),
      mySystemIndex(system),
      myStaffIndex(staff),
      myPositionIndex(position),
      mySelectionStart(position),
      myVoiceIndex(voice),
      myString(string)
{
}

ScoreLocation::ScoreLocation(Score &score, int system, int staff, int position,
                             int voice, int string)
    : ConstScoreLocation(score, system, staff, position, voice, string),
      myWriteableScore(score)
{
}

ScoreLocation::ScoreLocation(Score &score,
                             const ConstScoreLocation &src_location)
    : ConstScoreLocation(src_location), myWriteableScore(score)
{
    assert(&src_location.getScore() == &score);
}

Score &ScoreLocation::getScore()
{
    return myWriteableScore;
}

const Score &ConstScoreLocation::getScore() const
{
    return myScore;
}

int ConstScoreLocation::getPositionIndex() const
{
    return myPositionIndex;
}

void ConstScoreLocation::setPositionIndex(int position)
{
    myPositionIndex = position;
}

const Position *ConstScoreLocation::getPosition() const
{
    for (const Position &pos : getVoice().getPositions())
    {
        if (pos.getPosition() == myPositionIndex)
            return &pos;
    }

    return nullptr;
}

Position *ScoreLocation::getPosition()
{
    return const_cast<Position *>(ConstScoreLocation::getPosition());
}

const Position *ConstScoreLocation::findMultiBarRest() const
{
    const System &system = getSystem();
    const Barline *bar = getBarline();
    const Barline *nextBar = system.getNextBarline(bar->getPosition());

    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &pos : ScoreUtils::findInRange(
                     voice.getPositions(), bar->getPosition(),
                     nextBar->getPosition()))
            {
                if (pos.hasMultiBarRest())
                    return &pos;
            }
        }
    }

    return nullptr;
}

bool ConstScoreLocation::isEmptyBar() const
{
    const System &system = getSystem();
    const Barline *bar = getBarline();
    const Barline *nextBar = system.getNextBarline(bar->getPosition());

    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            if (!ScoreUtils::findInRange(voice.getPositions(),
                                         bar->getPosition(),
                                         nextBar->getPosition()).empty())
            {
                return false;
            }
        }
    }

    return true;
}

int ConstScoreLocation::getSelectionStart() const
{
    return mySelectionStart;
}

void ConstScoreLocation::setSelectionStart(int position)
{
    mySelectionStart = position;
}

bool ConstScoreLocation::hasSelection() const
{
    return mySelectionStart != myPositionIndex;
}

std::vector<Position *> ScoreLocation::getSelectedPositions()
{
    // Avoid duplicate logic between const and non-const versions.
    auto positions = ConstScoreLocation::getSelectedPositions();
    std::vector<Position *> nc_positions;
    for (const Position *pos : positions)
        nc_positions.push_back(const_cast<Position *>(pos));

    return nc_positions;
}

std::vector<const Position *> ConstScoreLocation::getSelectedPositions() const
{
    std::vector<const Position *> positions;
    const int min = std::min(myPositionIndex, mySelectionStart);
    const int max = std::max(myPositionIndex, mySelectionStart);

    for (const Position &pos : getVoice().getPositions())
    {
        if (pos.getPosition() >= min && pos.getPosition() <= max)
            positions.push_back(&pos);
    }

    return positions;
}

const Voice &ConstScoreLocation::getVoice() const
{
    return getStaff().getVoices()[myVoiceIndex];
}

Voice &ScoreLocation::getVoice()
{
    return getStaff().getVoices()[myVoiceIndex];
}

int ConstScoreLocation::getStaffIndex() const
{
    return myStaffIndex;
}

void ConstScoreLocation::setStaffIndex(int staff)
{
    myStaffIndex = staff;
}

const Staff &ConstScoreLocation::getStaff() const
{
    return getSystem().getStaves()[myStaffIndex];
}

Staff &ScoreLocation::getStaff()
{
    return getSystem().getStaves()[myStaffIndex];
}

int ConstScoreLocation::getSystemIndex() const
{
    return mySystemIndex;
}

void ConstScoreLocation::setSystemIndex(int system)
{
    mySystemIndex = system;
}

const System &ConstScoreLocation::getSystem() const
{
    return myScore.getSystems()[mySystemIndex];
}

System &ScoreLocation::getSystem()
{
    return myWriteableScore.getSystems()[mySystemIndex];
}

const Barline *ConstScoreLocation::getBarline() const
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

    for (Barline &bar : getSystem().getBarlines())
    {
        const int position = bar.getPosition();
        if (position > 0 && position >= min && position <= max)
            bars.push_back(&bar);
    }

    return bars;
}

int ConstScoreLocation::getString() const
{
    return myString;
}

void ConstScoreLocation::setString(int string)
{
    myString = string;
}

const Note *ConstScoreLocation::getNote() const
{
    const Position *position = getPosition();
    return position ? Utils::findByString(*position, myString) : nullptr;
}

Note *ScoreLocation::getNote()
{
    return const_cast<Note *>(ConstScoreLocation::getNote());
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
        for (Position *pos : selectedPositions)
        {
            for (Note &note : pos->getNotes())
                notes.push_back(&note);
        }
    }

    return notes;
}

int ConstScoreLocation::getVoiceIndex() const
{
    return myVoiceIndex;
}

void ConstScoreLocation::setVoiceIndex(int voice)
{
    myVoiceIndex = voice;
}

std::vector<const IrregularGrouping *>
ConstScoreLocation::getSelectedIrregularGroupings() const
{
    std::vector<const IrregularGrouping *> groups;
    const int min = std::min(myPositionIndex, mySelectionStart);
    const int max = std::max(myPositionIndex, mySelectionStart);
    const Voice &voice = getVoice();

    if (!hasSelection())
        return groups;

    for (const IrregularGrouping &group : voice.getIrregularGroupings())
    {
        const int groupLeft = group.getPosition();
        const int groupRight =
            voice.getPositions()[ScoreUtils::findIndexByPosition(
                                     voice.getPositions(), groupLeft) +
                                 group.getLength() - 1].getPosition();

        if (groupLeft >= min && groupRight <= max)
            groups.push_back(&group);
    }

    return groups;
}

std::ostream &operator <<(std::ostream &os, const ScoreLocation &location)
{
    os << "System: " << location.getSystemIndex() + 1;
    os << ", Staff: " << location.getStaffIndex() + 1;
    os << ", Position: " << location.getPositionIndex() + 1;
    os << ", String: " << location.getString() + 1;
    return os;
}
