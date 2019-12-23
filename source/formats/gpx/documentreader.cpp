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

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <iostream>
#include <score/generalmidi.h>
#include <score/score.h>
#include <sstream>
#include <stdexcept>

static const int POSITIONS_PER_SYSTEM = 35;

using namespace pugi;

/// Utility function for parsing a string of space-separated values.
template <typename T>
static void convertStringToList(const std::string &source, std::vector<T> &dest)
{
    std::stringstream ss(source);
    T item;
    dest.clear();

    while (ss >> item)
    {
        dest.push_back(item);
    }

    if (dest.empty())
        std::cerr << "Parsing of list failed!!" << std::endl;
}

Gpx::DocumentReader::DocumentReader(const std::string &xml)
{
    xml_parse_result result = myXmlData.load_string(xml.c_str());

    if (result.status != pugi::status_ok)
        throw std::runtime_error(result.description());

    // This is the "GPIF" node.
    myFile = myXmlData.first_child();
}

void Gpx::DocumentReader::readScore(Score &score)
{
    readHeader(score);
    readTracks(score);
    readBars();
    readVoices();
    readBeats();
    readRhythms();
    readNotes();
    readAutomations();

    readMasterBars(score);
}

void Gpx::DocumentReader::readHeader(Score &score)
{
    ScoreInfo info;
    SongData data;

    xml_node header = myFile.child("Score");

    data.setTitle(header.child_value("Title"));
    data.setArtist(header.child_value("Artist"));

    data.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Album,
        header.child_value("Album"),
        boost::gregorian::day_clock::local_day().year(), false));

    data.setAuthorInfo(SongData::AuthorInfo(header.child_value("Music"),
                                            header.child_value("Words")));
    data.setCopyright(header.child_value("Copyright"));
    data.setTranscriber(header.child_value("Tabber"));
    data.setPerformanceNotes(header.child_value("Instructions"));

    info.setSongData(data);
    score.setScoreInfo(info);
}

/// Imports the tracks and performs a conversion to the PowerTab Guitar class.
void Gpx::DocumentReader::readTracks(Score &score)
{
    const std::vector<std::string> presetNames = Midi::getPresetNames();

    for (xml_node track : myFile.child("Tracks"))
    {
        Player player;
        Instrument instrument;
        player.setDescription(track.child_value("Name"));
        instrument.setMidiPreset(track.child("GeneralMidi").child(
                                     "Program").text().as_int());
        instrument.setDescription(presetNames.at(instrument.getMidiPreset()));

        xml_node volume = track.child("ChannelStrip").child("Volume");
        player.setMaxVolume(volume.text().as_int(Player::MAX_VOLUME));

        // Not all tracks will have a Properties node ...
        xml_node properties = track.child("Properties");
        if (properties)
        {
            Tuning tuning = player.getTuning();
            // Read the tuning - need to convert from a string of numbers
            // separated by spaces to a vector of integers.
            xml_node pitches = properties.select_node(
                        "./Property/Pitches").node();
            if (pitches)
			{
				std::vector<int> tuningNotes;
                convertStringToList(pitches.child_value(), tuningNotes);

                tuning.setNotes(std::vector<uint8_t>(tuningNotes.rbegin(),
                                                     tuningNotes.rend()));
            }

            // Read capo
            xml_node capo = properties.select_node(
                        "./Property/Fret").node();
            tuning.setCapo(capo.text().as_int());

            player.setTuning(tuning);
        }

        score.insertPlayer(player);
        score.insertInstrument(instrument);
    }
}

void Gpx::DocumentReader::readBars()
{
    for (xml_node currentBar : myFile.child("Bars"))
    {
        Gpx::Bar bar;
        bar.id = currentBar.attribute("id").as_int();
        convertStringToList(currentBar.child_value("Voices"), bar.voiceIds);

        myBars[bar.id] = bar;
    }
}

void Gpx::DocumentReader::readVoices()
{
    for (xml_node currentVoice : myFile.child("Voices"))
    {
        Gpx::Voice voice;
        voice.id = currentVoice.attribute("id").as_int();
        convertStringToList(currentVoice.child_value("Beats"), voice.beatIds);

        myVoices[voice.id] = voice;
    }
}

void Gpx::DocumentReader::readBeats()
{
    for (xml_node currentBeat : myFile.child("Beats"))
    {
        Gpx::Beat beat;
        beat.id = currentBeat.attribute("id").as_int();
        beat.rhythmId = currentBeat.child("Rhythm").attribute("ref").as_int();
        convertStringToList(currentBeat.child_value("Notes"), beat.noteIds);

        beat.arpeggioType = currentBeat.child_value("Arpeggio");
        beat.freeText = currentBeat.child_value("FreeText");
        beat.tremoloPicking = !currentBeat.child("Tremolo").empty();
        beat.graceNote = !currentBeat.child("GraceNotes").empty();

        xml_node properties = currentBeat.child("Properties");
        if (properties)
        {
            // Search for brush direction in the properties list.
            xml_node brush = properties.select_node(
                        "./Property[@name = 'Brush']/Direction").node();
            if (brush)
            {
                beat.brushDirection = brush.child_value();
            }
        }

        myBeats[beat.id] = beat;
    }
}

void Gpx::DocumentReader::readRhythms()
{
    for (xml_node currentRhythm : myFile.child("Rhythms"))
    {
        Gpx::Rhythm rhythm;
        rhythm.id = currentRhythm.attribute("id").as_int();

        // Convert duration to PowerTab format.
        const std::string noteValueStr = currentRhythm.child_value("NoteValue");

        std::map<std::string, int> noteValuesToInt = {
			{ "Whole", 1 }, { "Half", 2 }, { "Quarter", 4 },
			{ "Eighth", 8 }, { "16th", 16 }, { "32nd", 32 },
			{ "64th", 64 }
		};

        assert(noteValuesToInt.find(noteValueStr) != noteValuesToInt.end());
        rhythm.noteValue = noteValuesToInt.find(noteValueStr)->second;

        // Handle dotted/double dotted notes
        int numDots = currentRhythm.child("AugmentationDot").attribute(
                    "count").as_int();

        rhythm.dotted = numDots == 1;
        rhythm.doubleDotted = numDots == 2;

        myRhythms[rhythm.id] = rhythm;
    }
}

void Gpx::DocumentReader::readNotes()
{
    for (xml_node currentNote : myFile.child("Notes"))
    {
        Gpx::TabNote note;
        note.id = currentNote.attribute("id").as_int();
        note.properties = currentNote.child("Properties");

        note.tied = currentNote.child("Tie").attribute(
                    "destination").as_string() == std::string("true");
        note.ghostNote = currentNote.child_value("AntiAccent") ==
                std::string("Normal");
        note.accentType = currentNote.child("Accent").text().as_int();
        note.vibratoType = currentNote.child_value("Vibrato");
        note.letRing = !currentNote.child("LetRing").empty();
        note.trillNote = currentNote.child("Trill").text().as_int(-1);

        myNotes[note.id] = note;
    }
}

void Gpx::DocumentReader::readAutomations()
{
    for (xpath_node node :
         myFile.select_nodes("./MasterTrack/Automations/Automation"))
    {
        xml_node currentAutomation = node.node();
        Gpx::Automation gpxAutomation;
        gpxAutomation.type = currentAutomation.child_value("Type");
        gpxAutomation.linear = currentAutomation.child(
                    "Linear").text().as_bool();
        gpxAutomation.bar = currentAutomation.child("Bar").text().as_int();
        gpxAutomation.position = currentAutomation.child(
                    "Position").text().as_double();
        gpxAutomation.visible = currentAutomation.child(
                    "Visible").text().as_bool();
        convertStringToList(currentAutomation.child_value(
                                "Value"), gpxAutomation.value);

        // TODO - this code doesn't support having multiple automations in a
        // bar.
        myAutomations[gpxAutomation.bar] = gpxAutomation;
    }
}

void Gpx::DocumentReader::readMasterBars(Score &score)
{
    System system;
    for (auto &player : score.getPlayers())
        system.insertStaff(Staff(player.getTuning().getStringCount()));

    // Set up an initial player change.
    PlayerChange change;
    for (int i = 0; i < score.getPlayers().size(); ++i)
        change.insertActivePlayer(i, ActivePlayer(i, i));
    system.insertPlayerChange(change);

    int barIndex = 0;
    int startPos = 0;
    for (xml_node masterBar : myFile.child("MasterBars"))
    {
        if (masterBar.name() != std::string("MasterBar"))
            continue;

        // Try to create a new system every so often.
        if (startPos > POSITIONS_PER_SYSTEM)
        {
            system.getBarlines().back().setPosition(startPos + 1);
            score.insertSystem(system);
            system = System();

            for (auto &player : score.getPlayers())
                system.insertStaff(Staff(player.getTuning().getStringCount()));

            startPos = 0;
        }

        Barline barline;

        if (myAutomations.find(barIndex) != myAutomations.end())
        {
            const Automation automation = myAutomations.find(barIndex)->second;
            if (automation.type == "Tempo")
            {
                if (automation.value.size() != 2)
                    throw std::runtime_error("Invalid tempo");

                TempoMarker marker(startPos);
                marker.setBeatsPerMinute(
                    automation.value[0] *
                    static_cast<int>(automation.value[1] / 2.0));
                system.insertTempoMarker(marker);
            }
        }

        readBarlineType(masterBar, barline);

        KeySignature key(barline.getKeySignature());
        readKeySignature(masterBar, key);
        barline.setKeySignature(key);

        TimeSignature time(barline.getTimeSignature());
        readTimeSignature(masterBar, time);
        barline.setTimeSignature(time);

        std::vector<int> barIds;
        convertStringToList(masterBar.child_value("Bars"), barIds);

        int nextPos = startPos;

        for (int i = 0; i < score.getPlayers().size() &&
                        i < static_cast<int>(barIds.size());
             ++i)
        {
            Staff &staff = system.getStaves()[i];
            int currentPos = (startPos != 0) ? startPos + 1 : 0;

            // TODO - import multiple voices.
            for (int beatId :
                 myVoices[myBars[barIds[i]].voiceIds.at(0)].beatIds)
            {
                const Gpx::Beat &beat = myBeats[beatId];

                // Create text item at this position if necessary.
                if (!beat.freeText.empty())
                    system.insertTextItem(TextItem(currentPos, beat.freeText));

                Position pos;
                if (beat.arpeggioType == "Up")
                    pos.setProperty(Position::ArpeggioUp);
                else if (beat.arpeggioType == "Down")
                    pos.setProperty(Position::ArpeggioDown);
                if (beat.brushDirection == "Up")
                    pos.setProperty(Position::PickStrokeDown);
                else if (beat.brushDirection == "Down")
                    pos.setProperty(Position::PickStrokeUp);

                pos.setProperty(Position::TremoloPicking, beat.tremoloPicking);
                pos.setProperty(Position::Acciaccatura, beat.graceNote);

                const Gpx::Rhythm &rhythm = myRhythms.at(beat.rhythmId);
                pos.setDurationType(static_cast<Position::DurationType>(
                                        rhythm.noteValue));
                pos.setProperty(Position::Dotted, rhythm.dotted);
                pos.setProperty(Position::DoubleDotted, rhythm.doubleDotted);

                for (int noteId : beat.noteIds)
                {
                    Note note = convertNote(noteId, pos,
                                            score.getPlayers()[i].getTuning());
                    if (Utils::findByString(pos, note.getString()))
                    {
                        std::cerr << "Colliding notes at string " <<
                                     note.getString() << std::endl;
                    }
                    else
                        pos.insertNote(note);
                }

                if (pos.getNotes().empty())
                    pos.setRest();

                pos.setPosition(currentPos++);
                staff.getVoices()[0].insertPosition(pos);
            }

            nextPos = std::max(nextPos, currentPos);
        }

        barline.setPosition(startPos);
        if (startPos == 0)
            system.getBarlines().front() = barline;
        else
            system.insertBarline(barline);

        startPos = nextPos;
        ++barIndex;
    }

    system.getBarlines().back().setPosition(startPos + 1);
    score.insertSystem(system);
}

void Gpx::DocumentReader::readBarlineType(const xml_node &masterBar,
                                          Barline &barline)
{
    xml_node repeat_node = masterBar.child("Repeat");

    if (repeat_node)
    {
        if (repeat_node.attribute("start").as_bool())
            barline.setBarType(Barline::RepeatStart);
        else
        {
            barline.setBarType(Barline::RepeatEnd);
            barline.setRepeatCount(repeat_node.attribute("count").as_int());
        }
    }
    else if (masterBar.child("FreeTime"))
        barline.setBarType(Barline::FreeTimeBar);
    else if (masterBar.child("DoubleBar"))
        barline.setBarType(Barline::DoubleBar);
    else
        barline.setBarType(Barline::SingleBar);
}

void Gpx::DocumentReader::readKeySignature(const xml_node &masterBar,
                                           KeySignature &key)
{
    xml_node key_node = masterBar.child("Key");

    // Guitar Pro numbers accidentals from -1 to -7 for flats.
    const int numAccidentals = key_node.child("AccidentalCount").text().as_int();
    key.setNumAccidentals(numAccidentals >= 0 ? numAccidentals : -numAccidentals);
    key.setSharps(numAccidentals >= 0);

    const std::string keyType = key_node.child_value("Mode");

    if (keyType == "Major")
        key.setKeyType(KeySignature::Major);
    else if (keyType == "Minor")
        key.setKeyType(KeySignature::Minor);
    else
        std::cerr << "Unknown key type: " << keyType << std::endl;
}

void Gpx::DocumentReader::readTimeSignature(const xml_node &masterBar,
                                            TimeSignature &timeSignature)
{
    const std::string timeString = masterBar.child_value("Time");

    // Import time signature (stored in text format - e.g. "4/4").
    std::stringstream ss(timeString);
    int timeSigValue0 = -1;
    int timeSigValue1 = -1;
    char slash = 0;

    if (ss >> timeSigValue0 >> slash >> timeSigValue1)
    {
        timeSignature.setBeatsPerMeasure(timeSigValue0);
        timeSignature.setBeatValue(timeSigValue1);
    }
    else
        std::cerr << "Parsing of time signature failed!!" << std::endl;
}

Note Gpx::DocumentReader::convertNote(int noteId, Position &position,
                                      const Tuning &tuning) const
{
    Gpx::TabNote gpxNote = myNotes.at(noteId);
    Note ptbNote;

    ptbNote.setProperty(Note::Tied, gpxNote.tied);
    ptbNote.setProperty(Note::GhostNote, gpxNote.ghostNote);

    position.setProperty(Position::Staccato, gpxNote.accentType == 1);
    position.setProperty(Position::Marcato, gpxNote.accentType == 8);
    position.setProperty(Position::Sforzando, gpxNote.accentType == 4);

    if (gpxNote.vibratoType == "Slight")
        position.setProperty(Position::Vibrato);
    else if (gpxNote.vibratoType == "Wide")
        position.setProperty(Position::WideVibrato);

    position.setProperty(Position::LetRing, gpxNote.letRing);

    if (gpxNote.trillNote != -1)
    {
        ptbNote.setTrilledFret(gpxNote.trillNote -
                               tuning.getNote(ptbNote.getString(), false));
    }

    for (xml_node property : gpxNote.properties)
    {
        const std::string propertyName = property.attribute("name").as_string();

        if (propertyName == "String")
        {
            ptbNote.setString(tuning.getStringCount() -
                              property.child("String").text().as_int() - 1);
        }
        else if (propertyName == "Fret")
            ptbNote.setFretNumber(property.child("Fret").text().as_int());
        else if (propertyName == "PalmMuted")
            position.setProperty(Position::PalmMuting);
        else if (propertyName == "Muted")
            ptbNote.setProperty(Note::Muted);
        else if (propertyName == "HopoOrigin")
            ptbNote.setProperty(Note::HammerOnOrPullOff);
        else if (propertyName == "LeftHandTapped")
            ptbNote.setProperty(Note::HammerOnFromNowhere);
        else if (propertyName == "Tapped")
            position.setProperty(Position::Tap);
        else if (propertyName == "Slide")
        {
            // TODO - implement this once slides are implemented for the new
            // file format.
#if 0
            const int flags = property.child("Flags").text().as_int();
            switch (flags)
            {
            case 16:
                ptbNote.SetSlideInto(TabNote::slideIntoFromBelow);
                break;
            case 32:
                ptbNote.SetSlideInto(TabNote::slideIntoFromAbove);
                break;
            case 17:
                ptbNote.SetSlideInto(TabNote::slideIntoShiftSlideUpwards);
                break;
            case 33:
                ptbNote.SetSlideInto(TabNote::slideIntoShiftSlideDownwards);
                break;
            case 18:
                ptbNote.SetSlideInto(TabNote::slideIntoLegatoSlideUpwards);
                break;
            case 34:
                ptbNote.SetSlideInto(TabNote::slideIntoLegatoSlideDownwards);
                break;
            case 1:
                ptbNote.SetSlideOutOf(TabNote::slideOutOfShiftSlide, 0);
                break;
            case 2:
                ptbNote.SetSlideOutOf(TabNote::slideOutOfLegatoSlide, 0);
                break;
            case 4:
                ptbNote.SetSlideOutOf(TabNote::slideOutOfDownwards, 0);
                break;
            case 8:
                ptbNote.SetSlideOutOf(TabNote::slideOutOfUpwards, 0);
                break;
            }
#endif
        }
        else if (propertyName == "HarmonicType")
        {
            const std::string harmonicType = property.child_value("HType");

            if (harmonicType == "Natural")
                ptbNote.setProperty(Note::NaturalHarmonic);
            else
            {
                std::cerr << "Unsupported Harmonic Type - " <<
                             harmonicType << std::endl;
            }
        }
        else
            std::cerr << "Unhandled property: " << propertyName << std::endl;
    }

    return ptbNote;
}
