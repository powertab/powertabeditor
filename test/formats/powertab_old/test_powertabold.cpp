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

#include <formats/powertab_old/powertaboldimporter.h>
#include <formats/powertab_old/powertabdocument/powertabdocument.h>
#include <score/score.h>

TEST_CASE("Formats/PowerTabOldImport/SongHeader", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/song_header.ptb", score);

    const SongData &data = score.getScoreInfo().getSongData();

    REQUIRE(data.getTitle() == "Some Title");
    REQUIRE(data.getArtist() == "Some Artist");

    REQUIRE(data.isVideoRelease());

    REQUIRE(data.getAuthorInfo().getComposer() == "Some Author");
    REQUIRE(data.getAuthorInfo().getLyricist() == "Some Lyricist");

    REQUIRE(data.getArranger() == "Some Arranger");
    REQUIRE(data.getTranscriber() == "Some Transcriber");
    REQUIRE(data.getCopyright() == "2001");
    REQUIRE(data.getLyrics() == "Some lyrics");
    REQUIRE(data.getPerformanceNotes() == "Some notes.");
}

TEST_CASE("Formats/PowerTabOldImport/Guitars", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/guitars.ptb", score);

    REQUIRE(score.getPlayers().size() == 2);
    REQUIRE(score.getInstruments().size() == 2);

    const Player &player1 = score.getPlayers()[0];
    const Player &player2 = score.getPlayers()[1];
    REQUIRE(player1.getDescription() == "First Player");
    REQUIRE(player2.getDescription() == "Second Player");
    REQUIRE(player2.getTuning().getStringCount() == 7);

    REQUIRE(score.getInstruments()[0].getDescription() == "Electric Guitar (clean)");
}

TEST_CASE("Formats/PowerTabOldImport/Barlines", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/barlines.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getBarlines().size() == 3);

    const Barline &barline = system.getBarlines()[1];

    REQUIRE(barline.getBarType() == Barline::DoubleBar);

    REQUIRE(barline.hasRehearsalSign());
    REQUIRE(barline.getRehearsalSign().getLetters() == "A");
    REQUIRE(barline.getRehearsalSign().getDescription() == "Intro");

    const KeySignature &key = barline.getKeySignature();
    REQUIRE(key.getKeyType() == KeySignature::Minor);
    REQUIRE(key.getNumAccidentals() == 2);
    REQUIRE(key.usesSharps());
    REQUIRE(!key.isCancellation());
    REQUIRE(key.isVisible());

    const TimeSignature &time = barline.getTimeSignature();
    REQUIRE(time.getMeterType() == TimeSignature::Normal);
    REQUIRE(time.getBeatsPerMeasure() == 5);
    REQUIRE(time.getBeatValue() == 8);
    REQUIRE(time.getBeamingPattern()[0] == 3);
    REQUIRE(time.getBeamingPattern()[1] == 2);
    REQUIRE(time.getBeamingPattern()[2] == 0);
    REQUIRE(time.getBeamingPattern()[3] == 0);
    REQUIRE(time.getNumPulses() == 5);
    REQUIRE(time.isVisible());
}

TEST_CASE("Formats/PowerTabOldImport/TempoMarkers", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/tempo_markers.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getTempoMarkers().size() == 1);
    const TempoMarker &tempo = system.getTempoMarkers()[0];

    REQUIRE(tempo.getPosition() == 3);
    REQUIRE(tempo.getMarkerType() == TempoMarker::StandardMarker);
    REQUIRE(tempo.getBeatType() == TempoMarker::Half);
    REQUIRE(tempo.getListessoBeatType() == TempoMarker::Quarter);
    REQUIRE(tempo.getTripletFeel() == TempoMarker::TripletFeelSixteenth);
    REQUIRE(tempo.getAlterationOfPace() == TempoMarker::NoAlterationOfPace);
    REQUIRE(tempo.getBeatsPerMinute() == 99);
    REQUIRE(tempo.getDescription() == "Fast Rock");
}

TEST_CASE("Formats/PowerTabOldImport/AlternateEndings", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/alternate_endings.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getAlternateEndings().size() == 1);
    const AlternateEnding &ending = system.getAlternateEndings()[0];

    REQUIRE(ending.getPosition() == 12);

    std::vector<int> numbers;
    numbers.push_back(2);
    numbers.push_back(3);
    REQUIRE(ending.getNumbers() == numbers);

    REQUIRE(ending.hasDaCapo());
    REQUIRE(!ending.hasDalSegno());
    REQUIRE(!ending.hasDalSegnoSegno());
}

TEST_CASE("Formats/PowerTabOldImport/Directions", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/directions.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getDirections().size() == 1);
    const Direction &direction = system.getDirections()[0];

    REQUIRE(direction.getPosition() == 9);
    REQUIRE(direction.getSymbols().size() == 2);

    const DirectionSymbol &symbol1 = direction.getSymbols()[0];
    REQUIRE(symbol1.getSymbolType() == DirectionSymbol::Coda);
    REQUIRE(symbol1.getRepeatNumber() == 1);
    REQUIRE(symbol1.getActiveSymbolType() == DirectionSymbol::ActiveNone);

    const DirectionSymbol &symbol2 = direction.getSymbols()[1];
    REQUIRE(symbol2.getSymbolType() == DirectionSymbol::Segno);
    REQUIRE(symbol2.getRepeatNumber() == 2);
    REQUIRE(symbol2.getActiveSymbolType() == DirectionSymbol::ActiveDaCapo);
}

TEST_CASE("Formats/PowerTabOldImport/Staves", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/staves.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getStaves().size() == 2);
    const Staff &staff1 = system.getStaves()[0];
    const Staff &staff2 = system.getStaves()[1];

    REQUIRE(staff1.getClefType() == Staff::TrebleClef);
    REQUIRE(staff1.getStringCount() == 6);
    REQUIRE(staff1.getViewType() == Staff::GuitarView);
    REQUIRE(staff1.getDynamics().size() == 0);

    REQUIRE(staff2.getClefType() == Staff::BassClef);
    REQUIRE(staff2.getStringCount() == 7);
    REQUIRE(staff2.getViewType() == Staff::GuitarView);
    REQUIRE(staff2.getDynamics().size() == 1);

    const Dynamic &dynamic = staff2.getDynamics()[0];
    REQUIRE(dynamic.getPosition() == 4);
    REQUIRE(dynamic.getVolume() == Dynamic::mp);
}

TEST_CASE("Formats/PowerTabOldImport/Positions", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/positions.ptb", score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const int voice = 0;

    REQUIRE(staff.getVoice(voice).size() == 3);
    const Position &pos1 = staff.getVoice(voice)[0];
    const Position &pos2 = staff.getVoice(voice)[1];
    const Position &pos3 = staff.getVoice(voice)[2];

    REQUIRE(pos1.getPosition() == 2);
    REQUIRE(pos1.getDurationType() == Position::HalfNote);
    REQUIRE(!pos1.hasMultiBarRest());
    REQUIRE(pos1.hasProperty(Position::DoubleDotted));
    REQUIRE(pos1.hasProperty(Position::Vibrato));
    REQUIRE(pos1.hasProperty(Position::ArpeggioDown));
    REQUIRE(pos1.hasProperty(Position::PickStrokeUp));
    REQUIRE(pos1.hasProperty(Position::LetRing));
    REQUIRE(!pos1.hasProperty(Position::PalmMuting));

    REQUIRE(pos2.isRest());
    REQUIRE(pos2.getDurationType() == Position::EighthNote);

    REQUIRE(pos3.getMultiBarRestCount() == 3);
}

TEST_CASE("Formats/PowerTabOldImport/Notes", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/notes.ptb", score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const Note &note1 = staff.getVoice(0)[0].getNotes()[0];
    const Note &note2 = staff.getVoice(0)[1].getNotes()[0];

    REQUIRE(note1.getFretNumber() == 3);
    REQUIRE(note1.getString() == 3);
    REQUIRE(note1.hasTappedHarmonic());
    REQUIRE(note1.getTappedHarmonicFret() == 15);

    REQUIRE(note1.hasProperty(Note::GhostNote));
    REQUIRE(note1.hasProperty(Note::HammerOnOrPullOff));

    REQUIRE(!note2.hasTappedHarmonic());
    REQUIRE(note2.hasTrill());
    REQUIRE(note2.getTrilledFret() == 1);

    REQUIRE(note2.hasProperty(Note::Octave15mb));
    REQUIRE(note2.hasProperty(Note::NaturalHarmonic));
}

TEST_CASE("Formats/PowerTabOldImport/GuitarIns", "")
{
    Score score;
    PowerTabOldImporter importer;
    importer.load("data/guitar_ins.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getPlayerChanges().size() == 4);

    const PlayerChange &change1 = system.getPlayerChanges()[0];
    const PlayerChange &change2 = system.getPlayerChanges()[1];
    const PlayerChange &change3 = system.getPlayerChanges()[2];
    const PlayerChange &change4 = system.getPlayerChanges()[3];

    REQUIRE(change1.getActivePlayers(0).size() == 1);
    REQUIRE(change1.getActivePlayers(1).empty());

    REQUIRE(change2.getActivePlayers(0).size() == 1);
    REQUIRE(change2.getActivePlayers(0).front().getPlayerNumber() == 0);
    REQUIRE(change2.getActivePlayers(1).size() == 1);

    REQUIRE(change3.getActivePlayers(0).size() == 1);
    REQUIRE(change3.getActivePlayers(0).front().getPlayerNumber() == 1);
    REQUIRE(change3.getActivePlayers(1).size() == 1);

    REQUIRE(change4.getActivePlayers(0).size() == 1);
    REQUIRE(change4.getActivePlayers(1).size() == 2);
}
