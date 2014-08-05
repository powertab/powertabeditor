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

#include <catch.hpp>

#include <formats/guitar_pro/guitarproimporter.h>
#include <QCoreApplication>
#include <score/score.h>

static void loadTest(GuitarProImporter &importer, const char *filename,
                     Score &score)
{
    QString path = QCoreApplication::applicationDirPath();
    path += "/";
    path += filename;

    importer.load(path.toStdString(), score);
}

TEST_CASE("Formats/GuitarPro/Barlines", "")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/barlines.gp5", score);

    const System &system = score.getSystems()[0];
    auto barlines = system.getBarlines();

    REQUIRE(barlines.size() == 6);
    REQUIRE(barlines[1].getBarType() == Barline::RepeatStart);
    REQUIRE(barlines[2].getBarType() == Barline::RepeatEnd);
    REQUIRE(barlines[2].getRepeatCount() == 2);
    REQUIRE(barlines[3].getBarType() == Barline::RepeatStart);
    REQUIRE(barlines[4].getBarType() == Barline::DoubleBar);
    REQUIRE(barlines[5].getBarType() == Barline::RepeatEnd);
    REQUIRE(barlines[5].getRepeatCount() == 4);
}

TEST_CASE("Formats/GuitarPro/RehearsalSigns", "")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/rehearsal_signs.gp5", score);

    const System &system = score.getSystems()[0];
    auto barlines = system.getBarlines();

    const Barline &barline1 = barlines[2];
    const Barline &barline2 = barlines[3];

    REQUIRE(barline1.hasRehearsalSign());
    REQUIRE(barline1.getRehearsalSign().getDescription() == "Chorus");
    REQUIRE(barline1.getRehearsalSign().getLetters() == "A");
    REQUIRE(barline2.hasRehearsalSign());
    REQUIRE(barline2.getRehearsalSign().getDescription() == "Solo");
    REQUIRE(barline2.getRehearsalSign().getLetters() == "B");
}

TEST_CASE("Formats/GuitarPro/KeySignatures", "")
{
    Score score;
    GuitarProImporter importer;
    loadTest(importer, "data/keys.gp5", score);

    const System &system = score.getSystems()[0];
    auto barlines = system.getBarlines();

    REQUIRE(barlines[0].getKeySignature().getKeyType() == KeySignature::Major);
    REQUIRE(barlines[0].getKeySignature().getNumAccidentals() == 1);
    REQUIRE(barlines[0].getKeySignature().isVisible());
    REQUIRE(barlines[0].getKeySignature().usesSharps());

    REQUIRE(barlines[1].getKeySignature().getKeyType() == KeySignature::Major);
    REQUIRE(barlines[1].getKeySignature().getNumAccidentals() == 1);
    REQUIRE(barlines[1].getKeySignature().isVisible());
    REQUIRE(barlines[1].getKeySignature().usesSharps() == false);

    REQUIRE(barlines[1].getKeySignature().getNumAccidentals() ==
            barlines[2].getKeySignature().getNumAccidentals());
    REQUIRE(!barlines[2].getKeySignature().isVisible());

    REQUIRE(barlines[3].getKeySignature().getKeyType() == KeySignature::Minor);
    REQUIRE(barlines[3].getKeySignature().getNumAccidentals() == 0);
    REQUIRE(barlines[3].getKeySignature().isVisible());
    REQUIRE(barlines[3].getKeySignature().isCancellation());

    REQUIRE(!barlines[4].getKeySignature().isVisible());
}

TEST_CASE("Formats/GuitarPro/TimeSignatures", "")
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

TEST_CASE("Formats/GuitarPro/AlternateEndings", "")
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