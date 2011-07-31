#ifndef GPX_DOCUMENTREADER_H
#define GPX_DOCUMENTREADER_H

#include <memory>
#include <boost/property_tree/ptree.hpp>

class PowerTabDocument;
class PowerTabFileHeader;
class Score;
class KeySignature;
class TimeSignature;

namespace Gpx
{

class DocumentReader
{
public:
    DocumentReader(const std::string& xml);

    void readDocument(std::shared_ptr<PowerTabDocument> doc);

private:
    typedef boost::property_tree::ptree ptree;

    ptree gpFile;

    void readHeader(PowerTabFileHeader& header);
    void readTracks(Score* score);
    void readBars();
    void readKeySignature(const ptree& masterBar, KeySignature& key);
    void readTimeSignature(const ptree& masterBar, TimeSignature& timeSignature);
};

}

#endif // GPX_DOCUMENTREADER_H
