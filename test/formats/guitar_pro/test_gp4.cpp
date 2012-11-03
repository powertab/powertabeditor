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

#include <boost/assign.hpp>

#include <formats/guitar_pro/guitarproimporter.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

const std::string FILE_TEST1 = "data/test1.gp4";

struct Gp4Fixture
{
    Gp4Fixture()
    {
        REQUIRE_NOTHROW(importer.load(FILE_TEST1));
        doc = importer.load(FILE_TEST1);
        score = doc->GetScore(0);
    }

    GuitarProImporter importer;
    boost::shared_ptr<PowerTabDocument> doc;
    Score* score;
};

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/HeaderConversion",
                 "The header and song information should be imported correctly.")
{
    const PowerTabFileHeader& header = doc->GetHeader();

    REQUIRE(header.GetSongTitle() == "FileName");
    REQUIRE(header.GetSongArtist() == "Artist");
    REQUIRE(header.GetSongAudioReleaseTitle() == "Album");
    REQUIRE(header.GetSongComposer() == "Author");
    REQUIRE(header.GetSongCopyright() == "Copyright 2011");
    REQUIRE(header.GetSongGuitarScoreTranscriber() == "Tab Creator");
    REQUIRE(header.GetSongGuitarScoreNotes()  == "Some Comments");
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/TrackImport",
                 "The guitars should be imported correctly.")
{
    REQUIRE(score->GetGuitarCount() == 2u);

    Score::GuitarConstPtr guitar1 = score->GetGuitar(0);

    // track information
    REQUIRE(guitar1->GetDescription() == "First Track");
    REQUIRE(guitar1->GetNumber() == 0u);
    REQUIRE(guitar1->GetCapo() == 1u);

    using namespace midi;
    using namespace boost::assign;
    std::vector<uint8_t> notes;
    notes += MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
            MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2, MIDI_NOTE_B1;
    // 7-string tuning
    REQUIRE(guitar1->GetTuning() == Tuning("", 0, false, notes));

    // midi settings
    REQUIRE(guitar1->GetPreset() == midi::MIDI_PRESET_OVERDRIVEN_GUITAR);
    REQUIRE(guitar1->GetInitialVolume() == 95u);
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/ReadBarline/BarlineProperties", "")
{
    System::BarlineConstPtr barline1 = score->GetSystem(0)->GetStartBar();
    REQUIRE(barline1->IsRepeatStart());
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/ReadBarline/TimeSignature", "")
{
    const TimeSignature& timeSig1 = score->GetSystem(0)->GetStartBar()->GetTimeSignature();

    REQUIRE(timeSig1.IsSameMeter(TimeSignature(5, 4)));
    REQUIRE(timeSig1.IsShown());
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/ReadBarline/KeySignature", "")
{
    /*System::BarlineConstPtr barline1 = score->GetSystem(0)->GetStartBar();

    // TODO - generate appropriate test case for key signatures (TuxGuitar export seems to be broken??)
    REQUIRE(barline1->GetKeySignature().IsSameKey(
                KeySignature(KeySignature::majorKey, KeySignature::twoSharps)));*/
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/ReadBarline/RehearsalSigns", "")
{
    // Check that rehearsal sign letters are set in sequential order (A, B, C, etc),
    // and that data is read correctly
    const RehearsalSign& sign1 = score->GetSystem(0)->GetStartBar()->GetRehearsalSign();

    REQUIRE(sign1.GetDescription() == "Section 1");
    REQUIRE(sign1.IsSet());
    REQUIRE(sign1.GetLetter() == 'A');

    const RehearsalSign& sign2 = score->GetSystem(0)->GetBarline(0)->GetRehearsalSign();

    REQUIRE(sign2.GetDescription() == "Section 2");
    REQUIRE(sign2.IsSet());
    REQUIRE(sign2.GetLetter() == 'B');
}

TEST_CASE("Formats/GuitarPro4Import/ReadBarline/AlternateEndings", "")
{
    // TODO - create test cases (TuxGuitar cannot export alternate endings to gp4)
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/TempoMarkers",
                 "Currently, only the initial tempo marker is imported.")
{
    REQUIRE(score->GetTempoMarkerCount() == 1u);
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/NoteProperties/PositionEffects", "")
{
    /// Check that the first position is tremolo picked and is tapped
    const Position* pos = score->GetSystem(0)->GetStaff(0)->GetPosition(0, 0);
    REQUIRE(pos->HasTremoloPicking());
    REQUIRE(pos->HasTap());

    REQUIRE(pos->HasPickStrokeDown());

    REQUIRE(pos->HasPalmMuting());

    // TODO - test tremolo bar
    // TODO - test arpeggios
    // TODO - test vibrato
    // TODO - test staccato
    // TODO - test let ring
}

TEST_CASE_METHOD(Gp4Fixture, "Formats/GuitarPro4Import/NoteProperties/NoteEffects", "")
{
    const Note* note = score->GetSystem(0)->GetStaff(0)->GetPosition(0, 1)->GetNote(0);

    // should have trill with fret 1
    uint8_t trilledFret = 0;
    note->GetTrill(trilledFret);
    REQUIRE(trilledFret == 1);

    note = score->GetSystem(0)->GetStaff(0)->GetPosition(0, 3)->GetNote(0);
    REQUIRE(note->HasPullOff());
}
