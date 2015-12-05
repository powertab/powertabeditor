/*
  * Copyright (C) 2015 Cameron White
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
  
#include "midifile.h"

#include "repeatcontroller.h"

#include <boost/rational.hpp>

#include <score/generalmidi.h>
#include <score/score.h>
#include <score/scorelocation.h>
#include <score/systemlocation.h>
#include <score/utils.h>
#include <score/voiceutils.h>

static const int PERCUSSION_CHANNEL = 9;
static const int METRONOME_CHANNEL = PERCUSSION_CHANNEL;
static const int DEFAULT_PPQ = 480;

static const int PITCH_BEND_RANGE = 24;
static const int DEFAULT_BEND = 64;
static const int SLIDE_OUT_STEPS = 5;

/// Pitch bend amount to bend a note by a quarter tone.
static const boost::rational<int> BEND_QUARTER_TONE(
    (Midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL - DEFAULT_BEND), 2 * PITCH_BEND_RANGE);

static const int SLIDE_BELOW_BEND = boost::rational_cast<int>(
    DEFAULT_BEND - SLIDE_OUT_STEPS * 2 * BEND_QUARTER_TONE);
static const int SLIDE_ABOVE_BEND = boost::rational_cast<int>(
    DEFAULT_BEND + SLIDE_OUT_STEPS * 2 * BEND_QUARTER_TONE);

enum Velocity : uint8_t
{
    DefaultVelocity = 127,
    MutedVelocity = 75,
    GhostVelocity = 50,
    PalmMutedVelocity = 112
};

/// Returns the MIDI channel that should be used for the player.
/// Since channel 10 is reserved for percussion, we can't use that
/// channel for regular instruments.
static int getChannel(int player)
{
    if (player >= PERCUSSION_CHANNEL)
        player++;
    return player;
}

static int getChannel(const ActivePlayer &player)
{
    return getChannel(player.getPlayerNumber());
}

static SystemLocation moveToNextBar(MidiEventList &event_list, int ticks,
                                    bool record_position_changes,
                                    const System &system,
                                    SystemLocation location, int next_bar_pos,
                                    RepeatController &repeat_controller)
{
    // Follow any repeats / directions in the bar.
    SystemLocation prev_location = location;
    SystemLocation new_location;
    for (int i = location.getPosition() + 1; i <= next_bar_pos; ++i)
    {
        location.setPosition(i);

        if (repeat_controller.checkForRepeat(prev_location, location,
                                             new_location))
        {
            if (record_position_changes)
            {
                event_list.append(
                    MidiEvent::positionChange(ticks, new_location));
            }

            return new_location;
        }
        else
            prev_location = location;
    }

    // Otherwise, move to the next bar.
    if (next_bar_pos == system.getBarlines().back().getPosition())
    {
        location.setSystem(location.getSystem() + 1);
        location.setPosition(0);
    }
    else
        location.setPosition(next_bar_pos);

    return location;
}

MidiFile::MidiFile() : myTicksPerBeat(0)
{
}

void MidiFile::load(const Score &score, const LoadOptions &options)
{
    myTicksPerBeat = DEFAULT_PPQ;

    RepeatController repeat_controller(score);

    MidiEventList master_track;
    MidiEventList metronome_track;

    // Set the initial channel volume and pitch bend range..
    std::vector<MidiEventList> regular_tracks(score.getPlayers().size());
    for (int i = 0; i < score.getPlayers().size(); ++i)
    {
        regular_tracks[i].append(
            MidiEvent::volumeChange(0, getChannel(i), Dynamic::fff));

        for (const MidiEvent &event :
             MidiEvent::pitchWheelRange(0, getChannel(i), PITCH_BEND_RANGE))
        {
            regular_tracks[i].append(event);
        }

    }

    SystemLocation location(0, 0);
    std::vector<uint8_t> active_bends;
    int system_index = -1;
    int current_tick = 0;
    int current_tempo = Midi::BEAT_DURATION_120_BPM;

    while (location.getSystem() < score.getSystems().size())
    {
        const System &system = score.getSystems()[location.getSystem()];
        const Barline *current_bar = ScoreUtils::findByPosition(
            system.getBarlines(), location.getPosition());
        const Barline *next_bar = system.getNextBarline(location.getPosition());

        if (location.getSystem() != system_index)
        {
            active_bends.resize(system.getStaves().size(), DEFAULT_BEND);
            system_index = location.getSystem();
        }

        const int start_tick = current_tick;
        current_tempo =
            addTempoEvent(master_track, start_tick, current_tempo, system,
                          current_bar->getPosition(), next_bar->getPosition());

        for (int staff_index = 0; staff_index < system.getStaves().size();
             ++staff_index)
        {
            const Staff &staff = system.getStaves()[staff_index];

            for (int voice_index = 0; voice_index < staff.getVoices().size();
                 ++voice_index)
            {
                const int end_tick = addEventsForBar(
                    regular_tracks, active_bends[staff_index], start_tick,
                    current_tempo, score, system, location.getSystem(), staff,
                    staff_index, staff.getVoices()[voice_index], voice_index,
                    current_bar->getPosition(), next_bar->getPosition(),
                    options);

                current_tick = std::max(current_tick, end_tick);
            }
        }

        // Generate metronome events.
        current_tick = std::max(
            current_tick,
            generateMetronome(metronome_track, start_tick, system, *current_bar,
                              *next_bar, location, options));

        location = moveToNextBar(
            metronome_track, current_tick, options.myRecordPositionChanges,
            system, location, next_bar->getPosition(), repeat_controller);
    }

    myTracks.push_back(master_track);
    myTracks.insert(myTracks.end(), regular_tracks.begin(), regular_tracks.end());
    if (options.myEnableMetronome)
        myTracks.push_back(metronome_track);

    for (MidiEventList &track : myTracks)
    {
        track.append(MidiEvent::endOfTrack(current_tick));
        track.convertToDeltaTicks();
    }
}

int MidiFile::generateMetronome(MidiEventList &event_list, int current_tick,
                                const System &system,
                                const Barline &current_bar,
                                const Barline &next_bar,
                                const SystemLocation &location,
                                const LoadOptions &options)
{
    const TimeSignature &time_sig = current_bar.getTimeSignature();

    const int num_pulses = time_sig.getNumPulses();
    const int beats_per_measure = time_sig.getBeatsPerMeasure();
    const int beat_value = time_sig.getBeatValue();

    // Figure out the duration of a pulse.
    const int duration = boost::rational_cast<int>(
        boost::rational<int>(4, beat_value) *
        boost::rational<int>(beats_per_measure, num_pulses) * myTicksPerBeat);

    // Check for multi-bar rests, as we need to generate more metronome events
    // to fill the extra bars.
    int num_repeats = 1;
    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &pos : ScoreUtils::findInRange(
                     voice.getPositions(), current_bar.getPosition(),
                     next_bar.getPosition()))
            {
                if (pos.hasMultiBarRest())
                {
                    num_repeats =
                        std::max(num_repeats, pos.getMultiBarRestCount());
                }
            }
        }
    }

    for (int repeat = 0; repeat < num_repeats; ++repeat)
    {
        for (int i = 0; i < num_pulses; ++i)
        {
            const uint8_t velocity =
                (i == 0) ? options.myStrongAccentVel : options.myWeakAccentVel;

            event_list.append(MidiEvent::noteOn(current_tick, METRONOME_CHANNEL,
                                                options.myMetronomePreset,
                                                velocity, location));

            current_tick += duration;

            event_list.append(
                MidiEvent::noteOff(current_tick, METRONOME_CHANNEL,
                                   options.myMetronomePreset, location));
        }
    }

    return current_tick;
}

int MidiFile::addTempoEvent(MidiEventList &event_list, int current_tick,
                            int current_tempo, const System &system,
                            int bar_start, int bar_end)
{
    auto markers = ScoreUtils::findInRange(system.getTempoMarkers(), bar_start,
                                           bar_end - 1);
    // If multiple tempo markers occur in a bar, just choose the last one.
    if (!markers.empty())
    {
        const TempoMarker &marker = markers.back();

        // Convert the values in the TempoMarker::BeatType enum to a factor that
        // will scale the bpm value to be in terms of quarter notes.
        boost::rational<int> scale(2, 1 << (marker.getBeatType() / 2));
        if (marker.getBeatType() % 2 != 0)
            scale *= boost::rational<int>(3, 2);

        // Compute the number of microseconds per quarter note.
        current_tempo = boost::rational_cast<int>(
            60000000 / (scale * marker.getBeatsPerMinute()));

        event_list.append(MidiEvent::setTempo(current_tick, current_tempo));
    }

    return current_tempo;
}

static int getWholeRestDuration(const System &system, const Voice &voice,
                                const Position &pos, int bar_start, int bar_end,
                                int original_duration)
{
    // If the whole rest is not the only item in the bar, treat it like a
    // regular rest.
    for (int i = bar_start; i < bar_end; ++i)
    {
        const Position *other_pos =
            ScoreUtils::findByPosition(voice.getPositions(), i);

        if (other_pos && other_pos != &pos)
            return original_duration;
    }

    // Otherwise, extend the rest for the entire bar.
    const Barline *barline =
        ScoreUtils::findByPosition(system.getBarlines(), bar_start);
    const TimeSignature& time_sig = barline->getTimeSignature();

    return boost::rational_cast<int>(
        time_sig.getBeatsPerMeasure() *
        boost::rational<int>(4, time_sig.getBeatValue()));
}

static int getActualNotePitch(const Note &note, const Tuning &tuning)
{
    const int open_string_pitch =
        tuning.getNote(note.getString(), false) + tuning.getCapo();
    int pitch = open_string_pitch + note.getFretNumber();

    if (note.hasProperty(Note::NaturalHarmonic))
    {
        pitch =
            open_string_pitch + Harmonics::getPitchOffset(note.getFretNumber());
    }

    if (note.hasTappedHarmonic())
    {
        pitch += Harmonics::getPitchOffset(note.getTappedHarmonicFret() -
                                           note.getFretNumber());
    }

    if (note.hasArtificialHarmonic())
    {
        static const int theKeyOffsets[] = { 0, 2, 4, 5, 7, 9, 10 };

        ArtificialHarmonic harmonic = note.getArtificialHarmonic();
        pitch = (Midi::getMidiNoteOctave(pitch) +
                 static_cast<int>(harmonic.getOctave()) + 2) * 12 +
                theKeyOffsets[harmonic.getKey()] + harmonic.getVariation();
    }

    return pitch;
}

/// Returns the appropriate note velocity type for the given position/note.
static Velocity getNoteVelocity(const Position &pos, const Note &note)
{
    if (note.hasProperty(Note::GhostNote))
        return Velocity::GhostVelocity;
    else if (note.hasProperty(Note::Muted))
        return Velocity::MutedVelocity;
    else if (pos.hasProperty(Position::PalmMuting))
        return Velocity::PalmMutedVelocity;
    else
        return Velocity::DefaultVelocity;
}

/// Compute the number of ticks for a grace note - it should correspond to about
/// a 32nd note at 120bpm.
static int getGraceNoteTicks(int ppq, int current_tempo)
{
    return boost::rational_cast<int>(
        boost::rational<int>(Midi::BEAT_DURATION_120_BPM, 8) /
        boost::rational<int>(current_tempo, ppq));
}

static int getArpeggioOffset(int ppq, int current_tempo)
{
    return boost::rational_cast<int>(
        boost::rational<int>(Midi::BEAT_DURATION_120_BPM, 16) /
        boost::rational<int>(current_tempo, ppq));
}

/// Holds basic information about a bend - used to simplify the generateBends
/// function.
struct BendEventInfo
{
    BendEventInfo(int tick, uint8_t bend_amount)
        : myTick(tick), myBendAmount(bend_amount)
    {
    }

    int myTick;
    uint8_t myBendAmount;
};

static void generateGradualBend(std::vector<BendEventInfo> &bends,
                                int start_tick, int duration, int start_bend,
                                int release_bend)
{
    const int num_events = std::abs(start_bend - release_bend);
    if (!num_events)
        return;

    const int event_duration = duration / num_events;
    for (int i = 1; i <= num_events; ++i)
    {
        const int tick = start_tick + i * event_duration;
        if (start_bend < release_bend)
            bends.push_back(BendEventInfo(tick, start_bend + i));
        else
            bends.push_back(BendEventInfo(tick, start_bend - i));
    }
}

static void generateBends(std::vector<BendEventInfo> &bends,
                          uint8_t &active_bend, int start_tick, int duration,
                          int ppq, const Note &note)
{
    const Bend &bend = note.getBend();

    const int bend_amount = boost::rational_cast<int>(
        DEFAULT_BEND + bend.getBentPitch() * BEND_QUARTER_TONE);
    const int release_amount = boost::rational_cast<int>(
        DEFAULT_BEND + bend.getReleasePitch() * BEND_QUARTER_TONE);

    switch (bend.getType())
    {
        case Bend::PreBend:
        case Bend::PreBendAndRelease:
        case Bend::PreBendAndHold:
            bends.push_back(BendEventInfo(start_tick, bend_amount));
            break;

        case Bend::NormalBend:
        case Bend::BendAndHold:
            // Perform a normal (gradual) bend.
            if (bend.getDuration() == 0)
            {
                // Bend over a 32nd note.
                generateGradualBend(bends, start_tick, ppq / 8, DEFAULT_BEND,
                                    bend_amount);
            }
            else if (bend.getDuration() == 1)
            {
                // Bend over the current note duration.
                generateGradualBend(bends, start_tick, duration, DEFAULT_BEND,
                                    bend_amount);
            }
            // TODO - implement bends that stretch over multiple notes.
            break;

        case Bend::BendAndRelease:
            // Bend up to the bent pitch for half of the note duration.
            generateGradualBend(bends, start_tick, duration / 2, DEFAULT_BEND,
                                bend_amount);
            break;
        default:
            break;
    }

    // Bend back down.
    switch (bend.getType())
    {
        case Bend::PreBend:
        case Bend::ImmediateRelease:
        case Bend::NormalBend:
            bends.push_back(
                BendEventInfo(start_tick + duration, release_amount));
            break;

        case Bend::PreBendAndRelease:
            generateGradualBend(bends, start_tick, duration, bend_amount,
                                release_amount);
            break;

        case Bend::BendAndRelease:
            generateGradualBend(bends, start_tick + duration / 2, duration / 2,
                                bend_amount, release_amount);
            break;

        case Bend::GradualRelease:
            generateGradualBend(bends, start_tick, duration, active_bend,
                                release_amount);
            break;
        default:
            break;
    }

    if (bend.getType() == Bend::BendAndHold ||
        bend.getType() == Bend::PreBendAndHold)
    {
        active_bend = bend_amount;
    }
    else
    {
        // Always return to the default bend, regardless of the release pitch.
        if (!bends.empty())
            bends.back().myBendAmount = DEFAULT_BEND;
        active_bend = DEFAULT_BEND;
    }
}

static void generateSlides(std::vector<BendEventInfo> &bends, int start_tick,
                           int note_duration, int ppq, const Note &note,
                           const Note *next_note)
{
    if (note.hasProperty(Note::ShiftSlide) ||
        note.hasProperty(Note::LegatoSlide) ||
        note.hasProperty(Note::SlideOutOfDownwards) ||
        note.hasProperty(Note::SlideOutOfUpwards))
    {
        int bend_amount = DEFAULT_BEND;

        if (note.hasProperty(Note::ShiftSlide) ||
            note.hasProperty(Note::LegatoSlide))
        {
            if (next_note)
            {
                bend_amount = boost::rational_cast<int>(
                    DEFAULT_BEND +
                    (next_note->getFretNumber() - note.getFretNumber()) * 2 *
                        BEND_QUARTER_TONE);
            }
            else
            {
                // Treat as a slide out of downwards.
                bend_amount = SLIDE_BELOW_BEND;
            }
        }
        else if (note.hasProperty(Note::SlideOutOfDownwards))
            bend_amount = SLIDE_BELOW_BEND;
        else if (note.hasProperty(Note::SlideOutOfUpwards))
            bend_amount = SLIDE_ABOVE_BEND;

        // Start the slide in the last part of the note duration, to make it
        // somewhat more realistic-sounding.
        const int slide_duration = note_duration / 3;
        generateGradualBend(bends, start_tick + note_duration - slide_duration,
                            slide_duration, DEFAULT_BEND, bend_amount);

        // Reset pitch wheel after note is played.
        bends.push_back(
            BendEventInfo(start_tick + note_duration, DEFAULT_BEND));
    }

    if (note.hasProperty(Note::SlideIntoFromAbove) ||
        note.hasProperty(Note::SlideIntoFromBelow))
    {
        int bend_amount = note.hasProperty(Note::SlideIntoFromAbove)
                              ? SLIDE_ABOVE_BEND
                              : SLIDE_BELOW_BEND;

        // Slide over a 16th note.
        const int slide_duration = ppq / 4;
        generateGradualBend(bends, start_tick, slide_duration, bend_amount,
                            DEFAULT_BEND);
    }
}

int MidiFile::addEventsForBar(
    std::vector<MidiEventList> &tracks, uint8_t &active_bend, int current_tick,
    int current_tempo, const Score &score, const System &system,
    int system_index, const Staff &staff, int staff_index, const Voice &voice,
    int voice_index, int bar_start, int bar_end, const LoadOptions &options)
{
    ScoreLocation location(score, system_index, staff_index, voice_index);
    const Voice *prev_voice = VoiceUtils::getAdjacentVoice(location, -1);
    const Voice *next_voice = VoiceUtils::getAdjacentVoice(location, 1);
    bool let_ring_active = false;

    for (int position = bar_start; position < bar_end; ++position)
    {
        // Handle player/instrument changes.
        const PlayerChange *current_players =
            ScoreUtils::findByPosition(system.getPlayerChanges(), position);
        if (current_players)
        {
            for (const ActivePlayer &player :
                 current_players->getActivePlayers(staff_index))
            {
                const Instrument &instrument =
                    score.getInstruments()[player.getInstrumentNumber()];

                tracks[player.getPlayerNumber()].append(
                    MidiEvent::programChange(current_tick, getChannel(player),
                                             instrument.getMidiPreset()));
            }
        }

        if (!current_players)
        {
            current_players =
                ScoreUtils::getCurrentPlayers(score, system_index, position);
        }
        std::vector<ActivePlayer> active_players;
        if (current_players)
            active_players = current_players->getActivePlayers(staff_index);

        // Handle dynamics.
        const Dynamic *dynamic =
            ScoreUtils::findByPosition(staff.getDynamics(), position);
        if (dynamic)
        {
            for (const ActivePlayer &player : active_players)
            {
                tracks[player.getPlayerNumber()].append(MidiEvent::volumeChange(
                    current_tick, getChannel(player), dynamic->getVolume()));
            }
        }

        // Handle notes.
        const Position *pos =
            ScoreUtils::findByPosition(voice.getPositions(), position);
        if (!pos)
            continue;

        const SystemLocation system_location(system_index, position);
        int duration = boost::rational_cast<int>(
            myTicksPerBeat * VoiceUtils::getDurationTime(voice, *pos));

        if (pos->isRest())
        {
            // For whole rests, they must last for the entire bar, regardless
            // of time signature.
            if (pos->getDurationType() == Position::WholeNote)
            {
                duration = getWholeRestDuration(system, voice, *pos, bar_start,
                                                bar_end, duration);

                // Extend for multi-bar rests.
                if (pos->hasMultiBarRest())
                    duration *= pos->getMultiBarRestCount();
            }

            current_tick += duration;
            continue;
        }

        // Handle grace notes.
        if (pos->hasProperty(Position::Acciaccatura))
        {
            duration = getGraceNoteTicks(myTicksPerBeat, current_tempo);
            current_tick -= duration;
        }

        // If there aren't any active players, treat as a rest.
        if (active_players.empty())
        {
            current_tick += duration;
            continue;
        }

        // Vibrato events (these apply to all notes in the position).
        if (pos->hasProperty(Position::Vibrato) ||
            pos->hasProperty(Position::WideVibrato))
        {
            const uint8_t width = pos->hasProperty(Position::Vibrato)
                                      ? options.myVibratoStrength
                                      : options.myWideVibratoStrength;

            for (const ActivePlayer &player : active_players)
            {
                const int channel = getChannel(player);

                // Add vibrato event, and an event to turn off the vibrato after
                // the note is done.
                tracks[player.getPlayerNumber()].append(
                    MidiEvent::modWheel(current_tick, channel, width));

                tracks[player.getPlayerNumber()].append(
                    MidiEvent::modWheel(current_tick + duration, channel, 0));
            }
        }

        // Let ring events (applied to all notes in the position).
        if (pos->hasProperty(Position::LetRing) && !let_ring_active)
        {
            for (const ActivePlayer &player : active_players)
            {
                tracks[player.getPlayerNumber()].append(MidiEvent::holdPedal(
                    current_tick, getChannel(player), true));
            }

            let_ring_active = true;
        }
        else if (!pos->hasProperty(Position::LetRing) && let_ring_active)
        {
            for (const ActivePlayer &player : active_players)
            {
                tracks[player.getPlayerNumber()].append(MidiEvent::holdPedal(
                    current_tick, getChannel(player), false));
            }

            let_ring_active = false;
        }
        // Make sure that we end the let ring after the last position in the bar.
        else if (let_ring_active &&
                 (pos ==
                  &ScoreUtils::findInRange(voice.getPositions(), bar_start,
                                           bar_end) .back()))
        {
            for (const ActivePlayer &player : active_players)
            {
                tracks[player.getPlayerNumber()].append(MidiEvent::holdPedal(
                    current_tick + duration, getChannel(player), false));
            }

            let_ring_active = false;
        }

        for (const Note &note : pos->getNotes())
        {
            // For arpeggios, delay the start of each note a small amount from
            // the last, and also adjust the duration correspondingly.
            if (pos->hasProperty(Position::ArpeggioDown) ||
                pos->hasProperty(Position::ArpeggioUp))
            {
                const int offset =
                    getArpeggioOffset(myTicksPerBeat, current_tempo);
                current_tick += offset;
                duration -= offset;
            }

            // Pick a tuning from one of the active players.
            // TODO - should we handle cases where different tunings are used
            // by players in the same staff?
            const int player_index = active_players.front().getPlayerNumber();
            const Tuning &tuning = score.getPlayers()[player_index].getTuning();
            int pitch = getActualNotePitch(note, tuning);
            const Velocity velocity = getNoteVelocity(*pos, note);

            // If this note is not tied to the previous note, play the note.
            if (!note.hasProperty(Note::Tied))
            {
                for (const ActivePlayer &active_player : active_players)
                {
                    const int player_index = active_player.getPlayerNumber();

                    tracks[player_index].append(MidiEvent::noteOn(
                        current_tick, getChannel(active_player), pitch,
                        velocity, system_location));
                }
            }
            // If the note is tied, make sure that the pitch is the same as the
            // previous note, so that the Stop Note event works correctly with
            // harmonics. There may be multiple notes tied together, though, so
            // we need to find the first note in the sequence.
            else
            {
                const Note *prev_note = &note;
                const Position *prev_pos = pos;
                const Voice *current_voice = &voice;

                while (prev_note && prev_note->hasProperty(Note::Tied))
                {
                    prev_pos = VoiceUtils::getPreviousPosition(
                        *current_voice, prev_pos->getPosition());
                    if (!prev_pos)
                    {
                        if (current_voice != prev_voice && prev_voice)
                        {
                            // Continue back to the previous system to handle
                            // ties between systems.
                            // TODO - handle ties that stretch across > 2 systems?
                            current_voice = prev_voice;
                            prev_pos = VoiceUtils::getPreviousPosition(
                                *prev_voice, std::numeric_limits<int>::max());
                        }
                        else
                            break;
                    }

                    prev_note = Utils::findByString(*prev_pos, note.getString());
                }

                if (prev_note)
                    pitch = getActualNotePitch(*prev_note, tuning);
            }

            // Generate all events that involve pitch bends.
            {
                std::vector<BendEventInfo> bend_events;

                if (note.hasProperty(Note::SlideIntoFromAbove) ||
                    note.hasProperty(Note::SlideIntoFromBelow) ||
                    note.hasProperty(Note::ShiftSlide) ||
                    note.hasProperty(Note::LegatoSlide) ||
                    note.hasProperty(Note::SlideOutOfDownwards) ||
                    note.hasProperty(Note::SlideOutOfUpwards))
                {
                    generateSlides(bend_events, current_tick, duration,
                                   myTicksPerBeat, note,
                                   VoiceUtils::getNextNote(voice, position,
                                                           note.getString()));
                }

                if (note.hasBend())
                {
                    generateBends(bend_events, active_bend, current_tick,
                                  duration, myTicksPerBeat, note);
                }

                for (const BendEventInfo &event : bend_events)
                {
                    for (const ActivePlayer &player : active_players)
                    {
                        tracks[player.getPlayerNumber()].append(
                            MidiEvent::pitchWheel(event.myTick,
                                                  getChannel(player),
                                                  event.myBendAmount));
                    }
                }
            }

            // Perform tremolo picking or trills - they work identically, except
            // trills alternate between two pitches.
            if (pos->hasProperty(Position::TremoloPicking) || note.hasTrill())
            {
                const int trem_pick_duration =
                    getGraceNoteTicks(myTicksPerBeat, current_tempo);
                const int num_notes = duration / trem_pick_duration;

                // Find the other pitch to alternate with (this is just the same
                // pitch for tremolo picking).
                int other_pitch = pitch;
                if (note.hasTrill())
                {
                    other_pitch =
                        pitch + (note.getTrilledFret() - note.getFretNumber());
                }

                for (int i = 0; i < num_notes; ++i)
                {
                    const int tick = current_tick + i * trem_pick_duration;

                    for (const ActivePlayer &player : active_players)
                    {
                        tracks[player.getPlayerNumber()].append(
                            MidiEvent::noteOff(tick,
                                               getChannel(player), pitch,
                                               system_location));
                    }

                    // Alternate to the other pitch (this has no effect for
                    // tremolo picking).
                    std::swap(pitch, other_pitch);

                    for (const ActivePlayer &player : active_players)
                    {
                        tracks[player.getPlayerNumber()].append(
                            MidiEvent::noteOn(tick, getChannel(player), pitch,
                                              velocity, system_location));
                    }
                }
            }

            bool tied_to_next_note = false;
            // Check if this note is tied to the next note.
            {
                const Note *next = VoiceUtils::getNextNote(
                    voice, position, note.getString(), next_voice);

                if (next && next->hasProperty(Note::Tied))
                    tied_to_next_note = true;
            }

            // End the note, unless we are tied to the next note.
            if (!tied_to_next_note)
            {
                // Shorten the note duration for certain effects.
                boost::rational<int> factor(1);
                if (pos->hasProperty(Position::Staccato))
                    factor /= 2;
                else if (pos->hasProperty(Position::PalmMuting))
                    factor /= boost::rational<int>(23, 20);
                else if (note.hasProperty(Note::Muted))
                    factor /= 8;

                int note_length = boost::rational_cast<int>(duration * factor);

                for (const ActivePlayer &player : active_players)
                {
                    tracks[player.getPlayerNumber()].append(MidiEvent::noteOff(
                        current_tick + note_length, getChannel(player), pitch,
                        system_location));
                }
            }
        }

        current_tick += duration;
    }

    return current_tick;
}
