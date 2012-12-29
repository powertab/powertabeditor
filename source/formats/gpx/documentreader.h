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
  
#ifndef GPX_DOCUMENTREADER_H
#define GPX_DOCUMENTREADER_H

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "pugixml/pugixml.hpp"

class Barline;
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
struct GpxAutomation;

class DocumentReader
{
public:
    DocumentReader(const std::string& xml);

    void readDocument(boost::shared_ptr<PowerTabDocument> doc);

private:
    pugi::xml_document xml_data;
    pugi::xml_node file;

    std::map<int, GpxBar> bars;
    std::map<int, GpxVoice> voices;
    std::map<int, GpxBeat> beats;
    std::map<int, GpxRhythm> rhythms;
    std::map<int, GpxNote> notes;
    std::map<int, GpxAutomation> automations;

    void readHeader(PowerTabFileHeader& header);
    void readTracks(Score* score);
    void readBars();
    void readVoices();
    void readBeats();
    void readRhythms();
    void readNotes();
    void readAutomations();

    void readMasterBars(Score* score);
    void readBarlineType(const pugi::xml_node &masterBar, boost::shared_ptr<Barline> barline);
    void readKeySignature(const pugi::xml_node &masterBar, KeySignature& key);
    void readTimeSignature(const pugi::xml_node &masterBar, TimeSignature& timeSignature);
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
    std::string arpeggioType;
    std::string brushDirection;
    bool tremoloPicking;
    bool graceNote;
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
    bool ghostNote;
    int accentType;
    std::string vibratoType;
    bool letRing;
    int trillNote; ///< Note value is stored in MIDI format (0-127)
    pugi::xml_node properties;
};

struct GpxAutomation
{
    std::string type;
    bool linear;
    int bar;
    double position;
    bool visible;
    std::vector<int> value;
};

}

#endif // GPX_DOCUMENTREADER_H
