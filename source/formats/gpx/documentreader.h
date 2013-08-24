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
  
#ifndef FORMATS_GPX_DOCUMENTREADER_H
#define FORMATS_GPX_DOCUMENTREADER_H

#include <map>
#include "pugixml/pugixml.hpp"
#include <score/note.h>
#include <vector>

class Barline;
class KeySignature;
class Position;
class Score;
class TimeSignature;
class Tuning;

namespace Gpx
{
struct Automation;
struct Bar;
struct Beat;
struct TabNote;
struct Rhythm;
struct Voice;

class DocumentReader
{
public:
    DocumentReader(const std::string &xml);

    void readScore(Score &score);

private:
    /// Loads the header information (song title, artist, etc).
    void readHeader(Score &score);
    void readTracks(Score &score);
    void readBars();
    void readVoices();
    void readBeats();
    void readRhythms();
    void readNotes();
    void readAutomations();

    /// Assembles the bars from the previously-read data.
    void readMasterBars(Score &score);

    void readBarlineType(const pugi::xml_node &masterBar, Barline &barline);
    void readKeySignature(const pugi::xml_node &masterBar, KeySignature &key);
    void readTimeSignature(const pugi::xml_node &masterBar,
                           TimeSignature &timeSignature);
    Note convertNote(int noteId, Position &position, const Tuning &tuning) const;

    pugi::xml_document myXmlData;
    pugi::xml_node myFile;

    std::map<int, Gpx::Bar> myBars;
    std::map<int, Gpx::Voice> myVoices;
    std::map<int, Gpx::Beat> myBeats;
    std::map<int, Gpx::Rhythm> myRhythms;
    std::map<int, Gpx::TabNote> myNotes;
    std::map<int, Gpx::Automation> myAutomations;
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
    std::string arpeggioType;
    std::string brushDirection;
    bool tremoloPicking;
    bool graceNote;
    std::vector<int> noteIds;
};

struct Rhythm
{
    int id;
    int noteValue;
    bool dotted;
    bool doubleDotted;
};

struct TabNote
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

struct Automation
{
    std::string type;
    bool linear;
    int bar;
    double position;
    bool visible;
    std::vector<int> value;
};
}

#endif
