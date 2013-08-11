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

#ifndef PAINTERS_STDNOTATIONNOTE_H
#define PAINTERS_STDNOTATIONNOTE_H

#include <QChar>
#include <vector>

class KeySignature;
class Note;
class Position;
class Score;
class Staff;
class System;
class Tuning;

class StdNotationNote
{
public:
    StdNotationNote(const Position &pos, const Note &note, double y);

    static std::vector<StdNotationNote> getNotesInStaff(
            const Score &score, const System &system, int systemIndex,
            const Staff &staff, int staffIndex);

    double getY() const;
    QChar getNoteHeadSymbol() const;
    int getPosition() const;

private:
    /// Return the offset of the note from the top of the staff.
    static double getNoteLocation(const Staff &staff, const Note &note,
                                  const KeySignature &key, const Tuning &tuning);
    /// Returns the number of octaves (from -2 to 2) that the note is shifted by.
    static int getOctaveOffset(const Note &note);

    double myY;
    QChar myNoteHeadSymbol;
    int myPosition;
};

#endif
