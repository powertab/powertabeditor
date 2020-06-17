/*
 * Copyright (C) 2020 Cameron White
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

#include <catch2/catch.hpp>

#include <app/appinfo.h>
#include <formats/gp7/gp7importer.h>
#include <score/generalmidi.h>
#include <score/keysignature.h>
#include <score/note.h>
#include <score/playerchange.h>
#include <score/score.h>
#include <score/scoreinfo.h>
#include <score/tempomarker.h>
#include <score/timesignature.h>
#include <util/tostring.h>

TEST_CASE("Formats/Gp7Import/ScoreInfo/Basic", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(
        importer.load(AppInfo::getAbsolutePath("data/score_info.gp"), score));

    const ScoreInfo &info = score.getScoreInfo();
    REQUIRE(info.getScoreType() == ScoreInfo::ScoreType::Song);
    const SongData &data = info.getSongData();
    REQUIRE(data.getTitle() == "The title");
    REQUIRE(data.getSubtitle() == "The subtitle");
    REQUIRE(data.getArtist() == "The artist");
    REQUIRE(data.isAudioRelease());
    REQUIRE(data.getAudioReleaseInfo().getTitle() == "The album");
    REQUIRE(data.getAuthorInfo().getComposer() == "The composer");
    REQUIRE(data.getAuthorInfo().getLyricist() == "The lyricist");
    REQUIRE(data.getArranger() == "");
    REQUIRE(data.getTranscriber() == "The transcriber");
    REQUIRE(data.getCopyright() == "The copyright date");
    REQUIRE(data.getLyrics() == "");
    REQUIRE(data.getPerformanceNotes() == "The instructions");
}

// Verify that the "Words & Music" style header is imported properly.
TEST_CASE("Formats/Gp7Import/ScoreInfo/WordsAndMusic", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(importer.load(
        AppInfo::getAbsolutePath("data/words_and_music.gp"), score));

    const ScoreInfo &info = score.getScoreInfo();
    REQUIRE(info.getScoreType() == ScoreInfo::ScoreType::Song);
    const SongData &data = info.getSongData();
    REQUIRE(data.getAuthorInfo().getComposer() == "The author");
    REQUIRE(data.getAuthorInfo().getLyricist() == "The author");
}

// Verify that players and instruments are imported correctly.
// This file has three tracks, but the second track has two staves.
TEST_CASE("Formats/Gp7Import/Tracks", "")
{
    Score score;
    Gp7Importer importer;

    REQUIRE_NOTHROW(
        importer.load(AppInfo::getAbsolutePath("data/tracks.gp"), score));

    REQUIRE(score.getPlayers().size() == 4);
    REQUIRE(score.getInstruments().size() == 3);

    {
        const Player &player = score.getPlayers()[0];
        REQUIRE(player.getDescription() == "Jazz Guitar");
        REQUIRE(player.getTuning().getCapo() == 2);
        REQUIRE(Util::toString(player.getTuning()) == "D A D G B E");
    }

    {
        const Instrument &instrument = score.getInstruments()[0];
        REQUIRE(instrument.getDescription() == "Jazz ES");
        REQUIRE(instrument.getMidiPreset() ==
                Midi::MIDI_PRESET_ELECTRIC_GUITAR_JAZZ);
    }

    {
        const Player &player = score.getPlayers()[1];
        REQUIRE(player.getDescription() == "Distortion Guitar");
        REQUIRE(player.getTuning().getCapo() == 0);
        REQUIRE(Util::toString(player.getTuning()) == "B E A D G B E");
    }

    {
        const Player &player = score.getPlayers()[2];
        REQUIRE(player.getDescription() == "Distortion Guitar");
        REQUIRE(player.getTuning().getCapo() == 0);
        REQUIRE(Util::toString(player.getTuning()) == "E A D G B E");
    }

    {
        const Instrument &instrument = score.getInstruments()[1];
        REQUIRE(instrument.getDescription() == "Distortion");
        REQUIRE(instrument.getMidiPreset() ==
                Midi::MIDI_PRESET_DISTORTION_GUITAR);
    }

    {
        const Player &player = score.getPlayers()[3];
        REQUIRE(player.getDescription() == "Electric Bass");
        REQUIRE(player.getTuning().getCapo() == 0);
        REQUIRE(Util::toString(player.getTuning()) == "E A D G");
    }

    {
        const Instrument &instrument = score.getInstruments()[2];
        REQUIRE(instrument.getDescription() == "Clean Precision");
        REQUIRE(instrument.getMidiPreset() ==
                Midi::MIDI_PRESET_ELECTRIC_BASS_FINGER);
    }

    // There should be a staff for each player.
    REQUIRE(score.getSystems().size() == 1);
    const System &system = score.getSystems()[0];
    REQUIRE(system.getStaves().size() == 4);

    // Clefs should be imported properly.
    REQUIRE(system.getStaves()[0].getClefType() == Staff::TrebleClef);
    REQUIRE(system.getStaves()[1].getClefType() == Staff::TrebleClef);
    REQUIRE(system.getStaves()[2].getClefType() == Staff::BassClef);
    REQUIRE(system.getStaves()[3].getClefType() == Staff::BassClef);

    // There should be an initial player change to assign the players and
    // instruments.
    REQUIRE(system.getPlayerChanges().size() == 1);
    const PlayerChange &change = system.getPlayerChanges()[0];
    REQUIRE(change.getPosition() == 0);
    REQUIRE(change.getActivePlayers(0) == std::vector{ ActivePlayer(0, 0) });
    REQUIRE(change.getActivePlayers(1) == std::vector{ ActivePlayer(1, 1) });
    REQUIRE(change.getActivePlayers(2) == std::vector{ ActivePlayer(2, 1) });
    REQUIRE(change.getActivePlayers(3) == std::vector{ ActivePlayer(3, 2) });
}

TEST_CASE("Formats/Gp7Import/Notes", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/notes.gp"), score);

    const System &system = score.getSystems()[0];
    const Staff &staff = system.getStaves()[0];
    const Voice &voice = staff.getVoices()[0];

    {
        // This position does not have any special properties.
        const Position &pos = voice.getPositions()[0];
        REQUIRE(pos.getDurationType() == Position::QuarterNote);
        REQUIRE(!pos.hasProperty(Position::Dotted));
        REQUIRE(!pos.hasProperty(Position::DoubleDotted));
        REQUIRE(!pos.hasProperty(Position::Vibrato));
        REQUIRE(!pos.hasProperty(Position::WideVibrato));
        REQUIRE(!pos.hasProperty(Position::ArpeggioUp));
        REQUIRE(!pos.hasProperty(Position::ArpeggioDown));
        REQUIRE(!pos.hasProperty(Position::PickStrokeUp));
        REQUIRE(!pos.hasProperty(Position::PickStrokeDown));
        REQUIRE(!pos.hasProperty(Position::Staccato));
        REQUIRE(!pos.hasProperty(Position::Marcato));
        REQUIRE(!pos.hasProperty(Position::Sforzando));
        REQUIRE(!pos.hasProperty(Position::TremoloPicking));
        REQUIRE(!pos.hasProperty(Position::PalmMuting));
        REQUIRE(!pos.hasProperty(Position::Tap));
        REQUIRE(!pos.hasProperty(Position::Acciaccatura));
        REQUIRE(!pos.hasProperty(Position::LetRing));
        REQUIRE(!pos.isRest());

        // This note does not have any special properties.
        const Note &note = pos.getNotes()[0];
        REQUIRE(!note.hasProperty(Note::Tied));
        REQUIRE(!note.hasProperty(Note::Muted));
        REQUIRE(!note.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(!note.hasProperty(Note::HammerOnFromNowhere));
        REQUIRE(!note.hasProperty(Note::NaturalHarmonic));
        REQUIRE(!note.hasProperty(Note::GhostNote));
        REQUIRE(!note.hasProperty(Note::Octave8va));
        REQUIRE(!note.hasProperty(Note::Octave15ma));
        REQUIRE(!note.hasProperty(Note::Octave8vb));
        REQUIRE(!note.hasProperty(Note::Octave15mb));
        REQUIRE(!note.hasProperty(Note::SlideIntoFromBelow));
        REQUIRE(!note.hasProperty(Note::SlideIntoFromAbove));
        REQUIRE(!note.hasProperty(Note::ShiftSlide));
        REQUIRE(!note.hasProperty(Note::LegatoSlide));
        REQUIRE(!note.hasProperty(Note::SlideOutOfDownwards));
        REQUIRE(!note.hasProperty(Note::SlideOutOfUpwards));

        REQUIRE(!note.hasArtificialHarmonic());
        REQUIRE(!note.hasTappedHarmonic());
        REQUIRE(!note.hasTrill());
        REQUIRE(!note.hasLeftHandFingering());
    }

    {
        const Position &pos = voice.getPositions()[1];
        REQUIRE(pos.getDurationType() == Position::QuarterNote);
        REQUIRE(pos.hasProperty(Position::Dotted));
        REQUIRE(!pos.hasProperty(Position::DoubleDotted));
        REQUIRE(pos.hasProperty(Position::Sforzando));
        REQUIRE(pos.hasProperty(Position::Tap));

        const Note &note = pos.getNotes()[0];
        REQUIRE(note.getFretNumber() == 3);
        REQUIRE(note.getString() == 1);
    }

    {
        const Position &pos = voice.getPositions()[2];
        REQUIRE(pos.getDurationType() == Position::EighthNote);
        REQUIRE(pos.hasProperty(Position::Staccato));
        REQUIRE(pos.hasProperty(Position::Sforzando));

        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasProperty(Note::Muted));
        REQUIRE(note.hasProperty(Note::Octave15mb));
    }

    {
        const Position &pos = voice.getPositions()[3];
        REQUIRE(pos.getDurationType() == Position::EighthNote);
        REQUIRE(pos.hasProperty(Position::DoubleDotted));
        REQUIRE(pos.hasProperty(Position::Marcato));
        REQUIRE(pos.hasProperty(Position::WideVibrato));

        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasProperty(Note::Octave8vb));
        REQUIRE(note.hasProperty(Note::HammerOnFromNowhere));
    }

    {
        const Note &note = voice.getPositions()[4].getNotes()[0];
        REQUIRE(note.hasProperty(Note::Tied));
        REQUIRE(note.hasProperty(Note::Octave15ma));
    }

    {
        const Position &pos = voice.getPositions()[5];
        REQUIRE(pos.hasProperty(Position::PalmMuting));
        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasProperty(Note::GhostNote));
        REQUIRE(note.hasProperty(Note::Octave8va));
        REQUIRE(note.hasProperty(Note::SlideIntoFromBelow));
        REQUIRE(note.hasProperty(Note::LegatoSlide));
    }

    {
        const Position &pos = voice.getPositions()[6];
        REQUIRE(pos.hasProperty(Position::Vibrato));
        REQUIRE(pos.hasProperty(Position::LetRing));
    }

    {
        const Position &pos = voice.getPositions()[7];
        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasProperty(Note::HammerOnOrPullOff));
        REQUIRE(note.hasProperty(Note::SlideIntoFromAbove));
    }

    {
        const Position &pos = voice.getPositions()[8];
        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasProperty(Note::NaturalHarmonic));
    }

    {
        const Note &note = voice.getPositions()[9].getNotes()[0];
        REQUIRE(note.hasArtificialHarmonic());
        REQUIRE(note.hasProperty(Note::SlideOutOfDownwards));
    }

    {
        const Note &note = voice.getPositions()[10].getNotes()[0];
        REQUIRE(note.hasTappedHarmonic());
        REQUIRE(note.getTappedHarmonicFret() == 9);
        REQUIRE(note.hasProperty(Note::SlideOutOfUpwards));
    }

    {
        const Note &note = voice.getPositions()[14].getNotes()[0];
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::None);
    }

    {
        const Note &note = voice.getPositions()[15].getNotes()[0];
        REQUIRE(note.hasTrill());
        REQUIRE(note.getTrilledFret() == 4);
    }

    {
        const Note &note = voice.getPositions()[16].getNotes()[0];
        REQUIRE(note.hasProperty(Note::LegatoSlide));
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::Little);
    }

    {
        const Note &note = voice.getPositions()[17].getNotes()[0];
        REQUIRE(note.hasProperty(Note::ShiftSlide));
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::Ring);
    }

    {
        const Note &note = voice.getPositions()[18].getNotes()[0];
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::Middle);
    }

    {
        const Note &note = voice.getPositions()[19].getNotes()[0];
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::Index);
    }

    {
        const Position &pos = voice.getPositions()[20];
        REQUIRE(pos.getDurationType() == Position::EighthNote);
        REQUIRE(pos.isRest());
    }

    {
        const Position &pos = voice.getPositions()[21];
        REQUIRE(pos.hasProperty(Position::Acciaccatura));
        REQUIRE(pos.hasProperty(Position::PickStrokeDown));
        const Note &note = pos.getNotes()[0];
        REQUIRE(note.hasLeftHandFingering());
        REQUIRE(note.getLeftHandFingering().getFinger() ==
                LeftHandFingering::Finger::Thumb);
    }

    {
        const Position &pos = voice.getPositions()[22];
        REQUIRE(pos.hasProperty(Position::TremoloPicking));
    }

    {
        const Position &pos = voice.getPositions()[23];
        REQUIRE(pos.hasProperty(Position::Acciaccatura));
        REQUIRE(pos.hasProperty(Position::PickStrokeUp));
    }

    {
        const Position &pos = voice.getPositions()[24];
        REQUIRE(pos.hasProperty(Position::ArpeggioDown));
    }

    {
        const Position &pos = voice.getPositions()[25];
        REQUIRE(pos.hasProperty(Position::ArpeggioUp));
    }
}

TEST_CASE("Formats/Gp7Import/Bars", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/bars.gp"), score);

    {
        const System &system = score.getSystems()[0];
        REQUIRE(system.getBarlines().size() == 4);

        {
            const Barline &bar = system.getBarlines()[0];
            REQUIRE(bar.getBarType() == Barline::SingleBar);

            TimeSignature time_sig;
            time_sig.setVisible();
            REQUIRE(bar.getTimeSignature() == time_sig);

            KeySignature key_sig(KeySignature::Minor, 2, true);
            key_sig.setVisible();
            REQUIRE(bar.getKeySignature() == key_sig);

            REQUIRE(bar.hasRehearsalSign());
            REQUIRE(bar.getRehearsalSign().getLetters() == "A");
            REQUIRE(bar.getRehearsalSign().getDescription() == "Intro");
        }

        {
            const Barline &bar = system.getBarlines()[1];
            REQUIRE(!bar.hasRehearsalSign());

            TimeSignature time_sig;
            time_sig.setBeatsPerMeasure(12);
            time_sig.setNumPulses(12);
            time_sig.setBeatValue(8);
            time_sig.setVisible();
            REQUIRE(bar.getTimeSignature() == time_sig);

            KeySignature key_sig(KeySignature::Major, 4, false);
            key_sig.setVisible();
            REQUIRE(bar.getKeySignature() == key_sig);
        }

        {
            const Barline &bar = system.getBarlines()[2];
            REQUIRE(bar.hasRehearsalSign());
            REQUIRE(bar.getRehearsalSign().getLetters() == "B");
            REQUIRE(bar.getRehearsalSign().getDescription() ==
                    "Custom Section");

            TimeSignature time_sig;
            time_sig.setVisible();
            REQUIRE(bar.getTimeSignature() == time_sig);

            KeySignature key_sig(KeySignature::Major, 0, true);
            key_sig.setVisible();
            key_sig.setCancellation();
            key_sig.setSharps(false);
            key_sig.setNumAccidentals(4);
            REQUIRE(bar.getKeySignature() == key_sig);
        }
    }

    {
        const System &system = score.getSystems()[1];
        REQUIRE(system.getBarlines().size() == 4);

        {
            const Barline &bar = system.getBarlines()[0];
            REQUIRE(bar.getBarType() == Barline::RepeatStart);
            REQUIRE(!bar.getTimeSignature().isVisible());

            // First key signature in the system should be visible.
            REQUIRE(bar.getKeySignature().isVisible());
            REQUIRE(bar.getKeySignature().getNumAccidentals() == 0);
        }

        {
            const Barline &bar = system.getBarlines()[1];
            REQUIRE(bar.getBarType() == Barline::DoubleBar);
        }

        {
            const Barline &bar = system.getBarlines()[2];
            REQUIRE(bar.getBarType() == Barline::FreeTimeBar);
            REQUIRE(!bar.getKeySignature().isVisible());
        }

        {
            const Barline &bar = system.getBarlines()[3];
            REQUIRE(bar.getBarType() == Barline::RepeatEnd);
            REQUIRE(bar.getRepeatCount() == 3);
        }
    }

    {
        const System &system = score.getSystems()[2];
        REQUIRE(system.getBarlines().size() == 5);

        {
            const Barline &bar = system.getBarlines()[0];
            REQUIRE(bar.getBarType() == Barline::RepeatStart);
            // First key signature in the system should be visible.
            REQUIRE(bar.getKeySignature().isVisible());
        }

        {
            const Barline &bar = system.getBarlines()[1];
            REQUIRE(bar.getBarType() == Barline::RepeatEnd);
        }

        {
            const Barline &bar = system.getBarlines()[2];
            REQUIRE(bar.getBarType() == Barline::RepeatStart);
        }

        {
            const Barline &bar = system.getBarlines()[3];
            REQUIRE(bar.getBarType() == Barline::RepeatEnd);
        }

        {
            const Barline &bar = system.getBarlines()[4];
            REQUIRE(bar.getBarType() == Barline::DoubleBarFine);
        }
    }
}

TEST_CASE("Formats/Gp7Import/TempoChanges", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/bars.gp"), score);

    const System &system = score.getSystems()[0];
    REQUIRE(system.getTempoMarkers().size() == 2);

    {
        const TempoMarker &marker = system.getTempoMarkers()[0];
        REQUIRE(marker.getDescription() == "Fast Rock");
        REQUIRE(marker.getMarkerType() == TempoMarker::StandardMarker);
        REQUIRE(marker.getBeatType() == TempoMarker::Eighth);
        REQUIRE(marker.getBeatsPerMinute() == 80);
    }

    {
        const TempoMarker &marker = system.getTempoMarkers()[1];
        REQUIRE(marker.getDescription() == "Medium Rock");
        REQUIRE(marker.getMarkerType() == TempoMarker::StandardMarker);
        REQUIRE(marker.getBeatType() == TempoMarker::Quarter);
        REQUIRE(marker.getBeatsPerMinute() == 110);
    }
}

TEST_CASE("Formats/Gp7Import/Fermatas", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/fermatas.gp"), score);

    {
        const Voice &voice =
            score.getSystems()[0].getStaves()[0].getVoices()[0];
        for (int i = 0; i < 5; ++i)
            REQUIRE(voice.getPositions()[i].hasProperty(Position::Fermata));
    }

    {
        const Voice &voice =
            score.getSystems()[0].getStaves()[1].getVoices()[0];
        REQUIRE(voice.getPositions()[0].hasProperty(Position::Fermata));
        REQUIRE(!voice.getPositions()[1].hasProperty(Position::Fermata));
        REQUIRE(!voice.getPositions()[2].hasProperty(Position::Fermata));
        REQUIRE(voice.getPositions()[3].hasProperty(Position::Fermata));
    }
}

TEST_CASE("Formats/Gp7Import/Text", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/text.gp"), score);

    {
        const System &system = score.getSystems()[0];
        const Voice &voice = system.getStaves()[0].getVoices()[0];

        REQUIRE(system.getTextItems().size() == 1);
        {
            const TextItem &item = system.getTextItems()[0];
            REQUIRE(item.getPosition() ==
                    voice.getPositions()[4].getPosition());
            REQUIRE(item.getContents() == "My label");
        }

        REQUIRE(system.getChords().size() == 3);
        {
            const ChordName &chord = system.getChords()[0].getChordName();
            REQUIRE(Util::toString(chord) == "D7sus4");
        }
        {
            const ChordName &chord = system.getChords()[1].getChordName();
            REQUIRE(Util::toString(chord) == "F#/Bx");
        }
        {
            const ChordName &chord = system.getChords()[2].getChordName();
            REQUIRE(Util::toString(chord) == "Bb°add9");
        }
    }

    {
        const System &system = score.getSystems()[1];

        REQUIRE(system.getChords().size() == 24);

        std::vector<std::string> actual_names;
        for (int i = 0; i < 24; ++i)
        {
            actual_names.push_back(
                Util::toString(system.getChords()[i].getChordName()));
        }

        // Note: the last one should be D13b11, but we don't currently have
        // that...
        const std::vector<std::string> expected_names = {
            "D",         "Dm",    "D+",    "D°",     "Dsus2",   "Dsus4",
            "D6",        "Dm6",   "D7",    "Dmaj7",  "Dmaj7+5", "Dm7",
            "Dm/maj7",   "Dm7b5", "D°7",   "D7sus2", "D7sus4",  "Dmaj7sus2",
            "Dmaj7sus4", "D5",    "Dadd9", "D+9",    "D9b5",    "D13"
        };

        REQUIRE(actual_names == expected_names);
    }
}

TEST_CASE("Formats/Gp7Import/AlternateEndings", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/alternate_endings.gp"), score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getBarlines().size() == 5);
    REQUIRE(system.getAlternateEndings().size() == 2);

    {
        const AlternateEnding &ending = system.getAlternateEndings()[0];
        REQUIRE(ending.getPosition() == system.getBarlines()[1].getPosition());
        REQUIRE(ending.getNumbers() == std::vector({1, 3}));
    }

    {
        const AlternateEnding &ending = system.getAlternateEndings()[1];
        REQUIRE(ending.getPosition() == system.getBarlines()[2].getPosition());
        REQUIRE(ending.getNumbers() == std::vector({2, 4}));
    }
}

TEST_CASE("Formats/Gp7Import/Directions", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/directions.gp"), score);

    {
        const System &system = score.getSystems()[0];
        REQUIRE(system.getDirections().size() == 5);

        {
            const Direction &dir = system.getDirections()[0];
            REQUIRE(dir.getSymbols().size() == 1);
            REQUIRE(dir.getSymbols()[0].getSymbolType() ==
                    DirectionSymbol::SegnoSegno);
        }
        {
            const Direction &dir = system.getDirections()[1];
            REQUIRE(dir.getSymbols().size() == 2);
            REQUIRE(dir.getSymbols()[0].getSymbolType() ==
                    DirectionSymbol::DaCapo);
            REQUIRE(dir.getSymbols()[1].getSymbolType() ==
                    DirectionSymbol::DalSegnoAlCoda);
        }
        {
            const Direction &dir = system.getDirections()[2];
            REQUIRE(dir.getSymbols().size() == 1);
            REQUIRE(dir.getSymbols()[0].getSymbolType() ==
                    DirectionSymbol::Segno);
        }
        {
            const Direction &dir = system.getDirections()[3];
            REQUIRE(dir.getSymbols().size() == 3);
            REQUIRE(dir.getSymbols()[0].getSymbolType() ==
                    DirectionSymbol::DalSegno);
            REQUIRE(dir.getSymbols()[1].getSymbolType() ==
                    DirectionSymbol::DalSegnoSegnoAlFine);
            REQUIRE(dir.getSymbols()[2].getSymbolType() ==
                    DirectionSymbol::ToCoda);
        }
        {
            const Direction &dir = system.getDirections()[4];
            REQUIRE(dir.getSymbols().size() == 2);
            REQUIRE(dir.getSymbols()[0].getSymbolType() ==
                    DirectionSymbol::Fine);
            REQUIRE(dir.getSymbols()[1].getSymbolType() ==
                    DirectionSymbol::ToDoubleCoda);
        }
    }

    {
        const System &system = score.getSystems()[1];
        REQUIRE(system.getDirections().size() == 1);

        const Direction &dir = system.getDirections()[0];
        REQUIRE(dir.getSymbols().size() == 2);
        REQUIRE(dir.getSymbols()[0].getSymbolType() == DirectionSymbol::Coda);
        REQUIRE(dir.getSymbols()[1].getSymbolType() ==
                DirectionSymbol::DoubleCoda);
    }
}

TEST_CASE("Formats/Gp7Import/IrregularGroups", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/irregular_groups.gp"), score);

    const Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
    REQUIRE(voice.getIrregularGroupings().size() == 5);

    {
        const IrregularGrouping &group = voice.getIrregularGroupings()[0];
        REQUIRE(group.getPosition() == 0);
        REQUIRE(group.getLength() == 5);
        REQUIRE(group.getNotesPlayed() == 6);
        REQUIRE(group.getNotesPlayedOver() == 4);
    }
    {
        const IrregularGrouping &group = voice.getIrregularGroupings()[1];
        REQUIRE(group.getPosition() == 5);
        REQUIRE(group.getLength() == 4);
        REQUIRE(group.getNotesPlayed() == 3);
        REQUIRE(group.getNotesPlayedOver() == 2);
    }
    {
        const IrregularGrouping &group = voice.getIrregularGroupings()[2];
        REQUIRE(group.getPosition() == 9);
        REQUIRE(group.getLength() == 3);
        REQUIRE(group.getNotesPlayed() == 3);
        REQUIRE(group.getNotesPlayedOver() == 2);
    }
    {
        const IrregularGrouping &group = voice.getIrregularGroupings()[3];
        REQUIRE(group.getPosition() == 12);
        REQUIRE(group.getLength() == 5);
        REQUIRE(group.getNotesPlayed() == 6);
        REQUIRE(group.getNotesPlayedOver() == 4);
    }
    {
        const IrregularGrouping &group = voice.getIrregularGroupings()[4];
        REQUIRE(group.getPosition() == 18);
        REQUIRE(group.getLength() == 4);
        REQUIRE(group.getNotesPlayed() == 6);
        REQUIRE(group.getNotesPlayedOver() == 4);
    }
}

TEST_CASE("Formats/Gp7Import/Bends", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/bends.gp"), score);

    const Voice &voice = score.getSystems()[0].getStaves()[0].getVoices()[0];
    {
        const Note &note = voice.getPositions()[0].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::NormalBend);
        REQUIRE(note.getBend().getBentPitch() == 2);
    }
    {
        const Note &note = voice.getPositions()[1].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::NormalBend);
        REQUIRE(note.getBend().getBentPitch() == 2);
    }
    {
        const Note &note = voice.getPositions()[2].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::BendAndHold);
        REQUIRE(note.getBend().getBentPitch() == 4);
    }
    {
        const Note &note = voice.getPositions()[3].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::GradualRelease);
        REQUIRE(note.getBend().getBentPitch() == 4);
        REQUIRE(note.getBend().getReleasePitch() == 2);
    }
    {
        const Note &note = voice.getPositions()[4].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::PreBendAndHold);
        REQUIRE(note.getBend().getBentPitch() == 4);
        REQUIRE(note.getBend().getReleasePitch() == 6);
    }
    {
        const Note &note = voice.getPositions()[5].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::PreBendAndHold);
        REQUIRE(note.getBend().getBentPitch() == 6);
        REQUIRE(note.getBend().getReleasePitch() == 6);
    }
    {
        const Note &note = voice.getPositions()[6].getNotes()[0];
        REQUIRE(note.hasBend());
        // This one doesn't translate perfectly from how GP's bends work.
        REQUIRE(note.getBend().getType() == Bend::BendAndRelease);
        REQUIRE(note.getBend().getBentPitch() == 10);
        REQUIRE(note.getBend().getReleasePitch() == 0);
    }
    {
        const Note &note = voice.getPositions()[7].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::PreBendAndRelease);
        REQUIRE(note.getBend().getBentPitch() == 4);
        REQUIRE(note.getBend().getReleasePitch() == 0);
    }
    {
        const Note &note = voice.getPositions()[8].getNotes()[0];
        REQUIRE(note.hasBend());
        REQUIRE(note.getBend().getType() == Bend::PreBend);
        REQUIRE(note.getBend().getBentPitch() == 4);
    }
}

static void
checkHarmonics(const Voice &voice, int start, int end, ChordName::Key key,
               ChordName::Variation variation,
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

TEST_CASE("Formats/Gp7Import/Harmonics", "")
{
    Score score;
    Gp7Importer importer;
    importer.load(AppInfo::getAbsolutePath("data/harmonics.gp"), score);

    using Octave = ArtificialHarmonic::Octave;

    {
        const Voice &voice =
            score.getSystems()[0].getStaves()[0].getVoices()[0];

        checkHarmonics(voice, 1, 1, ChordName::F, ChordName::NoVariation,
                       Octave::Loco);
        checkHarmonics(voice, 2, 3, ChordName::C, ChordName::NoVariation,
                       Octave::Octave8va);
        checkHarmonics(voice, 4, 5, ChordName::F, ChordName::NoVariation,
                       Octave::Octave8va);
        checkHarmonics(voice, 6, 8, ChordName::A, ChordName::NoVariation,
                       Octave::Octave8va);
        checkHarmonics(voice, 9, 9, ChordName::C, ChordName::NoVariation,
                       Octave::Octave15ma);
        checkHarmonics(voice, 10, 11, ChordName::D, ChordName::Sharp,
                       Octave::Octave15ma);
    }

    {
        const Voice &voice =
            score.getSystems()[1].getStaves()[0].getVoices()[0];

        checkHarmonics(voice, 0, 2, ChordName::D, ChordName::Sharp,
                       Octave::Octave15ma);
        checkHarmonics(voice, 3, 5, ChordName::F, ChordName::NoVariation,
                       Octave::Octave15ma);
    }
}
