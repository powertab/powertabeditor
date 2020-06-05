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

#include "converter.h"
#include "parser.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <formats/fileformat.h>
#include <score/playerchange.h>
#include <score/position.h>
#include <score/score.h>
#include <score/scoreinfo.h>

/// Convert the Guitar Pro file metadata.
static void
convertScoreInfo(const Gp7::ScoreInfo &gp_info, Score &score)
{
    ScoreInfo info;
    SongData data;

    // No support for subtitle.
    // TODO - consider adding this to SongData.
    data.setTitle(gp_info.myTitle);
    data.setArtist(gp_info.myArtist);

    data.setAudioReleaseInfo(SongData::AudioReleaseInfo(
        SongData::AudioReleaseInfo::ReleaseType::Album, gp_info.myAlbum,
        boost::gregorian::day_clock::local_day().year(), false));

    // WordsAndMusic never seems to be set.
    data.setAuthorInfo(SongData::AuthorInfo(gp_info.myMusic, gp_info.myWords));
    data.setCopyright(gp_info.myCopyright);
    data.setTranscriber(gp_info.myTabber);
    data.setPerformanceNotes(gp_info.myInstructions);
    // Skipping Notices since there isn't an equivalent.

    info.setSongData(data);
    score.setScoreInfo(info);
}

/// Create players and instruments from the Guitar Pro tracks, and the initial
/// staff -> player & instrument assignment.
static void
convertPlayers(const std::vector<Gp7::Track> &tracks, Score &score,
               PlayerChange &player_change)
{
    for (const Gp7::Track &track : tracks)
    {
        assert(!track.mySounds.empty());
        const int instrument_idx = score.getInstruments().size();

        for (const Gp7::Sound &sound : track.mySounds)
        {
            Instrument instrument;
            instrument.setDescription(sound.myLabel);
            instrument.setMidiPreset(sound.myMidiPreset);

            score.insertInstrument(instrument);
        }

        for (const Gp7::Staff &staff : track.myStaves)
        {
            Player player;
            player.setDescription(track.myName);

            Tuning tuning;
            tuning.setNotes(std::vector<uint8_t>(staff.myTuning.rbegin(),
                                                 staff.myTuning.rend()));
            tuning.setCapo(staff.myCapo);
            player.setTuning(tuning);

            const int player_idx = score.getPlayers().size();
            score.insertPlayer(player);

            // Each player will be assigned its own staff, and is initially
            // assigned to the track's first instrument.
            // TODO - import track automations.
            player_change.insertActivePlayer(
                player_idx, ActivePlayer(player_idx, instrument_idx));
        }
    }
}

/// Convert from a Guitar Pro clef type to our clef type.
static Staff::ClefType
convertClefType(Gp7::Bar::ClefType clef_type)
{
    switch (clef_type)
    {
        case Gp7::Bar::ClefType::F4:
            return Staff::BassClef;
        default:
            return Staff::TrebleClef;
    }
}

static Note
convertNote(Position &position, const Gp7::Beat &gp_beat,
            const Gp7::Note &gp_note, const Tuning &tuning)
{
    using HarmonicType = Gp7::Note::HarmonicType;

    Note note;
    note.setFretNumber(gp_note.myFret);
    // String numbers are flipped around.
    note.setString(tuning.getStringCount() - gp_note.myString - 1);

    note.setProperty(Note::Tied, gp_note.myTied);
    note.setProperty(Note::GhostNote, gp_note.myGhost);
    note.setProperty(Note::Muted, gp_note.myMuted);
    note.setProperty(Note::HammerOnOrPullOff, gp_note.myHammerOn);
    note.setProperty(Note::HammerOnFromNowhere, gp_note.myLeftHandTapped);

    if (gp_note.myPalmMuted)
        position.setProperty(Position::PalmMuting);

    if (gp_note.myTapped)
        position.setProperty(Position::Tap);

    // The following values are not supported:
    // - staccatissimo (bit 1)
    // - tenuto (bit 4)
    if (gp_note.myAccentTypes.test(0))
        position.setProperty(Position::Staccato);
    if (gp_note.myAccentTypes.test(2))
        position.setProperty(Position::Sforzando);
    if (gp_note.myAccentTypes.test(3))
        position.setProperty(Position::Marcato);

    if (gp_beat.myOttavia)
    {
        switch (*gp_beat.myOttavia)
        {
            case Gp7::Beat::Ottavia::O8va:
                note.setProperty(Note::Octave8va);
                break;
            case Gp7::Beat::Ottavia::O8vb:
                note.setProperty(Note::Octave8vb);
                break;
            case Gp7::Beat::Ottavia::O15ma:
                note.setProperty(Note::Octave15ma);
                break;
            case Gp7::Beat::Ottavia::O15mb:
                note.setProperty(Note::Octave15mb);
                break;
        }
    }

    if (gp_note.myHarmonic)
    {
        note.setProperty(Note::NaturalHarmonic,
                         gp_note.myHarmonic == HarmonicType::Natural);

        if (gp_note.myHarmonic == HarmonicType::Tap)
        {
            note.setTappedHarmonicFret(note.getFretNumber() +
                                       gp_note.myHarmonicFret);
        }

        // TODO - import artificial harmonics
    }

    return note;
}

static Position
convertPosition(const Gp7::Beat &gp_beat, const Gp7::Rhythm &gp_rhythm)
{
    Position pos;

    pos.setDurationType(
        static_cast<Position::DurationType>(gp_rhythm.myDuration));
    pos.setProperty(Position::Dotted, gp_rhythm.myDots == 1);
    pos.setProperty(Position::DoubleDotted, gp_rhythm.myDots == 2);

    return pos;
}

static void
convertSystem(const Gp7::Document &doc, Score &score, int bar_begin,
              int bar_end)
{
    System system;

    // For the first system, create the players and assign them to the staves.
    if (bar_begin == 0)
    {
        PlayerChange initial_player_change;
        convertPlayers(doc.myTracks, score, initial_player_change);
        system.insertPlayerChange(initial_player_change);
    }

    // Create a staff for each player.
    for (auto &&player : score.getPlayers())
        system.insertStaff(Staff(player.getTuning().getStringCount()));

    int start_pos = 0;
    for (int bar_idx = bar_begin; bar_idx < bar_end; ++bar_idx)
    {
        const int num_staves = score.getPlayers().size();
        const Gp7::MasterBar &master_bar = doc.myMasterBars.at(bar_idx);

        // Go through the bar for each staff.
        int end_pos = start_pos;
        for (int staff_idx = 0; staff_idx < num_staves ; ++staff_idx)
        {
            Staff &staff = system.getStaves()[staff_idx];
            const Player &player = score.getPlayers()[staff_idx];
            const Tuning &tuning = player.getTuning();

            const int gp_bar_idx = master_bar.myBarIds[staff_idx];
            const Gp7::Bar &gp_bar = doc.myBars.at(gp_bar_idx);

            // For the first bar in the system, set the staff's clefy type.
            if (bar_idx == bar_begin)
                staff.setClefType(convertClefType(gp_bar.myClefType));

            assert(gp_bar.myVoiceIds.size() == 4);
            for (int voice_idx = 0; voice_idx < Staff::NUM_VOICES; ++voice_idx)
            {
                const int gp_voice_idx = gp_bar.myVoiceIds[voice_idx];
                // Voice might not be used.
                if (gp_voice_idx < 0)
                    continue;

                const Gp7::Voice &gp_voice = doc.myVoices.at(gp_voice_idx);
                Voice &voice = staff.getVoices()[voice_idx];

                int voice_pos = start_pos;
                for (int gp_beat_idx : gp_voice.myBeatIds)
                {
                    const Gp7::Beat &gp_beat = doc.myBeats.at(gp_beat_idx);
                    const Gp7::Rhythm &gp_rhythm =
                        doc.myRhythms.at(gp_beat.myRhythmId);

                    // TODO - convert irregular groupings.

                    Position pos = convertPosition(gp_beat, gp_rhythm);
                    pos.setPosition(voice_pos++);

                    // Flag as a rest if there are no notes.
                    if (gp_beat.myNoteIds.empty())
                        pos.setRest();

                    // Import notes.
                    for (int gp_note_id : gp_beat.myNoteIds)
                    {
                        const Gp7::Note &gp_note = doc.myNotes.at(gp_note_id);

                        Note note = convertNote(pos, gp_beat, gp_note, tuning);
                        if (Utils::findByString(pos, note.getString()))
                            throw FileFormatException("Colliding notes!");
                        else
                            pos.insertNote(note);
                    }

                    voice.insertPosition(pos);
                }

                end_pos = std::max(voice_pos, end_pos);
            }
        }

        // Insert a new barline unless we're finishing the system.
        if (bar_idx != (bar_end - 1))
        {
            Barline barline;
            barline.setPosition(end_pos);
            system.insertBarline(barline);

            start_pos = end_pos + 1;
        }
    }

    score.insertSystem(system);
}

void
Gp7::convert(const Gp7::Document &doc, Score &score)
{
    convertScoreInfo(doc.myScoreInfo, score);

    int bar_idx = 0;
    for (int num_bars : doc.myScoreInfo.myScoreSystemsLayout)
    {
        const int bar_end = std::min(bar_idx + num_bars,
                                     static_cast<int>(doc.myMasterBars.size()));
        convertSystem(doc, score, bar_idx, bar_end);
        bar_idx += num_bars;
    }
}
