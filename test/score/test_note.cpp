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
  
#include <catch.hpp>

#include <boost/lexical_cast.hpp>
#include <score/note.h>
#include "test_serialization.h"

TEST_CASE("Score/Note/SimpleProperties", "")
{
    Note note;

    REQUIRE(!note.hasProperty(Note::HammerOnOrPullOff));
    note.setProperty(Note::HammerOnOrPullOff, true);
    REQUIRE(note.hasProperty(Note::HammerOnOrPullOff));
}

TEST_CASE("Score/Note/Trill", "")
{
    Note note;

    REQUIRE(!note.hasTrill());
    REQUIRE_THROWS(note.getTrilledFret());

    note.setTrilledFret(5);
    REQUIRE(note.hasTrill());
    REQUIRE(note.getTrilledFret() == 5);

    note.clearTrill();
    REQUIRE(!note.hasTrill());
    REQUIRE_THROWS(note.getTrilledFret());
}

TEST_CASE("Score/Note/TappedHarmonic", "")
{
    Note note;

    REQUIRE(!note.hasTappedHarmonic());
    REQUIRE_THROWS(note.getTappedHarmonicFret());

    note.setTappedHarmonicFret(5);
    REQUIRE(note.hasTappedHarmonic());
    REQUIRE(note.getTappedHarmonicFret() == 5);

    note.clearTappedHarmonic();
    REQUIRE(!note.hasTappedHarmonic());
    REQUIRE_THROWS(note.getTappedHarmonicFret());
}

TEST_CASE("Score/Note/ArtificialHarmonic", "")
{
    Note note;

    REQUIRE(!note.hasArtificialHarmonic());

    note.setArtificialHarmonic(ArtificialHarmonic(ChordName::D, ChordName::Flat,
            ArtificialHarmonic::Octave::Octave15ma));
    REQUIRE(note.hasArtificialHarmonic());
    REQUIRE(note.getArtificialHarmonic().getKey() == ChordName::D);

    note.clearArtificialHarmonic();
    REQUIRE(!note.hasArtificialHarmonic());
}

TEST_CASE("Score/Note/Bend", "")
{
    Note note;

    REQUIRE(!note.hasBend());

    note.setBend(
        Bend(Bend::BendAndHold, 2, 0, 0, Bend::LowPoint, Bend::MidPoint));
    REQUIRE(note.hasBend());
    REQUIRE(note.getBend().getType() == Bend::BendAndHold);

    note.clearBend();
    REQUIRE(!note.hasBend());
}

TEST_CASE("Score/Note/FingerHint", "")
{
    Note note;
    
    REQUIRE(!note.hasFingerHint());
    
    note.setFingerHint(FingerHint(FingerHint::Finger::Ring,
            FingerHint::DisplayPosition::BelowRight));
    REQUIRE(note.hasFingerHint());
    REQUIRE(note.getFingerHint().getFingerNumber() == FingerHint::Finger::Ring);
    
    note.clearFingerHint();
    REQUIRE(!note.hasFingerHint());
}

TEST_CASE("Score/Note/Bend/GetPitchText", "")
{
    REQUIRE(Bend::getPitchText(0) == "Standard");
    REQUIRE(Bend::getPitchText(4) == "Full");
    REQUIRE(Bend::getPitchText(2) == "1/2");
    REQUIRE(Bend::getPitchText(3) == "3/4");
    REQUIRE(Bend::getPitchText(7) == "1 3/4");
    REQUIRE(Bend::getPitchText(8) == "2");
}

TEST_CASE("Score/Note/ToString", "")
{
    Note note(3, 12);

    REQUIRE(boost::lexical_cast<std::string>(note) == "12");

    note.setTappedHarmonicFret(15);
    REQUIRE(boost::lexical_cast<std::string>(note) == "12(15)");
    note.clearTappedHarmonic();

    note.setProperty(Note::NaturalHarmonic);
    REQUIRE(boost::lexical_cast<std::string>(note) == "[12]");
    note.setProperty(Note::NaturalHarmonic, false);

    note.setProperty(Note::GhostNote);
    REQUIRE(boost::lexical_cast<std::string>(note) == "(12)");
    note.setProperty(Note::GhostNote, false);

    note.setTrilledFret(5);
    REQUIRE(boost::lexical_cast<std::string>(note) == "12(5)");

    Note mutedNote;
    mutedNote.setProperty(Note::Muted);
    REQUIRE(boost::lexical_cast<std::string>(mutedNote) == "x");
}

TEST_CASE("Score/Note/Harmonics/GetValidFretOffsets", "")
{
    std::vector<int> frets = Harmonics::getValidFretOffsets();

    REQUIRE(!frets.empty());
    // Check a couple valid and invalid frets.
    REQUIRE(std::find(frets.begin(), frets.end(), 12) != frets.end());
    REQUIRE(std::find(frets.begin(), frets.end(), 7) != frets.end());
    REQUIRE(std::find(frets.begin(), frets.end(), 8) == frets.end());
}

TEST_CASE("Score/Note/Serialization", "")
{
    Note note(3, 12);
    note.setProperty(Note::Octave15ma);
    note.setArtificialHarmonic(ArtificialHarmonic(ChordName::D, ChordName::Flat,
            ArtificialHarmonic::Octave::Octave15ma));

    Serialization::test("note", note);
}
