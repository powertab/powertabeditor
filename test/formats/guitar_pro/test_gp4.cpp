#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <formats/guitar_pro/guitarproimporter.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/powertabfileheader.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>

const std::string FILE_TEST1 = "data/test1.gp4";

struct Gp4Fixture
{
    Gp4Fixture()
    {
        BOOST_CHECK_NO_THROW(importer.load(FILE_TEST1));
        doc = importer.load(FILE_TEST1);
    }

    GuitarProImporter importer;
    std::shared_ptr<PowerTabDocument> doc;
};

BOOST_FIXTURE_TEST_SUITE(GuitarPro4Import, Gp4Fixture)

    /// Verify that the header and song information is imported correctly
    BOOST_AUTO_TEST_CASE(HeaderConversion)
    {
        const PowerTabFileHeader& header = doc->GetHeader();

        BOOST_CHECK_EQUAL(header.GetSongTitle(), "FileName");
        BOOST_CHECK_EQUAL(header.GetSongArtist(), "Artist");
        BOOST_CHECK_EQUAL(header.GetSongAudioReleaseTitle(), "Album");
        BOOST_CHECK_EQUAL(header.GetSongComposer(), "Author");
        BOOST_CHECK_EQUAL(header.GetSongAudioReleaseYear(), 2010);
        BOOST_CHECK_EQUAL(header.GetSongCopyright(), "Copyright 2011");
        BOOST_CHECK_EQUAL(header.GetSongGuitarScoreTranscriber(), "Tab Creator");
        BOOST_CHECK_EQUAL(header.GetSongGuitarScoreNotes(), "Some Comments");
    }

    /// Verify that guitars are imported correctly
    BOOST_AUTO_TEST_CASE(TrackImport)
    {
        const Score* score = doc->GetGuitarScore();

        BOOST_CHECK_EQUAL(score->GetGuitarCount(), 2);

        Score::GuitarConstPtr guitar1 = score->GetGuitar(0);

        BOOST_CHECK_EQUAL(guitar1->GetDescription(), "First Track");
        BOOST_CHECK_EQUAL(guitar1->GetNumber(), 0);
        BOOST_CHECK_EQUAL(guitar1->GetCapo(), 1);

        using namespace midi;
        // 7-string tuning
        BOOST_CHECK(guitar1->GetTuning() == Tuning("", 0, false,
                                                    {MIDI_NOTE_E4, MIDI_NOTE_B3, MIDI_NOTE_G3,
                                                    MIDI_NOTE_D3, MIDI_NOTE_A2, MIDI_NOTE_E2, MIDI_NOTE_B1}));
    }

    BOOST_AUTO_TEST_CASE(ReadBarline)
    {
        System::BarlineConstPtr barline1 = doc->GetGuitarScore()->GetSystem(0)->GetStartBar();

        BOOST_CHECK(barline1->GetTimeSignature().IsSameMeter(TimeSignature(5, 4)));

        // TODO - generate appropriate test case for key signatures (TuxGuitar export seems to be broken??)
        /*BOOST_CHECK(barline1->GetKeySignature().IsSameKey(KeySignature(KeySignature::majorKey,
                                                                               KeySignature::twoSharps)));*/
        BOOST_CHECK(barline1->IsRepeatStart());
    }

    BOOST_AUTO_TEST_CASE(TempoMarkers)
    {
        const Score* score = doc->GetGuitarScore();

        BOOST_CHECK_EQUAL(score->GetTempoMarkerCount(), 1);
    }

BOOST_AUTO_TEST_SUITE_END()
