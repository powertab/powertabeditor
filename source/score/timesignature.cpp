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

#include "timesignature.h"

#include <stdexcept>
#include <util/enumtostring.h>

const int TimeSignature::MIN_BEATS_PER_MEASURE = 1;
const int TimeSignature::MAX_BEATS_PER_MEASURE = 32;
const int TimeSignature::MIN_PULSES = 0;
const int TimeSignature::MAX_PULSES = 32;

TimeSignature::TimeSignature()
    : myMeterType(Normal),
      myBeatsPerMeasure(4),
      myBeatValue(4),
      myNumPulses(4),
      myIsVisible(false)
{
    myBeamingPattern[0] = 4;
    myBeamingPattern[1] = 0;
    myBeamingPattern[2] = 0;
    myBeamingPattern[3] = 0;
}

bool TimeSignature::operator==(const TimeSignature &other) const
{
    return myMeterType == other.myMeterType &&
           myBeatsPerMeasure == other.myBeatsPerMeasure &&
           myBeatValue == other.myBeatValue &&
           myBeamingPattern == other.myBeamingPattern &&
           myNumPulses == other.myNumPulses &&
           myIsVisible == other.myIsVisible;
}

TimeSignature::MeterType TimeSignature::getMeterType() const
{
    return myMeterType;
}

void TimeSignature::setMeterType(MeterType type)
{
    // FIXME - update the metronome pulses and beaming pattern automatically.
    switch (type)
    {
    case Normal:
        break;
    case CutTime:
        myBeatsPerMeasure = 2;
        myBeatValue = 2;
        break;
    case CommonTime:
        myBeatsPerMeasure = 4;
        myBeatValue = 4;
        break;
    }

    myMeterType = type;
}

int TimeSignature::getBeatsPerMeasure() const
{
    return myBeatsPerMeasure;
}

void TimeSignature::setBeatsPerMeasure(int beats)
{
    if (beats < MIN_BEATS_PER_MEASURE || beats > MAX_BEATS_PER_MEASURE)
        throw std::out_of_range("Invalid beats per measure");

    myBeatsPerMeasure = beats;
}

bool TimeSignature::isValidBeatValue(int beatValue)
{
    return beatValue == 1 || beatValue == 2 || beatValue == 4 ||
           beatValue == 8 || beatValue == 16 || beatValue == 32;
}

int TimeSignature::getBeatValue() const
{
    return myBeatValue;
}

void TimeSignature::setBeatValue(int beatValue)
{
    if (!isValidBeatValue(beatValue))
        throw std::out_of_range("Invalid beat value");

    myBeatValue = beatValue;
}

TimeSignature::BeamingPattern TimeSignature::getBeamingPattern() const
{
    return myBeamingPattern;
}

void TimeSignature::setBeamingPattern(const BeamingPattern &pattern)
{
    if (pattern[0] == 0)
        throw std::logic_error("Invalid beaming pattern");

    myBeamingPattern = pattern;
}

bool TimeSignature::isValidNumPulses(int pulses) const
{
    // The number of pulses must divide evenly into the number of beats.
    return pulses <= MAX_PULSES &&
            (pulses == 0 || getBeatsPerMeasure() % pulses == 0);
}

int TimeSignature::getNumPulses() const
{
    return myNumPulses;
}

void TimeSignature::setNumPulses(int pulses)
{
    // The number of pulses must divide evenly into the number of beats.
    if (!isValidNumPulses(pulses))
        throw std::out_of_range("Invalid number of pulses");

    myNumPulses = pulses;
}

bool TimeSignature::isVisible() const
{
    return myIsVisible;
}

void TimeSignature::setVisible(bool visible)
{
    myIsVisible = visible;
}

using MeterType = TimeSignature::MeterType;

UTIL_DEFINE_ENUMTOSTRING(MeterType, {
    { MeterType::Normal, "Normal" },
    { MeterType::CutTime, "CutTime" },
    { MeterType::CommonTime, "CommonTime" },
})
