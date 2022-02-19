/*
 * Copyright (C) 2020 Cameron White
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

#include "from_pt2.h"

#include "document.h"

#include <score/generalmidi.h>
#include <score/score.h>
#include <score/scorelocation.h>
#include <score/scoreinfo.h>
#include <score/utils.h>
#include <score/voiceutils.h>

#include <set>
#include <unordered_map>
#include <unordered_set>

/// Convert the Guitar Pro file metadata.
static Gp7::ScoreInfo
convertScoreInfo(const Score &score)
{
    const ScoreInfo &info = score.getScoreInfo();
    Gp7::ScoreInfo gp_info;

    if (info.getScoreType() == ScoreInfo::ScoreType::Song)
    {
        const SongData &song_data = info.getSongData();
        gp_info.myTitle = song_data.getTitle();
        gp_info.mySubtitle = song_data.getSubtitle();
        gp_info.myArtist = song_data.getArtist();

        if (song_data.isAudioRelease())
            gp_info.myAlbum = song_data.getAudioReleaseInfo().getTitle();
        else if (song_data.isVideoRelease())
            gp_info.myAlbum = song_data.getVideoReleaseInfo().getTitle();
        else if (song_data.isBootleg())
            gp_info.myAlbum = song_data.getBootlegInfo().getTitle();

        if (!song_data.isTraditionalAuthor())
        {
            gp_info.myMusic = song_data.getAuthorInfo().getComposer();
            gp_info.myWords = song_data.getAuthorInfo().getLyricist();
        }

        gp_info.myCopyright = song_data.getCopyright();
        gp_info.myTabber = song_data.getTranscriber();
        gp_info.myInstructions = song_data.getPerformanceNotes();
    }
    else
    {
        const LessonData &lesson_data = info.getLessonData();
        gp_info.myTitle = lesson_data.getTitle();
        gp_info.mySubtitle = lesson_data.getSubtitle();

        gp_info.myMusic = lesson_data.getAuthor();
        gp_info.myCopyright = lesson_data.getCopyright();
        gp_info.myInstructions = lesson_data.getNotes();
    }

    // Record the number of bars in each system (one less than the number of
    // barlines).
    for (const System &system : score.getSystems())
        gp_info.myScoreSystemsLayout.push_back(system.getBarlines().size() - 1);

    return gp_info;
}

using PlayerInstrumentMap = std::unordered_map<const Player *, std::set<int>>;

/// Find all of the instruments assigned to each player in the score.
static PlayerInstrumentMap
findPlayerInstruments(const Score &score)
{
    PlayerInstrumentMap map;

    for (const System &system : score.getSystems())
    {
        for (const PlayerChange &change : system.getPlayerChanges())
        {
            for (size_t staff = 0; staff < system.getStaves().size(); ++staff)
            {
                for (const ActivePlayer &active :
                     change.getActivePlayers(staff))
                {
                    const Player *player =
                        &score.getPlayers()[active.getPlayerNumber()];
                    map[player].insert(active.getInstrumentNumber());
                }
            }
        }
    }

    return map;
}

static std::vector<Gp7::Track>
convertTracks(const Score &score)
{
    std::vector<Gp7::Track> tracks;

    PlayerInstrumentMap player_instruments = findPlayerInstruments(score);
    int player_idx = 0;
    for (const Player &player : score.getPlayers())
    {
        Gp7::Track track;
        track.myName = player.getDescription();

        // Include only instruments that this player uses.
        for (int instrument_idx : player_instruments[&player])
        {
            const Instrument &inst = score.getInstruments()[instrument_idx];
            Gp7::Sound sound;
            sound.myLabel = inst.getDescription();
            sound.myName = "Instrument_" + std::to_string(instrument_idx);
            sound.myPath = "Midi/" + sound.myName;
            sound.myMidiPreset = inst.getMidiPreset();

            track.mySounds.push_back(sound);
        }

        const Tuning &tuning = player.getTuning();
        const std::vector<uint8_t> &notes = tuning.getNotes();

        Gp7::Staff staff;
        staff.myCapo = tuning.getCapo();
        staff.myTuning.assign(notes.rbegin(), notes.rend());
        track.myStaves.push_back(staff);

        // TODO - export chords

        // TODO - export player changes as sound automations
        // For now we just assign an initial instrument
        track.mySoundChanges.push_back({ 0, 0, 0 });

        track.myMidiChannel = Midi::getPlayerChannel(player_idx);

        tracks.push_back(std::move(track));
        ++player_idx;
    }

    return tracks;
}

static Gp7::Rhythm
convertRhythm(const Voice &voice, const Position &pos)
{
    Gp7::Rhythm rhythm;

    rhythm.myDuration = static_cast<int>(pos.getDurationType());
    if (pos.hasProperty(Position::Dotted))
        rhythm.myDots = 1;
    else if (pos.hasProperty(Position::DoubleDotted))
        rhythm.myDots = 2;

    // TODO - combine nested tuplets?
    for (const IrregularGrouping *group : VoiceUtils::getIrregularGroupsInRange(
             voice, pos.getPosition(), pos.getPosition()))
    {
        rhythm.myTupletNum = group->getNotesPlayed();
        rhythm.myTupletDenom = group->getNotesPlayedOver();
    }

    return rhythm;
}

static Gp7::Note::Pitch
convertPitch(const Tuning &tuning, const KeySignature &key, const Note &note,
             bool transpose)
{
    Gp7::Note::Pitch pitch;

    const uint8_t midi_note =
        tuning.getNote(note.getString(), transpose) + note.getFretNumber();

    std::string text = Midi::getMidiNoteText(
        midi_note, key.getKeyType() == KeySignature::Minor, key.usesSharps(),
        key.getNumAccidentals(), false);

    pitch.myNote = text[0];
    pitch.myAccidental = text.substr(1);
    pitch.myOctave = Midi::getMidiNoteOctave(midi_note, pitch.myNote) + 1;

    return pitch;
}

/// Convert from PT bend values (quarter steps) to GP bend values
/// (percentage of full steps)
static double
convertQuarterStepToPitch(int steps)
{
    return steps * 25.0;
}

static Gp7::Bend
convertBend(const Bend &bend)
{
    // TODO - handle bends stretching over multiple notes

    const double bent_pitch = convertQuarterStepToPitch(bend.getBentPitch());
    const double release_pitch = convertQuarterStepToPitch(bend.getReleasePitch());

    Gp7::Bend gp_bend;
    gp_bend.myDestOffset = 100;
    double middle_offset = 50;
    double middle_value = 0;
    double start_value = 0;
    double end_value = 0;

    switch (bend.getType())
    {
        case Bend::PreBendAndRelease:
            start_value = bent_pitch;
            middle_value = 0.5 * bent_pitch;
            end_value = 0;
            break;

        case Bend::PreBend:
        case Bend::PreBendAndHold:
            // Same behaviour, the only different for GP is if ties are present
            start_value = middle_value = end_value = bent_pitch;
            break;

        case Bend::NormalBend:
        case Bend::BendAndHold:
            middle_offset = 10; // Short bend
            middle_value = end_value = bent_pitch;
            break;

        case Bend::BendAndRelease:
            middle_value = bent_pitch;
            end_value = release_pitch;
            break;

        // TODO - these two modes probably need to locate the pitch from the
        // previous bend to get the correct starting point.
        case Bend::GradualRelease:
            start_value = bent_pitch;
            middle_value = 0.5 * (bent_pitch + release_pitch);
            end_value = release_pitch;
            break;
        case Bend::ImmediateRelease:
            start_value = middle_value = end_value = bent_pitch;
            break;
    }

    gp_bend.myOriginValue = start_value;
    gp_bend.myMiddleOffset1 = gp_bend.myMiddleOffset2 = middle_offset;
    gp_bend.myMiddleValue = middle_value;
    gp_bend.myDestValue = end_value;

    return gp_bend;
}

static Gp7::Bend
convertTremoloBar(const TremoloBar &trem)
{
    // TODO - handle events stretching over multiple notes

    const double pitch = convertQuarterStepToPitch(trem.getPitch());

    Gp7::Bend gp_bend;
    double middle_offset = 50;
    double middle_value = 0;
    double start_value = 0;
    double end_value = 0;
    double end_offset = 100;

    static constexpr double dip_middle = 15; // Short dip length
    static constexpr double dip_end = 30;

    switch (trem.getType())
    {
        case TremoloBar::Type::Dip:
            middle_offset = dip_middle;
            end_offset = dip_end;
            middle_value = -pitch;
            break;
        case TremoloBar::Type::InvertedDip:
            middle_offset = dip_middle;
            end_offset = dip_end;
            middle_value = pitch;
            break;
        case TremoloBar::Type::DiveAndRelease:
        case TremoloBar::Type::DiveAndHold:
            end_value = -pitch;
            middle_value = 0.5 * end_value;
            break;
        case TremoloBar::Type::Release:
            start_value = middle_value = end_value = pitch;
            break;
        // TODO - these two modes probably need to locate the pitch from the
        // previous bend to get the correct starting point.
        case TremoloBar::Type::ReturnAndRelease:
        case TremoloBar::Type::ReturnAndHold:
            end_value = pitch;
            middle_value = 0.5 * end_value;
            break;
    }

    gp_bend.myOriginValue = start_value;
    gp_bend.myMiddleOffset1 = gp_bend.myMiddleOffset2 = middle_offset;
    gp_bend.myMiddleValue = middle_value;
    gp_bend.myDestValue = end_value;
    gp_bend.myDestOffset = end_offset;

    return gp_bend;
}

static Gp7::Note::FingerType
convertFingering(LeftHandFingering::Finger f)
{
    using Finger = LeftHandFingering::Finger;
    using GpFingerType = Gp7::Note::FingerType;
    switch (f)
    {
        case Finger::None:
            return GpFingerType::Open;
        case Finger::Index:
            return GpFingerType::I;
        case Finger::Middle:
            return GpFingerType::M;
        case Finger::Ring:
            return GpFingerType::A;
        case Finger::Little:
            return GpFingerType::C;
        case Finger::Thumb:
            return GpFingerType::P;
    }
}

static void
convertBeat(Gp7::Document &doc, Gp7::Beat &beat, const System &system,
            const Voice &voice, const Tuning &tuning, const KeySignature &key,
            const Position &pos)
{
    beat.myGraceNote = pos.hasProperty(Position::Acciaccatura);
    beat.myTremoloPicking = pos.hasProperty(Position::TremoloPicking);

    beat.myBrushDown = pos.hasProperty(Position::PickStrokeDown);
    beat.myBrushUp = pos.hasProperty(Position::PickStrokeUp);
    beat.myArpeggioUp = pos.hasProperty(Position::ArpeggioUp);
    beat.myArpeggioDown = pos.hasProperty(Position::ArpeggioDown);

    if (pos.hasTremoloBar())
        beat.myWhammy = convertTremoloBar(pos.getTremoloBar());

    if (const TextItem *text = ScoreUtils::findByPosition(system.getTextItems(),
                                                          pos.getPosition()))
    {
        beat.myFreeText = text->getContents();
    }

    for (const Note &note: pos.getNotes())
    {
        Gp7::Note gp_note;

        // String numbers are flipped around
        gp_note.myString = tuning.getStringCount() - note.getString() - 1;
        gp_note.myFret = note.getFretNumber();

        // GP needs the actual pitch or else the note is ignored..
        gp_note.myConcertPitch = convertPitch(tuning, key, note, false);
        gp_note.myTransposedPitch = convertPitch(tuning, key, note, true);
        gp_note.myMidiPitch =
            tuning.getNote(note.getString(), false) + note.getFretNumber();

        // Note / position properties
        gp_note.myPalmMuted = pos.hasProperty(Position::PalmMuting);
        gp_note.myTapped = pos.hasProperty(Position::Tap);
        gp_note.myHammerOn = note.hasProperty(Note::HammerOnOrPullOff);
        gp_note.myLeftHandTapped = note.hasProperty(Note::HammerOnFromNowhere);
        gp_note.myMuted = note.hasProperty(Note::Muted);
        gp_note.myGhost = note.hasProperty(Note::GhostNote);
        gp_note.myVibrato = pos.hasProperty(Position::Vibrato);
        gp_note.myWideVibrato = pos.hasProperty(Position::WideVibrato);
        gp_note.myLetRing = pos.hasProperty(Position::LetRing);

        // The GP trill is the midi note value, not the fret number!
        if (note.hasTrill())
        {
            gp_note.myTrillNote =
                tuning.getNote(note.getString(), false) + note.getTrilledFret();
        }

        // Ties
        gp_note.myTieDest = note.hasProperty(Note::Tied);
        // TODO - handle ties between systems by providing the next_voice argument.
        if (auto next_note = VoiceUtils::getNextNote(voice, pos.getPosition(),
                                                     note.getString(), nullptr))
        {
            gp_note.myTieOrigin = next_note->hasProperty(Note::Tied);
        }

        // Accents
        using GpAccentType = Gp7::Note::AccentType;
        gp_note.myAccentTypes.set(int(GpAccentType::Staccato),
                                  pos.hasProperty(Position::Staccato));
        gp_note.myAccentTypes.set(int(GpAccentType::HeavyAccent),
                                  pos.hasProperty(Position::Sforzando));
        gp_note.myAccentTypes.set(int(GpAccentType::Accent),
                                  pos.hasProperty(Position::Marcato));

        // Slides
        using GpSlideType = Gp7::Note::SlideType;
        gp_note.mySlideTypes.set(int(GpSlideType::Shift),
                                 note.hasProperty(Note::ShiftSlide));
        gp_note.mySlideTypes.set(int(GpSlideType::Legato),
                                 note.hasProperty(Note::LegatoSlide));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideOutDown),
                                 note.hasProperty(Note::SlideOutOfDownwards));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideOutUp),
                                 note.hasProperty(Note::SlideOutOfUpwards));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideInAbove),
                                 note.hasProperty(Note::SlideIntoFromAbove));
        gp_note.mySlideTypes.set(int(GpSlideType::SlideInBelow),
                                 note.hasProperty(Note::SlideIntoFromBelow));

        // Octaves are stored on the beat in GP
        if (note.hasProperty(Note::Octave8va))
            beat.myOttavia = Gp7::Beat::Ottavia::O8va;
        else if (note.hasProperty(Note::Octave8vb))
            beat.myOttavia = Gp7::Beat::Ottavia::O8vb;
        else if (note.hasProperty(Note::Octave15ma))
            beat.myOttavia = Gp7::Beat::Ottavia::O15ma;
        else if (note.hasProperty(Note::Octave15mb))
            beat.myOttavia = Gp7::Beat::Ottavia::O15mb;

        if (note.hasBend())
            gp_note.myBend = convertBend(note.getBend());

        if (note.hasLeftHandFingering())
        {
            gp_note.myLeftFinger =
                convertFingering(note.getLeftHandFingering().getFinger());
        }

        const int note_id = doc.myNotes.size();
        doc.myNotes.emplace(note_id, gp_note);
        beat.myNoteIds.push_back(note_id);
    }
}

static void
convertBar(Gp7::Document &doc,
           std::unordered_map<Gp7::Rhythm, int> &unique_rhythms, Gp7::Bar &bar,
           const System &system, const Staff &staff, const Tuning &tuning,
           const KeySignature &key, int start_idx, int end_idx)
{
    switch (staff.getClefType())
    {
        case Staff::TrebleClef:
            bar.myClefType = Gp7::Bar::ClefType::G2;
            break;
        case Staff::BassClef:
            bar.myClefType = Gp7::Bar::ClefType::F4;
            break;
    }

    for (const Voice &voice : staff.getVoices())
    {
        Gp7::Voice gp_voice;

        for (const Position &pos :
             ScoreUtils::findInRange(voice.getPositions(), start_idx, end_idx))
        {
            Gp7::Beat beat;
            convertBeat(doc, beat, system, voice, tuning, key, pos);

            Gp7::Rhythm rhythm = convertRhythm(voice, pos);
            // Consolidate identical rhythms to reduce file size, which GP also
            // does.
            if (auto it = unique_rhythms.find(rhythm);
                it != unique_rhythms.end())
            {
                beat.myRhythmId = it->second;
            }
            else
            {
                const int rhythm_id = doc.myRhythms.size();
                doc.myRhythms.emplace(rhythm_id, rhythm);
                beat.myRhythmId = rhythm_id;

                unique_rhythms.emplace(rhythm, rhythm_id);
            }

            const int beat_id = doc.myBeats.size();
            doc.myBeats.emplace(beat_id, beat);
            gp_voice.myBeatIds.push_back(beat_id);
        }

        const int voice_id = doc.myVoices.size();
        doc.myVoices.emplace(voice_id, gp_voice);
        bar.myVoiceIds.push_back(voice_id);
    }
}

/// Create a GP master bar, from a pair of barlines in the score.
static Gp7::MasterBar
convertMasterBar(const Gp7::Document &doc, const System &system,
                 const Barline &start_line, const Barline &end_line)
{
    Gp7::MasterBar master_bar;

    if (start_line.hasRehearsalSign())
    {
        const RehearsalSign &sign = start_line.getRehearsalSign();
        master_bar.mySection = { sign.getLetters(), sign.getDescription() };
    }

    if (end_line.getBarType() == Barline::RepeatEnd)
    {
        master_bar.myRepeatEnd = true;
        master_bar.myRepeatCount = end_line.getRepeatCount();
    }

    master_bar.myRepeatStart =
        (start_line.getBarType() == Barline::RepeatStart);
    master_bar.myDoubleBar = (end_line.getBarType() == Barline::DoubleBar);
    master_bar.myFreeTime = (end_line.getBarType() == Barline::FreeTimeBar);

    // Time signature.
    const TimeSignature &time_sig = start_line.getTimeSignature();
    master_bar.myTimeSig.myBeats = time_sig.getNumPulses();
    master_bar.myTimeSig.myBeatValue = time_sig.getBeatValue();

    // Key signature.
    const KeySignature &key_sig = start_line.getKeySignature();
    master_bar.myKeySig.myAccidentalCount = key_sig.getNumAccidentals();
    master_bar.myKeySig.myMinor = key_sig.getKeyType() == KeySignature::Minor;
    master_bar.myKeySig.mySharps = key_sig.usesSharps();

    // Alternate endings.
    for (const AlternateEnding &ending : ScoreUtils::findInRange(
             system.getAlternateEndings(), start_line.getPosition(),
             end_line.getPosition() - 1))
    {
        for (int number : ending.getNumbers())
            master_bar.myAlternateEndings.push_back(number);
    }

    // Tempo markers.
    for (const TempoMarker &marker : ScoreUtils::findInRange(
             system.getTempoMarkers(), start_line.getPosition(),
             end_line.getPosition() - 1))
    {
        Gp7::TempoChange change;
        change.myPosition = 0; // TODO - compute location within bar.
        change.myIsVisible = true;
        change.myDescription = marker.getDescription();

        change.myIsLinear =
            (marker.getMarkerType() == TempoMarker::AlterationOfPace);

        if (change.myIsLinear)
        {
            // Copy the previous tempo, since this starts a linear change from
            // the current tempo to whatever the next tempo is.
            for (auto it = doc.myMasterBars.rbegin();
                 it != doc.myMasterBars.rend(); ++it)
            {
                if (!it->myTempoChanges.empty())
                {
                    const Gp7::TempoChange &last = it->myTempoChanges.back();
                    change.myBeatsPerMinute = last.myBeatsPerMinute;
                    change.myBeatType = last.myBeatType;
                    break;
                }
            }
        }
        else
        {
            change.myBeatsPerMinute = marker.getBeatsPerMinute();

            switch (marker.getBeatType())
            {
                using BeatType = Gp7::TempoChange::BeatType;

                case TempoMarker::Eighth:
                    change.myBeatType = BeatType::Eighth;
                    break;
                case TempoMarker::Half:
                    change.myBeatType = BeatType::Half;
                    break;
                case TempoMarker::HalfDotted:
                    change.myBeatType = BeatType::HalfDotted;
                    break;
                case TempoMarker::Quarter:
                    change.myBeatType = BeatType::Quarter;
                    break;
                case TempoMarker::QuarterDotted:
                    change.myBeatType = BeatType::QuarterDotted;
                    break;

                default:
                    // Other types aren't supported in GP
                    break;
            }
        }

        master_bar.myTempoChanges.push_back(change);
    }

    return master_bar;
}

static void
convertScore(const Score &score, Gp7::Document &doc)
{
    std::unordered_map<Gp7::Rhythm, int> unique_rhythms;

    ConstScoreLocation location(score);
    while (location.getSystemIndex() <
           static_cast<int>(score.getSystems().size()))
    {
        const System &system = location.getSystem();

        auto [current_bar, next_bar] = SystemUtils::getSurroundingBarlines(
            system, location.getPositionIndex());

        const PlayerChange *active_players = ScoreUtils::getCurrentPlayers(
            score, location.getSystemIndex(), location.getPositionIndex());

        // TODO - skip "empty" bars due to adjacent repeat end / start
        Gp7::MasterBar master_bar =
            convertMasterBar(doc, system, current_bar, next_bar);
        master_bar.myBarIds.assign(doc.myTracks.size(), -1);

        int staff_idx = 0;
        for (const Staff &staff : system.getStaves())
        {
            if (!active_players ||
                active_players->getActivePlayers(staff_idx).empty())
            {
                continue;
            }

            int player_idx = active_players->getActivePlayers(staff_idx)[0].getPlayerNumber();
            const Tuning &tuning = score.getPlayers()[player_idx].getTuning();
            if (tuning.getStringCount() != staff.getStringCount())
                continue; // Something is incorrect in the file...

            Gp7::Bar bar;
            convertBar(doc, unique_rhythms, bar, system, staff, tuning,
                       current_bar.getKeySignature(), current_bar.getPosition(),
                       next_bar.getPosition());

            const int bar_id = doc.myBars.size();
            doc.myBars.emplace(bar_id, bar);

            // Assign the bar to each player (track) in this staff.
            for (const ActivePlayer &player :
                 active_players->getActivePlayers(staff_idx))
            {
                master_bar.myBarIds[player.getPlayerNumber()] = bar_id;
            }

            ++staff_idx;
        }

        doc.myMasterBars.emplace_back(std::move(master_bar));

        // Move to next bar / system.
        if (&next_bar == &system.getBarlines().back())
        {
            location.setSystemIndex(location.getSystemIndex() + 1);
            location.setPositionIndex(0);
        }
        else
            location.setPositionIndex(next_bar.getPosition());
    }
}

Gp7::Document
Gp7::convert(const Score &score)
{
    Gp7::Document gp_doc;
    gp_doc.myScoreInfo = convertScoreInfo(score);

    gp_doc.myTracks = convertTracks(score);
    convertScore(score, gp_doc);

    return gp_doc;
}
