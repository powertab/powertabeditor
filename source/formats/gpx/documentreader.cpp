#include "documentreader.h"

#include <boost/property_tree/xml_parser.hpp>

#include <powertabdocument/powertabdocument.h>

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
