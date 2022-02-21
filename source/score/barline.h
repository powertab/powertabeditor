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

#include "fileversion.h"
#include "keysignature.h"
#include "rehearsalsign.h"
#include "timesignature.h"

#include <optional>
#include <util/enumtostring_fwd.h>

class Barline
{
public:
    enum BarType
    {
        SingleBar,
        DoubleBar,
        FreeTimeBar,
        RepeatStart,
        RepeatEnd,
        DoubleBarFine
    };

    Barline();
    Barline(int position, BarType type, int repeatCount = 0);

    bool operator==(const Barline &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the position within the system where the barline is anchored.
    int getPosition() const;
    /// Sets the position within the system where the barline is anchored.
    void setPosition(int position);

    /// Returns the type of barline (single, repeat end, etc).
    BarType getBarType() const;
    /// Sets the type of barline (single, repeat end, etc).
    void setBarType(BarType type);

    /// Returns the repeat count (used by repeat end bars).
    int getRepeatCount() const;
    /// Sets the repeat count for repeat end bars.
    void setRepeatCount(int count);

    /// Returns the key signature for the bar.
    const KeySignature &getKeySignature() const;
    /// Sets the key signature for the bar.
    void setKeySignature(const KeySignature &key);

    /// Returns the time signature for the bar.
    const TimeSignature &getTimeSignature() const;
    /// Sets the time signature for the bar.
    void setTimeSignature(const TimeSignature &time);

    /// Returns whether the barline has a rehearsal sign.
    bool hasRehearsalSign() const;
    /// Returns the rehearsal sign for the bar.
    const RehearsalSign &getRehearsalSign() const;
    RehearsalSign &getRehearsalSign();
    /// Sets the rehearsal sign for the bar.
    void setRehearsalSign(const RehearsalSign &sign);
    /// Clears the rehearsal sign for the bar.
    void clearRehearsalSign();

    /// Minimum valid number of repeats.
    static const int MIN_REPEAT_COUNT;

private:
    int myPosition;
    BarType myBarType;
    int myRepeatCount;
    KeySignature myKeySignature;
    TimeSignature myTimeSignature;
    std::optional<RehearsalSign> myRehearsalSign;
};

UTIL_DECLARE_ENUMTOSTRING(Barline::BarType)

template <class Archive>
void Barline::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("position", myPosition);
    ar("bar_type", myBarType);
    ar("num_repeats", myRepeatCount);
    ar("key_signature", myKeySignature);
    ar("time_signature", myTimeSignature);
    ar("rehearsal_sign", myRehearsalSign);
}

#endif
