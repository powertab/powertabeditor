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

/// Convert the Guitar Pro file metadata.
static Gp7::ScoreInfo
convertScoreInfo(const ScoreInfo &info)
{
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

    return gp_info;
}

static std::vector<Gp7::Track>
convertTracks(const Score &score)
{
    std::vector<Gp7::Track> tracks;

    for (const Player &player : score.getPlayers())
    {
        Gp7::Track track;
        track.myName = player.getDescription();

        // TODO - export only instruments that this player uses in the score?
        for (const Instrument &inst : score.getInstruments())
        {
            Gp7::Sound sound;
            sound.myLabel = inst.getDescription();
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

        tracks.push_back(std::move(track));
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

static void
convertBeat(Gp7::Document &doc, Gp7::Beat &beat, const Tuning &tuning,
            const KeySignature &key, const Position &pos)
{
    beat.myGraceNote = pos.hasProperty(Position::Acciaccatura);

    for (const Note &note: pos.getNotes())
    {
        Gp7::Note gp_note;

        // String numbers are flipped around
        gp_note.myString = tuning.getStringCount() - note.getString() - 1;
        gp_note.myFret = note.getFretNumber();

        // GP needs the actual pitch or else the note is ignored..
        gp_note.myConcertPitch = convertPitch(tuning, key, note, false);
        gp_note.myTransposedPitch = convertPitch(tuning, key, note, true);

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

        const int note_id = doc.myNotes.size();
        doc.myNotes.emplace(note_id, gp_note);
        beat.myNoteIds.push_back(note_id);
    }
}

static void
convertBar(Gp7::Document &doc, Gp7::Bar &bar, const Staff &staff,
           const Tuning &tuning, const KeySignature &key, int start_idx,
           int end_idx)
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
            convertBeat(doc, beat, tuning, key, pos);

            Gp7::Rhythm rhythm = convertRhythm(voice, pos);

            // TODO - consolidate identical rhythms? The files generated by GP
            // seem to do this.
            const int rhythm_id = doc.myRhythms.size();
            doc.myRhythms.emplace(rhythm_id, rhythm);
            beat.myRhythmId = rhythm_id;

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
convertMasterBar(const Barline &start_line, const Barline &end_line)
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

    return master_bar;
}

static void
convertScore(const Score &score, Gp7::Document &doc)
{
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
        Gp7::MasterBar master_bar = convertMasterBar(current_bar, next_bar);
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
            convertBar(doc, bar, staff, tuning, current_bar.getKeySignature(),
                       current_bar.getPosition(), next_bar.getPosition());

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
    gp_doc.myScoreInfo = convertScoreInfo(score.getScoreInfo());

    gp_doc.myTracks = convertTracks(score);
    convertScore(score, gp_doc);

    return gp_doc;
}
