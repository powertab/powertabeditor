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

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/// Contains common code such as flags for use when reading Guitar Pro files
namespace Gp
{

class InputStream;
struct Track;

struct Header
{
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
    /// TODO -  this should be transferred to the measure header since it's
    /// per-measure in GP5.
    bool myTripletFeel = false;
    int myLyricTrack = 0;
    std::vector<LyricLine> myLyrics;
};

struct Channel
{
    void load(InputStream &stream);

    int myInstrument = 0;
    int myVolume = 0;
    int myBalance = 0;
    int myChorus = 0;
    int myReverb = 0;
    int myPhaser = 0;
    int myTremolo = 0;

private:
    /// For some reason, channel properties (except instrument type) are stored
    /// in a different format, where 1 -> 7, 2 -> 15, ... , and 16 -> 127. This
    /// function reads the data and performs the necessary conversion.
    static uint8_t readChannelProperty(InputStream &stream);
};

struct GraceNote
{
    GraceNote();
    void load(InputStream &stream);

    enum Transition
    {
        NoTransition,
        SlideTransition,
        BendTransition,
        HammerTransition
    };

    int myFret;
    int myDynamic;
    Transition myTransition;
    int myDuration;
};

struct Bend
{
    struct Point
    {
        // Percentage of the note's duration where the point is placed.
        double myOffset = 0;
        // Percentage of a full step.
        int myValue = 0;
    };

    int myBendType = 0;
    int myBendValue = 0;
    std::vector<Point> myPoints;
};

struct Note
{
    Note(int string);
    void load(InputStream &stream, const Track &track);

    int myString;
    int myFret;
    std::optional<int> myLeftFinger;
    bool myHasAccent;
    bool myHasHeavyAccent;
    bool myIsGhostNote;
    bool myIsTied;
    bool myIsMuted;
    /// Optional dynamic, where 1 = ppp, ... 9 = fff.
    std::optional<int> myDynamic;
    std::optional<int> myTrilledFret;
    std::optional<GraceNote> myGraceNote;
    std::optional<Bend> myBend;
    bool myIsLetRing;
    bool myIsHammerOnOrPullOff;
    bool myHasPalmMute;
    bool myIsStaccato;
    bool myIsNaturalHarmonic;
    bool myIsTappedHarmonic = false;
    bool myIsArtificialHarmonic = false;
    double myHarmonicFret = 0;
    bool myIsVibrato;
    bool myIsTremoloPicked;

    bool myIsShiftSlide;
    bool myIsLegatoSlide;
    bool myIsSlideInAbove;
    bool myIsSlideInBelow;
    bool myIsSlideOutUp;
    bool myIsSlideOutDown;

private:
    void loadNoteEffects(InputStream &stream, const Track &track);
    void loadNoteEffectsGp3(InputStream &stream);
    void loadSlide(InputStream &stream);
    void loadHarmonic(InputStream &stream, const Track &track);
};

struct Beat
{
    Beat();
    void load(InputStream &stream, const Track &track);

    bool myIsEmpty;
    bool myIsDotted;
    bool myIsRest;
    int myDuration; ///< 4 -> quarter note, 8 -> eighth note, etc.
    std::optional<int> myIrregularGrouping;
    std::optional<std::string> myText;
    std::optional<int> myTempoChange;
    std::string myTempoChangeName;
    std::optional<Bend> myTremoloBar;
    bool myIsVibrato;
    bool myIsWideVibrato;
    bool myIsNaturalHarmonic;
    bool myIsArtificialHarmonic;
    bool myIsTremoloPicked;
    bool myPickstrokeUp;
    bool myPickstrokeDown;
    bool myIsTapped;
    bool myOctave8va;
    bool myOctave8vb;
    bool myOctave15ma;
    bool myOctave15mb;
    std::vector<Note> myNotes;

private:
    void loadChordDiagram(InputStream &stream);
    void loadOldChordDiagram(InputStream &stream);
    void loadBeatEffects(InputStream &stream);
    Bend loadTremoloBar(InputStream &stream);
    void loadMixTableChangeEvent(InputStream &stream);
    void loadNotes(InputStream &stream, const Track &track);
};

struct Staff
{
    Staff();
    void load(InputStream &stream, const Track &track);

    std::array<std::vector<Beat>, 2> myVoices;
};

struct Measure
{
    struct TimeSignatureChange
    {
        int myNumerator = 4;
        int myDenominator = 4;
    };

    struct KeySignatureChange
    {
        int myAccidentals = 0;
        bool myIsMinor = false;
    };

    Measure();
    void load(InputStream &stream);
    void loadStaves(InputStream &stream, const std::vector<Track> &tracks);

    bool myIsDoubleBar;
    bool myIsRepeatBegin;
    /// The numerator and denominator if there is a time signature change,
    std::optional<TimeSignatureChange> myTimeSignatureChange;
    /// If there is a repeat end, this contains the repeat count.
    std::optional<int> myRepeatEnd;
    /// If there is a key change, this contains the key and whether it is
    /// minor.
    std::optional<KeySignatureChange> myKeyChange;
    /// Optional rehearsal sign.
    std::optional<std::string> myMarker;
    /// Optional alternate ending number.
    std::optional<int> myAlternateEnding;

    /// One staff per track.
    std::vector<Staff> myStaves;

private:
    void loadMarker(InputStream &stream);
};

struct Track
{
    void load(InputStream &stream);

    bool myIsDrumTrack = false;
    std::string myName;
    int myNumStrings = 0;
    std::vector<uint8_t> myTuning;
    int myChannelIndex = -1; ///< Index into the list of channels.
    int myCapo = 0;
};

/// Index of the bar that each direction occurs at.
struct DirectionMap
{
    int myCoda = -1;
    int myDoubleCoda = -1;
    int mySegno = -1;
    int mySegnoSegno = -1;
    int myFine = -1;

    int myDaCapo = -1;
    int myDaCapoAlCoda = -1;
    int myDaCapoAlDoubleCoda = -1;
    int myDaCapoAlFine = -1;
    int myDaSegno = -1;
    int myDaSegnoAlCoda = -1;
    int myDaSegnoAlDoubleCoda = -1;
    int myDaSegnoAlFine = -1;
    int myDaSegnoSegno = -1;
    int myDaSegnoSegnoAlCoda = -1;
    int myDaSegnoSegnoAlDoubleCoda = -1;
    int myDaSegnoSegnoAlFine = -1;
    int myDaCoda = -1;
    int myDaDoubleCoda = -1;
};

struct Document
{
    void load(InputStream &stream);

    Header myHeader;
    int myStartTempo = 0;
    std::string myStartTempoName;
    bool myStartTempoVisible = true;
    int myInitialKey = 0;
    bool myOctave8va = false;
    std::vector<Channel> myChannels;
    DirectionMap myDirections;
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

}

#endif
