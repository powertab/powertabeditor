/*
  * Copyright (C) 2011 Cameron White
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

#include <doctest/doctest.h>

#include <app/appinfo.h>
#include <formats/guitar_pro/guitarproimporter.h>
#include <score/score.h>

static void loadTest(GuitarProImporter &importer, const char *filename,
                     Score &score)
{
    importer.load(AppInfo::getAbsolutePath(filename), score);
}

TEST_CASE("Formats/GuitarPro/Barlines")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/barlines.gp5", score);

    const System &system = score.getSystems()[0];
    auto barlines = system.getBarlines();

    REQUIRE(barlines.size() == 5);
    REQUIRE(barlines[1].getBarType() == Barline::RepeatStart);
    REQUIRE(barlines[2].getBarType() == Barline::RepeatEnd);
    REQUIRE(barlines[2].getRepeatCount() == 2);
    REQUIRE(barlines[3].getBarType() == Barline::RepeatStart);
    REQUIRE(barlines[4].getBarType() == Barline::DoubleBar);

    const System &system2 = score.getSystems()[1];
    auto barlines2 = system2.getBarlines();
    REQUIRE(barlines2[1].getBarType() == Barline::RepeatEnd);
    REQUIRE(barlines2[1].getRepeatCount() == 4);
}

TEST_CASE("Formats/GuitarPro/RehearsalSigns")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/rehearsal_signs.gp5", score);

    {
        const Barline &barline = score.getSystems()[0].getBarlines()[2];
        REQUIRE(barline.hasRehearsalSign());
        REQUIRE(barline.getRehearsalSign().getDescription() == "Chorus");
        REQUIRE(barline.getRehearsalSign().getLetters() == "A");
    }

    {
        const Barline &barline = score.getSystems()[1].getBarlines()[0];
        REQUIRE(barline.hasRehearsalSign());
        REQUIRE(barline.getRehearsalSign().getDescription() == "Solo");
        REQUIRE(barline.getRehearsalSign().getLetters() == "B");
    }
}

TEST_CASE("Formats/GuitarPro/KeySignatures")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/keys.gp5", score);

    {
        const System &system = score.getSystems()[0];
        auto barlines = system.getBarlines();

        REQUIRE(barlines[0].getKeySignature().getKeyType() ==
                KeySignature::Major);
        REQUIRE(barlines[0].getKeySignature().getNumAccidentals() == 1);
        REQUIRE(barlines[0].getKeySignature().isVisible());
        REQUIRE(barlines[0].getKeySignature().usesSharps());

        REQUIRE(barlines[1].getKeySignature().getKeyType() ==
                KeySignature::Major);
        REQUIRE(barlines[1].getKeySignature().getNumAccidentals() == 1);
        REQUIRE(barlines[1].getKeySignature().isVisible());
        REQUIRE(barlines[1].getKeySignature().usesSharps() == false);

        REQUIRE(barlines[1].getKeySignature().getNumAccidentals() ==
                barlines[2].getKeySignature().getNumAccidentals());
        REQUIRE(!barlines[2].getKeySignature().isVisible());
    }

    {
        const System &system = score.getSystems()[1];
        auto barlines = system.getBarlines();
        REQUIRE(barlines[0].getKeySignature().getKeyType() ==
                KeySignature::Minor);
        REQUIRE(barlines[0].getKeySignature().getNumAccidentals() == 0);
        REQUIRE(barlines[0].getKeySignature().isVisible());
        REQUIRE(barlines[0].getKeySignature().isCancellation());

        REQUIRE(!barlines[1].getKeySignature().isVisible());
    }
}

TEST_CASE("Formats/GuitarPro/TimeSignatures")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/time_signatures.gp5", score);

    const System &system = score.getSystems()[0];
    auto barlines = system.getBarlines();

    REQUIRE(barlines[0].getTimeSignature().getBeatsPerMeasure() == 3);
    REQUIRE(barlines[0].getTimeSignature().getNumPulses() == 3);
    REQUIRE(barlines[0].getTimeSignature().getBeatValue() == 4);
    REQUIRE(barlines[0].getTimeSignature().getMeterType() ==
            TimeSignature::Normal);
    REQUIRE(barlines[0].getTimeSignature().isVisible());

    REQUIRE(barlines[1].getTimeSignature().getBeatsPerMeasure() == 2);
    REQUIRE(barlines[1].getTimeSignature().getNumPulses() == 2);
    REQUIRE(barlines[1].getTimeSignature().getBeatValue() == 4);
    REQUIRE(barlines[1].getTimeSignature().isVisible());

    REQUIRE(barlines[2].getTimeSignature().getBeatsPerMeasure() == 4);
    REQUIRE(barlines[2].getTimeSignature().getNumPulses() == 4);
    REQUIRE(barlines[2].getTimeSignature().getBeatValue() == 4);
    REQUIRE(barlines[2].getTimeSignature().isVisible());

    REQUIRE(barlines[3].getTimeSignature().getBeatsPerMeasure() == 4);
    REQUIRE(barlines[3].getTimeSignature().getNumPulses() == 4);
    REQUIRE(barlines[3].getTimeSignature().getBeatValue() == 4);
    REQUIRE(barlines[3].getTimeSignature().isVisible() == false);
}

TEST_CASE("Formats/GuitarPro/AlternateEndings")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/alt_endings.gp5", score);

    const System &system = score.getSystems()[0];
    auto endings = system.getAlternateEndings();

    REQUIRE(endings.size() == 2);
    REQUIRE(endings[0].getNumbers().size() == 2);
    REQUIRE(endings[0].getNumbers()[0] == 1);
    REQUIRE(endings[0].getNumbers()[1] == 2);
    REQUIRE(endings[1].getNumbers().size() == 1);
    REQUIRE(endings[1].getNumbers()[0] == 3);
}

TEST_CASE("Formats/GuitarPro/Text")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/text.gp5", score);

    const System &system = score.getSystems()[0];
    auto texts = system.getTextItems();

    REQUIRE(texts.size() == 2);
    REQUIRE(texts[0].getContents() == "foo");
    REQUIRE(texts[1].getContents() == "bar");
}

TEST_CASE("Formats/GuitarPro/Positions")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/positions.gp5", score);

    const System &system = score.getSystems()[0];
    auto positions = system.getStaves()[0].getVoices()[0].getPositions();
    REQUIRE(positions.size() == 10);

    REQUIRE(positions[0].getDurationType() == Position::QuarterNote);
    REQUIRE(positions[0].hasProperty(Position::LetRing));

    REQUIRE(positions[1].getDurationType() == Position::QuarterNote);
    REQUIRE(positions[1].isRest());

    REQUIRE(positions[2].getDurationType() == Position::QuarterNote);
    REQUIRE(positions[2].hasProperty(Position::Staccato));

    REQUIRE(positions[3].getDurationType() == Position::QuarterNote);
    REQUIRE(positions[3].hasProperty(Position::Tap));

    REQUIRE(positions[4].getDurationType() == Position::HalfNote);
    REQUIRE(positions[4].hasProperty(Position::Vibrato));

    REQUIRE(positions[5].getDurationType() == Position::EighthNote);
    REQUIRE(positions[5].hasProperty(Position::Dotted));
    REQUIRE(positions[5].hasProperty(Position::PickStrokeDown));

    REQUIRE(positions[6].getDurationType() == Position::SixteenthNote);
    REQUIRE(positions[6].hasProperty(Position::PickStrokeUp));

    REQUIRE(positions[7].getDurationType() == Position::EighthNote);
    REQUIRE(positions[7].hasProperty(Position::Marcato));
    REQUIRE(positions[7].hasProperty(Position::PalmMuting));

    REQUIRE(positions[8].getDurationType() == Position::EighthNote);
    REQUIRE(positions[8].hasProperty(Position::Sforzando));

    const System &system2 = score.getSystems()[1];
    auto positions2 = system2.getStaves()[0].getVoices()[0].getPositions();

    REQUIRE(positions2[0].getDurationType() == Position::WholeNote);
    REQUIRE(positions2[0].hasProperty(Position::TremoloPicking));
}

TEST_CASE("Formats/GuitarPro/Notes")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/notes.gp5", score);

    const System &system = score.getSystems()[0];
    auto positions = system.getStaves()[0].getVoices()[0].getPositions();
    REQUIRE(positions.size() == 16);

    REQUIRE(positions[1].getNotes()[0].hasProperty(Note::Tied));
    REQUIRE(positions[2].getNotes()[0].hasProperty(Note::Muted));
    REQUIRE(positions[3].getNotes()[0].hasProperty(Note::HammerOnOrPullOff));
    REQUIRE(positions[5].getNotes()[0].hasProperty(Note::NaturalHarmonic));
    REQUIRE(positions[6].getNotes()[0].hasProperty(Note::GhostNote));

    REQUIRE(positions[7].getNotes()[0].hasProperty(Note::Octave8va));
    REQUIRE(positions[8].getNotes()[0].hasProperty(Note::Octave8vb));
    REQUIRE(positions[9].getNotes()[0].hasProperty(Note::Octave15ma));
    REQUIRE(positions[10].getNotes()[0].hasProperty(Note::Octave15mb));

    REQUIRE(positions[11].getNotes()[0].hasTrill());
    REQUIRE(positions[11].getNotes()[0].getTrilledFret() == 2);

    REQUIRE(positions[12].getNotes()[0].hasProperty(Note::SlideIntoFromBelow));
    REQUIRE(positions[12].getNotes()[0].hasProperty(Note::ShiftSlide));
}

TEST_CASE("Formats/GuitarPro/Tempos")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/tempos.gp5", score);

    const System &system = score.getSystems()[0];

#if 0
    REQUIRE(system.getTempoMarkers().size() == 2);
#else
    // Currently, multiple tempo markers in bars aren't imported.
    REQUIRE(system.getTempoMarkers().size() == 1);
#endif
    REQUIRE(system.getTempoMarkers()[0].getBeatsPerMinute() == 120);
}

TEST_CASE("Formats/GuitarPro/GraceNotes")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/gracenote.gp5", score);

    const System &system = score.getSystems()[0];
    auto positions = system.getStaves()[0].getVoices()[0].getPositions();

    REQUIRE(positions[1].hasProperty(Position::Acciaccatura));
    REQUIRE(positions[1].getNotes()[0].getFretNumber() == 6);
    REQUIRE(positions[1].getNotes()[0].getString() == 1);
    REQUIRE(positions[1].getDurationType() == Position::ThirtySecondNote);

    // TODO - GP6 apparently doesn't export this ...
#if 0
    REQUIRE(positions[1].getNotes()[0].hasProperty(Note::HammerOnOrPullOff));
#endif

    REQUIRE(positions[3].hasProperty(Position::Acciaccatura));
    REQUIRE(positions[3].getDurationType() == Position::SixtyFourthNote);

    REQUIRE(positions[5].hasProperty(Position::Acciaccatura));
    REQUIRE(positions[5].getDurationType() == Position::SixteenthNote);
}

TEST_CASE("Formats/GuitarPro/IrregularGroups")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/irregular.gp5", score);

    const System &system = score.getSystems()[0];
    auto groups = system.getStaves()[0].getVoices()[0].getIrregularGroupings();

    REQUIRE(groups.size() == 3);
    REQUIRE(groups[0].getLength() == 2);
    REQUIRE(groups[0].getNotesPlayed() == 3);
    REQUIRE(groups[0].getNotesPlayedOver() == 2);
    REQUIRE(groups[1].getLength() == 2);
    REQUIRE(groups[1].getNotesPlayed() == 3);
    REQUIRE(groups[1].getNotesPlayedOver() == 2);
    REQUIRE(groups[2].getLength() == 6);
    REQUIRE(groups[2].getNotesPlayed() == 6);
    REQUIRE(groups[2].getNotesPlayedOver() == 4);
}

static void
checkArtificialHarmonic(const Voice &voice, int start, int end,
                        ChordName::Key key, ChordName::Variation variation,
                        ArtificialHarmonic::Octave octave)
{
    for (int i = start; i <= end; ++i)
    {
        const Note &note = voice.getPositions()[i].getNotes()[0];
        REQUIRE(note.hasArtificialHarmonic());

        const ArtificialHarmonic &harmonic = note.getArtificialHarmonic();
        REQUIRE(harmonic.getKey() == key);
        REQUIRE(harmonic.getVariation() == variation);
        REQUIRE(harmonic.getOctave() == octave);
    }
}

TEST_CASE("Formats/GuitarPro/Harmonics")
{
    Score score;
    GuitarProImporter importer;
    importer.load(AppInfo::getAbsolutePath("data/harmonics.gp5"), score);

    using Octave = ArtificialHarmonic::Octave;

    {
        const Voice &voice =
            score.getSystems()[0].getStaves()[0].getVoices()[0];

        checkArtificialHarmonic(voice, 1, 1, ChordName::F, ChordName::NoVariation,
                       Octave::Loco);
        checkArtificialHarmonic(voice, 2, 3, ChordName::C, ChordName::NoVariation,
                       Octave::Octave8va);
        checkArtificialHarmonic(voice, 4, 5, ChordName::F, ChordName::NoVariation,
                       Octave::Octave8va);
        checkArtificialHarmonic(voice, 6, 8, ChordName::A, ChordName::NoVariation,
                       Octave::Octave8va);
        checkArtificialHarmonic(voice, 9, 9, ChordName::C, ChordName::NoVariation,
                       Octave::Octave15ma);
        checkArtificialHarmonic(voice, 10, 11, ChordName::D, ChordName::Sharp,
                       Octave::Octave15ma);
    }

    {
        const Voice &voice =
            score.getSystems()[1].getStaves()[0].getVoices()[0];

        checkArtificialHarmonic(voice, 0, 2, ChordName::D, ChordName::Sharp,
                       Octave::Octave15ma);
        checkArtificialHarmonic(voice, 3, 5, ChordName::F, ChordName::NoVariation,
                       Octave::Octave15ma);

        REQUIRE(voice.getPositions()[6].getNotes()[0].hasProperty(
            Note::NaturalHarmonic));
        REQUIRE(voice.getPositions()[7].getNotes()[0].hasTappedHarmonic());
        REQUIRE(voice.getPositions()[7].getNotes()[0].getTappedHarmonicFret() ==
                9);
    }
}
