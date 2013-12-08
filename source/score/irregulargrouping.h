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

#ifndef SCORE_IRREGULARGROUPING_H
#define SCORE_IRREGULARGROUPING_H

#include "fileversion.h"
#include <iosfwd>

class IrregularGrouping
{
public:
    IrregularGrouping();
    IrregularGrouping(int position, int length, int notesPlayed,
                      int notesPlayedOver);

    bool operator==(const IrregularGrouping &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the index of the start position of the group.
    int getPosition() const;
    /// Sets the index of start position of the group.
    void setPosition(int position);

    /// Returns the number of positions in the group.
    int getLength() const;
    /// Sets the number of positions in the group.
    void setLength(int length);

    /// Returns the numerator of the irregular group (i.e. 3 for a triplet).
    int getNotesPlayed() const;
    /// Sets the numerator of the irregular group (i.e. 3 for a triplet).
    void setNotesPlayed(int notes);

    /// Return the denominator of the irregular group (i.e. 2 for a triplet).
    int getNotesPlayedOver() const;
    /// Sets the denominator of the irregular group (i.e. 2 for a triplet).
    void setNotesPlayedOver(int notes);

private:
    int myPosition;
    int myLength;
    int myNotesPlayed;
    int myNotesPlayedOver;
};

template <class Archive>
void IrregularGrouping::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("position", myPosition);
    ar("length", myLength);
    ar("notes_played", myNotesPlayed);
    ar("notes_played_over", myNotesPlayedOver);
}

std::ostream &operator<<(std::ostream &os, const IrregularGrouping &group);

#endif
