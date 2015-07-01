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
    boost::optional<GraceNote> myGraceNote;
    bool myIsLetRing;
    bool myIsHammerOnOrPullOff;
    bool myHasPalmMute;
    bool myIsStaccato;
    bool myIsNaturalHarmonic;
    bool myIsVibrato;
    bool myIsTremoloPicked;

    bool myIsShiftSlide;
    bool myIsLegatoSlide;
    bool myIsSlideInAbove;
    bool myIsSlideInBelow;
    bool myIsSlideOutUp;
    bool myIsSlideOutDown;

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

    bool myIsEmpty;
    bool myIsDotted;
    bool myIsRest;
    int myDuration; ///< 4 -> quarter note, 8 -> eighth note, etc.
    boost::optional<int> myIrregularGrouping;
    boost::optional<std::string> myText;
    boost::optional<int> myTempoChange;
    bool myIsVibrato;
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
    void loadTremoloBar(InputStream &stream);
    void loadMixTableChangeEvent(InputStream &stream);
    void loadNotes(InputStream &stream);
};

struct Staff
{
    Staff();
    void load(InputStream &stream);

    std::array<std::vector<Beat>, 2> myVoices;
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
    boost::optional<std::pair<int8_t, bool>> myKeyChange;
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
    std::vector<uint8_t> myTuning;
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

}

#endif