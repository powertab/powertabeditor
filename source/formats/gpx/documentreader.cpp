#include "documentreader.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>

#include <formats/scorearranger.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/system.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>

Gpx::DocumentReader::DocumentReader(const std::string& xml)
{
    std::stringstream xmlStream;
    xmlStream << xml;
    read_xml(xmlStream, gpFile);
}

void Gpx::DocumentReader::readDocument(std::shared_ptr<PowerTabDocument> doc)
{
    readHeader(doc->GetHeader());
    readTracks(doc->GetGuitarScore());

    readBars();
    readVoices();
    readBeats();
    readRhythms();
    readNotes();

    readMasterBars(doc->GetGuitarScore());
}

/// Loads the header information (song title, artist, etc)
void Gpx::DocumentReader::readHeader(PowerTabFileHeader& header)
{
    const ptree& gpHeader = gpFile.get_child("GPIF.Score");

    header.SetSongTitle(gpHeader.get<std::string>("Title"));
    header.SetSongArtist(gpHeader.get<std::string>("Artist"));
    header.SetSongAudioReleaseTitle(gpHeader.get<std::string>("Album"));
    header.SetSongLyricist(gpHeader.get<std::string>("Words"));
    header.SetSongComposer(gpHeader.get<std::string>("Music"));
    header.SetSongCopyright(gpHeader.get<std::string>("Copyright"));

    header.SetSongGuitarScoreTranscriber(gpHeader.get<std::string>("Tabber"));
    header.SetSongGuitarScoreNotes(gpHeader.get<std::string>("Instructions"));
}

namespace
{
/// Utility function for converting a string of space-separated values to a vector of that type
template <typename T>
void convertStringToList(const std::string& source, std::vector<T>& dest)
{
    using namespace boost::spirit::qi;

    auto begin = source.begin();
    bool parsed = parse(begin, source.end(),
                        auto_ % ' ', dest);

    if (!parsed)
    {
        std::cerr << "Parsing of list failed!!" << std::endl;
    }
}
}

/// Imports the tracks and performs a conversion to the PowerTab Guitar class
void Gpx::DocumentReader::readTracks(Score *score)
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Tracks"))
    {
        const ptree& track = node.second;

        Score::GuitarPtr guitar = std::make_shared<Guitar>();
        guitar->GetTuning().SetToStandard();

        guitar->SetDescription(track.get<std::string>("Name"));
        guitar->SetPreset(track.get<int>("GeneralMidi.Program"));

        guitar->SetInitialVolume(track.get("ChannelStrip.Volume", Guitar::DEFAULT_INITIAL_VOLUME));

        // Not all tracks will have a Properties node ...
        boost::optional<const ptree&> properties = track.get_child_optional("Properties");
        if (properties)
        {
            // Read the tuning - need to convert from a string of numbers separated by spaces to
            // a vector of integers
            const std::string tuningString = properties->get<std::string>("Property.Pitches");

            std::vector<int> tuningNotes;
            convertStringToList(tuningString, tuningNotes);

            guitar->GetTuning().SetTuningNotes(std::vector<uint8_t>(tuningNotes.rbegin(),
                                                                    tuningNotes.rend()));

            // Read capo
            guitar->SetCapo(properties->get("Property.Fret", 0));
        }

        score->InsertGuitar(guitar);
    }
}

/// Assemble the bars from the previously-read data
void Gpx::DocumentReader::readMasterBars(Score* score)
{
    std::vector<BarData> ptbBars;

    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.MasterBars"))
    {
        if (node.first != "MasterBar")
        {
            continue;
        }

        BarData barData;

        const ptree& masterBar = node.second;

        System::BarlinePtr barline = std::make_shared<Barline>();

        readKeySignature(masterBar, barline->GetKeySignature());
        readTimeSignature(masterBar, barline->GetTimeSignature());

        std::vector<int> barIds;
        convertStringToList(masterBar.get<std::string>("Bars"), barIds);

        for (size_t i = 0; i < barIds.size(); i++)
        {
            std::vector<Position*> positions;

            // only import a single voice
            BOOST_FOREACH(int beatId, voices[bars[barIds[i]].voiceIds.at(0)].beatIds)
            {
                GpxBeat beat = beats[beatId];

                Position pos;

                GpxRhythm rhythm = rhythms.at(beat.rhythmId);
                pos.SetDurationType(rhythm.noteValue);
                pos.SetDotted(rhythm.dotted);
                pos.SetDoubleDotted(rhythm.doubleDotted);

                BOOST_FOREACH(int noteId, beat.noteIds)
                {
                    Note* note = convertNote(noteId, pos, score->GetGuitar(i)->GetTuning());
                    if (pos.GetNoteByString(note->GetString()))
                    {
                        std::cerr << "Colliding notes at string " << note->GetString() << std::endl;
                        delete note;
                    }
                    else
                    {
                        pos.InsertNote(note);
                    }
                }

                if (pos.GetNoteCount() == 0)
                {
                    pos.SetRest(true);
                }

                positions.push_back(pos.CloneObject());
            }

            barData.positionLists.push_back(positions);
        }

        barData.barline = barline;
        ptbBars.push_back(barData);
    }

    arrangeScore(score, ptbBars);
}

void Gpx::DocumentReader::readBars()
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Bars"))
    {
        const ptree& currentBar = node.second;

        Gpx::GpxBar bar;
        bar.id = currentBar.get<int>("<xmlattr>.id");
        convertStringToList(currentBar.get<std::string>("Voices"), bar.voiceIds);

        bars[bar.id] = bar;
    }
}

void Gpx::DocumentReader::readKeySignature(const Gpx::DocumentReader::ptree& masterBar, KeySignature& key)
{
    // Guitar Pro numbers accidentals from -1 to -7 for flats, but PowerTab uses
    // 8 - 14 (with 1-7 for sharps)
    const int numAccidentals = masterBar.get<int>("Key.AccidentalCount");
    key.SetKeyAccidentals(numAccidentals >= 0 ? numAccidentals : 7 - numAccidentals);

    const std::string keyType = masterBar.get<std::string>("Key.Mode");

    if (keyType == "Major")
    {
        key.SetKeyType(KeySignature::majorKey);
    }
    else if (keyType == "Minor")
    {
        key.SetKeyType(KeySignature::minorKey);
    }
    else
    {
        std::cerr << "Unknown key type" << std::endl;
    }
}

void Gpx::DocumentReader::readTimeSignature(const Gpx::DocumentReader::ptree& masterBar,
                                            TimeSignature& timeSignature)
{
    const std::string timeString = masterBar.get<std::string>("Time");

    std::vector<int> timeSigValues;
    using namespace boost::spirit::qi;

    auto begin = timeString.begin();
    // import time signature (stored in text format - e.g. "4/4")
    bool parsed = parse(begin, timeString.end(),
                        int_ >> '/' >> int_, timeSigValues);

    if (!parsed)
    {
        std::cerr << "Parsing of time signature failed!!" << std::endl;
    }
    else
    {
        timeSignature.SetMeter(timeSigValues[0], timeSigValues[1]);
    }
}

void Gpx::DocumentReader::readVoices()
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Voices"))
    {
        const ptree& currentVoice = node.second;

        Gpx::GpxVoice voice;
        voice.id = currentVoice.get<int>("<xmlattr>.id");
        convertStringToList(currentVoice.get<std::string>("Beats"), voice.beatIds);

        voices[voice.id] = voice;
    }
}

void Gpx::DocumentReader::readBeats()
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Beats"))
    {
        const ptree& currentBeat = node.second;

        Gpx::GpxBeat beat;
        beat.id = currentBeat.get<int>("<xmlattr>.id");
        beat.rhythmId = currentBeat.get<int>("Rhythm.<xmlattr>.ref");
        convertStringToList(currentBeat.get("Notes", ""), beat.noteIds);

        beats[beat.id] = beat;
    }
}

void Gpx::DocumentReader::readRhythms()
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Rhythms"))
    {
        const ptree& currentRhythm = node.second;

        Gpx::GpxRhythm rhythm;
        rhythm.id = currentRhythm.get<int>("<xmlattr>.id");

        // convert duration to PowerTab format
        const std::string noteValueStr = currentRhythm.get<std::string>("NoteValue");

        std::map<std::string, int> noteValuesToInt = {
            {"Whole", 1}, {"Half", 2}, {"Quarter", 4}, {"Eighth", 8},
            {"16th", 16}, {"32nd", 32}, {"64th", 64}
        };

        assert(noteValuesToInt.find(noteValueStr) != noteValuesToInt.end());
        rhythm.noteValue = noteValuesToInt.find(noteValueStr)->second;

        // Handle dotted/double dotted notes
        const int numDots = currentRhythm.get("AugmentationDot.<xmlattr>.count", 0);
        std::cerr << "Dots: " << numDots << std::endl;
        rhythm.dotted = numDots == 1;
        rhythm.doubleDotted = numDots == 2;

        rhythms[rhythm.id] = rhythm;
    }
}

void Gpx::DocumentReader::readNotes()
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Notes"))
    {
        const ptree& currentNote = node.second;

        Gpx::GpxNote note;
        note.id = currentNote.get<int>("<xmlattr>.id");
        note.properties = currentNote.get_child("Properties");

        note.tied = currentNote.get("Tie.<xmlattr>.destination", "") == "true";

        notes[note.id] = note;
    }
}

Note* Gpx::DocumentReader::convertNote(int noteId, Position& position,
                                       const Tuning& tuning) const
{
    Gpx::GpxNote gpxNote = notes.at(noteId);
    Note ptbNote;

    ptbNote.SetTied(gpxNote.tied);

    BOOST_FOREACH(const ptree::value_type& node, gpxNote.properties)
    {
        const ptree& property = node.second;
        const std::string propertyName = property.get<std::string>("<xmlattr>.name");

        if (propertyName == "String")
        {
            ptbNote.SetString(tuning.GetStringCount() - property.get<int>("String") - 1);
        }
        else if (propertyName == "Fret")
        {
            ptbNote.SetFretNumber(property.get<int>("Fret"));
        }
        else if (propertyName == "PalmMuted")
        {
            position.SetPalmMuting(true);
        }
        else
        {
            std::cerr << "Unhandled property: " << propertyName << std::endl;
        }
    }

    return ptbNote.CloneObject();
}
