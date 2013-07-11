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

#ifndef SCORE_NOTE_H
#define SCORE_NOTE_H

#include <bitset>
#include <boost/serialization/access.hpp>

namespace Score {

class Note
{
public:
    enum SimpleProperty
    {
        Tied,
        Muted,
        HammerOn,
        PullOff,
        HammerOnFromNowhere,
        PullOffToNowhere,
        NaturalHarmonic,
        GhostNote,
        Octave8va,
        Octave15ma,
        Octave8vb,
        Octave15mb,
        NumSimpleProperties
    };

    Note();
    Note(int string, int fretNumber);

    bool operator==(const Note &other) const;

    /// Returns the string that the note is located on.
    int getString() const;
    /// Sets the string that the note is located on.
    void setString(int string);

    /// Returns the fret number that the note is located at.
    int getFretNumber() const;
    /// Sets the fret number where the note is located.
    void setFretNumber(int fret);

    /// Returns whether the note is tied, muted, etc.
    bool hasProperty(SimpleProperty property) const;
    /// Sets whether the note is tied, muted, etc.
    void setProperty(SimpleProperty property, bool set = true);

private:
    int myString;
    int myFretNumber;
    std::bitset<NumSimpleProperties> mySimpleProperties;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myString & myFretNumber & mySimpleProperties;
    }
};

}

#endif
