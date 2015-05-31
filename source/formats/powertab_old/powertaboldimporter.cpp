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

#include <formats/powertab_old/powertabdocument/alternateending.h>
#include <formats/powertab_old/powertabdocument/barline.h>
#include <formats/powertab_old/powertabdocument/chordtext.h>
#include <formats/powertab_old/powertabdocument/direction.h>
#include <formats/powertab_old/powertabdocument/dynamic.h>
#include <formats/powertab_old/powertabdocument/floatingtext.h>
#include <formats/powertab_old/powertabdocument/guitar.h>
#include <formats/powertab_old/powertabdocument/guitarin.h>
#include <formats/powertab_old/powertabdocument/note.h>
#include <formats/powertab_old/powertabdocument/position.h>
#include <formats/powertab_old/powertabdocument/powertabdocument.h>
#include <formats/powertab_old/powertabdocument/score.h>
#include <formats/powertab_old/powertabdocument/staff.h>
#include <formats/powertab_old/powertabdocument/system.h>
#include <formats/powertab_old/powertabdocument/tempomarker.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils/scoremerger.h>
#include <score/utils/scorepolisher.h>

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab 1.7 Document", { "ptb" }))
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
    ScoreUtils::addStandardFilters(score);
    
    assert(document.GetNumberOfScores() == 2);

    // Convert the guitar score.
    Score guitarScore;
    convert(*document.GetScore(0), guitarScore);

    // Convert and then merge the bass score.
    Score bassScore;
    convert(*document.GetScore(1), bassScore);
    ScoreMerger::merge(score, guitarScore, bassScore);

    // Reformat the score, since the guitar and bass score from v1.7 may have
    // had different spacing.
    ScoreUtils::polishScore(score);
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
            data.setAudioReleaseInfo(SongData::AudioReleaseInfo(
                static_cast<SongData::AudioReleaseInfo::ReleaseType>(
                                             header.GetSongAudioReleaseType()),
                header.GetSongAudioReleaseTitle(),
                header.GetSongAudioReleaseYear(),
                header.IsSongAudioReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_PUBLIC_VIDEO)
        {
            data.setVideoReleaseInfo(SongData::VideoReleaseInfo(
                header.GetSongVideoReleaseTitle(),
                header.IsSongVideoReleaseLive()));
        }
        else if (releaseType == PowerTabFileHeader::RELEASETYPE_BOOTLEG)
        {
            data.setBootlegInfo(SongData::BootlegInfo(
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
            data.setAuthorInfo(SongData::AuthorInfo(header.GetSongComposer(),
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
        convert(*oldScore.GetGuitar(i), score);

    for (size_t i = 0; i < oldScore.GetSystemCount(); ++i)
    {
        System system;
        convert(oldScore, oldScore.GetSystem(i), system);
        score.insertSystem(system);
    }

    // Convert Guitar In's to player changes.
    convertGuitarIns(oldScore, score);

    // Set up an initial dynamic for each guitar's initial volumes.
    convertInitialVolumes(oldScore, score);

    // Convert floating text to the new text items.
    convertFloatingText(oldScore, score);
}

void PowerTabOldImporter::convert(const PowerTabDocument::Guitar &guitar,
                                  Score &score)
{
    Player player;
    player.setDescription(guitar.GetDescription());
    player.setPan(guitar.GetPan());

    Tuning tuning;
    convert(guitar.GetTuning(), tuning);
    tuning.setCapo(guitar.GetCapo());
    player.setTuning(tuning);

    score.insertPlayer(player);

    Instrument instrument;
    instrument.setMidiPreset(guitar.GetPreset());

    // Use the MIDI preset name as the description.
    const std::vector<std::string> presetNames = Midi::getPresetNames();
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
    // Ensure that there are a reasonable number of positions in the staff
    // so that things aren't too stretched out.
    int lastPosition = 30;

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
        lastPosition = std::max(lastPosition, bar.getPosition());

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
    std::vector<std::shared_ptr<PowerTabDocument::TempoMarker>> tempos;
    oldScore.GetTempoMarkersInSystem(tempos, oldSystem);
    for (auto &tempo : tempos)
    {
        TempoMarker marker;
        convert(*tempo, marker);
        system.insertTempoMarker(marker);
    }

    // Import alternate endings.
    std::vector<std::shared_ptr<PowerTabDocument::AlternateEnding>> endings;
    oldScore.GetAlternateEndingsInSystem(endings, oldSystem);
    for (auto &ending : endings)
    {
        AlternateEnding newEnding;
        convert(*ending, newEnding);
        system.insertAlternateEnding(newEnding);
    }

    // Import directions.
    for (size_t i = 0; i < oldSystem->GetDirectionCount(); ++i)
    {
        Direction direction;
        convert(*oldSystem->GetDirection(i), direction);
        system.insertDirection(direction);
    }

    // Import chord text symbols.
    for (size_t i = 0; i < oldSystem->GetChordTextCount(); ++i)
    {
        ChordText chord;
        convert(*oldSystem->GetChordText(i), chord);
        system.insertChord(chord);
    }

    std::vector<PowerTabDocument::Score::DynamicPtr> dynamics;
    oldScore.GetDynamicsInSystem(dynamics, oldSystem);

    // Import staves.
    for (size_t i = 0; i < oldSystem->GetStaffCount(); ++i)
    {
        // Dynamics are now stored in the staff instead of the system.
        std::vector<PowerTabDocument::Score::DynamicPtr> dynamicsInStaff;
        for (auto &dynamic : dynamics)
        {
            if (dynamic->GetStaff() == i)
                dynamicsInStaff.push_back(dynamic);
        }

        Staff staff;
        int lastPosInStaff = convert(*oldSystem->GetStaff(i), dynamicsInStaff,
                                     staff);
        system.insertStaff(staff);
        lastPosition = std::max(lastPosition, lastPosInStaff);
    }

    system.getBarlines().back().setPosition(lastPosition + 1);
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

    std::array<uint8_t, 4> pattern;
    oldTime.GetBeamingPattern(pattern[0], pattern[1], pattern[2], pattern[3]);
    std::array<int, 4> newPattern;
    std::copy(pattern.begin(), pattern.end(), newPattern.begin());
    time.setBeamingPattern(newPattern);

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
    for (auto &number : numbers)
        ending.addNumber(number);

    ending.setDaCapo(oldEnding.IsDaCapoSet());
    ending.setDalSegno(oldEnding.IsDalSegnoSet());
    ending.setDalSegnoSegno(oldEnding.IsDalSegnoSegnoSet());
}

void PowerTabOldImporter::convert(
        const PowerTabDocument::Direction &oldDirection, Direction &direction)
{
    direction.setPosition(static_cast<int>(oldDirection.GetPosition()));

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

/// Convert between the old key format and the new key format. This is fairly
/// awkward to do.
static void convertKey(PowerTabDocument::ChordName::Key oldKey,
                       uint8_t oldVariation, ChordName::Key &key,
                       ChordName::Variation &variation)
{
    // Convert keys to the new format.
    switch (oldKey)
    {
        case PowerTabDocument::ChordName::C:
            key = ChordName::C;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::CSharp:
            key = ChordName::C;
            variation = ChordName::Sharp;
            break;
        case PowerTabDocument::ChordName::D:
            key = ChordName::D;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::EFlat:
            key = ChordName::E;
            variation = ChordName::Flat;
            break;
        case PowerTabDocument::ChordName::E:
            key = ChordName::E;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::F:
            key = ChordName::F;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::FSharp:
            key = ChordName::F;
            variation = ChordName::Sharp;
            break;
        case PowerTabDocument::ChordName::G:
            key = ChordName::G;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::AFlat:
            key = ChordName::A;
            variation = ChordName::Flat;
            break;
        case PowerTabDocument::ChordName::A:
            key = ChordName::A;
            variation = ChordName::NoVariation;
            break;
        case PowerTabDocument::ChordName::BFlat:
            key = ChordName::B;
            variation = ChordName::Flat;
            break;
        case PowerTabDocument::ChordName::B:
            key = ChordName::B;
            variation = ChordName::NoVariation;
            break;
    }

    // Convert variations to the new format. For example, the up variation of D
    // is now Ebb, and the down variation is C##.
    if (oldVariation == PowerTabDocument::ChordName::variationUp)
    {
        key = static_cast<ChordName::Key>((key + 1) % ChordName::NumKeys);

        if (variation == ChordName::NoVariation)
        {
            variation = (key == ChordName::F || key == ChordName::C)
                            ? ChordName::Flat
                            : ChordName::DoubleFlat;
        }
        else if (variation == ChordName::Flat)
            variation = ChordName::DoubleFlat;
        else
            variation = ChordName::Flat;
    }
    else if (oldVariation == PowerTabDocument::ChordName::variationDown)
    {
        key = static_cast<ChordName::Key>((key - 1) % ChordName::NumKeys);

        if (variation == ChordName::NoVariation)
        {
            variation = (key == ChordName::B || key == ChordName::E)
                            ? ChordName::Sharp
                            : ChordName::DoubleSharp;
        }
        else if (variation == ChordName::Flat)
            variation = ChordName::Sharp;
        else
            variation = ChordName::DoubleSharp;
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::ChordText &oldChord,
                                  ChordText &chord)
{
    chord.setPosition(oldChord.GetPosition());

    ChordName name;
    const PowerTabDocument::ChordName &oldName(oldChord.GetChordNameConstRef());

    uint8_t oldTonic, oldTonicVariation;
    oldName.GetTonic(oldTonic, oldTonicVariation);
    ChordName::Key tonic;
    ChordName::Variation tonicVariation;
    convertKey(static_cast<PowerTabDocument::ChordName::Key>(oldTonic),
               oldTonicVariation, tonic, tonicVariation);

    name.setTonicKey(tonic);
    name.setTonicVariation(tonicVariation);

    uint8_t oldBassKey, oldBassVariation;
    oldName.GetBassNote(oldBassKey, oldBassVariation);
    ChordName::Key bassKey;
    ChordName::Variation bassVariation;
    convertKey(static_cast<PowerTabDocument::ChordName::Key>(oldBassKey),
               oldBassVariation, bassKey, bassVariation);

    name.setBassKey(bassKey);
    name.setBassVariation(bassVariation);

    name.setFormula(static_cast<ChordName::Formula>(oldName.GetFormula()));

    for (unsigned int i = PowerTabDocument::ChordName::extended9th;
         i <= PowerTabDocument::ChordName::suspended4th; i *= 2)
    {
        if (oldName.IsFormulaModificationFlagSet(i))
        {
            name.setModification(static_cast<ChordName::FormulaModification>(
                static_cast<int>(std::log(i) / std::log(2))));
        }
    }

    name.setBrackets(oldName.HasBrackets());
    name.setNoChord(oldName.IsNoChord());

    chord.setChordName(name);
}

int PowerTabOldImporter::convert(
        const PowerTabDocument::Staff &oldStaff,
        const std::vector<PowerTabDocument::Score::DynamicPtr> &dynamics,
        Staff &staff)
{
    int lastPosition = 0;
    staff.setClefType(static_cast<Staff::ClefType>(oldStaff.GetClef()));
    staff.setStringCount(oldStaff.GetTablatureStaffType());

    // Import dynamics.
    for (auto &dynamic : dynamics)
    {
        // Ignore dynamics for rhythm slashes.
        if (dynamic->IsStaffVolumeSet())
        {
            Dynamic newDynamic;
            convert(*dynamic, newDynamic);
            staff.insertDynamic(newDynamic);
            lastPosition = std::max(lastPosition, newDynamic.getPosition());
        }
    }

    // Import positions.
    for (size_t voice = 0; voice < PowerTabDocument::Staff::NUM_STAFF_VOICES;
         ++voice)
    {
        for (size_t i = 0; i < oldStaff.GetPositionCount(voice); ++i)
        {
            Position position;
            convert(*oldStaff.GetPosition(voice, i), position);
            staff.getVoices()[voice].insertPosition(position);
            lastPosition = std::max(position.getPosition(), lastPosition);
        }
    }

    // Import irregular groups.
    for (size_t voice = 0; voice < PowerTabDocument::Staff::NUM_STAFF_VOICES;
         ++voice)
    {
        Voice &v = staff.getVoices()[voice];
        int startPos = 0;
        int positionCount = 0;
        uint8_t notesPlayed = 0;
        uint8_t notesPlayedOver = 0;

        for (size_t i = 0; i < oldStaff.GetPositionCount(voice); ++i)
        {
            const auto &position = *oldStaff.GetPosition(voice, i);
            if (position.IsIrregularGroupingStart())
            {
                startPos = position.GetPosition();
                positionCount = 1;
                position.GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);
            }
            else if (position.IsIrregularGroupingMiddle())
                positionCount++;
            else if (position.IsIrregularGroupingEnd())
            {
                positionCount++;
                v.insertIrregularGrouping(IrregularGrouping(
                    startPos, positionCount, notesPlayed, notesPlayedOver));

                startPos = 0;
                positionCount = 0;
                notesPlayed = 0;
                notesPlayedOver = 0;
            }
            // If there is a grace note in the middle of the irregular grouping,
            // it won't have the irregularGroupingMiddle flag set.
            else if (position.IsAcciaccatura())
                positionCount++;
        }
    }

    return lastPosition;
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

    if (oldNote.HasArtificialHarmonic())
    {
        uint8_t oldKey = 0;
        uint8_t oldVariation = 0;
        uint8_t octave = 0;
        oldNote.GetArtificialHarmonic(oldKey, oldVariation, octave);

        ChordName::Key newKey;
        ChordName::Variation newVariation;
        convertKey(static_cast<PowerTabDocument::ChordName::Key>(oldKey),
                   oldVariation, newKey, newVariation);

        note.setArtificialHarmonic(ArtificialHarmonic(
            newKey, newVariation,
            static_cast<ArtificialHarmonic::Octave>(octave)));
    }

    if (oldNote.HasBend())
    {
        uint8_t type = 0, bentPitch = 0, releasePitch = 0, duration = 0,
                drawStartPoint = 0, drawEndPoint = 0;
        oldNote.GetBend(type, bentPitch, releasePitch, duration, drawStartPoint, drawEndPoint);

        note.setBend(Bend(static_cast<Bend::BendType>(type), bentPitch,
                          releasePitch, duration,
                          static_cast<Bend::DrawPoint>(drawStartPoint),
                          static_cast<Bend::DrawPoint>(drawEndPoint)));
    }

    // Import simple properties.
    if (oldNote.IsTied())
        note.setProperty(Note::Tied);
    if (oldNote.IsMuted())
        note.setProperty(Note::Muted);
    if (oldNote.HasHammerOn())
        note.setProperty(Note::HammerOnOrPullOff);
    if (oldNote.HasPullOff())
        note.setProperty(Note::HammerOnOrPullOff);
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

    if (oldNote.HasSlideIntoFromBelow())
        note.setProperty(Note::SlideIntoFromBelow);
    if (oldNote.HasSlideIntoFromAbove())
        note.setProperty(Note::SlideIntoFromAbove);

    if (oldNote.HasShiftSlide())
        note.setProperty(Note::ShiftSlide);
    if (oldNote.HasLegatoSlide())
        note.setProperty(Note::LegatoSlide);
    if (oldNote.HasSlideOutOfDownwards())
        note.setProperty(Note::SlideOutOfDownwards);
    if (oldNote.HasSlideOutOfUpwards())
        note.setProperty(Note::SlideOutOfUpwards);
}

namespace {

typedef std::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS> ActivePlayers;

PlayerChange getPlayerChange(const ActivePlayers &activePlayers,
                             int currentPosition)
{
    PlayerChange change;
    change.setPosition(currentPosition);

    for (int player = 0; player < static_cast<int>(activePlayers.size());
         ++player)
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
    std::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS> activePlayers;
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
        for (auto &guitarIn : guitarIns)
        {
            // For now, ignore guitar ins that only affect rhythm slashes.
            if (!guitarIn->HasStaffGuitarsSet())
                continue;

            // After combining all guitar in's at a position, write out a player
            // change.
            if (guitarIn->GetPosition() != currentPosition)
            {
                score.getSystems()[i].insertPlayerChange(getPlayerChange(
                    activePlayers, static_cast<int>(currentPosition)));
            }

            // Clear out any players that are currently active for this staff.
            const int staff = guitarIn->GetStaff();
            for (auto &activePlayer : activePlayers)
            {
                if (activePlayer == staff)
                    activePlayer = -1;
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
            getPlayerChange(activePlayers, static_cast<int>(currentPosition)));
    }
}

void PowerTabOldImporter::convertInitialVolumes(
    const PowerTabDocument::Score &oldScore, Score &score)
{
    if (oldScore.GetGuitarInCount() > 0)
    {
        auto firstIn = oldScore.GetGuitarIn(0);
        const SystemLocation startPos(firstIn->GetSystem(), firstIn->GetPosition());
        System &system = score.getSystems()[firstIn->GetSystem()];

        // If there was a dynamic at or before the first guitar in, then that
        // dynamic is used.
        if (oldScore.GetDynamicCount() > 0)
        {
            auto firstDynamic = oldScore.GetDynamic(0);
            if (SystemLocation(firstDynamic->GetSystem(),
                               firstDynamic->GetPosition()) <= startPos)
            {
                return;
            }
        }

        for (size_t i = 0; i < oldScore.GetGuitarInCount(); ++i)
        {
            // Import guitar ins from every staff at the first location.
            auto guitarIn = oldScore.GetGuitarIn(i);
            const SystemLocation pos(guitarIn->GetSystem(),
                                     guitarIn->GetPosition());
            if (pos != startPos)
                continue;
            else if (!guitarIn->HasStaffGuitarsSet())
                continue;

            std::bitset<8> activeGuitars(guitarIn->GetStaffGuitars());
            for (size_t j = 0; j < oldScore.GetGuitarCount(); ++j)
            {
                if (activeGuitars[j])
                {
                    Dynamic dynamic(
                        guitarIn->GetPosition(),
                        static_cast<Dynamic::VolumeLevel>(
                            oldScore.GetGuitar(j)->GetInitialVolume()));

                    system.getStaves()[guitarIn->GetStaff()].insertDynamic(dynamic);
                    break;
                }
            }
        }
    }
}

void PowerTabOldImporter::convertFloatingText(
    const PowerTabDocument::Score &oldScore, Score &score)
{
    for (size_t i = 0, n = oldScore.GetFloatingTextCount(); i < n; ++i)
    {
        // Convert from an absolute (x,y) position to a (system,position)
        // location.
        auto floatingText = oldScore.GetFloatingText(i);
        PowerTabDocument::Rect location = floatingText->GetRect();

        // Figure out which system to attach to.
        size_t attachedSystemIdx = oldScore.GetSystemCount() - 1;
        for (size_t j = 0, n = oldScore.GetSystemCount(); j < n; ++j)
        {
            auto system = oldScore.GetSystem(j);
            if (system->GetRect().GetBottom() > location.GetTop())
            {
                attachedSystemIdx = j;
                break;
            }
        }

        // Figure out what position in the system to use.
        auto system = oldScore.GetSystem(attachedSystemIdx);
        const int position =
            static_cast<int>(system->GetPositionFromX(location.GetX()));

        // Create the text item in the new score.
        TextItem item(position, floatingText->GetText());
        score.getSystems()[attachedSystemIdx].insertTextItem(item);
    }
}
