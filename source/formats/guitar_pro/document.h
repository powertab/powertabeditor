/*
  * Copyright (C) 2014 Cameron White
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

#ifndef FORMATS_GP_FILEFORMAT_H
#define FORMATS_GP_FILEFORMAT_H

#include <boost/optional/optional.hpp>
#include <string>
#include <vector>

/// Contains common code such as flags for use when reading Guitar Pro files
namespace Gp
{

class InputStream;

struct Header
{
    Header();
    void load(InputStream &stream);

    struct LyricLine
    {
        LyricLine(int measure, const std::string &contents);

        int myMeasure;
        std::string myContents;
    };

    std::string myTitle;
    std::string mySubtitle;
    std::string myArtist;
    std::string myAlbum;
    std::string myLyricist;
    std::string myComposer;
    std::string myCopyright;
    std::string myTranscriber;
    std::string myInstructions;
    std::vector<std::string> myNotices;
    bool myTripletFeel;
    int myLyricTrack;
    std::vector<LyricLine> myLyrics;
};

struct Channel
{
    Channel();
    void load(InputStream &stream);

    int myInstrument;
    int myVolume;
    int myBalance;
    int myChorus;
    int myReverb;
    int myPhaser;
    int myTremolo;

private:
    /// For some reason, channel properties (except instrument type) are stored
    /// in a different format, where 1 -> 7, 2 -> 15, ... , and 16 -> 127. This
    /// function reads the data and performs the necessary conversion.
    static uint8_t readChannelProperty(InputStream &stream);
};

struct Note
{
    Note(int string);
    void load(InputStream &stream);

    int myString;
    int myFret;
    bool myHasAccent;
    bool myHasHeavyAccent;
    bool myIsGhostNote;
    bool myIsTied;
    bool myIsMuted;
    /// Optional dynamic, where 1 = ppp, ... 9 = fff.
    boost::optional<int> myDynamic;
    boost::optional<int> myTrilledFret;
    bool myIsLetRing;
    bool myIsHammerOnOrPullOff;
    bool myHasPalmMute;
    bool myIsStaccato;
    bool myIsNaturalHarmonic;

private:
    void loadNoteEffects(InputStream &stream);
    void loadNoteEffectsGp3(InputStream &stream);
    void loadBend(InputStream &stream);
    void loadSlide(InputStream &stream);
    void loadHarmonic(InputStream &stream);
};

struct Beat
{
    Beat();
    void load(InputStream &stream);

    bool myIsDotted;
    bool myIsRest;
    int myDuration; ///< 4 -> quarter note, 8 -> eighth note, etc.
    boost::optional<int> myIrregularGrouping;
    boost::optional<std::string> myText;
    bool myIsVibrato;
    bool myIsNaturalHarmonic;
    bool myIsArtificialHarmonic;
    bool myArpeggioUp;
    bool myArpeggioDown;
    bool myIsTremoloPicked;
    bool myPickstrokeUp;
    bool myPickstrokeDown;
    bool myIsTapped;
    std::vector<Note> myNotes;

private:
    void loadChordDiagram(InputStream &stream);
    void loadBeatEffects(InputStream &stream);
    void loadTremoloBar(InputStream &stream);
    void loadMixTableChangeEvent(InputStream &stream);
    void loadNotes(InputStream &stream);
};

struct Staff
{
    Staff();
    void load(InputStream &stream);

    std::vector<Beat> myFirstVoice;
    std::vector<Beat> mySecondVoice;
};

struct Measure
{
    Measure();
    void load(InputStream &stream);
    void loadStaves(InputStream &stream, int numTracks);

    bool myIsDoubleBar;
    bool myIsRepeatBegin;
    /// The numerator and denominator if there is a time signature change,
    boost::optional<std::pair<int, int>> myTimeSignatureChange;
    /// If there is a repeat end, this contains the repeat count.
    boost::optional<int> myRepeatEnd;
    /// If there is a key change, this contains the key and whether it is
    /// minor.
    boost::optional<std::pair<int, bool>> myKeyChange;
    /// Optional rehearsal sign.
    boost::optional<std::string> myMarker;
    /// Optional alternate ending number.
    boost::optional<int> myAlternateEnding;

    /// One staff per track.
    std::vector<Staff> myStaves;

private:
    void loadMarker(InputStream &stream);
};

struct Track
{
    Track();
    void load(InputStream &stream);

    bool myIsDrumTrack;
    std::string myName;
    int myNumStrings;
    std::vector<int> myTuning;
    int myChannelIndex; ///< Index into the list of channels.
    int myCapo;
};

struct Document
{
    Document();
    void load(InputStream &stream);

    Header myHeader;
    int myStartTempo;
    int myInitialKey;
    bool myOctave8va;
    std::vector<Channel> myChannels;
    std::vector<Measure> myMeasures;
    std::vector<Track> myTracks;
};

/// Supported Guitar Pro file versions
enum Version
{
    Version3,
    Version4,
    Version5_0,
    Version5_1
};

#if 0
enum MeasureHeaderFlags
{
    Numerator,
    Denominator,
    RepeatBegin,
    RepeatEnd,
    AltEnding,
    Marker,
    KeySignatureChange,
    DoubleBar
};

enum BeatHeaderFlags
{
    Dotted,
    ChordDiagram,
    Text,
    NoteEffects,
    MixTableChangeEvent,
    IrregularGrouping,
    BeatStatus
};

enum NoteFlags
{
    TimeIndependentDuration,
    HeavyAccentedNote,
    GhostNote,
    HasNoteEffects,
    Dynamic,
    NoteType,
    AccentedNote,
    FingeringType
};

enum NoteTypes
{
    NormalNote = 0x01,
    TiedNote = 0x02,
    MutedNote = 0x03
};

enum PositionEffectFlags1
{
    VibratoGp3_1, // TODO - not sure what the difference is (perhaps strong vs
                  // normal vibrato?)
    VibratoGp3_2,
    NaturalHarmonicGp3,
    ArtificialHarmonicGp3,
    FadeInGp3,
    HasTapping = 5, // start of GP4-compatible flags
    HasStrokeEffect
};

enum PositionEffectFlags2
{
    HasRasguedo,
    Pickstroke,
    HasTremoloBarEvent
};

enum PickstrokeTypes
{
    PickstrokeNone,
    PickstrokeUp,
    PickstrokeDown
};

enum TappingTypes
{
    TremoloBarGp3 = 0,
    TappingNone = 0,
    Tapping,
    Slapping,
    Popping
};

enum NoteEffectFlags1
{
    HasBend,
    HasHammerOnOrPullOff,
    HasSlideOutVer3, ///< Slide out from the current note (used in gp3)
    HasLetRing,
    HasGraceNote
};

enum NoteEffectFlags2
{
    HasStaccato,
    HasPalmMute,
    HasTremoloPicking,
    HasSlide,
    HasHarmonic,
    HasTrill,
    HasVibrato
};

enum HarmonicTypes
{
    HarmonicNone = 0,
    NaturalHarmonic = 1,
    ArtificalHarmonicGp5 = 2,
    TappedHarmonic = 3,
    PinchHarmonic = 4,
    SemiHarmonic = 5,
    Artificial5 = 15,
    Artificial7 = 17,
    Artificial12 = 22
};

enum BendTypes
{
    BendNone,
    NormalBend,
    BendAndRelease,
    BendAndReleaseAndBend,
    Prebend,
    PrebendAndRelease,
    Dip,
    Dive,
    ReleaseUp,
    InvertedDip,
    TremoloReturn,
    ReleaseDown
};

enum BeatStatus
{
    BeatEmpty = 0x00,
    BeatRest = 0x02
};

enum MiscConstants
{
    NumberOfStrings = 7, ///< Max number of strings for an instrument
    NumberOfBarres = 5, ///< Max number of barres in a chord
    NumberOfStringsGp3 = 6 ///< Max number of strings for an instrument in GP3
};

enum FixedLengthStrings
{
    TrackDescriptionLength = 40,
    ChordDiagramDescriptionLength = 20
};

enum ChordDiagramFlags
{
    Gp4ChordFormat ///< Indicates that the chord diagram is in the Gp4 format
};
#endif
}

#endif
