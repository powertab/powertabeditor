/*
  * Copyright (C) 2013 Cameron White
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

#include "powertaboldimporter.h"

#include <boost/assign/list_of.hpp>
#include "powertabdocument/alternateending.h"
#include "powertabdocument/barline.h"
#include "powertabdocument/direction.h"
#include "powertabdocument/dynamic.h"
#include "powertabdocument/guitar.h"
#include "powertabdocument/powertabdocument.h"
#include "powertabdocument/score.h"
#include "powertabdocument/staff.h"
#include "powertabdocument/system.h"
#include "powertabdocument/tempomarker.h"
#include <score/score.h>
#include <score/generalmidi.h>

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab Document",
                                    boost::assign::list_of("ptb")))
{
}

void PowerTabOldImporter::load(const std::string &filename, Score &score)
{
    PowerTabDocument::Document document;
    document.Load(filename);

    // TODO - handle line spacing, font settings, etc.
    ScoreInfo info;
    convert(document.GetHeader(), info);
    score.setScoreInfo(info);

    // TODO - merge bass score.
    convert(*document.GetScore(0), score);
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::PowerTabFileHeader &header, ScoreInfo &info)
{
    using namespace PowerTabDocument;

    if (header.GetFileType() == PowerTabFileHeader::FILETYPE_SONG)
    {
        SongData data;

        data.setTitle(header.GetSongTitle());
        data.setArtist(header.GetSongArtist());

        const uint8_t releaseType = header.GetSongReleaseType();
        if (releaseType == PowerTabFileHeader::RELEASETYPE_PUBLIC_AUDIO)
        {
            data.setAudioReleaseInfo(SongData::AudioData(
                static_cast<SongData::AudioData::AudioReleaseType>(
                                             header.GetSongAudioReleaseType()),
                header.GetSongAudioReleaseTitle(),
                header.GetSongAudioReleaseYear(),
                header.IsSongAudioReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_PUBLIC_VIDEO)
        {
            data.setVideoReleaseInfo(SongData::VideoData(
                header.GetSongVideoReleaseTitle(),
                header.IsSongVideoReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_BOOTLEG)
        {
            data.setBootlegInfo(SongData::BootlegData(
                header.GetSongBootlegTitle(),
                header.GetSongBootlegDate()));
        }
        else
        {
            data.setUnreleased();
        }

        if (header.GetSongAuthorType() == PowerTabFileHeader::AUTHORTYPE_TRADITIONAL)
            data.setTraditionalAuthor();
        else
        {
            data.setAuthorInfo(SongData::AuthorData(header.GetSongComposer(),
                                                    header.GetSongLyricist()));
        }

        data.setArranger(header.GetSongArranger());
        data.setTranscriber(header.GetSongGuitarScoreTranscriber());
        data.setCopyright(header.GetSongCopyright());
        data.setLyrics(header.GetSongLyrics());
        data.setPerformanceNotes(header.GetSongGuitarScoreNotes());

        info.setSongData(data);
    }
    else
    {
        LessonData data;

        data.setTitle(header.GetLessonTitle());
        data.setSubtitle(header.GetLessonSubtitle());
        data.setMusicStyle(static_cast<LessonData::MusicStyle>(
                               header.GetLessonMusicStyle()));
        data.setDifficultyLevel(static_cast<LessonData::DifficultyLevel>(
                                    header.GetLessonLevel()));
        data.setAuthor(header.GetLessonAuthor());
        data.setNotes(header.GetLessonNotes());
        data.setCopyright(header.GetLessonCopyright());

        info.setLessonData(data);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Score &oldScore,
                                  Score &score)
{
    // Convert guitars to players and instruments.
    for (size_t i = 0; i < oldScore.GetGuitarCount(); ++i)
    {
        convert(*oldScore.GetGuitar(i), score);
    }

    for (size_t i = 0; i < oldScore.GetSystemCount(); ++i)
    {
        System system;
        convert(oldScore, oldScore.GetSystem(i), system);
        score.insertSystem(system);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Guitar &guitar,
                                  Score &score)
{
    Player player;
    player.setDescription(guitar.GetDescription());
    player.setMaxVolume(guitar.GetInitialVolume());
    player.setPan(guitar.GetPan());

    Tuning tuning;
    convert(guitar.GetTuning(), tuning);
    tuning.setCapo(guitar.GetCapo());
    player.setTuning(tuning);

    score.insertPlayer(player);

    Instrument instrument;
    instrument.setMidiPreset(guitar.GetPreset());

    // Use the MIDI preset name as the description.
    std::vector<std::string> presetNames;
    Midi::getMidiPresetNames(presetNames);
    instrument.setDescription(presetNames.at(guitar.GetPreset()));

    score.insertInstrument(instrument);
}

void PowerTabOldImporter::convert(const PowerTabDocument::Tuning &oldTuning,
                                  Tuning &tuning)
{
    tuning.setName(oldTuning.GetName());
    tuning.setNotes(oldTuning.GetTuningNotes());
    tuning.setMusicNotationOffset(oldTuning.GetMusicNotationOffset());
    tuning.setSharps(oldTuning.UsesSharps());
    // The capo is set from the Guitar object.
}

void PowerTabOldImporter::convert(const PowerTabDocument::Score &oldScore,
                                  PowerTabDocument::Score::SystemConstPtr oldSystem,
                                  System &system)
{
    // Import barlines.
    Barline &startBar = system.getBarlines()[0];
    convert(*oldSystem->GetStartBar(), startBar);

    Barline &endBar = system.getBarlines()[1];
    convert(*oldSystem->GetEndBar(), endBar);

    for (size_t i = 0; i < oldSystem->GetBarlineCount(); ++i)
    {
        Barline bar;
        convert(*oldSystem->GetBarline(i), bar);
        system.insertBarline(bar);
    }

    // Import tempo markers.
    std::vector<boost::shared_ptr<PowerTabDocument::TempoMarker> > tempos;
    oldScore.GetTempoMarkersInSystem(tempos, oldSystem);
    for (size_t i = 0; i < tempos.size(); ++i)
    {
        TempoMarker marker;
        convert(*tempos[i], marker);
        system.insertTempoMarker(marker);
    }

    // Import alternate endings.
    std::vector<boost::shared_ptr<PowerTabDocument::AlternateEnding> > endings;
    oldScore.GetAlternateEndingsInSystem(endings, oldSystem);
    for (size_t i = 0; i < endings.size(); ++i)
    {
        AlternateEnding ending;
        convert(*endings[i], ending);
        system.insertAlternateEnding(ending);
    }

    // Import directions.
    for (size_t i = 0; i < oldSystem->GetDirectionCount(); ++i)
    {
        Direction direction;
        convert(*oldSystem->GetDirection(i), direction);
        system.insertDirection(direction);
    }

    std::vector<PowerTabDocument::Score::DynamicPtr> dynamics;
    oldScore.GetDynamicsInSystem(dynamics, oldSystem);

    // Import staves.
    for (size_t i = 0; i < oldSystem->GetStaffCount(); ++i)
    {
        // Dynamics are now stored in the staff instead of the system.
        std::vector<PowerTabDocument::Score::DynamicPtr> dynamicsInStaff;
        for (size_t j = 0; j < dynamics.size(); ++j)
        {
            if (dynamics[j]->GetStaff() == i)
                dynamicsInStaff.push_back(dynamics[j]);
        }

        Staff staff;
        convert(*oldSystem->GetStaff(i), dynamicsInStaff, staff);
        system.insertStaff(staff);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Barline &oldBar,
                                  Barline &bar)
{
    bar.setPosition(oldBar.GetPosition());
    bar.setBarType(static_cast<Barline::BarType>(oldBar.GetType()));
    bar.setRepeatCount(oldBar.GetRepeatCount());

    if (oldBar.GetRehearsalSign().IsSet())
    {
        RehearsalSign sign;
        convert(oldBar.GetRehearsalSign(), sign);
        bar.setRehearsalSign(sign);
    }

    KeySignature key;
    convert(oldBar.GetKeySignature(), key);
    bar.setKeySignature(key);

    TimeSignature time;
    convert(oldBar.GetTimeSignature(), time);
    bar.setTimeSignature(time);
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::RehearsalSign &oldSign, RehearsalSign &sign)
{
    sign.setLetters(std::string(1, oldSign.GetLetter()));
    sign.setDescription(oldSign.GetDescription());
}

void PowerTabOldImporter::convert(const PowerTabDocument::KeySignature &oldKey,
                                  KeySignature &key)
{
    key.setKeyType(static_cast<KeySignature::KeyType>(oldKey.GetKeyType()));
    key.setNumAccidentals(oldKey.NumberOfAccidentals());
    key.setSharps(oldKey.UsesSharps());
    key.setVisible(oldKey.IsShown());
    key.setCancellation(oldKey.IsCancellation());
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::TimeSignature &oldTime, TimeSignature &time)
{
    TimeSignature::MeterType type = TimeSignature::Normal;
    if (oldTime.IsCutTime())
        type = TimeSignature::CutTime;
    else if (oldTime.IsCommonTime())
        type = TimeSignature::CommonTime;

    time.setMeterType(type);
    time.setBeatsPerMeasure(oldTime.GetBeatsPerMeasure());
    time.setBeatValue(oldTime.GetBeatAmount());

    TimeSignature::BeamingPattern pattern;
    oldTime.GetBeamingPattern(pattern[0], pattern[1], pattern[2], pattern[3]);
    time.setBeamingPattern(pattern);

    time.setNumPulses(oldTime.GetPulses());
    time.setVisible(oldTime.IsShown());
}

void PowerTabOldImporter::convert(const PowerTabDocument::TempoMarker &oldTempo,
                                  TempoMarker &tempo)
{
    tempo.setPosition(oldTempo.GetPosition());
    tempo.setMarkerType(static_cast<TempoMarker::MarkerType>(
                            oldTempo.GetType()));
    tempo.setBeatType(static_cast<TempoMarker::BeatType>(oldTempo.GetBeatType()));
    tempo.setListessoBeatType(static_cast<TempoMarker::BeatType>(
                                  oldTempo.GetListessoBeatType()));
    tempo.setTripletFeel(static_cast<TempoMarker::TripletFeelType>(
                             oldTempo.GetTripletFeelType()));

    TempoMarker::AlterationOfPaceType alteration(TempoMarker::NoAlterationOfPace);
    if (oldTempo.IsRitardando())
        alteration = TempoMarker::Ritardando;
    else if (oldTempo.IsAccelerando())
        alteration = TempoMarker::Accelerando;

    tempo.setAlterationOfPace(alteration);
    tempo.setBeatsPerMinute(oldTempo.GetBeatsPerMinute());
    tempo.setDescription(oldTempo.GetDescription());
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::AlternateEnding &oldEnding,
        AlternateEnding &ending)
{
    ending.setPosition(oldEnding.GetPosition());

    std::vector<uint8_t> numbers = oldEnding.GetListOfNumbers();
    for (size_t i = 0; i < numbers.size(); ++i)
        ending.addNumber(numbers[i]);

    ending.setDaCapo(oldEnding.IsDaCapoSet());
    ending.setDalSegno(oldEnding.IsDalSegnoSet());
    ending.setDalSegnoSegno(oldEnding.IsDalSegnoSegnoSet());
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::Direction &oldDirection, Direction &direction)
{
    direction.setPosition(oldDirection.GetPosition());

    for (size_t i = 0; i < oldDirection.GetSymbolCount(); ++i)
    {
        uint8_t type = 0;
        uint8_t active = 0;
        uint8_t repeat = 0;
        oldDirection.GetSymbol(i, type, active, repeat);

        direction.insertSymbol(DirectionSymbol(
                static_cast<DirectionSymbol::SymbolType>(type),
                static_cast<DirectionSymbol::ActiveSymbolType>(active),
                repeat));
    }
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::Staff &oldStaff,
        const std::vector<PowerTabDocument::Score::DynamicPtr> &dynamics,
        Staff &staff)
{
    staff.setClefType(static_cast<Staff::ClefType>(oldStaff.GetClef()));
    staff.setStringCount(oldStaff.GetTablatureStaffType());
    staff.setViewType(Staff::GuitarView);

    // Import dynamics.
    for (size_t i = 0; i < dynamics.size(); ++i)
    {
        Dynamic dynamic;
        convert(*dynamics[i], dynamic);
        staff.insertDynamic(dynamic);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Dynamic &oldDynamic,
                                  Dynamic &dynamic)
{
    dynamic.setPosition(oldDynamic.GetPosition());
    dynamic.setVolume(static_cast<Dynamic::VolumeLevel>(
                          oldDynamic.GetStaffVolume()));
}
