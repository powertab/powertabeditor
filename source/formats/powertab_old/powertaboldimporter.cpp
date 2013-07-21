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
#include "powertabdocument/guitarin.h"
#include "powertabdocument/note.h"
#include "powertabdocument/position.h"
#include "powertabdocument/powertabdocument.h"
#include "powertabdocument/score.h"
#include "powertabdocument/staff.h"
#include "powertabdocument/system.h"
#include "powertabdocument/tempomarker.h"
#include <score/score.h>
#include <score/generalmidi.h>

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab Document (v1.7)",
                                    boost::assign::list_of("ptb")))
{
}

void PowerTabOldImporter::load(const std::string &filename, Score &score)
{
    PowerTabDocument::Document document;
    document.Load(filename);

    // TODO - handle font settings, etc.
    ScoreInfo info;
    convert(document.GetHeader(), info);
    score.setScoreInfo(info);

    score.setLineSpacing(document.GetTablatureStaffLineSpacing());

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

    // Convert Guitar In's to player changes.
    convertGuitarIns(oldScore, score);
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

        // Copy the key and time signature of the last bar into the end bar,
        // since the v2.0 file format expects this.
        if (i == oldSystem->GetBarlineCount() - 1)
        {
            KeySignature key = bar.getKeySignature();
            key.setVisible(false);
            system.getBarlines().back().setKeySignature(key);

            TimeSignature time = bar.getTimeSignature();
            time.setVisible(false);
            system.getBarlines().back().setTimeSignature(time);
        }
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
    key.setNumAccidentals(oldKey.GetKeyAccidentalsIncludingCancel());
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
    if (alteration == TempoMarker::NoAlterationOfPace)
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

    // Import positions.
    for (size_t voice = 0; voice < PowerTabDocument::Staff::NUM_STAFF_VOICES;
         ++voice)
    {
        for (size_t i = 0; i < oldStaff.GetPositionCount(voice); ++i)
        {
            Position position;
            convert(*oldStaff.GetPosition(voice, i), position);
            staff.insertPosition(voice, position);
        }
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Dynamic &oldDynamic,
                                  Dynamic &dynamic)
{
    dynamic.setPosition(oldDynamic.GetPosition());
    dynamic.setVolume(static_cast<Dynamic::VolumeLevel>(
                          oldDynamic.GetStaffVolume()));
}

void PowerTabOldImporter::convert(const PowerTabDocument::Position &oldPosition,
                                  Position &position)
{
    position.setPosition(oldPosition.GetPosition());
    position.setDurationType(static_cast<Position::DurationType>(
                                 oldPosition.GetDurationType()));

    // Import simple properties.
    if (oldPosition.IsDotted())
        position.setProperty(Position::Dotted);
    if (oldPosition.IsDoubleDotted())
        position.setProperty(Position::DoubleDotted);
    if (oldPosition.IsRest())
        position.setRest();
    if (oldPosition.HasVibrato())
        position.setProperty(Position::Vibrato);
    if (oldPosition.HasWideVibrato())
        position.setProperty(Position::WideVibrato);
    if (oldPosition.HasArpeggioUp())
        position.setProperty(Position::ArpeggioUp);
    if (oldPosition.HasArpeggioDown())
        position.setProperty(Position::ArpeggioDown);
    if (oldPosition.HasPickStrokeUp())
        position.setProperty(Position::PickStrokeUp);
    if (oldPosition.HasPickStrokeDown())
        position.setProperty(Position::PickStrokeDown);
    if (oldPosition.IsStaccato())
        position.setProperty(Position::Staccato);
    if (oldPosition.HasMarcato())
        position.setProperty(Position::Marcato);
    if (oldPosition.HasSforzando())
        position.setProperty(Position::Sforzando);
    if (oldPosition.HasTremoloPicking())
        position.setProperty(Position::TremoloPicking);
    if (oldPosition.HasPalmMuting())
        position.setProperty(Position::PalmMuting);
    if (oldPosition.HasTap())
        position.setProperty(Position::Tap);
    if (oldPosition.IsAcciaccatura())
        position.setProperty(Position::Acciaccatura);
    if (oldPosition.IsTripletFeel1st())
        position.setProperty(Position::TripletFeelFirst);
    if (oldPosition.IsTripletFeel2nd())
        position.setProperty(Position::TripletFeelSecond);
    if (oldPosition.HasLetRing())
        position.setProperty(Position::LetRing);
    if (oldPosition.HasFermata())
        position.setProperty(Position::Fermata);

    if (oldPosition.HasMultibarRest())
    {
        uint8_t count = 0;
        oldPosition.GetMultibarRest(count);
        position.setMultiBarRest(count);
    }

    // Import notes.
    for (size_t i = 0; i < oldPosition.GetNoteCount(); ++i)
    {
        Note note;
        convert(*oldPosition.GetNote(i), note);
        position.insertNote(note);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Note &oldNote,
                                  Note &note)
{
    note.setFretNumber(oldNote.GetFretNumber());
    note.setString(oldNote.GetString());

    if (oldNote.HasTappedHarmonic())
    {
        uint8_t fret = 0;
        oldNote.GetTappedHarmonic(fret);
        note.setTappedHarmonicFret(fret);
    }

    if (oldNote.HasTrill())
    {
        uint8_t fret = 0;
        oldNote.GetTrill(fret);
        note.setTrilledFret(fret);
    }

    // Import simple properties.
    if (oldNote.IsTied())
        note.setProperty(Note::Tied);
    if (oldNote.IsMuted())
        note.setProperty(Note::Muted);
    if (oldNote.HasHammerOn())
        note.setProperty(Note::HammerOn);
    if (oldNote.HasPullOff())
        note.setProperty(Note::PullOff);
    if (oldNote.HasHammerOnFromNowhere())
        note.setProperty(Note::HammerOnFromNowhere);
    if (oldNote.HasPullOffToNowhere())
        note.setProperty(Note::PullOffToNowhere);
    if (oldNote.IsNaturalHarmonic())
        note.setProperty(Note::NaturalHarmonic);
    if (oldNote.IsGhostNote())
        note.setProperty(Note::GhostNote);
    if (oldNote.IsOctave8va())
        note.setProperty(Note::Octave8va);
    if (oldNote.IsOctave8vb())
        note.setProperty(Note::Octave8vb);
    if (oldNote.IsOctave15ma())
        note.setProperty(Note::Octave15ma);
    if (oldNote.IsOctave15mb())
        note.setProperty(Note::Octave15mb);
}

namespace {

typedef boost::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS> ActivePlayers;

PlayerChange getPlayerChange(const ActivePlayers &activePlayers,
                             int currentPosition)
{
    PlayerChange change;
    change.setPosition(currentPosition);

    for (size_t player = 0; player < activePlayers.size(); ++player)
    {
        const int staff = activePlayers[player];
        if (staff >= 0)
        {
            change.insertActivePlayer(staff,
                                      ActivePlayer(player, player));
        }
    }

    return change;
}

}

void PowerTabOldImporter::convertGuitarIns(
        const PowerTabDocument::Score &oldScore, Score &score)
{
    // For each guitar, keep track of its current staff.
    boost::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS> activePlayers;
    activePlayers.fill(-1);

    for (size_t i = 0; i < oldScore.GetSystemCount(); ++i)
    {
        std::vector<PowerTabDocument::Score::GuitarInPtr> guitarIns;
        oldScore.GetGuitarInsInSystem(guitarIns, oldScore.GetSystem(i));
        if (guitarIns.empty())
            continue;

        size_t currentPosition = guitarIns.front()->GetPosition();

        // In v1.7, each staff has separate guitar ins. In the new format,
        // player changes occur at the system level so we need to combine
        // the guitar ins from several staves.
        for (size_t j = 0; j < guitarIns.size(); ++j)
        {
            PowerTabDocument::Score::GuitarInPtr guitarIn = guitarIns[j];

            // After combining all guitar in's at a position, write out a player
            // change.
            if (guitarIn->GetPosition() != currentPosition)
            {
                score.getSystems()[i].insertPlayerChange(
                            getPlayerChange(activePlayers, currentPosition));
            }

            // Clear out any players that are currently active for this staff.
            const int staff = guitarIn->GetStaff();
            for (size_t k = 0; k < activePlayers.size(); ++k)
            {
                if (activePlayers[k] == staff)
                    activePlayers[k] = -1;
            }

            // Set the active players for this staff.
            std::bitset<8> activeGuitars(guitarIn->GetStaffGuitars());
            for (size_t k = 0; k < activePlayers.size(); ++k)
            {
                if (activeGuitars[k])
                    activePlayers[k] = staff;
            }

            currentPosition = guitarIn->GetPosition();
        }

        // After processing all of the guitar ins in the system, write out a
        // final player change.
        score.getSystems()[i].insertPlayerChange(
                    getPlayerChange(activePlayers, currentPosition));
    }
}
