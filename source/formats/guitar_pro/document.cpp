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

#include "document.h"

#include <algorithm>
#include <bitset>
#include <iostream>
#include <unordered_map>

#include <formats/guitar_pro/inputstream.h>
#include <formats/fileformat.h>
#include <score/generalmidi.h>

static constexpr int NUM_LYRIC_LINES = 5;
static constexpr int NUM_MIDI_CHANNELS = 64;
static constexpr int TRACK_DESCRIPTION_LENGTH = 40;
static constexpr int DIAGRAM_DESCRIPTION_LENGTH = 20;
static constexpr int NUMBER_OF_STRINGS = 7;
static constexpr int GP3_NUMBER_OF_STRINGS = 6;
static constexpr int NUMBER_OF_BARRES = 5;

using Flags = std::bitset<8>;

namespace MeasureHeader
{
enum MeasureHeader
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
}

namespace TrackHeader
{
enum TrackHeader
{
    DrumTrack,
    TwelveString,
    BanjoTrack
};
}

namespace BeatHeader
{
enum BeatHeader
{
    Dotted,
    ChordDiagram,
    Text,
    NoteEffects,
    MixTableChangeEvent,
    IrregularGrouping,
    BeatStatus
};
}

namespace BeatStatus
{
enum BeatStatus
{
    Empty = 0x00,
    Rest = 0x02
};
}

namespace BeatEffects
{
enum BeatEffects
{
    Vibrato = 0,
    HasRasguedo = 0,
    WideVibrato = 1,
    Pickstroke = 1,
    NaturalHarmonicGp3 = 2,
    HasTremoloBarEvent = 2,
    ArtificialHarmonicGp3 = 3,
    FadeInGp3 = 4,
    Octave8va = 4,
    HasTapping = 5,
    Octave8vb = 5,
    HasStrokeEffect = 6,
    Octave15ma = 6,
    Octave15mb = 8
};
}

namespace PickstrokeType
{
enum PickstrokeType
{
    None,
    Up,
    Down
};
}

namespace TapType
{
enum TapType
{
    TremoloBarGp3 = 0,
    TappingNone = 0,
    Tapping,
    Slapping,
    Popping
};
}

namespace NoteHeader
{
enum NoteHeader
{
    TimeIndependentDuration,
    HeavyAccent,
    GhostNote,
    HasNoteEffects,
    Dynamic,
    NoteType,
    Accented,
    FingeringType
};
}

namespace NoteType
{
enum NoteType
{
    Normal = 0x01,
    Tied = 0x02,
    Muted = 0x03
};
}

namespace NoteEffects
{
enum NoteEffects
{
    HasBend = 0,
    HasStaccato = 0,
    HasHammerOnOrPullOff = 1,
    HasPalmMute = 1,
    HasSlideOutVer3 = 2,
    HasTremoloPicking = 2,
    HasLetRing = 3,
    HasSlide = 3,
    HasGraceNote = 4,
    HasHarmonic = 4,
    HasTrill = 5,
    HasVibrato = 6
};
}

namespace HarmonicType
{
enum HarmonicType
{
    NaturalHarmonic = 1,
    ArtificalHarmonicGp5 = 2,
    TappedHarmonic = 3,
    PinchHarmonic = 4,
    SemiHarmonic = 5,
    Artificial5 = 15,
    Artificial7 = 17,
    Artificial12 = 22
};
}

namespace Gp
{
Header::LyricLine::LyricLine(int measure, const std::string &contents)
    : myMeasure(measure), myContents(contents)
{
}

void Header::load(InputStream &stream)
{
    myTitle = stream.readString();
    mySubtitle = stream.readString();
    myArtist = stream.readString();
    myAlbum = stream.readString();

    myLyricist = stream.readString();
    if (stream.version() > Version4)
        myComposer = stream.readString();
    else
        myComposer = myLyricist;

    myCopyright = stream.readString();
    myTranscriber = stream.readString();

    myInstructions = stream.readString();

    const uint32_t n = stream.read<uint32_t>();
    for (uint32_t i = 0; i < n; ++i)
        myNotices.push_back(stream.readString());

    if (stream.version() <= Version4)
        myTripletFeel = stream.readBool();

    if (stream.version() >= Version4)
    {
        myLyricTrack = stream.read<int32_t>();

        for (int i = 0; i < NUM_LYRIC_LINES; ++i)
        {
            const int n = stream.read<int32_t>();
            myLyrics.emplace_back(n, stream.readIntString());
        }
    }

    if (stream.version() > Version4)
    {
        // RSE master effect.
        if (stream.version() == Version5_1)
            stream.skip(19);

        // Page setup information.
        stream.skip(30);
        for (int i = 0; i < 10; ++i)
            stream.readString();
    }
}

void Channel::load(InputStream &stream)
{
    myInstrument =
        std::clamp<int32_t>(stream.read<int32_t>(), 0, Midi::NUM_MIDI_PRESETS);
    myVolume = readChannelProperty(stream);
    myBalance = readChannelProperty(stream);
    myChorus = readChannelProperty(stream);
    myReverb = readChannelProperty(stream);
    myPhaser = readChannelProperty(stream);
    myTremolo = readChannelProperty(stream);

    // Unused bytes, possibly for backwards compatibility with older versions?
    stream.skip(2);
}

uint8_t Channel::readChannelProperty(InputStream &stream)
{
    uint8_t value = stream.read<uint8_t>();
    if (value != 0)
        value = (value * 8) - 1;

    return value;
}

GraceNote::GraceNote()
    : myFret(0), myDynamic(0), myTransition(NoTransition), myDuration(0)
{
}

void GraceNote::load(InputStream &stream)
{
    myFret = stream.read<int8_t>();
    myDynamic = stream.read<int8_t>();
    myTransition = static_cast<Transition>(stream.read<int8_t>());

    int duration = stream.read<int8_t>();
    switch (duration)
    {
    case 3:
        myDuration = 64;
        break;
    case 2:
        myDuration = 32;
        break;
    case 1:
    default:
        myDuration = 16;
        break;
    }

    // Extra flags in GP5 - dead / onBeat.
    if (stream.version() > Version4)
        stream.skip(1);
}

Note::Note(int string)
    : myString(string),
      myFret(0),
      myHasAccent(false),
      myHasHeavyAccent(false),
      myIsGhostNote(false),
      myIsTied(false),
      myIsMuted(false),
      myIsLetRing(false),
      myIsHammerOnOrPullOff(false),
      myHasPalmMute(false),
      myIsStaccato(false),
      myIsNaturalHarmonic(false),
      myIsVibrato(false),
      myIsTremoloPicked(false),
      myIsShiftSlide(false),
      myIsLegatoSlide(false),
      myIsSlideInAbove(false),
      myIsSlideInBelow(false),
      myIsSlideOutUp(false),
      myIsSlideOutDown(false)

{
}

void Note::load(InputStream &stream, const Track &track)
{
    const Flags flags = stream.read<uint8_t>();

    myHasAccent = flags.test(NoteHeader::Accented);

    if (stream.version() > Version4)
        myHasHeavyAccent = flags.test(NoteHeader::HeavyAccent);

    myIsGhostNote = flags.test(NoteHeader::GhostNote);

    if (flags.test(NoteHeader::NoteType))
    {
        const uint8_t noteType = stream.read<uint8_t>();
        if (noteType == NoteType::Tied)
            myIsTied = true;
        if (noteType == NoteType::Muted)
            myIsMuted = true;
    }

    if (stream.version() <= Version4 &&
        flags.test(NoteHeader::TimeIndependentDuration))
    {
        // Ignore - I think this repeats the Beat duration?
        stream.skip(1);
        stream.skip(1);
    }

    if (flags.test(NoteHeader::Dynamic))
        myDynamic = stream.read<int8_t>();

    // If there is a non-empty note, read the fret number.
    if (flags.test(NoteHeader::NoteType))
        myFret = stream.read<int8_t>();

    if (flags.test(NoteHeader::FingeringType))
    {
        myLeftFinger = stream.read<int8_t>();

        // TODO - support right hand fingering (#291).
        stream.skip(1);
    }

    if (stream.version() > Version4)
    {
        // This is a double with the duration represented as a percenta.
        if (flags.test(NoteHeader::TimeIndependentDuration))
            stream.skip(8);

        // Extra set of flags ('swap accidentals'?).
        stream.skip(1);
    }

    if (flags.test(NoteHeader::HasNoteEffects))
    {
        if (stream.version() >= Version4)
            loadNoteEffects(stream, track);
        else if (stream.version() == Version3)
            loadNoteEffectsGp3(stream);
    }
}

static Bend
loadBend(InputStream &stream)
{
    Bend bend;
    bend.myBendType = stream.read<int8_t>();
    bend.myBendValue = stream.read<int32_t>();

    const int num_points = stream.read<int32_t>();
    for (int i = 0; i < num_points; ++i)
    {
        Bend::Point point;
        // Convert from 0-60 to a percentage.
        point.myOffset = (stream.read<int32_t>() * 100.0) / 60.0;
        point.myValue = stream.read<int32_t>();
        bend.myPoints.push_back(point);

        // Bend vibrato - ignore.
        stream.skip(1);
    }

    return bend;
}

void Note::loadNoteEffects(InputStream &stream, const Track &track)
{
    const Flags header1 = stream.read<uint8_t>();
    const Flags header2 = stream.read<uint8_t>();

    if (header1.test(NoteEffects::HasBend))
        myBend = loadBend(stream);

    if (header1.test(NoteEffects::HasGraceNote))
    {
        GraceNote note;
        note.load(stream);
        myGraceNote = note;
    }

    if (header2.test(NoteEffects::HasTremoloPicking))
    {
        // Ignore - Power Tab does not allow different values for the tremolo
        // picking duration (e.g. eighth notes).
        stream.skip(1);
        myIsTremoloPicked = true;
    }

    if (header2.test(NoteEffects::HasSlide))
        loadSlide(stream);

    if (header2.test(NoteEffects::HasHarmonic))
        loadHarmonic(stream, track);

    if (header2.test(NoteEffects::HasTrill))
    {
        myTrilledFret = stream.read<int8_t>();
        // Trill duration - not supported in Power Tab.
        stream.skip(1);
    }

    if (header1.test(NoteEffects::HasLetRing))
        myIsLetRing = true;
    if (header1.test(NoteEffects::HasHammerOnOrPullOff))
        myIsHammerOnOrPullOff = true;
    if (header2.test(NoteEffects::HasPalmMute))
        myHasPalmMute = true;
    if (header2.test(NoteEffects::HasStaccato))
        myIsStaccato = true;
    if (header2.test(NoteEffects::HasVibrato))
        myIsVibrato = true;
}

void Note::loadNoteEffectsGp3(InputStream &stream)
{
    const Flags flags = stream.read<uint8_t>();

    myIsLetRing = flags.test(NoteEffects::HasLetRing);
    myIsHammerOnOrPullOff = flags.test(NoteEffects::HasHammerOnOrPullOff);

    if (flags.test(NoteEffects::HasSlideOutVer3))
        myIsSlideOutDown = true;

    if (flags.test(NoteEffects::HasBend))
        myBend = loadBend(stream);

    if (flags.test(NoteEffects::HasGraceNote))
    {
        GraceNote note;
        note.load(stream);
        myGraceNote = note;
    }
}

void Note::loadSlide(InputStream &stream)
{
    const int slideValue = stream.read<int8_t>();

    if (stream.version() <= Gp::Version4)
    {
        /* Slide values are as follows:
            -2 : slide into from above
            -1 : slide into from below
            0  : no slide
            1  : shift slide
            2  : legato slide
            3  : slide out of downwards
            4  : slide out of upwards
        */
        switch (slideValue)
        {
            case -2:
                myIsSlideInAbove = true;
                break;
            case -1:
                myIsSlideInBelow = true;
                break;
            case 1:
                myIsShiftSlide = true;
                break;
            case 2:
                myIsLegatoSlide = true;
                break;
            case 3:
                myIsSlideOutDown = true;
                break;
            case 4:
                myIsSlideOutUp = true;
                break;
        }
    }
    else
    {
        std::bitset<8> flags(slideValue);

        myIsShiftSlide = flags.test(0);
        myIsLegatoSlide = flags.test(1);
        myIsSlideOutDown = flags.test(2);
        myIsSlideOutUp = flags.test(3);
        myIsSlideInBelow = flags.test(4);
        myIsSlideInAbove = flags.test(5);
    }
}

void Note::loadHarmonic(InputStream &stream, const Track &track)
{
    const uint8_t harmonic = stream.read<uint8_t>();

    if (harmonic == HarmonicType::NaturalHarmonic)
        myIsNaturalHarmonic = true;
    else if (harmonic == HarmonicType::TappedHarmonic)
    {
        myIsTappedHarmonic = true;
        myHarmonicFret = stream.read<int8_t>();
    }
    else if (harmonic == HarmonicType::Artificial5)
    {
        myIsArtificialHarmonic = true;
        myHarmonicFret = 5;
    }
    else if (harmonic == HarmonicType::Artificial7)
    {
        myIsArtificialHarmonic = true;
        myHarmonicFret = 7;
    }
    else if (harmonic == HarmonicType::Artificial12)
    {
        myIsArtificialHarmonic = true;
        myHarmonicFret = 12;
    }
    else if (harmonic == HarmonicType::ArtificalHarmonicGp5)
    {
        int harmonic_base_pitch = stream.read<int8_t>();
        const int accidental = stream.read<int8_t>();
        harmonic_base_pitch += accidental;
        int octave = stream.read<int8_t>();

        const int base_pitch =
            Midi::getMidiNotePitch(track.myTuning[myString] + myFret);
        if (base_pitch > harmonic_base_pitch)
            ++octave;

        const int pitch_offset = octave * 12 + (harmonic_base_pitch - base_pitch);

        const std::unordered_map<int, double> theHarmonicFrets = {
            { 12, 12 },  { 19, 7 },   { 24, 5 },  { 28, 4 },
            { 31, 3.2 }, { 34, 2.7 }, { 36, 2.4 }
        };
        auto it = theHarmonicFrets.find(pitch_offset);
        myHarmonicFret = (it != theHarmonicFrets.end()) ? it->second : 12;
        myIsArtificialHarmonic = true;
    }
}

Beat::Beat()
    : myIsEmpty(false),
      myIsDotted(false),
      myIsRest(false),
      myDuration(1),
      myIsVibrato(false),
      myIsWideVibrato(false),
      myIsNaturalHarmonic(false),
      myIsArtificialHarmonic(false),
      myIsTremoloPicked(false),
      myPickstrokeUp(false),
      myPickstrokeDown(false),
      myIsTapped(false),
      myOctave8va(false),
      myOctave8vb(false),
      myOctave15ma(false),
      myOctave15mb(false)
{
}

void Beat::load(InputStream &stream, const Track &track)
{
    const Flags flags = stream.read<uint8_t>();

    myIsDotted = flags.test(BeatHeader::Dotted);

    if (flags.test(BeatHeader::BeatStatus))
    {
        const uint8_t status = stream.read<uint8_t>();

        if (status == BeatStatus::Empty)
            myIsEmpty = true;
        else if (status == BeatStatus::Rest)
            myIsRest = true;
    }

    // Read the duration.
    {
        const int8_t duration = stream.read<int8_t>();

        // Durations are stored as 0 -> quarter note, -1 -> half note, 1 ->
        // eight note, etc. We need to convert to 1 = whole note, 2 = half note,
        // 4 = quarter note, etc.
        myDuration = (1 << (duration + 2));
    }

    if (flags.test(BeatHeader::IrregularGrouping))
        myIrregularGrouping = stream.read<int32_t>();

    if (flags.test(BeatHeader::ChordDiagram))
        loadChordDiagram(stream);

    if (flags.test(BeatHeader::Text))
        myText = stream.readString();

    if (flags.test(BeatHeader::NoteEffects))
        loadBeatEffects(stream);

    if (flags.test(BeatHeader::MixTableChangeEvent))
        loadMixTableChangeEvent(stream);

    loadNotes(stream, track);

    // Handle octave symbols.
    if (stream.version() > Version4)
    {
        std::bitset<16> flags = stream.read<uint16_t>();
        myOctave8va = flags.test(BeatEffects::Octave8va);
        myOctave8vb = flags.test(BeatEffects::Octave8vb);
        myOctave15ma = flags.test(BeatEffects::Octave15ma);
        myOctave15mb = flags.test(BeatEffects::Octave15mb);

        // The other bits are mostly used for beaming.
        // The 'breakSecondary' flag requires reading an extra byte.
        if (flags.test(11) != 0)
            stream.skip(1);
    }
}

void Beat::loadChordDiagram(InputStream &stream)
{
    const bool new_format = stream.readBool();
    if (!new_format)
    {
        loadOldChordDiagram(stream);
        return;
    }

    if (stream.version() == Version3)
    {
        stream.skip(25);
        stream.readFixedLengthString(34); // Chord name.
        stream.skip(4); // Top fret of chord.

        // Strings that are used.
        for (int i = 0; i < GP3_NUMBER_OF_STRINGS; ++i)
            stream.skip(4);
        stream.skip(36);
        return;
    }

    stream.skip(1); // Sharps/flats.

    // Blank bytes for backwards compatibility with gp3.
    stream.skip(3);

    stream.skip(1); // root of chord
    stream.skip(1); // chord type
    stream.skip(1); // extension (9, 11, 13)
    stream.skip(4); // bass note of chord
    stream.skip(4); // diminished/augmented
    stream.skip(1); // "add" chord

    stream.readFixedLengthString(DIAGRAM_DESCRIPTION_LENGTH);

    // more blank bytes for backwards compatibility
    stream.skip(2);

    stream.skip(1); // tonality of the 5th
    stream.skip(1); // tonality of the 9th
    stream.skip(1); // tonality of the 11th

    stream.skip(4); // base fret of the chord

    // fret numbers for each string
    for (int i = 0; i < NUMBER_OF_STRINGS; ++i)
        stream.skip(4);

    stream.skip(1); // number of barres in the chord

    for (int i = 0; i < NUMBER_OF_BARRES; ++i)
        stream.skip(1); // fret of the barre

    for (int i = 0; i < NUMBER_OF_BARRES; ++i)
        stream.skip(1); // barre start

    for (int i = 0; i < NUMBER_OF_BARRES; ++i)
        stream.skip(1); // barre end

    // Omission1, Omission3, Omission5, Omission7, Omission9,
    // Omission11, Omission13, and another blank byte
    stream.skip(8);

    // fingering of chord
    for (int i = 0; i < NUMBER_OF_STRINGS; ++i)
        stream.skip(1);

    stream.skip(1); // show fingering
}

void Beat::loadOldChordDiagram(InputStream &stream)
{
    stream.readString(); // chord diagram name

    const uint32_t baseFret = stream.read<uint32_t>();

    if (baseFret != 0)
    {
        for (int i = 0; i < GP3_NUMBER_OF_STRINGS; ++i)
            stream.skip(4); // fret number
    }
}

void Beat::loadBeatEffects(InputStream &stream)
{
    const Flags flags1 = stream.read<uint8_t>();
    Flags flags2;

    myIsVibrato = flags1.test(BeatEffects::Vibrato);
    myIsWideVibrato = flags1.test(BeatEffects::WideVibrato);

    // GP3 effect decoding.
    if (stream.version() == Version3)
    {
        myIsNaturalHarmonic = flags1.test(BeatEffects::NaturalHarmonicGp3);
        myIsArtificialHarmonic =
            flags1.test(BeatEffects::ArtificialHarmonicGp3);
    }
    else
        flags2 = stream.read<uint8_t>();

    if (flags1.test(BeatEffects::HasTapping))
    {
        const uint8_t type = stream.read<uint8_t>();

        // In GP3, a value of 0 indicates a tremolo bar.
        if (type == TapType::TremoloBarGp3 && stream.version() == Version3)
            myTremoloBar = loadTremoloBar(stream);
        else
        {
            // Ignore slapping and popping.
            if (type == TapType::Tapping)
                myIsTapped = true;

            // TODO - figure out the meaning of this data.
            if (stream.version() == Version3)
                stream.skip(4);
        }
    }

    if (stream.version() >= Version4 &&
        flags2.test(BeatEffects::HasTremoloBarEvent))
    {
        myTremoloBar = loadTremoloBar(stream);
    }

    if (flags1.test(BeatEffects::HasStrokeEffect))
    {
        // Upstroke and downstroke duration values - we will just use these for
        // toggling pickstroke up/down.
        myPickstrokeDown = stream.readBool();
        myPickstrokeUp = stream.readBool();
    }

    if (stream.version() >= Version4)
        myIsTremoloPicked = flags2.test(BeatEffects::HasRasguedo);

    if (stream.version() >= Version4 && flags2.test(BeatEffects::Pickstroke))
    {
        const uint8_t pickstrokeType = stream.read<uint8_t>();

        if (pickstrokeType == PickstrokeType::Up)
            myPickstrokeUp = true;
        else if (pickstrokeType == PickstrokeType::Down)
            myPickstrokeDown = true;
    }
}

Bend
Beat::loadTremoloBar(InputStream &stream)
{
    if (stream.version() == Version3)
    {
        // Just import as a dip.
        Bend bend;
        bend.myBendType = 6; // Dip
        bend.myBendValue = stream.read<int32_t>();
        bend.myPoints.push_back(Bend::Point{ 0, 0 });
        bend.myPoints.push_back(Bend::Point{ 50, bend.myBendValue });
        bend.myPoints.push_back(Bend::Point{ 100, 0 });

        return bend;
    }
    else
    {
        // Otherwise, the format exactly matches that of a bend.
        return loadBend(stream);
    }
}

void Beat::loadMixTableChangeEvent(InputStream &stream)
{
    // TODO - implement conversions for this.
    stream.skip(1); // instrument

    if (stream.version() > Version4)
        stream.skip(16); // RSE Info???

    int8_t volume = stream.read<int8_t>(); // volume
    int8_t pan = stream.read<uint8_t>(); // pan
    int8_t chorus = stream.read<uint8_t>(); // chorus
    int8_t reverb = stream.read<uint8_t>(); // reverb
    int8_t phaser = stream.read<uint8_t>(); // phaser
    int8_t tremolo = stream.read<uint8_t>(); // tremolo

    if (stream.version() > Version4)
        myTempoChangeName = stream.readString();

    // New tempo.
    int32_t tempo = stream.read<int32_t>();
    if (tempo > 0)
        myTempoChange = tempo;

    if (volume >= 0)
        stream.skip(1); // volume change duration

    if (pan >= 0)
        stream.skip(1); // pan change duration

    if (chorus >= 0)
        stream.skip(1); // chorus change duration

    if (reverb >= 0)
        stream.skip(1); // reverb change duration

    if (phaser >= 0)
        stream.skip(1); // phaser change duration

    if (tremolo >= 0)
        stream.skip(1); // tremolo change duration

    if (tempo >= 0)
    {
        stream.skip(1); // tempo change duration

        if (stream.version() == Version5_1)
            stream.skip(1);
    }

    if (stream.version() >= Version4)
    {
        // Details of score-wide or track-specific changes.
        stream.skip(1);
    }

    if (stream.version() > Version4)
    {
        stream.skip(1);
        if (stream.version() == Version5_1)
        {
            // RSE instrument effect & category.
            stream.readString();
            stream.readString();
        }
    }
}

void Beat::loadNotes(InputStream &stream, const Track &track)
{
    const Flags stringsPlayed = stream.read<uint8_t>();

    for (int i = NUMBER_OF_STRINGS - 1; i >= 0; --i)
    {
        if (stringsPlayed.test(i))
        {
            Note note(NUMBER_OF_STRINGS - i - 1);
            note.load(stream, track);
            myNotes.push_back(note);
        }
    }
}

Staff::Staff()
{
}

void Staff::load(InputStream &stream, const Track &track)
{
    int numBeats = stream.read<int32_t>();
    for (int i = 0; i < numBeats; ++i)
    {
        Beat beat;
        beat.load(stream, track);
        myVoices[0].push_back(beat);
    }

    if (stream.version() > Version4)
    {
        numBeats = stream.read<int32_t>();
        for (int i = 0; i < numBeats; ++i)
        {
            Beat beat;
            beat.load(stream, track);
            myVoices[1].push_back(beat);
        }
    }

    // Info about line breaks.
    if (stream.version() > Version4)
        stream.skip(1);
}

Measure::Measure() : myIsDoubleBar(false), myIsRepeatBegin(false)
{
}

void Measure::load(InputStream &stream)
{
    const Flags flags = stream.read<uint8_t>();

    if (flags.test(MeasureHeader::Numerator) ||
        flags.test(MeasureHeader::Denominator))
    {
        TimeSignatureChange time;
        if (flags.test(MeasureHeader::Numerator))
            time.myNumerator = stream.read<int8_t>();

        if (flags.test(MeasureHeader::Denominator))
            time.myDenominator = stream.read<int8_t>();

        myTimeSignatureChange = time;
    }

    myIsDoubleBar = flags.test(MeasureHeader::DoubleBar);
    myIsRepeatBegin = flags.test(MeasureHeader::RepeatBegin);
    if (flags.test(MeasureHeader::RepeatEnd))
        myRepeatEnd = stream.read<int8_t>();

    if (flags.test(MeasureHeader::AltEnding))
        myAlternateEnding = stream.read<int8_t>();

    if (flags.test(MeasureHeader::Marker))
        loadMarker(stream);

    if (flags.test(MeasureHeader::KeySignatureChange))
    {
        KeySignatureChange key;
        key.myAccidentals = stream.read<int8_t>();
        key.myIsMinor = stream.readBool();
        myKeyChange = key;
    }

    if (stream.version() > Version4)
    {
        // Time signature beams.
        if (flags.test(MeasureHeader::Numerator) ||
            flags.test(MeasureHeader::Denominator))
        {
            stream.skip(4);
        }

        // Unknown blank byte.
        if (!flags.test(MeasureHeader::AltEnding))
            stream.skip(1);

        // Triplet feel.
        stream.skip(1);
    }
}

void Measure::loadMarker(InputStream &stream)
{
    myMarker = stream.readString();
    // Ignore the marker's color.
    stream.skip(4);
}

void Measure::loadStaves(InputStream &stream, const std::vector<Track> &tracks)
{
    for (const Track &track : tracks)
    {
        Staff staff;
        staff.load(stream, track);
        myStaves.push_back(staff);
    }
}

void Track::load(InputStream &stream)
{
    const Flags flags = stream.read<uint8_t>();

    // Ignore 12-string and banjo tracks, but record whether this is a drum
    // track.
    myIsDrumTrack = flags.test(TrackHeader::DrumTrack);

    myName = stream.readFixedLengthString(TRACK_DESCRIPTION_LENGTH);
    myNumStrings = stream.read<int32_t>();

    for (int i = 0; i < NUMBER_OF_STRINGS; ++i)
    {
        int note = stream.read<int32_t>();
        if (i < myNumStrings)
            myTuning.push_back(note);
    }

    // MIDI port - not needed.
    stream.skip(4);

    myChannelIndex = stream.read<int32_t>();

    // MIDI channel used for effects.
    stream.skip(4);
    // Number of frets.
    stream.skip(4);

    myCapo = stream.read<int32_t>();

    // Track color.
    stream.skip(4);

    if (stream.version() > Version4)
    {
        // Track settings.
        stream.skip(2);
        // RSE auto-accentuate.
        stream.skip(1);
        // Channel bank.
        stream.skip(1);
        // RSE humanize.
        stream.skip(1);
        // Unknown RSE data.
        stream.skip(24);
        // RSE midi instrument.
        stream.skip(4);
        // Unknown.
        stream.skip(4);
        // RSE sound bank.
        stream.skip(4);
        if (stream.version() == Version5_0)
        {
            // RSE effect number.
            stream.skip(2);
            // Unknown.
            stream.skip(1);
        }
        else if (stream.version() == Version5_1)
        {
            // RSE effect number.
            stream.skip(4);
            // RSE equalizer.
            stream.skip(4);
            // RSE effect name.
            stream.readString();
            // RSE effect category.
            stream.readString();
        }
    }
}

void Document::load(InputStream &stream)
{
    myHeader.load(stream);

    if (stream.version() > Version4)
        myStartTempoName = stream.readString();

    myStartTempo = stream.read<int32_t>();

    if (stream.version() == Version5_1)
        myStartTempoVisible = stream.readBool();

    myInitialKey = stream.read<int32_t>();

    if (stream.version() >= Version4)
        myOctave8va = stream.readBool();

    for (int i = 0; i < NUM_MIDI_CHANNELS; ++i)
    {
        Channel channel;
        channel.load(stream);
        myChannels.push_back(channel);
    }

    if (stream.version() > Version4)
    {
        // The direction indices aren't zero-based.
        auto readDirectionIndex = [&stream]() {
            return stream.read<int16_t>() - 1;
        };

        myDirections.myCoda = readDirectionIndex();
        myDirections.myDoubleCoda = readDirectionIndex();
        myDirections.mySegno = readDirectionIndex();
        myDirections.mySegnoSegno = readDirectionIndex();
        myDirections.myFine = readDirectionIndex();

        myDirections.myDaCapo = readDirectionIndex();
        myDirections.myDaCapoAlCoda = readDirectionIndex();
        myDirections.myDaCapoAlDoubleCoda = readDirectionIndex();
        myDirections.myDaCapoAlFine = readDirectionIndex();
        myDirections.myDaSegno = readDirectionIndex();
        myDirections.myDaSegnoAlCoda = readDirectionIndex();
        myDirections.myDaSegnoAlDoubleCoda = readDirectionIndex();
        myDirections.myDaSegnoAlFine = readDirectionIndex();
        myDirections.myDaSegnoSegno = readDirectionIndex();
        myDirections.myDaSegnoSegnoAlCoda = readDirectionIndex();
        myDirections.myDaSegnoSegnoAlDoubleCoda = readDirectionIndex();
        myDirections.myDaSegnoSegnoAlFine = readDirectionIndex();
        myDirections.myDaCoda = readDirectionIndex();
        myDirections.myDaDoubleCoda = readDirectionIndex();

        // Master reverb.
        stream.skip(4);
    }

    const int numMeasures = stream.read<int32_t>();
    const int numTracks = stream.read<int32_t>();

    for (int i = 0; i < numMeasures; ++i)
    {
        // Seems to be a blank byte here in GP5 files.
        if (stream.version() > Version4 && i > 0)
            stream.skip(1);

        Measure measure;
        measure.load(stream);
        myMeasures.push_back(measure);
    }

    for (int i = 0; i < numTracks; ++i)
    {
        // Seems to be a blank byte.
        if (stream.version() > Version4)
        {
            if (i == 0 || stream.version() == Version5_0)
                stream.skip(1);
        }

        Track track;
        track.load(stream);
        myTracks.push_back(track);
    }

    // Seems to be blank bytes.
    if (stream.version() == Version5_0)
        stream.skip(2);
    else if (stream.version() == Version5_1)
        stream.skip(1);

    for (int i = 0; i < numMeasures; ++i)
        myMeasures[i].loadStaves(stream, myTracks);
}

}
