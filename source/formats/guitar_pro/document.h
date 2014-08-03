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

struct Document
{
    void load(InputStream &stream);

    Header myHeader;
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
    NumberOfMidiChannels =
        64, ///< Used for reading/writing the table of MIDI channels
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
