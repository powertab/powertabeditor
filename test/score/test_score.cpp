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
  
#include <doctest/doctest.h>

#include <app/paths.h>
#include <formats/powertab/powertabimporter.h>
#include <score/score.h>

TEST_CASE("Score/Score/Systems")
{
    Score score;

    REQUIRE(score.getSystems().size() == 0);

    System system;
    score.insertSystem(system);
    REQUIRE(score.getSystems().size() == 1);
    score.removeSystem(0);
    REQUIRE(score.getSystems().size() == 0);
}

TEST_CASE("Score/Score/Players")
{
    Score score;

    REQUIRE(score.getPlayers().size() == 0);

    Player player;
    score.insertPlayer(player);
    REQUIRE(score.getPlayers().size() == 1);
    score.removePlayer(0);
    REQUIRE(score.getPlayers().size() == 0);
}

TEST_CASE("Score/Score/Instruments")
{
    Score score;

    REQUIRE(score.getInstruments().size() == 0);

    Instrument instrument;
    score.insertInstrument(instrument);
    REQUIRE(score.getInstruments().size() == 1);
    score.removeInstrument(0);
    REQUIRE(score.getInstruments().size() == 0);
}

TEST_CASE("Score/Score/ViewFilters")
{
    Score score;

    REQUIRE(score.getViewFilters().size() == 0);

    ViewFilter filter1;
    filter1.addRule(FilterRule(FilterRule::Subject::PlayerName, "foo"));
    score.insertViewFilter(filter1);

    ViewFilter filter2;
    filter2.addRule(FilterRule(FilterRule::Subject::PlayerName, "bar"));
    score.insertViewFilter(filter2);

    REQUIRE(score.getViewFilters().size() == 2);
    score.removeViewFilter(1);
    REQUIRE(score.getViewFilters().size() == 1);
    REQUIRE(score.getViewFilters()[0] == filter1);
}

TEST_CASE("Score/Score/ChordDiagrams")
{
    Score score;

    REQUIRE(score.getChordDiagrams().size() == 0);

    ChordDiagram diagram1;
    diagram1.setTopFret(1);
    score.insertChordDiagram(diagram1);

    ChordDiagram diagram2;
    diagram2.setTopFret(2);
    score.insertChordDiagram(diagram2);

    REQUIRE(score.getChordDiagrams().size() == 2);
    score.removeChordDiagram(1);
    REQUIRE(score.getChordDiagrams().size() == 1);
    REQUIRE(score.getChordDiagrams()[0] == diagram1);
}

// Verify that we don't rely on the order of JSON keys (see bug #294).
TEST_CASE("Score/Score/Deserialization")
{
    Score score;
    PowerTabImporter importer;
    importer.load(Paths::getAppDirPath("data/reordered.pt2"), score);

    REQUIRE(score.getSystems().size() == 2);
    REQUIRE(score.getSystems()[0].getAlternateEndings().size() == 2);
}

TEST_CASE("Score/Score/FindAllChordNames")
{
    ChordName name;
    name.setTonicKey(ChordName::F);
    name.setTonicVariation(ChordName::Flat);
    name.setBassKey(ChordName::F);
    name.setBassVariation(ChordName::Flat);
    name.setFormula(ChordName::Major7th);

    ChordName name2 = name;
    name2.setModification(ChordName::Extended11th);

    ChordName name3 = name;
    name3.setModification(ChordName::Extended9th);

    Score score;

    ChordDiagram diagram1;
    diagram1.setChordName(name2);
    ChordDiagram diagram2;
    diagram2.setChordName(name);
    score.insertChordDiagram(diagram1);
    score.insertChordDiagram(diagram2);

    System system;
    system.insertChord(ChordText(2, name3));
    system.insertChord(ChordText(4, name));
    score.insertSystem(system);

    REQUIRE(ScoreUtils::findAllChordNames(score) ==
            std::vector<ChordName>({ name2, name, name3 }));
}

TEST_CASE("Score/Score/UniquePlayerName")
{
    Score score;
    Player player1;
    player1.setDescription("Player 1");
    Player player2;
    player2.setDescription("Player 3");

    score.insertPlayer(player1);
    score.insertPlayer(player2);

    REQUIRE(ScoreUtils::createUniquePlayerName(score, "Player") == "Player 4");
}

TEST_CASE("Score/Score/UniqueInstrumentName")
{
    Score score;
    Instrument i1;
    i1.setDescription("Inst 1");
    Instrument i2;
    i2.setDescription("Inst 3");

    score.insertInstrument(i1);
    score.insertInstrument(i2);

    REQUIRE(ScoreUtils::createUniqueInstrumentName(score, "Inst") == "Inst 4");
}
