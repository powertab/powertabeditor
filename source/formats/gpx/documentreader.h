#ifndef GPX_DOCUMENTREADER_H
#define GPX_DOCUMENTREADER_H

#include <memory>
#include <vector>
#include <map>

// Temporarily disable -Wtype-limits due to wchar_t warning with Mingw and Boost
#ifdef __MINGW32__
#pragma GCC diagnostic ignored "-Wtype-limits" 
#endif

#include <boost/property_tree/ptree.hpp>

class PowerTabDocument;
class PowerTabFileHeader;
class Score;
class Position;
class KeySignature;
class TimeSignature;
class Note;
class Tuning;

namespace Gpx
{

struct GpxVoice;
struct GpxBar;
struct GpxBeat;
struct GpxRhythm;
struct GpxNote;

class DocumentReader
{
public:
    DocumentReader(const std::string& xml);

    void readDocument(std::shared_ptr<PowerTabDocument> doc);

private:
    typedef boost::property_tree::ptree ptree;

    ptree gpFile;

    std::map<int, GpxBar> bars;
    std::map<int, GpxVoice> voices;
    std::map<int, GpxBeat> beats;
    std::map<int, GpxRhythm> rhythms;
    std::map<int, GpxNote> notes;

    void readHeader(PowerTabFileHeader& header);
    void readTracks(Score* score);
    void readBars();
    void readVoices();
    void readBeats();
    void readRhythms();
    void readNotes();

    void readMasterBars(Score* score);
    void readKeySignature(const ptree& masterBar, KeySignature& key);
    void readTimeSignature(const ptree& masterBar, TimeSignature& timeSignature);
    Note* convertNote(int noteId, Position& position, const Tuning& tuning) const;
};

struct GpxBar
{
    int id;
    std::vector<int> voiceIds;
};

struct GpxVoice
{
    int id;
    std::vector<int> beatIds;
};

struct GpxBeat
{
    int id;
    int rhythmId;
    std::vector<int> noteIds;
};

struct GpxRhythm
{
    int id;
    int noteValue;
    bool dotted;
    bool doubleDotted;
};

struct GpxNote
{
    int id;
    bool tied;
    boost::property_tree::ptree properties;
};

}

#endif // GPX_DOCUMENTREADER_H
