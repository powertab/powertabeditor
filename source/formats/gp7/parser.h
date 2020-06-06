/*
 * Copyright (C) 2020 Cameron White
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

#ifndef FORMATS_GP7_PARSER_H
#define FORMATS_GP7_PARSER_H

#include "score/timesignature.h"
#include <bitset>
#include <optional>
#include <pugixml.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace Gp7
{
/// Contains metadata about the score.
struct ScoreInfo
{
    std::string myTitle;
    std::string mySubtitle;
    std::string myArtist;
    std::string myAlbum;
    std::string myWords;
    std::string myMusic;
    std::string myCopyright;
    std::string myTabber;
    std::string myInstructions;
    std::string myNotices;
    /// This seems to be the number of bars per system when in multi-track
    /// view.
    std::vector<int> myScoreSystemsLayout;
};

struct TempoChange
{
    enum class BeatType
    {
        Eighth,
        Quarter,
        QuarterDotted,
        Half,
        HalfDotted
    };

    /// Specifies the location within the bar, from 0 to 1. (e.g. if 0.75 and
    /// in 4/4 time, the tempo change occurs on the last beat).
    double myPosition = 0;
    /// Text to be displayed along with the tempo change.
    std::string myDescription;
    /// Whether to linearly interpolate speed until the next tempo marker.
    bool myIsLinear = false;
    /// Whether the tempo change is visible.
    bool myIsVisible = true;
    /// Tempo in beats per minute.
    int myBeatsPerMinute = -1;
    /// Unit that the beats are specified in.
    BeatType myBeatType = BeatType::Quarter;
};

struct Staff
{
    std::vector<int> myTuning;
    int myCapo = 0;
};

struct Sound
{
    std::string myLabel;
    int myMidiPreset = -1;
};

struct Track
{
    std::string myName;
    std::vector<int> mySystemsLayout;
    // A track typically has one staff, but can have two staves with different
    // tunings.
    std::vector<Staff> myStaves;
    /// There can be multiple sounds (although every staff in the track uses
    /// the same active sound). Automations describe when the sounds are
    /// changed.
    std::vector<Sound> mySounds;
};

struct MasterBar
{
    struct Section
    {
        std::string myLetter;
        std::string myText;
    };

    struct TimeSignature
    {
        bool operator==(const TimeSignature &other) const;
        bool operator!=(const TimeSignature &other) const;

        int myBeats = 4;
        int myBeatValue = 4;
    };

    struct KeySignature
    {
        bool operator==(const KeySignature &other) const;
        bool operator!=(const KeySignature &other) const;

        int myAccidentalCount = 0;
        bool myMinor = false;
        bool mySharps = false;
    };

    std::vector<int> myBarIds;
    std::optional<Section> mySection;
    TimeSignature myTimeSig;
    KeySignature myKeySig;
    bool myDoubleBar = false;
    bool myFreeTime = false;
    bool myRepeatStart = false;
    bool myRepeatEnd = false;
    int myRepeatCount = 0;

    std::vector<TempoChange> myTempoChanges;
};

struct Bar
{
    enum class ClefType
    {
        G2,
        F4,
        C3,
        C4,
        /// This is the clef type for drums.
        Neutral
    };

    ClefType myClefType = ClefType::G2;
    std::vector<int> myVoiceIds;
};

struct Voice
{
    std::vector<int> myBeatIds;
};

struct Beat
{
    enum class Ottavia
    {
        O8va,
        O8vb,
        O15ma,
        O15mb
    };

    int myRhythmId = -1;
    /// If there aren't any note ids, this is a rest!
    std::vector<int> myNoteIds;
    std::optional<Ottavia> myOttavia;
    bool myTremoloPicking = false;
    bool myGraceNote = false;
    bool myBrushUp = false;
    bool myBrushDown = false;
    bool myArpeggioDown = false;
    bool myArpeggioUp = false;
};

/// A duration, which is shared across many beats.
struct Rhythm
{
    /// Duration, where a whole note is 1, quarter note is 4, etc.
    int myDuration = 4;
    /// Number of dots (0-2).
    int myDots = 0;
    /// Numerator and denominator of the tuplet the note belongs to.
    int myTupletNum = 0;
    int myTupletDenom = 0;
};

struct Note
{
    enum HarmonicType
    {
        Natural,
        Artificial,
        Tap,
        Semi,
        Feedback
    };

    enum class AccentType : int
    {
        Staccato,
        Staccatissimo,
        Accent,
        HeavyAccent,
        Tenuto,
        NumTypes
    };

    enum class SlideType : int
    {
        Shift,
        Legato,
        SlideOutDown,
        SlideOutUp,
        SlideInBelow,
        SlideInAbove,
        NumTypes
    };

    int myString = 0;
    int myFret = 0;
    bool myPalmMuted = false;
    bool myMuted = false;
    bool myTied = false;
    bool myGhost = false;
    bool myTapped = false;
    bool myHammerOn = false;
    bool myLeftHandTapped = false;
    bool myVibrato = false;
    bool myWideVibrato = false;
    bool myLetRing = false;
    /// Flags for various combinations of accent types.
    std::bitset<size_t(AccentType::NumTypes)> myAccentTypes;
    std::optional<HarmonicType> myHarmonic;
    double myHarmonicFret = 0;
    /// Flags for various combinations of slide types.
    std::bitset<size_t(SlideType::NumTypes)> mySlideTypes;
    std::optional<int> myTrillNote;
};

/// Container for a Guitar Pro 7 document.
struct Document
{
    ScoreInfo myScoreInfo;
    std::vector<Track> myTracks;
    std::vector<MasterBar> myMasterBars;
    std::unordered_map<int, Bar> myBars;
    std::unordered_map<int, Voice> myVoices;
    std::unordered_map<int, Beat> myBeats;
    std::unordered_map<int, Note> myNotes;
    std::unordered_map<int, Rhythm> myRhythms;
};

/// Parses the score.gpif XML file.
Document parse(const pugi::xml_document &root);

} // namespace Gp7

#endif
