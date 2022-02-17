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

#ifndef FORMATS_GP7_DOCUMENT_H
#define FORMATS_GP7_DOCUMENT_H

#include <bitset>
#include <boost/rational.hpp>
#include <boost/functional/hash.hpp>
#include <optional>
#include <pugixml.hpp>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace Gp7
{
/// Guitar Pro 6 files (.gpx) are very similar.
enum class Version
{
    V6,
    V7
};

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
        Eighth = 1,
        Quarter = 2,
        QuarterDotted = 3,
        Half = 4,
        HalfDotted = 5
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
    // Unique name, path, and role. Used for sound changes.
    std::string myName;
    std::string myPath;
    std::string myRole = "User";
    int myMidiPreset = -1;
};

struct ChordName
{
    struct Note
    {
        std::string myStep;
        int myAccidental = 0;
    };

    struct Degree
    {
        enum class Alteration
        {
            Perfect,
            Augmented,
            Diminished,
            Major,
            Minor
        };

        Alteration myAlteration = Alteration::Perfect;
        bool myOmitted = false;
    };

    Note myKeyNote;
    Note myBassNote;

    std::optional<Degree> mySecond;
    std::optional<Degree> myThird;
    std::optional<Degree> myFourth;
    std::optional<Degree> myFifth;
    std::optional<Degree> mySixth;
    std::optional<Degree> mySeventh;
    std::optional<Degree> myNinth;
    std::optional<Degree> myEleventh;
    std::optional<Degree> myThirteenth;
};

struct ChordDiagram
{
    int myBaseFret = 0;
    std::vector<int> myFrets;
};

struct Chord
{
    ChordName myName;
    ChordDiagram myDiagram;
};

struct SoundChange
{
    int myBar = -1;
    double myPosition = -1; // Number of beats within the bar.
    int mySoundId = -1;
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
    std::vector<SoundChange> mySoundChanges;
    // For export only. MIDI channels are normally automatically determined.
    int myMidiChannel = -1;

    std::unordered_map<int, Chord> myChords;
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

    enum class DirectionTarget
    {
        Fine,
        Coda,
        DoubleCoda,
        Segno,
        SegnoSegno
    };

    enum class DirectionJump
    {
        DaCapo,
        DaCapoAlCoda,
        DaCapoAlDoubleCoda,
        DaCapoAlFine,
        DaSegno,
        DaSegnoAlCoda,
        DaSegnoAlDoubleCoda,
        DaSegnoAlFine,
        DaSegnoSegno,
        DaSegnoSegnoAlCoda,
        DaSegnoSegnoAlDoubleCoda,
        DaSegnoSegnoAlFine,
        DaCoda,
        DaDoubleCoda
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
    std::vector<int> myAlternateEndings;

    std::vector<DirectionTarget> myDirectionTargets;
    std::vector<DirectionJump> myDirectionJumps;

    // These are actually stored on the MasterTrack in the XML data, but are
    // stored on the appropriate master bar for convenience
    std::vector<TempoChange> myTempoChanges;

    /// Fermatas apply to all tracks, and are applied at a specific beat
    /// fraction.
    std::set<boost::rational<int>> myFermatas;
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

/// A bend or whammy event (both have the same data).
struct Bend
{
    double myOriginValue = 0;
    double myOriginOffset = 0;

    double myMiddleValue = 0;
    double myMiddleOffset1 = 0;
    double myMiddleOffset2 = 0;

    double myDestValue = 0;
    double myDestOffset = 0;
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
    std::optional<int> myChordId;
    /// If there aren't any note ids, this is a rest!
    std::vector<int> myNoteIds;
    std::optional<Ottavia> myOttavia;
    bool myTremoloPicking = false;
    bool myGraceNote = false;
    bool myBrushUp = false;
    bool myBrushDown = false;
    bool myArpeggioDown = false;
    bool myArpeggioUp = false;
    std::string myFreeText;
    std::optional<Bend> myWhammy;
};

/// A duration, which is shared across many beats.
struct Rhythm
{
    bool operator==(const Rhythm&other) const
    {
        return myDuration == other.myDuration && myDots == other.myDots &&
               myTupletNum == other.myTupletNum &&
               myTupletDenom == other.myTupletDenom;
    }

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
        Pinch,
        Tap,
        Semi,
        Feedback
    };

    enum class AccentType : int
    {
        Staccato,
        Staccatissimo,
        HeavyAccent,
        Accent,
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

    enum class FingerType
    {
        Open,
        C,
        A,
        M,
        I,
        P
    };

    struct Pitch
    {
        char myNote = 'A';
        std::string myAccidental;
        int myOctave = 0;
    };

    int myString = 0;
    int myFret = 0;
    Pitch myConcertPitch; // Used only for export
    Pitch myTransposedPitch;

    bool myPalmMuted = false;
    bool myMuted = false;
    bool myTieOrigin = false;
    bool myTieDest = false;
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
    std::optional<FingerType> myLeftFinger;
    std::optional<Bend> myBend;
};

/// Container for a Guitar Pro 7 document.
struct Document
{
    void addBar(MasterBar &master_bar, Bar bar);
    void addVoice(Bar &bar, Voice voice);
    void addBeat(Voice &voice, Beat beat);
    void addNote(Beat &beat, Note note);
    void addRhythm(Beat &beat, Rhythm rhythm);

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
Document from_xml(const pugi::xml_document &root, Version version);

/// Creates the score.gpif XML file from the intermediate representation.
pugi::xml_document to_xml(const Document &doc);

} // namespace Gp7

/// Support for std::unordered_set etc
template<>
struct std::hash<Gp7::Rhythm>
{
    size_t operator()(const Gp7::Rhythm &rhythm) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, rhythm.myDuration);
        boost::hash_combine(seed, rhythm.myDots);
        boost::hash_combine(seed, rhythm.myTupletNum);
        boost::hash_combine(seed, rhythm.myTupletDenom);
        return seed;
    }
};

#endif
