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

#ifndef SCORE_BARLINE_H
#define SCORE_BARLINE_H

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include "keysignature.h"
#include "rehearsalsign.h"
#include "timesignature.h"

namespace Score {

class Barline
{
public:
    enum BarType
    {
        SingleBar,
        DoubleBar,
        FreeTimeBar,
        RepeatStart,
        OpenBar,
        RepeatEnd,
        DoubleBarFine
    };

    Barline();

    bool operator==(const Barline &other) const;

    /// Returns the type of barline (single, repeat end, etc).
    BarType getBarType() const;
    /// Sets the type of barline (single, repeat end, etc).
    void setBarType(BarType type);

    /// Returns the repeat count (used by repeat end bars).
    int getRepeatCount() const;
    /// Sets the repeat count for repeat end bars.
    void setRepeatCount(int count);

    /// Returns the position within the system where the barline is anchored.
    int getPosition() const;
    /// Sets the position within the system where the barline is anchored.
    void setPosition(int position);

    /// Returns the key signature for the bar.
    const KeySignature &getKeySignature() const;
    /// Sets the key signature for the bar.
    void setKeySignature(const KeySignature &key);

    /// Returns the time signature for the bar.
    const TimeSignature &getTimeSignature() const;
    /// Sets the time signature for the bar.
    void setTimeSignature(const TimeSignature &time);

    /// Returns the rehearsal sign for the bar, if one exists.
    const boost::optional<RehearsalSign> &getRehearsalSign() const;
    /// Sets (or clears) the rehearsal sign for the bar.
    void setRehearsalSign(const boost::optional<RehearsalSign> &sign);

private:
    BarType myBarType;
    int myRepeatCount;
    int myPosition;
    KeySignature myKeySignature;
    TimeSignature myTimeSignature;
    boost::optional<RehearsalSign> myRehearsalSign;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myBarType & myRepeatCount & myPosition & myKeySignature &
             myTimeSignature & myRehearsalSign;
    }
};

}

#endif

