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

#include "note.h"

#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>

const int Note::MIN_FRET_NUMBER = 0;
const int Note::MAX_FRET_NUMBER = 29;

namespace {
    /// Mapping of frets to pitch offsets (counted in half-steps or frets).
    /// For example, the natural harmonic at the 7th fret is an octave and
    /// a fifth (19 frets) above the pitch of the open string.
	const std::map<int, int> theHarmonicOffsets = {
        {2, 38}, {3, 31}, {4, 28}, {5, 24}, {7, 19}, {9, 28}, {10, 38},
        {12, 12}, {14, 38}, {15, 34}, {16, 28}, {17, 36}, {19, 19}, {22, 34},
        {24, 24}, {28, 28}
    };
}

Note::Note()
    : myString(0),
      myFretNumber(0),
      myTrilledFret(-1),
      myTappedHarmonicFret(-1)
{
}

Note::Note(int string, int fretNumber)
    : myString(string),
      myFretNumber(fretNumber),
      myTrilledFret(-1),
      myTappedHarmonicFret(-1)
{
}

bool Note::operator==(const Note &other) const
{
    return myString == other.myString && myFretNumber == other.myFretNumber &&
           mySimpleProperties == other.mySimpleProperties &&
           myTrilledFret == other.myTrilledFret &&
           myTappedHarmonicFret == other.myTappedHarmonicFret &&
           myArtificialHarmonic == other.myArtificialHarmonic &&
           myBend == other.myBend;
}

int Note::getString() const
{
    return myString;
}

void Note::setString(int string)
{
    myString = string;
}

int Note::getFretNumber() const
{
    return myFretNumber;
}

void Note::setFretNumber(int fret)
{
    myFretNumber = fret;
}

bool Note::hasProperty(SimpleProperty property) const
{
    return mySimpleProperties.test(property);
}

void Note::setProperty(SimpleProperty property, bool set)
{
    // Handle any mutually exclusive properties.
    if (set)
    {
        // Clear all other octave properties if setting an octave property.
        if (property >= Octave8va && property <= Octave15mb)
        {
            for (int p = Octave8va; p <= Octave15mb; ++p)
                mySimpleProperties.set(static_cast<SimpleProperty>(p), false);
        }

        // Clear all hammeron/pulloff properties.
        if (property >= HammerOnOrPullOff && property <= PullOffToNowhere)
        {
            for (int p = HammerOnOrPullOff; p <= PullOffToNowhere; ++p)
                mySimpleProperties.set(static_cast<SimpleProperty>(p), false);
        }

        // Clear any mutually-exclusive slide types.
        if (property == SlideIntoFromAbove)
            mySimpleProperties.set(SlideIntoFromBelow, false);
        if (property == SlideIntoFromBelow)
            mySimpleProperties.set(SlideIntoFromAbove, false);

        if (property >= ShiftSlide && property <= SlideOutOfUpwards)
        {
            for (int p = ShiftSlide; p <= SlideOutOfUpwards; ++p)
                mySimpleProperties.set(static_cast<SimpleProperty>(p), false);
        }
    }

    mySimpleProperties.set(property, set);
}

bool Note::hasTrill() const
{
    return myTrilledFret != -1;
}

int Note::getTrilledFret() const
{
    if (!hasTrill())
        throw std::logic_error("Note does not have a trill");

    return myTrilledFret;
}

void Note::setTrilledFret(int fret)
{
    if (fret < 0)
        throw std::out_of_range("Invalid fret number");

    myTrilledFret = fret;
}

void Note::clearTrill()
{
    myTrilledFret = -1;
}

bool Note::hasTappedHarmonic() const
{
    return myTappedHarmonicFret != -1;
}

int Note::getTappedHarmonicFret() const
{
    if (!hasTappedHarmonic())
        throw std::logic_error("Note does not have a tapped harmonic");

    return myTappedHarmonicFret;
}

void Note::setTappedHarmonicFret(int fret)
{
    if (fret < 0)
        throw std::out_of_range("Invalid fret number");

    myTappedHarmonicFret = fret;
}

void Note::clearTappedHarmonic()
{
    myTappedHarmonicFret = -1;
}

bool Note::hasArtificialHarmonic() const
{
    return myArtificialHarmonic.is_initialized();
}

const ArtificialHarmonic &Note::getArtificialHarmonic() const
{
    return myArtificialHarmonic.get();
}

void Note::setArtificialHarmonic(const ArtificialHarmonic &harmonic)
{
    myArtificialHarmonic = harmonic;
}

void Note::clearArtificialHarmonic()
{
    myArtificialHarmonic.reset();
}

bool Note::hasBend() const
{
    return myBend.is_initialized();
}

const Bend &Note::getBend() const
{
    return myBend.get();
}

void Note::setBend(const Bend &bend)
{
    myBend = bend;
}

void Note::clearBend()
{
    myBend.reset();
}

std::ostream &operator<<(std::ostream &os, const Note &note)
{
    // For muted notes, display 'x'.
    if (note.hasProperty(Note::Muted))
    {
        os << "x";
        return os;
    }

    // For tapped harmonics, display '7(14)', where 14 is the tapped note
    // For natural harmonics, display '[12]'
    // For ghost notes, display '(12)'
    // Otherwise, just display the fret number

    int noteValue = note.getFretNumber();
    // For tapped harmonics and trills, display original note first, and
    // tapped/trilled note after.
    if (note.hasTappedHarmonic() || note.hasTrill())
    {
        os << noteValue;

        if (note.hasTappedHarmonic())
            noteValue = note.getTappedHarmonicFret();
        else
            noteValue = note.getTrilledFret();
    }

    std::string brackets = "";
    if (note.hasTappedHarmonic() || note.hasProperty(Note::GhostNote) ||
        note.hasTrill())
    {
        brackets = "()";
    }
    else if (note.hasProperty(Note::NaturalHarmonic))
    {
        brackets = "[]";
    }

    if (!brackets.empty())
        os << brackets[0];
    os << noteValue;
    if (!brackets.empty())
        os << brackets[1];

    return os;
}

std::vector<int> Harmonics::getValidFretOffsets()
{
    std::vector<int> frets;

    for (const auto &pair : theHarmonicOffsets)
        frets.push_back(pair.first);

    return frets;
}

int Harmonics::getPitchOffset(int fretOffset)
{
    auto it = theHarmonicOffsets.find(fretOffset);
    return (it != theHarmonicOffsets.end()) ? it->second : 0;
}

ArtificialHarmonic::ArtificialHarmonic()
    : myKey(ChordName::C),
      myVariation(ChordName::NoVariation),
      myOctave(Octave::Loco)
{
}

ArtificialHarmonic::ArtificialHarmonic(ChordName::Key key,
                                       ChordName::Variation variation,
                                       ArtificialHarmonic::Octave octave)
    : myKey(key),
      myVariation(variation),
      myOctave(octave)
{
}

bool ArtificialHarmonic::operator==(const ArtificialHarmonic &other) const
{
    return myKey == other.myKey &&
           myVariation == other.myVariation &&
           myOctave == other.myOctave;
}

ChordName::Key ArtificialHarmonic::getKey() const
{
    return myKey;
}

ChordName::Variation ArtificialHarmonic::getVariation() const
{
    return myVariation;
}

ArtificialHarmonic::Octave ArtificialHarmonic::getOctave()
{
    return myOctave;
}

Bend::Bend()
    : myBendType(NormalBend),
      myBentPitch(4),
      myReleasePitch(0),
      myDuration(0),
      myStartPoint(LowPoint),
      myEndPoint(MidPoint)
{
}

Bend::Bend(BendType type, int bentPitch, int releasePitch, int duration,
           DrawPoint startPoint, DrawPoint endPoint)
    : myBendType(type),
      myBentPitch(bentPitch),
      myReleasePitch(releasePitch),
      myDuration(duration),
      myStartPoint(startPoint),
      myEndPoint(endPoint)
{
}

bool Bend::operator==(const Bend &other) const
{
    return myBendType == other.myBendType && myBentPitch == other.myBentPitch &&
           myReleasePitch == other.myReleasePitch &&
           myDuration == other.myDuration &&
           myStartPoint == other.myStartPoint && myEndPoint == other.myEndPoint;
}

Bend::BendType Bend::getType() const
{
    return myBendType;
}

int Bend::getBentPitch() const
{
    return myBentPitch;
}

int Bend::getReleasePitch() const
{
    return myReleasePitch;
}

int Bend::getDuration() const
{
    return myDuration;
}

Bend::DrawPoint Bend::getStartPoint() const
{
    return myStartPoint;
}

Bend::DrawPoint Bend::getEndPoint() const
{
    return myEndPoint;
}

std::string Bend::getPitchText(int pitch)
{
    std::ostringstream text;

    if (pitch == 0)
        text << "Standard";
    else if (pitch == 4)
        text << "Full";
    else
    {
        // Display a fraction.
        const int quotient = pitch / 4;
        const int remainder = pitch % 4;

        // Handle whole numbers.
        if (quotient != 0)
        {
            text << quotient;

            if (remainder != 0)
                text << " ";
        }

        // Fractional part.
        if (remainder != 0)
        {
            // Reduce the fraction.
            if (remainder == 1 || remainder == 3)
                text << remainder << "/" << 4;
            else
                text << "1/2";
        }
    }

    return text.str();
}
