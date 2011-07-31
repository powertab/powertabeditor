#ifndef GPX_DOCUMENTREADER_H
#define GPX_DOCUMENTREADER_H

#include <memory>
#include <vector>
#include <map>
#include <boost/property_tree/ptree.hpp>

class PowerTabDocument;
class PowerTabFileHeader;
class Score;
class KeySignature;
class TimeSignature;

namespace Gpx
{

struct Voice;
struct Bar;
struct Beat;
struct Rhythm;
struct Note;

class DocumentReader
{
public:
    DocumentReader(const std::string& xml);

    void readDocument(std::shared_ptr<PowerTabDocument> doc);

private:
    typedef boost::property_tree::ptree ptree;

    ptree gpFile;

    std::map<int, Bar> bars;
    std::map<int, Voice> voices;
    std::map<int, Beat> beats;
    std::map<int, Rhythm> rhythms;
    std::map<int, Note> notes;

    void readHeader(PowerTabFileHeader& header);
    void readTracks(Score* score);
    void readBars();
    void readVoices();
    void readBeats();
    void readRhythms();
    void readNotes();

    void readMasterBars();
    void readKeySignature(const ptree& masterBar, KeySignature& key);
    void readTimeSignature(const ptree& masterBar, TimeSignature& timeSignature);
};

struct Bar
{
    int id;
    std::vector<int> voiceIds;
};

struct Voice
{
    int id;
    std::vector<int> beatIds;
};

struct Beat
{
    int id;
    int rhythmId;
    std::vector<int> noteIds;
};

struct Rhythm
{
    int id;
    int noteValue;
};

struct Note
{
    int id;
    boost::property_tree::ptree properties;
};

}

#endif // GPX_DOCUMENTREADER_H
