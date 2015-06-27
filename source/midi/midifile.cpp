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

#include <boost/rational.hpp>
#include <QSettings>

#include <app/settings.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/scorelocation.h>
#include <score/systemlocation.h>
#include <score/utils.h>
#include <score/voiceutils.h>

static const int PERCUSSION_CHANNEL = 9;
static const int METRONOME_CHANNEL = PERCUSSION_CHANNEL;
static const int DEFAULT_PPQ = 480;

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

MidiFile::MidiFile() : myTicksPerBeat(0)
{
}

void MidiFile::load(const Score &score)
{
    myTicksPerBeat = DEFAULT_PPQ;

    MidiEventList master_track;
    MidiEventList metronome_track;

    // Set the initial channel volume.
    std::vector<MidiEventList> regular_tracks(score.getPlayers().size());
    for (int i = 0; i < score.getPlayers().size(); ++i)
    {
        regular_tracks[i].append(
            MidiEvent::volumeChange(0, getChannel(i), Dynamic::fff));
    }

    SystemLocation location(0, 0);
    int current_tick = 0;
    while (location.getSystem() < score.getSystems().size())
    {
        const System &system = score.getSystems()[location.getSystem()];
        const Barline *current_bar = ScoreUtils::findByPosition(
            system.getBarlines(), location.getPosition());
        const Barline *next_bar = system.getNextBarline(location.getPosition());

        const int start_tick = current_tick;

        addTempoEvent(master_track, start_tick, system,
                      current_bar->getPosition(), next_bar->getPosition());

        for (int staff_index = 0; staff_index < system.getStaves().size();
             ++staff_index)
        {
            const Staff &staff = system.getStaves()[staff_index];

            for (int voice_index = 0; voice_index < staff.getVoices().size();
                 ++voice_index)
            {
                const int end_tick = addEventsForBar(
                    regular_tracks, start_tick, score, system,
                    location.getSystem(), staff, staff_index,
                    staff.getVoices()[voice_index], voice_index,
                    current_bar->getPosition(), next_bar->getPosition());

                current_tick = std::max(current_tick, end_tick);
            }
        }

        // Generate metronome events.
        current_tick = std::max(
            current_tick, generateMetronome(metronome_track, start_tick, system,
                                            *current_bar, *next_bar, location));

        // Move to the next bar.
        // TODO - handle repeats.
        if (next_bar == &system.getBarlines().back())
        {
            location.setSystem(location.getSystem() + 1);
            location.setPosition(0);
        }
        else
            location.setPosition(next_bar->getPosition());
    }

    myTracks.push_back(master_track);
    myTracks.insert(myTracks.end(), regular_tracks.begin(), regular_tracks.end());
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
                                const SystemLocation &location)
{
    QSettings settings;
    const uint8_t strong_vel =
        settings.value(Settings::MIDI_METRONOME_STRONG_ACCENT,
                       Settings::MIDI_METRONOME_STRONG_ACCENT_DEFAULT).toUInt();
    const uint8_t weak_vel =
        settings.value(Settings::MIDI_METRONOME_WEAK_ACCENT,
                       Settings::MIDI_METRONOME_WEAK_ACCENT_DEFAULT).toUInt();
    const uint8_t preset =
        Midi::MIDI_PERCUSSION_PRESET_OFFSET +
        settings.value(Settings::MIDI_METRONOME_PRESET,
                       Settings::MIDI_METRONOME_PRESET_DEFAULT).toUInt();

    const TimeSignature &time_sig = current_bar.getTimeSignature();

    const int num_pulses = time_sig.getNumPulses();
    const int beats_per_measure = time_sig.getBeatsPerMeasure();
    const int beat_value = time_sig.getBeatValue();
    const int position = current_bar.getPosition();

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
            const uint8_t velocity = (i == 0) ? strong_vel : weak_vel;

            event_list.append(MidiEvent::noteOn(current_tick, METRONOME_CHANNEL,
                                                preset, velocity, location));

            current_tick += duration;

            event_list.append(MidiEvent::noteOff(
                current_tick, METRONOME_CHANNEL, preset, location));
        }
    }

    return current_tick;
}

void MidiFile::addTempoEvent(MidiEventList &event_list, int current_tick,
                             const System &system, int bar_start, int bar_end)
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
        int tick_duration = boost::rational_cast<int>(
            60000000 / (scale * marker.getBeatsPerMinute()));

        event_list.append(MidiEvent::setTempo(current_tick, tick_duration));
    }
}

static int getWholeRestDuration(const System &system, int system_index,
                                const Voice &voice, const Position &pos,
                                int bar_start, int bar_end,
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

int MidiFile::addEventsForBar(std::vector<MidiEventList> &tracks,
                              int current_tick, const Score &score,
                              const System &system, int system_index,
                              const Staff &staff, int staff_index,
                              const Voice &voice, int voice_index,
                              int bar_start, int bar_end)
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
                duration =
                    getWholeRestDuration(system, system_index, voice, *pos,
                                         bar_start, bar_end, duration);

                // Extend for multi-bar rests.
                if (pos->hasMultiBarRest())
                    duration *= pos->getMultiBarRestCount();
            }

            current_tick += duration;
            continue;
        }

#if 0
        // Handle grace notes.
        if (pos.hasProperty(Position::Acciaccatura))
        {
            duration = GRACE_NOTE_DURATION;
            startTime -= duration;
        }
#endif

        // If there aren't any active players, treat as a rest.
        if (active_players.empty())
        {
            current_tick += duration;
            continue;
        }

#if 0
        // Vibrato events (these apply to all notes in the position).
        if (pos.hasProperty(Position::Vibrato) ||
            pos.hasProperty(Position::WideVibrato))
        {
            VibratoEvent::VibratoType type = pos.hasProperty(Position::Vibrato)
                    ? VibratoEvent::NormalVibrato : VibratoEvent::WideVibrato;

            for (const ActivePlayer &player : active_players)
            {
                const int channel = getChannel(player);

                // Add vibrato event, and an event to turn off the vibrato after
                // the note is done.
                eventList.emplace_back(
                    new VibratoEvent(channel, startTime, position, system_index,
                                     VibratoEvent::VibratoOn, type));

                eventList.emplace_back(
                    new VibratoEvent(channel, startTime + duration, position,
                                     system_index, VibratoEvent::VibratoOff));
            }
        }

        // Let ring events (applied to all notes in the position).
        if (pos.hasProperty(Position::LetRing) && !letRingActive)
        {
            for (const ActivePlayer &player : active_players)
            {
                eventList.emplace_back(
                    new LetRingEvent(getChannel(player), startTime, position,
                                     system_index, LetRingEvent::LetRingOn));
            }

            letRingActive = true;
        }
        else if (!pos.hasProperty(Position::LetRing) && letRingActive)
        {
            for (const ActivePlayer &player : active_players)
            {
                eventList.emplace_back(
                    new LetRingEvent(getChannel(player), startTime, position,
                                     system_index, LetRingEvent::LetRingOff));
            }

            letRingActive = false;
        }
        // Make sure that we end the let ring after the last position in the bar.
        else if (letRingActive &&
                 (&pos == &ScoreUtils::findInRange(voice.getPositions(),
                                                   bar_start, bar_end).back()))
        {
            for (const ActivePlayer &player : active_players)
            {
                eventList.emplace_back(new LetRingEvent(
                    getChannel(player), startTime + duration, position,
                    system_index, LetRingEvent::LetRingOff));
            }

            letRingActive = false;
        }
#endif

        for (const Note &note : pos->getNotes())
        {
#if 0
            // For arpeggios, delay the start of each note a small amount from
            // the last, and also adjust the duration correspondingly.
            if (pos.hasProperty(Position::ArpeggioDown) ||
                pos.hasProperty(Position::ArpeggioUp))
            {
                current_tick += ARPEGGIO_OFFSET;
                duration -= ARPEGGIO_OFFSET;
            }
#endif

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
#if 0
            {
                std::vector<BendEventInfo> bendEvents;

                if (note.hasProperty(Note::SlideIntoFromAbove) ||
                    note.hasProperty(Note::SlideIntoFromBelow) ||
                    note.hasProperty(Note::ShiftSlide) ||
                    note.hasProperty(Note::LegatoSlide) ||
                    note.hasProperty(Note::SlideOutOfDownwards) ||
                    note.hasProperty(Note::SlideOutOfUpwards))
                {
                    generateSlides(bendEvents, startTime, duration,
                                   currentTempo, note,
                                   VoiceUtils::getNextNote(voice, position,
                                                           note.getString()));
                }

                if (note.hasBend())
                {
                    generateBends(bendEvents, activePitchBend, startTime,
                                  duration, currentTempo, note);
                }

#if 0
                // only generate tremolo bar events once, since they apply to all notes in
                // the position
                if (position->HasTremoloBar() && j == 0)
                {
                    generateTremoloBar(bendEvents, startTime, duration, currentTempo, position);
                }
#endif

                for (const BendEventInfo &event : bendEvents)
                {
                    for (const active_player &player : active_players)
                    {
                        eventList.emplace_back(new BendEvent(
                            getChannel(player), event.timestamp, position,
                            system_index, event.pitchBendAmount));
                    }
                }
            }
#endif

#if 0
            // Perform tremolo picking or trills - they work identically, except
            // trills alternate between two pitches.
            if (pos.hasProperty(Position::TremoloPicking) || note.hasTrill())
            {
                const double tremPickNoteDuration = GRACE_NOTE_DURATION;
                const int numNotes = duration / tremPickNoteDuration;

                // Find the other pitch to alternate with (this is just the same
                // pitch for tremolo picking).
                int otherPitch = pitch;
                if (note.hasTrill())
                {
                    otherPitch = pitch + (note.getTrilledFret() -
                                          note.getFretNumber());
                }

                for (int i = 0; i < numNotes; ++i)
                {
                    const double currentStartTime = startTime +
                            i * tremPickNoteDuration;

                    for (const active_player &player : active_players)
                    {
                        eventList.emplace_back(new StopNoteEvent(
                            getChannel(player), currentStartTime, position,
                            system_index, pitch));
                    }

                    // Alternate to the other pitch (this has no effect for
                    // tremolo picking).
                    std::swap(pitch, otherPitch);

                    for (const ActivePlayer &active_player : active_players)
                    {
                        const Player &player = myScore.getPlayers()[
                                active_player.getPlayerNumber()];
                        const Instrument &instrument = myScore.getInstruments()[
                                active_player.getInstrumentNumber()];

                        eventList.emplace_back(new PlayNoteEvent(
                            getChannel(active_player), currentStartTime,
                            tremPickNoteDuration, pitch, position, system_index,
                            player, instrument, velocity));
                    }
                }
            }
#endif

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
