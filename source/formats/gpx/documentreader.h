#ifndef GPX_DOCUMENTREADER_H
#define GPX_DOCUMENTREADER_H

#include <memory>
#include <iosfwd>
#include <boost/property_tree/ptree.hpp>

class PowerTabDocument;
class PowerTabFileHeader;
class Score;

namespace Gpx
{

class DocumentReader
{
public:
    DocumentReader(const std::string& xml);

    void readDocument(std::shared_ptr<PowerTabDocument> doc);

private:
    boost::property_tree::ptree gpFile;

    void readHeader(PowerTabFileHeader& header);
    void readTracks(Score* score);
};

}

#endif // GPX_DOCUMENTREADER_H
