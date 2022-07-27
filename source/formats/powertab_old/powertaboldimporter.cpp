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
#include <formats/powertab_old/powertabdocument/chorddiagram.h>
#include <formats/powertab_old/powertabdocument/chordtext.h>
#include <formats/powertab_old/powertabdocument/direction.h>
#include <formats/powertab_old/powertabdocument/dynamic.h>
#include <formats/powertab_old/powertabdocument/floatingtext.h>
#include <formats/powertab_old/powertabdocument/guitar.h>
#include <formats/powertab_old/powertabdocument/guitarin.h>
#include <formats/powertab_old/powertabdocument/note.h>
#include <formats/powertab_old/powertabdocument/position.h>
#include <formats/powertab_old/powertabdocument/powertabdocument.h>
#include <formats/powertab_old/powertabdocument/rhythmslash.h>
#include <formats/powertab_old/powertabdocument/score.h>
#include <formats/powertab_old/powertabdocument/staff.h>
#include <formats/powertab_old/powertabdocument/system.h>
#include <formats/powertab_old/powertabdocument/tempomarker.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils/scoremerger.h>
#include <score/utils/scorepolisher.h>

#include <cmath>
#include <unordered_map>

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab 1.7 Document", { "ptb" }))
{
}

void PowerTabOldImporter::load(const std::filesystem::path &filename,
                               Score &score)
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
#if 1
    Score guitarScore;
    convert(*document.GetScore(0), guitarScore);

    // Convert and then merge the bass score.
    Score bassScore;
    convert(*document.GetScore(1), bassScore);
    ScoreMerger::merge(score, guitarScore, bassScore);

    // Reformat the score, since the guitar and bass score from v1.7 may have
    // had different spacing.
    ScoreUtils::polishScore(score);
#else
    // For debugging, optionally import just one score without merging / polishing.
    convert(*document.GetScore(0), score);
#endif
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
        key = static_cast<ChordName::Key>((key + ChordName::NumKeys - 1) %
                                          ChordName::NumKeys);

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

static ChordName
convertChordName(const PowerTabDocument::ChordName &old_name)
{
    ChordName name;

    uint8_t oldTonic, oldTonicVariation;
    old_name.GetTonic(oldTonic, oldTonicVariation);
    ChordName::Key tonic;
    ChordName::Variation tonicVariation;
    convertKey(static_cast<PowerTabDocument::ChordName::Key>(oldTonic),
               oldTonicVariation, tonic, tonicVariation);

    name.setTonicKey(tonic);
    name.setTonicVariation(tonicVariation);

    uint8_t oldBassKey, oldBassVariation;
    old_name.GetBassNote(oldBassKey, oldBassVariation);
    ChordName::Key bassKey;
    ChordName::Variation bassVariation;
    convertKey(static_cast<PowerTabDocument::ChordName::Key>(oldBassKey),
               oldBassVariation, bassKey, bassVariation);

    name.setBassKey(bassKey);
    name.setBassVariation(bassVariation);

    name.setFormula(static_cast<ChordName::Formula>(old_name.GetFormula()));

    for (unsigned int i = PowerTabDocument::ChordName::extended9th;
         i <= PowerTabDocument::ChordName::suspended4th; i *= 2)
    {
        if (old_name.IsFormulaModificationFlagSet(i))
        {
            name.setModification(static_cast<ChordName::FormulaModification>(
                static_cast<int>(std::log(i) / std::log(2))));
        }
    }

    name.setBrackets(old_name.HasBrackets());
    name.setNoChord(old_name.IsNoChord());

    return name;
}

static void
convertChordDiagrams(const PowerTabDocument::Score &old_score, Score &score)
{
    for (size_t i = 0; i < old_score.GetChordDiagramCount(); ++i)
    {
        const PowerTabDocument::ChordDiagram &old_diagram =
            *old_score.GetChordDiagram(i);

        ChordDiagram diagram;
        diagram.setChordName(convertChordName(old_diagram.GetChordName()));
        // In PTE 2.0, the top fret is always 1 above the first fret that can
        // be played. In PTE 1.7 this was inconsistent when the top fret was 0.
        diagram.setTopFret(
            old_diagram.GetTopFret() > 0 ? old_diagram.GetTopFret() - 1 : 0);

        std::vector<int> fret_numbers;
        for (size_t j = 0; j < old_diagram.GetStringCount(); ++j)
        {
            int fret = old_diagram.GetFretNumber(j);
            fret_numbers.push_back(
                (fret == PowerTabDocument::ChordDiagram::stringMuted) ? -1 : fret);
        }
        diagram.setFretNumbers(fret_numbers);

        score.insertChordDiagram(diagram);
    }
}

void PowerTabOldImporter::convert(const PowerTabDocument::Score &oldScore,
                                  Score &score)
{
    // Convert guitars to players and instruments.
    for (size_t i = 0; i < oldScore.GetGuitarCount(); ++i)
        convert(*oldScore.GetGuitar(i), score);

    // Convert chord diagrams (required before importing rhythm slashes) and
    // build a map to find diagrams by name when converting rhythm slashes.
    convertChordDiagrams(oldScore, score);

    ChordDiagramMap chord_diagrams;
    for (const ChordDiagram &diagram : score.getChordDiagrams())
        chord_diagrams[diagram.getChordName()] = diagram;

    for (size_t i = 0; i < oldScore.GetSystemCount(); ++i)
    {
        // Note: it's important for the system to already be inserted, for
        // queries like ScoreUtils::getCurrentChordText().
        score.insertSystem(System());
        convert(oldScore, oldScore.GetSystem(i), chord_diagrams, score,
                score.getSystems().back());
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

    // It seems possible to get an invalid preset number of 255 here (Bug #332).
    // In v1.7 this shows up as the acoustic grand instrument.
    uint8_t preset = guitar.GetPreset();
    if (preset > Midi::LAST_MIDI_PRESET)
        preset = Midi::MIDI_PRESET_ACOUSTIC_GRAND;

    Instrument instrument;
    instrument.setMidiPreset(preset);

    // Use the MIDI preset name as the description.
    instrument.setDescription(Midi::getPresetNames().at(preset));

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

using ChordDiagramMap = std::unordered_map<ChordName, ChordDiagram>;

static void
convertRhythmSlashes(const PowerTabDocument::System &old_system,
                     const Score &score, const ChordDiagramMap &chord_diagrams,
                     Staff &staff)
{
    // TODO - are rhythm slashes from v1.7 files always 6 strings?

    Voice &voice = staff.getVoices()[0];

    // Convert rhythm slashes to normal notes.
    const ChordText *prev_chord = nullptr;
    std::optional<size_t> prev_slash_idx;
    std::optional<size_t> triplet_start_idx;
    for (size_t i = 0; i < old_system.GetRhythmSlashCount(); ++i)
    {
        const PowerTabDocument::System::RhythmSlashPtr slash =
            old_system.GetRhythmSlash(i);
        Position pos;

        if (!slash->IsRest())
        {
            const ChordText *chord = ScoreUtils::getCurrentChordText(
                score, score.getSystems().size() - 1, slash->GetPosition());

            if (slash->IsSingleNote())
            {
                uint8_t string_num = 0, fret_num = 0;
                slash->GetSingleNoteData(string_num, fret_num);

                pos.insertNote(Note(string_num, fret_num));
            }
            else if (chord)
            {
                if (chord == prev_chord)
                {
                    assert(prev_slash_idx.has_value());

                    // Until there is a chord change, play the same notes as the
                    // previous slash. In particular, this is important because
                    // single note slashes remain active until a chord change.
                    const Position &prev_pos = voice.getPositions()[*prev_slash_idx];
                    for (const Note &note : prev_pos.getNotes())
                    {
                        pos.insertNote(
                            Note(note.getString(), note.getFretNumber()));
                    }
                }
                else if (auto it = chord_diagrams.find(chord->getChordName());
                         it != chord_diagrams.end())
                {
                    const ChordDiagram &diagram = it->second;
                    for (int s = 0; s < diagram.getStringCount(); ++s)
                    {
                        if (diagram.getFretNumber(s) >= 0)
                            pos.insertNote(Note(s, diagram.getFretNumber(s)));
                    }
                }
            }

            prev_chord = chord;
            prev_slash_idx = voice.getPositions().size();
        }

        pos.setPosition(slash->GetPosition());
        pos.setDurationType(
            static_cast<Position::DurationType>(slash->GetDurationType()));

        if (slash->IsDotted())
            pos.setProperty(Position::Dotted);
        if (slash->IsDoubleDotted())
            pos.setProperty(Position::DoubleDotted);
        if (slash->IsRest())
            pos.setRest();
        if (slash->IsStaccato())
            pos.setProperty(Position::Staccato);
        if (slash->HasPickStrokeUp())
            pos.setProperty(Position::PickStrokeUp);
        if (slash->HasPickStrokeDown())
            pos.setProperty(Position::PickStrokeDown);
        if (slash->HasArpeggioUp())
            pos.setProperty(Position::ArpeggioUp);
        if (slash->HasArpeggioDown())
            pos.setProperty(Position::ArpeggioDown);
        if (slash->IsTripletFeel1st())
            pos.setProperty(Position::TripletFeelFirst);
        if (slash->IsTripletFeel2nd())
            pos.setProperty(Position::TripletFeelSecond);
        if (slash->HasMarcato())
            pos.setProperty(Position::Marcato);
        if (slash->HasSforzando())
            pos.setProperty(Position::Sforzando);

        for (Note &note : pos.getNotes())
        {
            if (slash->IsTied())
                note.setProperty(Note::Tied);
            if (slash->IsMuted())
                note.setProperty(Note::Muted);
            if (slash->HasSlideIntoFromBelow())
                note.setProperty(Note::SlideIntoFromBelow);
            if (slash->HasSlideIntoFromAbove())
                note.setProperty(Note::SlideIntoFromAbove);
            if (slash->HasSlideOutOfDownwards())
                note.setProperty(Note::SlideOutOfDownwards);
            if (slash->HasSlideOutOfUpwards())
                note.setProperty(Note::SlideOutOfUpwards);
        }

        voice.insertPosition(pos);

        // Add irregular groups for triplets.
        if (slash->IsTripletStart())
        {
            assert(!triplet_start_idx.has_value());
            triplet_start_idx = i;
        }
        else if (slash->IsTripletMiddle())
        {
            assert(triplet_start_idx.has_value());
        }
        else if (slash->IsTripletEnd())
        {
            assert(triplet_start_idx.has_value());

            const int group_start =
                voice.getPositions()[*triplet_start_idx].getPosition();
            const int num_positions = i - *triplet_start_idx + 1;
            voice.insertIrregularGrouping(
                IrregularGrouping(group_start, num_positions, 3, 2));

            triplet_start_idx.reset();
        }
    }
}

void
PowerTabOldImporter::convert(const PowerTabDocument::Score &oldScore,
                             PowerTabDocument::Score::SystemConstPtr oldSystem,
                             const ChordDiagramMap &chord_diagrams, 
                             Score &score, System &system)
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

    // Import rhythm slashes as an additional staff before any other staves.
    if (oldSystem->GetRhythmSlashCount() > 0)
    {
        Staff staff;

        for (auto &dynamic : dynamics)
        {
            if (dynamic->IsRhythmSlashVolumeSet())
            {
                Dynamic new_dynamic(
                    dynamic->GetPosition(),
                    static_cast<VolumeLevel>(dynamic->GetRhythmSlashVolume()));
                staff.insertDynamic(new_dynamic);
                lastPosition =
                    std::max(lastPosition, new_dynamic.getPosition());
            }
        }

        convertRhythmSlashes(*oldSystem, score, chord_diagrams, staff);
        system.insertStaff(staff);
    }

    // Import staves.
    for (size_t i = 0; i < oldSystem->GetStaffCount(); ++i)
    {
        // Dynamics are now stored in the staff instead of the system.
        std::vector<PowerTabDocument::Score::DynamicPtr> dynamicsInStaff;
        for (auto &dynamic : dynamics)
        {
            if (dynamic->GetStaff() == i && dynamic->IsStaffVolumeSet())
                dynamicsInStaff.push_back(dynamic);
        }

        Staff staff;
        int lastPosInStaff = convert(*oldSystem->GetStaff(i), dynamicsInStaff,
                                     staff);
        system.insertStaff(std::move(staff));
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

void PowerTabOldImporter::convert(const PowerTabDocument::ChordText &oldChord,
                                  ChordText &chord)
{
    chord.setPosition(oldChord.GetPosition());
    chord.setChordName(convertChordName(oldChord.GetChordNameConstRef()));
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
            Dynamic new_dynamic(
                dynamic->GetPosition(),
                static_cast<VolumeLevel>(dynamic->GetStaffVolume()));
            staff.insertDynamic(new_dynamic);
            lastPosition = std::max(lastPosition, new_dynamic.getPosition());
        }
    }

    // Import positions.
    for (size_t voice_idx = 0;
         voice_idx < PowerTabDocument::Staff::NUM_STAFF_VOICES; ++voice_idx)
    {
        Voice &voice = staff.getVoices()[voice_idx];
        voice.setPositionsCapacity(oldStaff.GetPositionCount(voice_idx));
        for (size_t i = 0; i < oldStaff.GetPositionCount(voice_idx); ++i)
        {
            Position position;
            convert(*oldStaff.GetPosition(voice_idx, i), position);
            voice.insertPosition(std::move(position));
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
                if (notesPlayedOver > 0)
                {
                    v.insertIrregularGrouping(IrregularGrouping(
                        startPos, positionCount, notesPlayed, notesPlayedOver));
                }

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

    if (oldPosition.HasVolumeSwell())
    {
        uint8_t start_volume, end_volume, duration;
        oldPosition.GetVolumeSwell(start_volume, end_volume, duration);

        position.setVolumeSwell(
            VolumeSwell(static_cast<VolumeLevel>(start_volume),
                        static_cast<VolumeLevel>(end_volume), duration));
    }

    if (oldPosition.HasTremoloBar())
    {
        uint8_t type, duration, pitch;
        oldPosition.GetTremoloBar(type, duration, pitch);

        position.setTremoloBar(
            TremoloBar(static_cast<TremoloBar::Type>(type), pitch, duration));
    }

    // Import notes.
    position.setNotesCapacity(oldPosition.GetNoteCount());
    for (size_t i = 0; i < oldPosition.GetNoteCount(); ++i)
    {
        Note note;
        convert(*oldPosition.GetNote(i), note);
        position.insertNote(std::move(note));
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

using PlayerStaffMap =
    std::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS>;

PlayerChange
getPlayerChange(const PlayerStaffMap &player_staves, int position)
{
    PlayerChange change;
    change.setPosition(position);

    for (int player = 0; player < static_cast<int>(player_staves.size());
         ++player)
    {
        const int staff = player_staves[player];
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
    std::array<int, PowerTabDocument::Score::MAX_NUM_GUITARS> player_staves;
    player_staves.fill(-1);

    int prev_slash_offset = 0;
    for (size_t i = 0; i < oldScore.GetSystemCount(); ++i)
    {
        PowerTabDocument::Score::SystemConstPtr system = oldScore.GetSystem(i);
        std::vector<PowerTabDocument::Score::GuitarInPtr> guitarIns;
        oldScore.GetGuitarInsInSystem(guitarIns, system);

        // Rhythm slashes introduce an extra staff at the beginning of the
        // system.
        const int slash_offset = system->GetRhythmSlashCount() ? 1 : 0;
        size_t current_pos = 0;

        if (prev_slash_offset == slash_offset)
        {
            if (guitarIns.empty())
                continue; // Nothing to do.

            current_pos = guitarIns.front()->GetPosition();
        }
        else
        {
            // Rhythm slashes appeared or disappeared, so we need to shift any
            // existing staff assignments.
            // If rhythm slashes disappeared, this removes the assignment from
            // staff 0.
            const int offset = slash_offset - prev_slash_offset;
            for (int &staff : player_staves)
            {
                if (staff >= 0)
                    staff += offset;
            }
        }

        prev_slash_offset = slash_offset;

        // In v1.7, each staff has separate guitar ins. In the new format,
        // player changes occur at the system level so we need to combine
        // the guitar ins from several staves.
        for (auto &guitarIn : guitarIns)
        {
            // After combining all guitar in's at a position, write out a player
            // change.
            if (guitarIn->GetPosition() != current_pos)
            {
                score.getSystems()[i].insertPlayerChange(getPlayerChange(
                    player_staves, static_cast<int>(current_pos)));
            }

            const int staff_idx = guitarIn->GetStaff() + slash_offset;
            std::bitset<8> staff_guitars(guitarIn->GetStaffGuitars());
            std::bitset<8> slash_guitars(guitarIn->GetRhythmSlashGuitars());

            for (size_t player_idx = 0; player_idx < player_staves.size();
                 ++player_idx)
            {
                int &player_staff = player_staves[player_idx];

                // Clear out any players that are currently active for this
                // staff.
                if (guitarIn->HasStaffGuitarsSet() && player_staff == staff_idx)
                    player_staff = -1;

                if (guitarIn->HasRhythmSlashGuitarsSet() && player_staff == 0)
                    player_staff = -1;

                // Set any active players for this staff.
                if (staff_guitars[player_idx])
                    player_staff = staff_idx;
                else if (slash_guitars[player_idx])
                    player_staff = 0;
            }

            current_pos = guitarIn->GetPosition();
        }

        // After processing all of the guitar ins in the system, write out a
        // final player change.
        score.getSystems()[i].insertPlayerChange(
            getPlayerChange(player_staves, static_cast<int>(current_pos)));
    }
}

/// The initial volume level doesn't seem to always match the volume level of a
/// dynamic, so pick the closest one.
static VolumeLevel
findVolumeLevel(uint8_t volume)
{
    for (auto level :
         { VolumeLevel::fff, VolumeLevel::ff, VolumeLevel::f, VolumeLevel::mf,
           VolumeLevel::mp, VolumeLevel::p, VolumeLevel::pp })
    {
        if (volume >= static_cast<uint8_t>(level))
            return level;
    }

    return VolumeLevel::ppp;
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
                        findVolumeLevel(
                            oldScore.GetGuitar(j)->GetInitialVolume()));

                    system.getStaves()[guitarIn->GetStaff()].insertDynamic(dynamic);
                    break;
                }
            }
        }
    }
}

void
PowerTabOldImporter::convertFloatingText(
    const PowerTabDocument::Score &oldScore, Score &score)
{
    // Since floating text in v1.7 has arbitrary positions, we might need to
    // merge together multiple text items.

    using TextItemMap = std::unordered_map<int, std::string>;
    std::unordered_map<int, TextItemMap> system_text_items;

    for (size_t i = 0, n = oldScore.GetFloatingTextCount(); i < n; ++i)
    {
        // Convert from an absolute (x,y) position to a (system,position)
        // location.
        auto floatingText = oldScore.GetFloatingText(i);
        PowerTabDocument::Rect location = floatingText->GetRect();

        // Figure out which system to attach to.
        int system_idx = oldScore.GetSystemCount() - 1;
        for (size_t j = 0, n = oldScore.GetSystemCount(); j < n; ++j)
        {
            auto system = oldScore.GetSystem(j);
            if (system->GetRect().GetBottom() > location.GetTop())
            {
                system_idx = j;
                break;
            }
        }

        // Figure out what position in the system to use.
        auto system = oldScore.GetSystem(system_idx);
        const int position =
            static_cast<int>(system->GetPositionFromX(location.GetX()));

        // Combine the text with newlines if multiple text items end up at the
        // same position.
        std::string &contents = system_text_items[system_idx][position];

        if (!contents.empty())
            contents += '\n';

        contents += floatingText->GetText();
    }

    // Create all of the text items.
    for (auto &&[system_idx, text_items] : system_text_items)
    {
        System &system = score.getSystems()[system_idx];

        for (auto &&[position_idx, contents] : text_items)
        {
            TextItem item(position_idx, contents);
            system.insertTextItem(item);
        }
    }
}
