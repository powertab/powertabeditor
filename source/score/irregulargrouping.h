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

class IrregularGrouping
{
public:
    IrregularGrouping();
    IrregularGrouping(int position, int length, int notesPlayed,
                      int notesPlayedOver);

    bool operator==(const IrregularGrouping &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the start position of the group.
    int getPosition() const;
    /// Sets the start position of the group.
    void setPosition(int position);

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

#endif
