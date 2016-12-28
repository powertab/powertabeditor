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

#ifndef SCORE_VOICEUTILS_H
#define SCORE_VOICEUTILS_H

#include <boost/rational.hpp>
#include <vector>

class ScoreLocation;
class IrregularGrouping;
class Note;
class Position;
class Voice;

namespace VoiceUtils
{
/// Finds the corresponding voice in a following (or preceding) system.
const Voice *getAdjacentVoice(const ScoreLocation &location, int offset);

/// Returns the next position in the staff, if it exists.
const Position *getNextPosition(const Voice &voice, int position);

/// Returns the previous position in the staff, if it exists.
const Position *getPreviousPosition(const Voice &voice, int position);

/// Finds the next note in the staff on the given string. Optionally, the next
/// system can be checked.
const Note *getNextNote(const Voice &voice, int position, int string,
                        const Voice *nextVoice = nullptr);

/// Finds the previous note in the staff on the given string.
const Note *getPreviousNote(const Voice &voice, int position, int string,
                            const Voice *prevVoice = nullptr);

/// Determines whether the specified note can be tied to the previous note.
/// The previous position must have a note at the same string and fret.
bool canTieNote(const Voice &voice, int position, const Note &note);

/// Determines whether the specified note can have a hammeron or pulloff.
/// The next position must have a note at the same string and a different fret.
bool canHammerOnOrPullOff(const Voice &voice, int position, const Note &note);

/// Determines whether the position contains a note with a hammeron.
bool hasNoteWithHammerOn(const Voice &voice, const Position &pos);

/// Finds all irregular groups that overlap with the given range of positions.
std::vector<const IrregularGrouping *> getIrregularGroupsInRange(
    const Voice &voice, int left, int right);

/// Returns the note duration, including dots, irregular groupings, etc.
/// This does not include tempo, and the durations are relative to a
/// quarter note (i.e. a quarter note is 1, eighth note is 1/2, etc).
boost::rational<int> getDurationTime(const Voice &voice, const Position &pos);
}

#endif
