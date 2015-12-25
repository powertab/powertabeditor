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

#include <app/appinfo.h>
#include <formats/powertab/powertabimporter.h>
#include <formats/powertab_old/powertaboldimporter.h>
#include <formats/powertab_old/powertabdocument/powertabdocument.h>
#include <score/score.h>

static void loadTest(FileFormatImporter &importer, const char *filename,
                     Score &score)
{
    importer.load(AppInfo::getAbsolutePath(filename), score);
}

TEST_CASE("Formats/PowerTabOldImport/SongHeader", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/song_header.ptb", score);

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
    loadTest(importer, "data/guitars.ptb", score);

    // Should contain two players from the guitar score and one from the bass score.
    REQUIRE(score.getPlayers().size() == 3);
    REQUIRE(score.getInstruments().size() == 3);

    const Player &player1 = score.getPlayers()[0];
    const Player &player2 = score.getPlayers()[1];
    const Player &player3 = score.getPlayers()[2];
    REQUIRE(player1.getDescription() == "First Player");
    REQUIRE(player2.getDescription() == "Second Player");
    REQUIRE(player2.getTuning().getStringCount() == 7);
    REQUIRE(player3.getTuning().getStringCount() == 4);

    REQUIRE(score.getInstruments()[0].getDescription() == "Electric Guitar (clean)");
}

TEST_CASE("Formats/PowerTabOldImport/Barlines", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/barlines.ptb", score);

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
    loadTest(importer, "data/tempo_markers.ptb", score);

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
    loadTest(importer, "data/alternate_endings.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getAlternateEndings().size() == 1);
    const AlternateEnding &ending = system.getAlternateEndings()[0];

    std::vector<int> numbers;
    numbers.push_back(2);
    numbers.push_back(3);
    REQUIRE(ending.getNumbers() == numbers);

    REQUIRE(ending.hasDaCapo());
    REQUIRE(!ending.hasDalSegno());
    REQUIRE(!ending.hasDalSegnoSegno());
}

// TODO - re-enable this test when merging of directions is implemented.
TEST_CASE("Formats/PowerTabOldImport/Directions", "[!hide]")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/directions.ptb", score);

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
    loadTest(importer, "data/staves.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getStaves().size() == 3);
    const Staff &staff1 = system.getStaves()[0];
    const Staff &staff2 = system.getStaves()[1];

    REQUIRE(staff1.getClefType() == Staff::TrebleClef);
    REQUIRE(staff1.getStringCount() == 6);
    REQUIRE(staff1.getDynamics().size() == 1);

    REQUIRE(staff2.getClefType() == Staff::BassClef);
    REQUIRE(staff2.getStringCount() == 7);
    REQUIRE(staff2.getDynamics().size() == 1);

    const Dynamic &dynamic = staff2.getDynamics()[0];
    REQUIRE(dynamic.getPosition() == 4);
    REQUIRE(dynamic.getVolume() == Dynamic::mp);
}

TEST_CASE("Formats/PowerTabOldImport/Positions", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/positions.ptb", score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const Voice &voice = staff.getVoices().front();

    REQUIRE(voice.getPositions().size() == 3);
    const Position &pos1 = voice.getPositions()[0];
    const Position &pos2 = voice.getPositions()[1];
    const Position &pos3 = voice.getPositions()[2];

    REQUIRE(pos1.getPosition() == 0);
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
    loadTest(importer, "data/notes.ptb", score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const Voice &voice = staff.getVoices().front();
    const Note &note1 = voice.getPositions()[0].getNotes()[0];
    const Note &note2 = voice.getPositions()[1].getNotes()[0];

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
    loadTest(importer, "data/guitar_ins.ptb", score);

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

TEST_CASE("Formats/PowerTabOldImport/ChordText", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/chordtext.ptb", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getChords().size() == 2);

    const ChordText &chordText1 = system.getChords()[0];
    REQUIRE(chordText1.getPosition() == 4);

    const ChordName &chord1 = chordText1.getChordName();

    REQUIRE(chord1.getTonicKey() == ChordName::F);
    REQUIRE(chord1.getTonicVariation() == ChordName::Sharp);
    REQUIRE(chord1.getBassKey() == ChordName::B);
    REQUIRE(chord1.getBassVariation() == ChordName::Flat);

    REQUIRE(chord1.getFormula() == ChordName::Major7th);
    REQUIRE(chord1.hasModification(ChordName::Added9th));
    REQUIRE(chord1.hasModification(ChordName::Raised11th));
    REQUIRE(chord1.hasBrackets());

    const ChordName chord2 = system.getChords()[1].getChordName();
    REQUIRE(chord2.getTonicKey() == ChordName::C);
    REQUIRE(chord2.getTonicVariation() == ChordName::NoVariation);
    REQUIRE(chord2.getBassKey() == ChordName::F);
    REQUIRE(chord2.getBassVariation() == ChordName::NoVariation);
}

TEST_CASE("Formats/PowerTabOldImport/Bends", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/bends.ptb", score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const Voice &voice = staff.getVoices().front();
    const Note &note1 = voice.getPositions()[0].getNotes()[0];
    const Note &note2 = voice.getPositions()[1].getNotes()[0];

    REQUIRE(note1.hasBend());
    const Bend &bend1 = note1.getBend();
    REQUIRE(bend1.getType() == Bend::BendAndHold);
    REQUIRE(bend1.getBentPitch() == 3);
    REQUIRE(bend1.getReleasePitch() == 0);
    REQUIRE(bend1.getDuration() == 1);
    REQUIRE(bend1.getStartPoint() == Bend::MidPoint);
    REQUIRE(bend1.getEndPoint() == Bend::HighPoint);

    REQUIRE(note2.hasBend());
    const Bend &bend2 = note2.getBend();
    REQUIRE(bend2.getType() == Bend::ImmediateRelease);
    REQUIRE(bend2.getStartPoint() == Bend::MidPoint);
    REQUIRE(bend2.getEndPoint() == Bend::MidPoint);
}

TEST_CASE("Formats/PowerTabOldImport/FloatingText", "")
{
    Score score;
    PowerTabOldImporter importer;
    loadTest(importer, "data/floating_text.ptb", score);

    REQUIRE(score.getSystems()[0].getTextItems().empty());
    REQUIRE(score.getSystems()[2].getTextItems().empty());

    const System &system1 = score.getSystems()[1];
    REQUIRE(system1.getTextItems().size() == 1);
    REQUIRE(system1.getTextItems()[0].getPosition() == 11);
    REQUIRE(system1.getTextItems()[0].getContents() == "foo");
}

TEST_CASE("Formats/PowerTabOldImport/MergeMultiBarRests", "")
{
    Score score;
    Score expected_score;

    PowerTabOldImporter old_importer;
    PowerTabImporter importer;
    loadTest(old_importer, "data/merge_multibar_rests.ptb", score);
    loadTest(importer, "data/merge_multibar_rests_correct.pt2", expected_score);

    REQUIRE(score == expected_score);
}
