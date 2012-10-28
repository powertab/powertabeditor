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
  
#include "documentreader.h"

#include <iostream>
#include <sstream>

#include "pugixml/pugixml.hpp"
#include "pugixml/foreach.hpp"

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/assign/list_of.hpp>

#include <formats/scorearranger.h>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/system.h>
#include <powertabdocument/position.h>
#include <powertabdocument/note.h>
#include <powertabdocument/tempomarker.h>

using namespace pugi;

Gpx::DocumentReader::DocumentReader(const std::string& xml)
{
    xml_parse_result result = xml_data.load(xml.c_str());

    if (result.status != pugi::status_ok)
    {
        throw std::runtime_error(result.description());
    }

    file = xml_data.first_child(); // The "GPIF" node.
}

void Gpx::DocumentReader::readDocument(boost::shared_ptr<PowerTabDocument> doc)
{
    readHeader(doc->GetHeader());
    readTracks(doc->GetPlayerScore());

    readBars();
    readVoices();
    readBeats();
    readRhythms();
    readNotes();
    readAutomations();

    readMasterBars(doc->GetPlayerScore());
}

/// Loads the header information (song title, artist, etc).
void Gpx::DocumentReader::readHeader(PowerTabFileHeader& header)
{
    xml_node gpHeader = file.child("Score");

    header.SetSongTitle(gpHeader.child_value("Title"));
    header.SetSongArtist(gpHeader.child_value("Artist"));
    header.SetSongAudioReleaseTitle(gpHeader.child_value("Album"));
    header.SetSongLyricist(gpHeader.child_value("Words"));
    header.SetSongComposer(gpHeader.child_value("Music"));
    header.SetSongCopyright(gpHeader.child_value("Copyright"));

    header.SetSongGuitarScoreTranscriber(gpHeader.child_value("Tabber"));
    header.SetSongGuitarScoreNotes(gpHeader.child_value("Instructions"));
}

namespace
{
/// Utility function for converting a string of space-separated values to a vector of that type
template <typename T>
void convertStringToList(const std::string& source, std::vector<T>& dest)
{
    std::stringstream ss(source);
    T item;
    dest.clear();

    while (ss >> item)
    {
        dest.push_back(item);
    }

    if (dest.empty())
    {
        std::cerr << "Parsing of list failed!!" << std::endl;
    }
}
}

/// Imports the tracks and performs a conversion to the PowerTab Guitar class.
void Gpx::DocumentReader::readTracks(Score *score)
{
    BOOST_FOREACH(xml_node track, file.child("Tracks"))
    {
        Score::GuitarPtr guitar = boost::make_shared<Guitar>();
        guitar->GetTuning().SetToStandard();

        guitar->SetDescription(track.child_value("Name"));
        guitar->SetPreset(track.child("GeneralMidi").child("Program").text().as_int());

        xml_node volume = track.child("ChannelStrip").child("Volume");
        guitar->SetInitialVolume(volume.text().as_int(Guitar::DEFAULT_INITIAL_VOLUME));

        // Not all tracks will have a Properties node ...
        xml_node properties = track.child("Properties");
        if (properties)
        {
            // Read the tuning - need to convert from a string of numbers separated by spaces to
            // a vector of integers
            xml_node pitches = properties.select_single_node("./Property/Pitches").node();
            if (pitches)
			{
				std::vector<int> tuningNotes;
                convertStringToList(pitches.child_value(), tuningNotes);

				guitar->GetTuning().SetTuningNotes(std::vector<uint8_t>(tuningNotes.rbegin(), tuningNotes.rend()));
			}

            // Read capo
            xml_node capo = properties.select_single_node("./Property/Fret").node();
            guitar->SetCapo(capo.text().as_int());
        }

        score->InsertGuitar(guitar);
    }
}

/// Assemble the bars from the previously-read data.
void Gpx::DocumentReader::readMasterBars(Score* score)
{
    std::vector<BarData> ptbBars;

    BOOST_FOREACH(xml_node masterBar, file.child("MasterBars"))
    {
        if (masterBar.name() != std::string("MasterBar"))
        {
            continue;
        }

        BarData barData;
        System::BarlinePtr barline = boost::make_shared<Barline>();

        if (automations.find(ptbBars.size()) != automations.end())
        {
            GpxAutomation automation = automations.find(ptbBars.size())->second;
            if (automation.type == "Tempo")
            {
                barData.tempoMarker = boost::make_shared<TempoMarker>();

                if (automation.value.size() != 2)
                {
                    throw std::runtime_error("Invalid tempo");
                }

                barData.tempoMarker->SetBeatsPerMinute(automation.value[0] *
                                                    automation.value[1] / 2.0);
            }
        }

        readKeySignature(masterBar, barline->GetKeySignature());
        readTimeSignature(masterBar, barline->GetTimeSignature());

        std::vector<int> barIds;
        convertStringToList(masterBar.child_value("Bars"), barIds);

        for (size_t i = 0; i < score->GetGuitarCount() && i < barIds.size(); ++i)
        {
            std::vector<Position*> positions;

            // only import a single voice
            BOOST_FOREACH(int beatId, voices[bars[barIds[i]].voiceIds.at(0)].beatIds)
            {
                GpxBeat beat = beats[beatId];

                Position pos;

                pos.SetArpeggioUp(beat.arpeggioType == "Up");
                pos.SetArpeggioDown(beat.arpeggioType == "Down");
                pos.SetPickStrokeDown(beat.brushDirection == "Up");
                pos.SetPickStrokeUp(beat.brushDirection == "Down");

                pos.SetTremoloPicking(beat.tremoloPicking);
                pos.SetAcciaccatura(beat.graceNote);

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
    BOOST_FOREACH(xml_node currentBar, file.child("Bars"))
    {
        Gpx::GpxBar bar;
        bar.id = currentBar.attribute("id").as_int();
        convertStringToList(currentBar.child_value("Voices"), bar.voiceIds);

        bars[bar.id] = bar;
    }
}

void Gpx::DocumentReader::readKeySignature(
        const pugi::xml_node& masterBar, KeySignature& key)
{
    xml_node key_node = masterBar.child("Key");

    // Guitar Pro numbers accidentals from -1 to -7 for flats, but PowerTab uses
    // 8 - 14 (with 1-7 for sharps).
    const int numAccidentals = key_node.child("AccidentalCount").text().as_int();
    key.SetKeyAccidentals(numAccidentals >= 0 ? numAccidentals : 7 - numAccidentals);

    const std::string keyType = key_node.child_value("Mode");

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
        std::cerr << "Unknown key type: " << keyType << std::endl;
    }
}

void Gpx::DocumentReader::readTimeSignature(const xml_node& masterBar,
                                            TimeSignature& timeSignature)
{
    const std::string timeString = masterBar.child_value("Time");

    // Import time signature (stored in text format - e.g. "4/4").
    std::stringstream ss(timeString);
    int timeSigValue0 = -1;
    int timeSigValue1 = -1;
    char slash = 0;

    if (ss >> timeSigValue0 >> slash >> timeSigValue1)
    {
        timeSignature.SetMeter(timeSigValue0, timeSigValue1);
    }
    else
    {
        std::cerr << "Parsing of time signature failed!!" << std::endl;
    }
}

void Gpx::DocumentReader::readVoices()
{
    BOOST_FOREACH(xml_node currentVoice, file.child("Voices"))
    {
        Gpx::GpxVoice voice;
        voice.id = currentVoice.attribute("id").as_int();
        convertStringToList(currentVoice.child_value("Beats"), voice.beatIds);

        voices[voice.id] = voice;
    }
}

void Gpx::DocumentReader::readBeats()
{
    BOOST_FOREACH(xml_node currentBeat, file.child("Beats"))
    {
        Gpx::GpxBeat beat;
        beat.id = currentBeat.attribute("id").as_int();
        beat.rhythmId = currentBeat.child("Rhythm").attribute("ref").as_int();
        convertStringToList(currentBeat.child_value("Notes"), beat.noteIds);

        beat.arpeggioType = currentBeat.child_value("Arpeggio");
        beat.tremoloPicking = !currentBeat.child("Tremolo").empty();
        beat.graceNote = !currentBeat.child("GraceNotes").empty();

        xml_node properties = currentBeat.child("Properties");
        if (properties)
		{
            // Search for brush direction in the properties list.
            xml_node brush = properties.select_single_node("./Property[@name = 'Brush']/Direction").node();
            if (brush)
            {
                beat.brushDirection = brush.child_value();
            }
		}

        beats[beat.id] = beat;
    }
}

void Gpx::DocumentReader::readRhythms()
{
    BOOST_FOREACH(xml_node currentRhythm, file.child("Rhythms"))
    {
        Gpx::GpxRhythm rhythm;
        rhythm.id = currentRhythm.attribute("id").as_int();

        // Convert duration to PowerTab format.
        const std::string noteValueStr = currentRhythm.child_value("NoteValue");

        std::map<std::string, int> noteValuesToInt = boost::assign::map_list_of
            ("Whole", 1) ("Half", 2) ("Quarter", 4) ("Eighth", 8)
            ("16th", 16) ("32nd", 32) ("64th", 64);

        assert(noteValuesToInt.find(noteValueStr) != noteValuesToInt.end());
        rhythm.noteValue = noteValuesToInt.find(noteValueStr)->second;

        // Handle dotted/double dotted notes
        int numDots = currentRhythm.child("AugmentationDot").attribute("count").as_int();

        std::cerr << "Dots: " << numDots << std::endl;
        rhythm.dotted = numDots == 1;
        rhythm.doubleDotted = numDots == 2;

        rhythms[rhythm.id] = rhythm;
    }
}

void Gpx::DocumentReader::readNotes()
{
    BOOST_FOREACH(xml_node currentNote, file.child("Notes"))
    {
        Gpx::GpxNote note;
        note.id = currentNote.attribute("id").as_int();
        note.properties = currentNote.child("Properties");

        note.tied = currentNote.child("Tie").attribute("destination").as_string() == std::string("true");
        note.ghostNote = currentNote.child_value("AntiAccent") == std::string("Normal");
        note.accentType = currentNote.child("Accent").text().as_int();
        note.vibratoType = currentNote.child_value("Vibrato");
        note.letRing = !currentNote.child("LetRing").empty();
        note.trillNote = currentNote.child("Trill").text().as_int(-1);

        notes[note.id] = note;
    }
}

void Gpx::DocumentReader::readAutomations()
{
    BOOST_FOREACH(xpath_node node, file.select_nodes("./MasterTrack/Automations/Automation"))
    {
        xml_node currentAutomation = node.node();
        Gpx::GpxAutomation gpxAutomation;
        gpxAutomation.type = currentAutomation.child_value("Type");
        gpxAutomation.linear = currentAutomation.child("Linear").text().as_bool();
        gpxAutomation.bar = currentAutomation.child("Bar").text().as_int();
        gpxAutomation.position = currentAutomation.child("Position").text().as_double();
        gpxAutomation.visible = currentAutomation.child("Visible").text().as_bool();
        convertStringToList(currentAutomation.child_value("Value"), gpxAutomation.value);

        // TODO - this code doesn't support having multiple automations in a
        // bar.
        automations[gpxAutomation.bar] = gpxAutomation;
    }
}

Note* Gpx::DocumentReader::convertNote(int noteId, Position& position,
                                       const Tuning& tuning) const
{
    Gpx::GpxNote gpxNote = notes.at(noteId);
    Note ptbNote;

    ptbNote.SetTied(gpxNote.tied);
    ptbNote.SetGhostNote(gpxNote.ghostNote);

    position.SetStaccato(gpxNote.accentType == 1);
    position.SetMarcato(gpxNote.accentType == 8);
    position.SetSforzando(gpxNote.accentType == 4);

    if (gpxNote.vibratoType == "Slight")
    {
        position.SetVibrato();
    }
    else if (gpxNote.vibratoType == "Wide")
    {
        position.SetWideVibrato();
    }

    position.SetLetRing(gpxNote.letRing);

    if (gpxNote.trillNote != -1)
    {
        ptbNote.SetTrill(gpxNote.trillNote - tuning.GetNote(ptbNote.GetString()));
    }

    BOOST_FOREACH(xml_node property, gpxNote.properties)
    {
        const std::string propertyName = property.attribute("name").as_string();

        if (propertyName == "String")
        {
            ptbNote.SetString(tuning.GetStringCount() -
                              property.child("String").text().as_int() - 1);
        }
        else if (propertyName == "Fret")
        {
            ptbNote.SetFretNumber(property.child("Fret").text().as_int());
        }
        else if (propertyName == "PalmMuted")
        {
            position.SetPalmMuting(true);
        }
        else if (propertyName == "Muted")
        {
            ptbNote.SetMuted(true);
        }
        else if (propertyName == "HopoOrigin")
        {
            // if the note should actually have a pulloff, this will later be fixed by the Layout::FixHammerons function
            // - this is because we can't decide between hammerons and pulloffs without knowing the next
            //   note (which hasn't been read at this point ...)
            ptbNote.SetHammerOn(true);
        }
        else if (propertyName == "LeftHandTapped")
        {
            ptbNote.SetHammerOnFromNowhere();
        }
        else if (propertyName == "Tapped")
        {
            position.SetTap();
        }
        else if (propertyName == "Slide")
        {
            const int flags = property.child("Flags").text().as_int();
            switch (flags)
            {
            case 16:
                ptbNote.SetSlideInto(Note::slideIntoFromBelow);
                break;
            case 32:
                ptbNote.SetSlideInto(Note::slideIntoFromAbove);
                break;
            case 17:
                ptbNote.SetSlideInto(Note::slideIntoShiftSlideUpwards);
                break;
            case 33:
                ptbNote.SetSlideInto(Note::slideIntoShiftSlideDownwards);
                break;
            case 18:
                ptbNote.SetSlideInto(Note::slideIntoLegatoSlideUpwards);
                break;
            case 34:
                ptbNote.SetSlideInto(Note::slideIntoLegatoSlideDownwards);
                break;
            case 1:
                ptbNote.SetSlideOutOf(Note::slideOutOfShiftSlide, 0);
                break;
            case 2:
                ptbNote.SetSlideOutOf(Note::slideOutOfLegatoSlide, 0);
                break;
            case 4:
                ptbNote.SetSlideOutOf(Note::slideOutOfDownwards, 0);
                break;
            case 8:
                ptbNote.SetSlideOutOf(Note::slideOutOfUpwards, 0);
                break;
            }
        }
        else if (propertyName == "HarmonicType")
        {
            const std::string harmonicType = property.child_value("HType");

            if (harmonicType == "Natural")
            {
                ptbNote.SetNaturalHarmonic();
            }
            else
            {
                std::cerr << "Unsupported Harmonic Type - " << harmonicType << std::endl;
            }
        }
        else
        {
            std::cerr << "Unhandled property: " << propertyName << std::endl;
        }
    }

    return ptbNote.CloneObject();
}
