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

#ifndef SCORE_POSITION_H
#define SCORE_POSITION_H

#include <boost/rational.hpp>
#include <boost/serialization/access.hpp>
#include <bitset>

namespace Score {

class Position
{
public:
    enum DurationType
    {
        WholeNote = 1,
        HalfNote = 2,
        QuarterNote = 4,
        EighthNote = 8,
        SixteenthNote = 16,
        ThirtySecondNote = 32,
        SixtyFourthNote = 64
    };

    enum SimpleProperty
    {
        Dotted,
        DoubleDotted,
        Rest,
        Vibrato,
        WideVibrato,
        ArpeggioUp,
        ArpeggioDown,
        PickStrokeUp,
        PickStrokeDown,
        Staccato,
        Marcato,
        Sforzando,
        TremoloPicking,
        PalmMuting,
        Tap,
        Acciaccatura,
        TripletFeelFirst,
        TripletFeelSecond,
        LetRing,
        Fermata,
        NumSimpleProperties
    };

    Position();
    Position(int position, DurationType duration = EighthNote);

    bool operator==(const Position &other) const;

    /// Returns the position within the staff where the position is anchored.
    int getPosition() const;
    /// Sets the position within the staff where the position is anchored.
    void setPosition(int position);

    /// Returns the position's duration type (e.g. half note).
    DurationType getDurationType() const;
    /// Sets the position's duration type (e.g. half note).
    void setDurationType(DurationType duration);

    /// Returns whether the position has vibrato, palm muting, etc.
    bool hasProperty(SimpleProperty property) const;
    /// Sets whether the position has vibrato, palm muting, etc.
    void setProperty(SimpleProperty property, bool set = true);

    /// Returns whether the position is a rest.
    bool isRest() const;
    /// Sets whether the position is a rest.
    void setRest(bool set = true);

private:
    int myPosition;
    DurationType myDurationType;
    std::bitset<NumSimpleProperties> mySimpleProperties;

    /// The total irregular group timing (if the object is part of a nested
    /// group, the timings are multiplied together).
    boost::rational<int> myIrregularGroupTiming;
    // TODO - add an external class for each irregular group.

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myPosition & myDurationType & mySimpleProperties &
             myIrregularGroupTiming;
    }
};

}

#endif
