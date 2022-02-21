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

#ifndef SCORE_TIMESIGNATURE_H
#define SCORE_TIMESIGNATURE_H

#include <array>
#include "fileversion.h"
#include <util/enumtostring_fwd.h>

class TimeSignature
{
public:
    enum MeterType
    {
        Normal,
        CutTime,
        CommonTime
    };

    TimeSignature();

    bool operator==(const TimeSignature &other) const;

	template <class Archive>
	void serialize(Archive &ar, const FileVersion version);

    /// Returns the type of meter (normal, cut time, or common time).
    MeterType getMeterType() const;
    /// Sets the type of meter (normal, cut time, or common time).
    void setMeterType(MeterType type);

    /// Returns the number of beats in the measure.
    int getBeatsPerMeasure() const;
    /// Sets the number of beats in the measure.
    void setBeatsPerMeasure(int beats);

    /// Determines whether a beat value is valid.
    static bool isValidBeatValue(int beatValue);
    /// Returns the beat value (quarter note == 4, eighth note == 8, etc).
    int getBeatValue() const;
    /// Sets the beat value for the measure.
    void setBeatValue(int beatValue);

    typedef std::array<int, 4> BeamingPattern;
    /// Returns the beaming pattern for the measure.
    BeamingPattern getBeamingPattern() const;
    /// Sets the beaming pattern for the measure.
    void setBeamingPattern(const BeamingPattern &pattern);

    /// Returns whether a number of metronome pulses is valid.
    bool isValidNumPulses(int pulses) const;
    /// Returns the number of metronome pulses in the measure.
    int getNumPulses() const;
    /// Sets the number of metronome pulses in the measure.
    void setNumPulses(int pulses);

    /// Returns whether the time signature should be displayed.
    bool isVisible() const;
    /// Sets whether the time signature should be displayed.
    void setVisible(bool visible = true);

    /// Minimum valid number of beats in a measure.
    static const int MIN_BEATS_PER_MEASURE;
    /// Maximum valid number of beats in a measure.
    static const int MAX_BEATS_PER_MEASURE;
    /// Minimum valid number of metronome pulses in a measure.
    static const int MIN_PULSES;
    /// Maximum valid number of metronome pulses in a measure.
    static const int MAX_PULSES;

private:
    MeterType myMeterType;
    int myBeatsPerMeasure;
    int myBeatValue;
    BeamingPattern myBeamingPattern;
    int myNumPulses;
    bool myIsVisible;
};

UTIL_DECLARE_ENUMTOSTRING(TimeSignature::MeterType)

template <class Archive>
void TimeSignature::serialize(Archive &ar, const FileVersion /*version*/)
{
	ar("meter_type", myMeterType);
	ar("num_beats", myBeatsPerMeasure);
	ar("beat_value", myBeatValue);
	ar("pattern", myBeamingPattern);
	ar("num_pulses", myNumPulses);
	ar("visible", myIsVisible);
}

#endif
