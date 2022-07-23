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

#include "tempomarker.h"

#include <stdexcept>
#include <util/enumtostring.h>

const int TempoMarker::MIN_BEATS_PER_MINUTE = 1;
const int TempoMarker::MAX_BEATS_PER_MINUTE = 1000;
const int TempoMarker::DEFAULT_BEATS_PER_MINUTE = 120;

TempoMarker::TempoMarker()
    : myPosition(0),
      myMarkerType(StandardMarker),
      myBeatType(Quarter),
      myListessoBeatType(Quarter),
      myTripletFeel(NoTripletFeel),
      myAlterationOfPace(NoAlterationOfPace),
      myBeatsPerMinute(DEFAULT_BEATS_PER_MINUTE),
      myDescription("")
{
}

TempoMarker::TempoMarker(int position)
    : myPosition(position),
      myMarkerType(StandardMarker),
      myBeatType(Quarter),
      myListessoBeatType(Quarter),
      myTripletFeel(NoTripletFeel),
      myAlterationOfPace(NoAlterationOfPace),
      myBeatsPerMinute(DEFAULT_BEATS_PER_MINUTE),
      myDescription("")
{
}

bool TempoMarker::operator==(const TempoMarker &other) const
{
    return myPosition == other.myPosition &&
           myMarkerType == other.myMarkerType &&
           myBeatType == other.myBeatType &&
           myListessoBeatType == other.myListessoBeatType &&
           myTripletFeel == other.myTripletFeel &&
           myAlterationOfPace == other.myAlterationOfPace &&
           myBeatsPerMinute == other.myBeatsPerMinute &&
           myDescription == other.myDescription;
}

int TempoMarker::getPosition() const
{
    return myPosition;
}

void TempoMarker::setPosition(int position)
{
    myPosition = position;
}

TempoMarker::MarkerType TempoMarker::getMarkerType() const
{
    return myMarkerType;
}

void TempoMarker::setMarkerType(MarkerType type)
{
    myMarkerType = type;
}

TempoMarker::BeatType TempoMarker::getBeatType() const
{
    return myBeatType;
}

void TempoMarker::setBeatType(BeatType type)
{
    myBeatType = type;
}

TempoMarker::BeatType TempoMarker::getListessoBeatType() const
{
    return myListessoBeatType;
}

void TempoMarker::setListessoBeatType(BeatType type)
{
    myListessoBeatType = type;
}

TempoMarker::TripletFeelType TempoMarker::getTripletFeel() const
{
    return myTripletFeel;
}

void TempoMarker::setTripletFeel(TripletFeelType type)
{
    myTripletFeel = type;
}

TempoMarker::AlterationOfPaceType TempoMarker::getAlterationOfPace() const
{
    return myAlterationOfPace;
}

void TempoMarker::setAlterationOfPace(AlterationOfPaceType type)
{
    myAlterationOfPace = type;
}

int TempoMarker::getBeatsPerMinute() const
{
    return myBeatsPerMinute;
}

void TempoMarker::setBeatsPerMinute(int bpm)
{
    if (bpm < MIN_BEATS_PER_MINUTE || bpm > MAX_BEATS_PER_MINUTE)
        throw std::out_of_range("Invalid beats per minute");

    myBeatsPerMinute = bpm;
}

const std::string &TempoMarker::getDescription() const
{
    return myDescription;
}

void TempoMarker::setDescription(const std::string &description)
{
    myDescription = description;
}

using MarkerType = TempoMarker::MarkerType;
using BeatType = TempoMarker::BeatType;
using TripletFeelType = TempoMarker::TripletFeelType;
using AlterationOfPaceType = TempoMarker::AlterationOfPaceType;

UTIL_DEFINE_ENUMTOSTRING(MarkerType, {
    { MarkerType::NotShown, "NotShown" },
    { MarkerType::StandardMarker, "Standard" },
    { MarkerType::ListessoMarker, "Listesso" },
    { MarkerType::AlterationOfPace, "AlterationOfPace" },
})

UTIL_DEFINE_ENUMTOSTRING(BeatType, {
    { BeatType::Half, "Half" },
    { BeatType::HalfDotted, "HalfDotted" },
    { BeatType::Quarter, "Quarter" },
    { BeatType::QuarterDotted, "QuarterDotted" },
    { BeatType::Eighth, "Eighth" },
    { BeatType::EighthDotted, "EighthDotted" },
    { BeatType::Sixteenth, "Sixteenth" },
    { BeatType::SixteenthDotted, "SixteenthDotted" },
    { BeatType::ThirtySecond, "ThirtySecond" },
    { BeatType::ThirtySecondDotted, "ThirtySecondDotted" },
})

UTIL_DEFINE_ENUMTOSTRING(TripletFeelType, {
    { TripletFeelType::NoTripletFeel, "None" },
    { TripletFeelType::TripletFeelEighth, "Eighth" },
    { TripletFeelType::TripletFeelSixteenth, "Sixteenth" },
    { TripletFeelType::TripletFeelEighthOff, "EighthOff" },
    { TripletFeelType::TripletFeelSixteenthOff, "SixteenthOff" },
})

UTIL_DEFINE_ENUMTOSTRING(AlterationOfPaceType, {
    { AlterationOfPaceType::NoAlterationOfPace, "None" },
    { AlterationOfPaceType::Accelerando, "Accelerando" },
    { AlterationOfPaceType::Ritardando, "Ritardando" },
})
