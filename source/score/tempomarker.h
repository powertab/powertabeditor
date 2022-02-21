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

#ifndef SCORE_TEMPOMARKER_H
#define SCORE_TEMPOMARKER_H

#include "fileversion.h"
#include <string>
#include <util/enumtostring_fwd.h>

class TempoMarker
{
public:
    enum MarkerType
    {
        NotShown,
        StandardMarker, ///< Standard bpm marker (i.e. quarter note = 120).
        ListessoMarker, ///< Listesso (i.e. quarter note = half note).
        AlterationOfPace ///< Accel. or rit.
    };

    enum BeatType
    {
        Half,
        HalfDotted,
        Quarter,
        QuarterDotted,
        Eighth,
        EighthDotted,
        Sixteenth,
        SixteenthDotted,
        ThirtySecond,
        ThirtySecondDotted
    };

    enum TripletFeelType
    {
        NoTripletFeel,
        TripletFeelEighth,
        TripletFeelSixteenth,
        TripletFeelEighthOff,
        TripletFeelSixteenthOff
    };

    enum AlterationOfPaceType
    {
        NoAlterationOfPace,
        Accelerando,
        Ritardando
    };

    TempoMarker();
    explicit TempoMarker(int position);

    bool operator==(const TempoMarker &other) const;

	template <class Archive>
	void serialize(Archive &ar, const FileVersion version);

    /// Returns the position within the system where the marker is anchored.
    int getPosition() const;
    /// Sets the position within the system where the marker is anchored.
    void setPosition(int position);

    /// Returns the type of tempo marker (standard, listesso, etc).
    MarkerType getMarkerType() const;
    /// Sets the type of tempo marker (standard, listesso, etc).
    void setMarkerType(MarkerType type);

    /// Returns the tempo marker's beat type (quarter, eighth, etc).
    BeatType getBeatType() const;
    /// Sets the tempo marker's beat type (quarter, eighth, etc).
    void setBeatType(BeatType type);

    /// Returns the tempo marker's listesso beat type (quarter, eighth, etc).
    BeatType getListessoBeatType() const;
    /// Sets the tempo marker's listesso beat type (quarter, eighth, etc).
    void setListessoBeatType(BeatType type);

    /// Returns the tempo marker's triplet feel (e.g. eighth).
    TripletFeelType getTripletFeel() const;
    /// Sets the tempo marker's triplet feel type.
    void setTripletFeel(TripletFeelType type);

    /// Returns whether the marker has an alteration of pace.
    AlterationOfPaceType getAlterationOfPace() const;
    /// Sets the marker's alteration of pace.
    void setAlterationOfPace(AlterationOfPaceType type);

    /// Returns the number of beats per minute for the beat type.
    int getBeatsPerMinute() const;
    /// Sets the number of beats per minute for the beat type.
    void setBeatsPerMinute(int bpm);

    /// Returns a description of the tempo marker.
    const std::string &getDescription() const;
    /// Sets the description of the tempo marker.
    void setDescription(const std::string &description);

    /// The minimum valid beats per minute.
    static const int MIN_BEATS_PER_MINUTE;
    /// The maximum valid beats per minute.
    static const int MAX_BEATS_PER_MINUTE;
    /// The default number of beats per minute.
    static const int DEFAULT_BEATS_PER_MINUTE;

private:
    int myPosition;
    MarkerType myMarkerType;
    BeatType myBeatType;
    BeatType myListessoBeatType;
    TripletFeelType myTripletFeel;
    AlterationOfPaceType myAlterationOfPace;
    int myBeatsPerMinute;
    std::string myDescription;
};

UTIL_DECLARE_ENUMTOSTRING(TempoMarker::MarkerType)
UTIL_DECLARE_ENUMTOSTRING(TempoMarker::BeatType)
UTIL_DECLARE_ENUMTOSTRING(TempoMarker::TripletFeelType)
UTIL_DECLARE_ENUMTOSTRING(TempoMarker::AlterationOfPaceType)

template <class Archive>
void TempoMarker::serialize(Archive &ar, const FileVersion /*version*/)
{
	ar("position", myPosition);
	ar("marker_type", myMarkerType);
	ar("beat_type", myBeatType);
	ar("listesso_type", myListessoBeatType);
	ar("triplet_feel", myTripletFeel);
	ar("alteration_of_pace", myAlterationOfPace);
	ar("bpm", myBeatsPerMinute);
	ar("description", myDescription);
}

#endif
