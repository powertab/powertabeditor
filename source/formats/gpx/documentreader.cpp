#include "documentreader.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>

#include <sstream>

using boost::property_tree::ptree;

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

/// Imports the tracks and performs a conversion to the PowerTab Guitar class
void Gpx::DocumentReader::readTracks(Score *score)
{
    BOOST_FOREACH(const ptree::value_type& node, gpFile.get_child("GPIF.Tracks"))
    {
        const ptree& track = node.second;

        Score::GuitarPtr guitar = std::make_shared<Guitar>();

        guitar->SetDescription(track.get<std::string>("Name"));
        guitar->SetPreset(track.get<int>("GeneralMidi.Program"));

        guitar->SetInitialVolume(track.get("ChannelStrip.Volume", Guitar::DEFAULT_INITIAL_VOLUME));

        // Not all tracks will have a Properties node ...
        boost::optional<const ptree&> properties = track.get_child_optional("Properties");
        if (properties)
        {
            // Read the tuning - need to convert from a string of numbers separated by spaces to
            // a vector of integers
            std::stringstream tuningString (properties->get<std::string>("Property.Pitches"));

            std::vector<uint8_t> tuningNotes;
            std::copy(std::istream_iterator<int>(tuningString), std::istream_iterator<int>(),
                      std::back_inserter(tuningNotes));

            guitar->GetTuning().SetTuningNotes(tuningNotes);

            // Read capo
            guitar->SetCapo(properties->get("Property.Fret", 0));
        }

        score->InsertGuitar(guitar);
    }
}
