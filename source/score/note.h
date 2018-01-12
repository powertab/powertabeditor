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

#ifndef SCORE_NOTE_H
#define SCORE_NOTE_H

#include <bitset>
#include <boost/optional.hpp>
#include "chordname.h"
#include "fileversion.h"
#include <iosfwd>
#include <vector>

class ArtificialHarmonic
{
public:
    enum class Octave
    {
        Loco,
        Octave8va,
        Octave15ma
    };

    ArtificialHarmonic();
    ArtificialHarmonic(ChordName::Key key, ChordName::Variation variation,
                       Octave octave);

    bool operator==(const ArtificialHarmonic &other) const;

    ChordName::Key getKey() const;
    ChordName::Variation getVariation() const;
    Octave getOctave();

    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/)
    {
        ar("key", myKey);
        ar("variation", myVariation);
        ar("octave", myOctave);
    }

private:
    ChordName::Key myKey;
    ChordName::Variation myVariation;
    Octave myOctave;
};

class Bend
{
public:
    enum BendType
    {
        NormalBend,
        BendAndRelease,
        BendAndHold,
        PreBend,
        PreBendAndRelease,
        PreBendAndHold,
        GradualRelease,
        ImmediateRelease
    };

    enum DrawPoint
    {
        LowPoint,
        MidPoint,
        HighPoint
    };

    Bend();
    Bend(BendType type, int bentPitch, int releasePitch = 0, int duration = 0,
         DrawPoint startPoint = LowPoint, DrawPoint endPoint = MidPoint);

    bool operator==(const Bend &other) const;

    BendType getType() const;
    int getBentPitch() const;
    int getReleasePitch() const;
    int getDuration() const;
    DrawPoint getStartPoint() const;
    DrawPoint getEndPoint() const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/)
    {
        ar("type", myBendType);
        ar("bent_pitch", myBentPitch);
        ar("release_pitch", myReleasePitch);
        ar("duration", myDuration);
        ar("start_point", myStartPoint);
        ar("end_point", myEndPoint);
    }

    /// Returns a text version of the bend pitch (e.g. "Full" or "1 1/2").
    static std::string getPitchText(int pitch);

private:
    BendType myBendType;
    int myBentPitch;
    int myReleasePitch;
    int myDuration;
    DrawPoint myStartPoint;
    DrawPoint myEndPoint;
};

class FingerHint
{
public:
    enum Finger
    {
        None = 0,
        Index = 1,
        Middle = 2,
        Ring = 3,
        Little = 4
    };
    
    /// Position relative to note head at which to display number.
    enum DisplayPosition
    {
        AboveLeft,
        Above,
        AboveRight,
        Right,
        BelowRight,
        Below,
        BelowLeft,
        Left
    };
    
    FingerHint();
    FingerHint(Finger finger, DisplayPosition pos = AboveLeft);
    
    /// Sets the finger this hint is showing.
    void setFinger(Finger finger);
    /// Returns the finger number this hint is showing.
    unsigned int getFingerNumber() const;
    /// Returns the display position relative to the note head.
    DisplayPosition getDisplayPosition() const;
    
    template <class Archive>
    void serialize(Archive &ar, const FileVersion /*version*/)
    {
        ar("finger", myFinger);
        ar("display_position", myDisplayPosition);
    }
    
private:
    Finger myFinger;
    DisplayPosition myDisplayPosition;
};

class Note
{
public:
    enum SimpleProperty
    {
        Tied,
        Muted,
        HammerOnOrPullOff,
        HammerOnFromNowhere,
        PullOffToNowhere,
        NaturalHarmonic,
        GhostNote,
        Octave8va,
        Octave15ma,
        Octave8vb,
        Octave15mb,
        SlideIntoFromBelow,
        SlideIntoFromAbove,
        ShiftSlide,
        LegatoSlide,
        SlideOutOfDownwards,
        SlideOutOfUpwards,
        NumSimpleProperties
    };

    Note();
    Note(int string, int fretNumber);

    bool operator==(const Note &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns the string that the note is located on.
    int getString() const;
    /// Sets the string that the note is located on.
    void setString(int string);

    /// Returns the fret number that the note is located at.
    int getFretNumber() const;
    /// Sets the fret number where the note is located.
    void setFretNumber(int fret);

    /// Returns whether the note is tied, muted, etc.
    bool hasProperty(SimpleProperty property) const;
    /// Sets whether the note is tied, muted, etc.
    void setProperty(SimpleProperty property, bool set = true);

    /// Returns whether the note has a trill.
    bool hasTrill() const;
    /// Returns the fret that the note is trilled with.
    int getTrilledFret() const;
    /// Sets the fret that the note is trilled with.
    void setTrilledFret(int fret);
    /// Removes the trill for this note.
    void clearTrill();

    /// Returns whether the note has a tapped harmonic.
    bool hasTappedHarmonic() const;
    /// Returns the fret that is tapped.
    int getTappedHarmonicFret() const;
    /// Sets the fret that is tapped.
    void setTappedHarmonicFret(int fret);
    /// Removes the tapped harmonic for this note.
    void clearTappedHarmonic();

    /// Returns whether the note has an artificial harmonic.
    bool hasArtificialHarmonic() const;
    /// Returns the artificial harmonic for this note.
    const ArtificialHarmonic &getArtificialHarmonic() const;
    /// Adds an artificial harmonic to this note.
    void setArtificialHarmonic(const ArtificialHarmonic &harmonic);
    /// Removes the artificial harmonic for this note.
    void clearArtificialHarmonic();

    /// Returns whether the note has a bend.
    bool hasBend() const;
    /// Returns the bend for this note.
    const Bend &getBend() const;
    /// Adds a bend to this note.
    void setBend(const Bend &bend);
    /// Removes the bend for this note.
    void clearBend();

    /// Returns whether the note has a finger hint.
    bool hasFingerHint() const;
    /// Returns the finger hint for this note.
    const FingerHint &getFingerHint() const;
    /// Adds a finger hint to this note.
    void setFingerHint(const FingerHint &hint);
    /// Removes the finger hint for this note.
    void clearFingerHint();

    static const int MIN_FRET_NUMBER;
    static const int MAX_FRET_NUMBER;

private:
    int myString;
    int myFretNumber;
    std::bitset<NumSimpleProperties> mySimpleProperties;
    int myTrilledFret;
    int myTappedHarmonicFret;
    boost::optional<ArtificialHarmonic> myArtificialHarmonic;
    boost::optional<Bend> myBend;
    boost::optional<FingerHint> myFingerHint;
};

template <class Archive>
void Note::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("string", myString);
    ar("fret", myFretNumber);
    ar("properties", mySimpleProperties);
    ar("trill", myTrilledFret);
    ar("tapped_harmonic", myTappedHarmonicFret);
    ar("artificial_harmonic", myArtificialHarmonic);
    ar("bend", myBend);
    ar("finger_hint", myFingerHint);
}

/// Useful utility functions for working with natural and tapped harmonics.
namespace Harmonics {

/// Returns a list of all fret offsets that produce harmonics (e.g. 7, 12, etc.)
std::vector<int> getValidFretOffsets();

/// Returns the pitch offset corresponding to the given fret offset.
/// For example, a fret offset of 12 has a pitch offset of 12 (one octave), and
/// a fret offset of 7 has pitch offset 19 (octave and a fifth).
int getPitchOffset(int fretOffset);
}

/// Creates a text representation of the note, including brackets for ghost
/// notes, harmonics, etc.
std::ostream &operator<<(std::ostream &os, const Note &note);

#endif
