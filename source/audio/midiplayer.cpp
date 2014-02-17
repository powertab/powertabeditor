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
  
#include "midiplayer.h"

#include <app/settings.h>
#include <audio/bendevent.h>
#include <audio/letringevent.h>
#include <audio/metronomeevent.h>
#include <audio/midievent.h>
#include <audio/midioutputdevice.h>
#include <audio/playnoteevent.h>
#include <audio/repeatcontroller.h>
#include <audio/restevent.h>
#include <audio/stopnoteevent.h>
#include <audio/vibratoevent.h>
#include <audio/volumechangeevent.h>
#include <boost/math/special_functions/round.hpp>
#include <QDebug>
#include <QSettings>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils.h>
#include <score/voiceutils.h>

// Channel 10 is used for percussion in General MIDI.
static const int PERCUSSION_CHANNEL = 9;
static const int METRONOME_CHANNEL = PERCUSSION_CHANNEL;

/// For grace notes, use the duration of 32nd note at 120bpm, which is fairly
/// fast.
static const double GRACE_NOTE_DURATION = 62.5;

static const double ARPEGGIO_OFFSET = 30.0;

/// A MIDI event that does nothing, but is useful for triggering a position
/// change.
class DummyEvent : public MidiEvent
{
public:
    DummyEvent(int channel, double startTime, double duration,
               int position, int system)
        : MidiEvent(channel, startTime, duration, position, system)
    {
    }

    virtual void performEvent(MidiOutputDevice &) const override
    {
    }
};

MidiPlayer::MidiPlayer(const Score &score, int startSystem, int startPosition,
                       int speed)
    : myScore(score),
      myStartSystem(startSystem),
      myStartPosition(startPosition),
      myIsPlaying(false),
      myPlaybackSpeed(speed)
{
}

MidiPlayer::~MidiPlayer()
{
    setIsPlaying(false);
    wait();
}

void MidiPlayer::run()
{
    setIsPlaying(true);

    EventList eventList;
    generateEvents(eventList);

    // Sort the events by their start time, and play them in order.
    std::stable_sort(
        eventList.begin(), eventList.end(),
        [](const std::unique_ptr<MidiEvent> & e1,
           const std::unique_ptr<MidiEvent> & e2) { return *e1 < *e2; });
    playMidiEvents(eventList);
}

void MidiPlayer::setIsPlaying(bool set)
{
    myIsPlaying = set;
}

bool MidiPlayer::isPlaying() const
{
    return myIsPlaying;
}

void MidiPlayer::generateEvents(EventList &eventList)
{
    double time = 0;

    int systemIndex = 0;
    for (const System &system : myScore.getSystems())
    {
        std::vector<uint8_t> activePitchBends(system.getStaves().size(),
                                              BendEvent::DEFAULT_BEND);

        for (const Barline &leftBar : system.getBarlines())
        {
            const Barline *rightBar = system.getNextBarline(
                        leftBar.getPosition());
            if (!rightBar)
                break;

            const double barStartTime = time;

            int staffIndex = 0;
            for (const Staff &staff : system.getStaves())
            {
                for (const Voice &voice : staff.getVoices())
                {
                    const double endTime = generateEventsForBar(
                        system, systemIndex, staff, staffIndex, voice,
                        leftBar.getPosition(), rightBar->getPosition(),
                        barStartTime, eventList, activePitchBends[staffIndex]);

                    // Force playback to be synchronized at each bar in case
                    // some staves have too many or too few notes.
                    time = std::max(time, endTime);
                }

                ++staffIndex;
            }

            // Don't add in the metronome for empty bars, but add an empty event
            // for the left bar so that repeats, etc. are triggered.
            if (time == barStartTime)
            {
                eventList.emplace_back(new DummyEvent(METRONOME_CHANNEL, time,
                                                      0, leftBar.getPosition(),
                                                      systemIndex));
                continue;
            }

            // Add metronome ticks for the bar.
            time = generateMetronome(system, systemIndex, leftBar, barStartTime,
                                     time, eventList);
        }

        // Add event at the end bar's position in order to trigger any
        // repeats or alternate endings. We don't need this for any other bars
        // since there are metronome events at the other bars.
        eventList.emplace_back(new DummyEvent(
            METRONOME_CHANNEL, time, 0,
            system.getBarlines().back().getPosition(), systemIndex));

        ++systemIndex;
    }
}

/// Returns the appropriate note velocity type for the given position/note.
static PlayNoteEvent::VelocityType getNoteVelocity(const Position &pos,
                                                   const Note &note)
{
    if (note.hasProperty(Note::GhostNote))
        return PlayNoteEvent::GhostVelocity;
    else if (note.hasProperty(Note::Muted))
        return PlayNoteEvent::MutedVelocity;
    else if (pos.hasProperty(Position::PalmMuting))
        return PlayNoteEvent::PalmMutedVelocity;
    else
        return PlayNoteEvent::DefaultVelocity;
}

/// Returns the MIDI channel that should be used for the player.
/// Since channel 10 is reserved for percussion, we can't use that
/// channel for regular instruments.
static int getChannel(const ActivePlayer &player)
{
    int channel = player.getPlayerNumber();
    if (channel >= PERCUSSION_CHANNEL)
        channel++;
    return channel;
}

double MidiPlayer::generateEventsForBar(const System &system, int systemIndex,
                                        const Staff &staff, int staffIndex,
                                        const Voice &voice, int leftPos,
                                        int rightPos, const double barStartTime,
                                        EventList &eventList,
                                        uint8_t &activePitchBend)
{
    double startTime = barStartTime;
    bool letRingActive = false;

    for (const Position &pos :
         ScoreUtils::findInRange(voice.getPositions(), leftPos, rightPos))
    {
        const int position = pos.getPosition();
        const double currentTempo = getCurrentTempo(systemIndex, position);

        // Each note at a position has the same duration.
        double duration = calculateNoteDuration(systemIndex, voice, pos);

        const PlayerChange *currentPlayers = ScoreUtils::getCurrentPlayers(
                    myScore, systemIndex, pos.getPosition());

        std::vector<ActivePlayer> activePlayers;
        if (currentPlayers)
            activePlayers = currentPlayers->getActivePlayers(staffIndex);

        if (pos.isRest())
        {
            // For whole rests, they must last for the entire bar, regardless
            // of time signature.
            if (pos.getDurationType() == Position::WholeNote)
            {
                duration = getWholeRestDuration(system, systemIndex, voice, pos,
                                                duration);

                // Extend for multi-bar rests.
                if (pos.hasMultiBarRest())
                    duration *= pos.getMultiBarRestCount();
            }

            for (const ActivePlayer &player : activePlayers)
            {
                eventList.emplace_back(new RestEvent(getChannel(player),
                                                     startTime, duration,
                                                     position, systemIndex));
            }

            startTime += duration;
            continue;
        }

        // Handle grace notes.
        if (pos.hasProperty(Position::Acciaccatura))
        {
            duration = GRACE_NOTE_DURATION;
            startTime -= duration;
        }

        // If there aren't any active players, treat as a rest.
        if (activePlayers.empty())
        {
            startTime += duration;
            continue;
        }

#if 0
        // If the position has an arpeggio, sort the notes by string in the specified direction.
        // This is so the notes can be played in the correct order, with a slight delay between each
        if (position->HasArpeggioDown())
        {
            position->SortNotesDown();
        }
        else if (position->HasArpeggioUp())
        {
            position->SortNotesUp();
        }
#endif

        // Vibrato events (these apply to all notes in the position).
        if (pos.hasProperty(Position::Vibrato) ||
            pos.hasProperty(Position::WideVibrato))
        {
            VibratoEvent::VibratoType type = pos.hasProperty(Position::Vibrato)
                    ? VibratoEvent::NormalVibrato : VibratoEvent::WideVibrato;

            for (const ActivePlayer &player : activePlayers)
            {
                const int channel = getChannel(player);

                // Add vibrato event, and an event to turn off the vibrato after
                // the note is done.
                eventList.emplace_back(
                    new VibratoEvent(channel, startTime, position, systemIndex,
                                     VibratoEvent::VibratoOn, type));

                eventList.emplace_back(
                    new VibratoEvent(channel, startTime + duration, position,
                                     systemIndex, VibratoEvent::VibratoOff));
            }
        }

        // Handle dynamics.
        {
            const Dynamic *dynamic = ScoreUtils::findByPosition(
                        staff.getDynamics(), position);
            if (dynamic)
            {
                for (const ActivePlayer &player : activePlayers)
                {
                    eventList.emplace_back(new VolumeChangeEvent(
                        getChannel(player), startTime, position, systemIndex,
                        dynamic->getVolume()));
                }
            }
        }

        // Let ring events (applied to all notes in the position).
        if (pos.hasProperty(Position::LetRing) && !letRingActive)
        {
            for (const ActivePlayer &player : activePlayers)
            {
                eventList.emplace_back(
                    new LetRingEvent(getChannel(player), startTime, position,
                                     systemIndex, LetRingEvent::LetRingOn));
            }

            letRingActive = true;
        }
        else if (!pos.hasProperty(Position::LetRing) && letRingActive)
        {
            for (const ActivePlayer &player : activePlayers)
            {
                eventList.emplace_back(
                    new LetRingEvent(getChannel(player), startTime, position,
                                     systemIndex, LetRingEvent::LetRingOff));
            }

            letRingActive = false;
        }
        // Make sure that we end the let ring after the last position in the bar.
        else if (letRingActive &&
                 (&pos == &ScoreUtils::findInRange(voice.getPositions(),
                                                   leftPos, rightPos).back()))
        {
            for (const ActivePlayer &player : activePlayers)
            {
                eventList.emplace_back(new LetRingEvent(
                    getChannel(player), startTime + duration, position,
                    systemIndex, LetRingEvent::LetRingOff));
            }

            letRingActive = false;
        }

        for (const Note &note : pos.getNotes())
        {
            // For arpeggios, delay the start of each note a small amount from
            // the last, and also adjust the duration correspondingly.
            if (pos.hasProperty(Position::ArpeggioDown) ||
                pos.hasProperty(Position::ArpeggioUp))
            {
                startTime += ARPEGGIO_OFFSET;
                duration -= ARPEGGIO_OFFSET;
            }

            // Pick a tuning from one of the active players.
            // TODO - should we handle cases where different tunings are used
            // by players in the same staff?
            const int playerIndex = activePlayers.front().getPlayerNumber();
            const Tuning &tuning = myScore.getPlayers()[playerIndex].getTuning();
            int pitch = getActualNotePitch(note, tuning);

            const PlayNoteEvent::VelocityType velocity = getNoteVelocity(pos, note);

            // If this note is not tied to the previous note, play the note.
            if (!note.hasProperty(Note::Tied))
            {
                for (const ActivePlayer &activePlayer : activePlayers)
                {
                    const Player &player = myScore.getPlayers()[
                            activePlayer.getPlayerNumber()];
                    const Instrument &instrument = myScore.getInstruments()[
                            activePlayer.getInstrumentNumber()];

                    eventList.emplace_back(new PlayNoteEvent(
                        getChannel(activePlayer), startTime, duration, pitch,
                        position, systemIndex, player, instrument,
                        note.hasProperty(Note::Muted), velocity));
                }
            }
            // If the note is tied, make sure that the pitch is the same as the
            // previous note, so that the Stop Note event works correctly with
            // harmonics. There may be multiple notes tied together, though, so
            // we need to find the first note in the sequence.
            else
            {
                const Note *prevNote = &note;
                const Position *prevPos = &pos;

                while (prevNote && prevNote->hasProperty(Note::Tied))
                {
                    prevPos = VoiceUtils::getPreviousPosition(
                        voice, prevPos->getPosition());
                    if (!prevPos)
                        break;

                    prevNote = Utils::findByString(*prevPos, note.getString());
                }

                if (prevNote)
                    pitch = getActualNotePitch(*prevNote, tuning);
            }

            // Generate all events that involve pitch bends.
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
                    for (const ActivePlayer &player : activePlayers)
                    {
                        eventList.emplace_back(new BendEvent(
                            getChannel(player), event.timestamp, position,
                            systemIndex, event.pitchBendAmount));
                    }
                }
            }
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

                    for (const ActivePlayer &player : activePlayers)
                    {
                        eventList.emplace_back(new StopNoteEvent(
                            getChannel(player), currentStartTime, position,
                            systemIndex, pitch));
                    }

                    // Alternate to the other pitch (this has no effect for
                    // tremolo picking).
                    std::swap(pitch, otherPitch);

                    for (const ActivePlayer &activePlayer : activePlayers)
                    {
                        const Player &player = myScore.getPlayers()[
                                activePlayer.getPlayerNumber()];
                        const Instrument &instrument = myScore.getInstruments()[
                                activePlayer.getInstrumentNumber()];

                        eventList.emplace_back(new PlayNoteEvent(
                            getChannel(activePlayer), currentStartTime,
                            tremPickNoteDuration, pitch, position, systemIndex,
                            player, instrument, note.hasProperty(Note::Muted),
                            velocity));
                    }
                }
            }

            bool tiedToNextNote = false;
            // Check if this note is tied to the next note.
            {
                const Note *next =
                    VoiceUtils::getNextNote(voice, position, note.getString());

                if (next && next->hasProperty(Note::Tied))
                    tiedToNextNote = true;
            }

            // End the note, unless we are tied to the next note.
            if (!tiedToNextNote)
            {
                double noteLength = duration;

                // Shorten the note duration for certain effects.
                if (pos.hasProperty(Position::Staccato))
                    noteLength /= 2.0;
                else if (pos.hasProperty(Position::PalmMuting))
                    noteLength /= 1.15;

                for (const ActivePlayer &player : activePlayers)
                {
                    eventList.emplace_back(new StopNoteEvent(
                        getChannel(player), startTime + noteLength, position,
                        systemIndex, pitch));
                }
            }
        }

        startTime += duration;
    }

    return startTime;
}

void MidiPlayer::performCountIn(MidiOutputDevice &device,
                                const SystemLocation &location)
{
    QSettings settings;

    const System &system = myScore.getSystems()[location.getSystem()];
    const Barline *barline = system.getPreviousBarline(location.getPosition());
    // Use the start bar if necessary.
    if (!barline)
        barline = &system.getBarlines().front();

    // Figure out the time signature and tempo where the playback is starting.
    const TimeSignature &timeSig = barline->getTimeSignature();
    const uint8_t numPulses = timeSig.getNumPulses();
    const uint8_t beatsPerMeasure = timeSig.getBeatsPerMeasure();
    const uint8_t beatValue = timeSig.getBeatValue();

    // Figure out the duration of a pulse.
    const double tempo = getCurrentTempo(location.getSystem(), location.getPosition());
    const double duration = (tempo * 4.0 / beatValue) * beatsPerMeasure / numPulses;

    const uint8_t velocity =
        settings.value(Settings::MIDI_METRONOME_COUNTIN_VOLUME,
                       Settings::MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT).toUInt();
    const uint8_t preset =
        Midi::MIDI_PERCUSSION_PRESET_OFFSET +
        settings.value(Settings::MIDI_METRONOME_COUNTIN_PRESET,
                       Settings::MIDI_METRONOME_COUNTIN_PRESET_DEFAULT).toUInt();

    const double speedShiftFactor = 100.0 / myPlaybackSpeed;

    device.setChannelMaxVolume(METRONOME_CHANNEL, Midi::MAX_MIDI_CHANNEL_VOLUME);
    // Play the count-in.
    for (uint8_t i = 0; i < numPulses; ++i)
    {
        if (!isPlaying())
            break;

        device.playNote(METRONOME_CHANNEL, preset, velocity);
        usleep(1000 * duration * speedShiftFactor);
        device.stopNote(METRONOME_CHANNEL, preset);
    }
}

void MidiPlayer::playMidiEvents(const EventList &eventList)
{
    boost::optional<SystemLocation> startLocation =
        SystemLocation(myStartSystem, myStartPosition);

    MidiOutputDevice device;
    // Set the port for RtMidi.
    QSettings settings;
    device.initialize(
                settings.value(Settings::MIDI_PREFERRED_API,
                               Settings::MIDI_PREFERRED_API_DEFAULT).toInt(),
                settings.value(Settings::MIDI_PREFERRED_PORT,
                               Settings::MIDI_PREFERRED_PORT_DEFAULT).toInt());

    // Set pitch bend settings for each channel to one octave.
    for (int i = 0; i < Midi::NUM_MIDI_CHANNELS_PER_PORT; ++i)
        device.setPitchBendRange(i, BendEvent::PITCH_BEND_RANGE);

    if (settings.value(Settings::MIDI_METRONOME_ENABLE_COUNTIN,
                       Settings::MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT).toBool())
    {
        performCountIn(device, *startLocation);
    }

    RepeatController repeatController(myScore);

    SystemLocation currentLocation;
    SystemLocation prevLocation;

    typedef EventList::const_iterator MidiEventIterator;
    MidiEventIterator activeEvent = eventList.begin();

    while (activeEvent != eventList.end())
    {
        if (!isPlaying())
            return;

        const SystemLocation eventLocation((*activeEvent)->getSystem(),
                                           (*activeEvent)->getPosition());

#if defined(LOG_MIDI_EVENTS)
        qDebug() << "Playback location: " << eventLocation.getSystem() << ", "
                 << eventLocation.getPosition();
#endif

        if (startLocation)
        {
            // If we haven't reached the starting position yet, keep going.
            if (eventLocation < *startLocation)
            {
                ++activeEvent;
                continue;
            }
            // If we just reached the starting position, update the system index
            // explicitly to avoid the "currentPosition = 0" effect of a normal
            // system change.
            else
            {
                emit playbackSystemChanged(startLocation->getSystem());
                currentLocation.setSystem(startLocation->getSystem());
                prevLocation = currentLocation;
                startLocation.reset();
            }
        }

        // If we've moved to a new position, move the caret.
        if (eventLocation.getPosition() > currentLocation.getPosition())
        {
            prevLocation = currentLocation;
            currentLocation.setPosition(eventLocation.getPosition());
            emit playbackPositionChanged(currentLocation.getPosition());
        }

        // Moving on to a new system, so we need to reset the position to 0 to
        // ensure playback begins at the start of the staff.
        if (eventLocation.getSystem() != currentLocation.getSystem())
        {
            currentLocation.setSystem(eventLocation.getSystem());
            currentLocation.setPosition(0);
            prevLocation = currentLocation;
            emit playbackSystemChanged(currentLocation.getSystem());
        }

        SystemLocation newLocation;
        if (repeatController.checkForRepeat(prevLocation, currentLocation,
                                            newLocation))
        {
#ifdef LOG_MIDI_EVENTS
            qDebug() << "Moving to: " << newLocation.getSystem()
                     << ", " << newLocation.getPosition();
            qDebug() << "From position: " << currentLocation.getSystem()
                     << ", " << currentLocation.getPosition()
                     << " at " << activeEvent->getStartTime();
#endif
            startLocation = newLocation;
            currentLocation = prevLocation = SystemLocation(0, 0);
            emit playbackSystemChanged(startLocation->getSystem());
            emit playbackPositionChanged(startLocation->getPosition());
            activeEvent = eventList.begin();
            continue;
        }

        (*activeEvent)->performEvent(device);

        // Add delay between this event and the next one.
        MidiEventIterator nextEvent = boost::next(activeEvent);
        if (nextEvent != eventList.end())
        {
            const int sleepDuration = abs((*nextEvent)->getStartTime() -
                                          (*activeEvent)->getStartTime());

            // Slow down or speed up playback.
            const double speedShiftFactor = 100.0 / myPlaybackSpeed;

            if (sleepDuration)
            {
                usleep(1000 * sleepDuration * speedShiftFactor);
            }
        }
        else // last note
        {
            usleep(1000 * (*activeEvent)->getDuration());
        }

        ++activeEvent;
    }
}

double MidiPlayer::getCurrentTempo(int system, int position) const
{
    const TempoMarker *marker = getCurrentTempoMarker(system, position);

    // Default tempo in case there is no tempo marker in the score.
    double bpm = TempoMarker::DEFAULT_BEATS_PER_MINUTE;
    TempoMarker::BeatType beatType = TempoMarker::Quarter;

    if (marker)
    {
        bpm = marker->getBeatsPerMinute();
        Q_ASSERT(bpm != 0);

        beatType = marker->getBeatType();
    }

    // Convert the values in the TempoMarker::BeatType enum to a factor that
    // will scale the bpm value to be in terms of quarter notes.
    double factor = 2.0;
    if (beatType % 2 == 0)
    {
        factor /= std::max(1.0, static_cast<double>(beatType));
    }
    else
    {
        factor /= std::max(1.0, static_cast<double>(beatType - 1));
        factor *= 1.5;
    }

    Q_ASSERT(factor > 0);

    // Convert bpm to millisecond duration.
    return (60.0 / (bpm * factor) * 1000.0);
}

const TempoMarker *MidiPlayer::getCurrentTempoMarker(int systemIndex,
                                                     int position) const
{
    const TempoMarker *lastMarker = nullptr;

    int i = 0;
    for (const System &system : myScore.getSystems())
    {
        if (i > systemIndex)
            break;

        for (const TempoMarker &marker : system.getTempoMarkers())
        {
            if (i < systemIndex ||
               (i == systemIndex && marker.getPosition() <= position))
            {
                lastMarker = &marker;
            }
        }

        ++i;
    }

    return lastMarker;
}

double MidiPlayer::calculateNoteDuration(int system, const Voice &voice,
                                         const Position &pos) const
{
    const double tempo = getCurrentTempo(system, pos.getPosition());
    return VoiceUtils::getDurationTime(voice, pos) * tempo;
}

double MidiPlayer::getWholeRestDuration(const System &system, int systemIndex,
                                        const Voice &voice, const Position &pos,
                                        double originalDuration) const
{
    const Barline *prevBar = system.getPreviousBarline(pos.getPosition());
    // Use the start bar if necessary.
    if (!prevBar)
        prevBar = &system.getBarlines().front();

    const Barline *nextBar = system.getNextBarline(pos.getPosition());
    Q_ASSERT(nextBar);

    // If the whole rest is not the only item in the bar, treat it like a
    // regular rest.
    for (int i = prevBar->getPosition(); i < nextBar->getPosition(); ++i)
    {
        const Position *otherPos =
            ScoreUtils::findByPosition(voice.getPositions(), i);

        if (otherPos && otherPos != &pos)
            return originalDuration;
    }

    // Otherwise, extend the rest for the entire bar.
    const TimeSignature& currentTimeSignature = prevBar->getTimeSignature();

    const double tempo = getCurrentTempo(systemIndex, pos.getPosition());
    double beatDuration = currentTimeSignature.getBeatValue();
    double duration = tempo * 4.0 / beatDuration;
    int numBeats = currentTimeSignature.getBeatsPerMeasure();
    duration *= numBeats;

    return duration;
}

double MidiPlayer::generateMetronome(const System &system, int systemIndex,
                                     const Barline &barline, double startTime,
                                     const double notesEndTime,
                                     EventList &eventList) const
{
    const TimeSignature& timeSig = barline.getTimeSignature();

    uint8_t numPulses = timeSig.getNumPulses();
    Q_ASSERT(timeSig.isValidNumPulses(numPulses));

    const uint8_t beatsPerMeasure = timeSig.getBeatsPerMeasure();
    const uint8_t beatValue = timeSig.getBeatValue();
    const int position = barline.getPosition();

    // Figure out duration of pulse.
    const double tempo = getCurrentTempo(systemIndex, position);
    const double duration = (tempo * 4.0 / beatValue) * beatsPerMeasure / numPulses;

    // Check for multi-bar rests, as we need to generate more metronome events
    // to fill the extra bars.
    int repeatCount = 1;
    const Barline *nextBar = system.getNextBarline(barline.getPosition());
    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &pos : ScoreUtils::findInRange(
                     voice.getPositions(), barline.getPosition(),
                     nextBar->getPosition()))
            {
                if (pos.hasMultiBarRest())
                {
                    repeatCount = std::max(repeatCount,
                                           pos.getMultiBarRestCount());
                }
            }
        }
    }

    // If there are too many notes in the bar, add some more metronome pulses.
    // If there are too few notes, we don't remove any metronome pulses.
    if (repeatCount == 1)
    {
        numPulses = std::max<uint8_t>(numPulses,
                                      (notesEndTime - startTime) / duration);
    }

    for (int repeat = 0; repeat < repeatCount; ++repeat)
    {
        for (uint8_t i = 0; i < numPulses; ++i)
        {
            MetronomeEvent::VelocityType velocity = (i == 0) ?
                        MetronomeEvent::StrongAccent :
                        MetronomeEvent::WeakAccent;

            eventList.emplace_back(
                new MetronomeEvent(METRONOME_CHANNEL, startTime, duration,
                                   position, systemIndex, velocity));
            startTime += duration;
            eventList.emplace_back(new StopNoteEvent(
                METRONOME_CHANNEL, startTime, position, systemIndex,
                MetronomeEvent::getMetronomePreset()));
        }
    }

    startTime = std::max(startTime, notesEndTime);
    return startTime;
}

int MidiPlayer::getActualNotePitch(const Note &note, const Tuning &tuning) const
{
    const int openStringPitch = tuning.getNote(note.getString(), false) +
            tuning.getCapo();
    int pitch = openStringPitch + note.getFretNumber();
    
    if (note.hasProperty(Note::NaturalHarmonic))
        pitch = openStringPitch + Harmonics::getPitchOffset(note.getFretNumber());
    
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

void MidiPlayer::generateBends(std::vector<BendEventInfo> &bends,
                               uint8_t &activePitchBend, double startTime,
                               double duration, double currentTempo,
                               const Note &note)
{
    const Bend &bend = note.getBend();

    const uint8_t bendAmount =
        boost::math::round(BendEvent::DEFAULT_BEND +
                           bend.getBentPitch() * BendEvent::BEND_QUARTER_TONE);
    const uint8_t releaseAmount = boost::math::round(
        BendEvent::DEFAULT_BEND +
        bend.getReleasePitch() * BendEvent::BEND_QUARTER_TONE);

    switch (bend.getType())
    {
    case Bend::PreBend:
    case Bend::PreBendAndRelease:
    case Bend::PreBendAndHold:
        bends.push_back(BendEventInfo(startTime, bendAmount));
        break;

    case Bend::NormalBend:
    case Bend::BendAndHold:
        // Perform a normal (gradual) bend.
        if (bend.getDuration() == 0)
        {
            // Bend over a 32nd note.
            generateGradualBend(bends, startTime, currentTempo / 8.0,
                                BendEvent::DEFAULT_BEND, bendAmount);
        }
        else if (bend.getDuration() == 1)
        {
            // Bend over the current note duration.
            generateGradualBend(bends, startTime, duration,
                                BendEvent::DEFAULT_BEND, bendAmount);
        }
        // TODO - implement bends that stretch over multiple notes.
        break;

    case Bend::BendAndRelease:
        // Bend up to the bent pitch for half of the note duration.
        generateGradualBend(bends, startTime, duration / 2,
                            BendEvent::DEFAULT_BEND, bendAmount);
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
        bends.push_back(BendEventInfo(startTime + duration, releaseAmount));
        break;

    case Bend::PreBendAndRelease:
        generateGradualBend(bends, startTime, duration, bendAmount, releaseAmount);
        break;

    case Bend::BendAndRelease:
        generateGradualBend(bends, startTime + duration / 2, duration / 2,
                            bendAmount, releaseAmount);
        break;

    case Bend::GradualRelease:
        generateGradualBend(bends, startTime, duration, activePitchBend,
                            releaseAmount);
        break;
    default:
        break;
    }

    if (bend.getType() == Bend::BendAndHold ||
        bend.getType() == Bend::PreBendAndHold)
    {
        activePitchBend = bendAmount;
    }
    else
        activePitchBend = releaseAmount;
}

void MidiPlayer::generateGradualBend(std::vector<BendEventInfo> &bends,
                                     double startTime, double duration,
                                     int startBendAmount,
                                     int releaseBendAmount) const
{
    const int numBendEvents = abs(startBendAmount - releaseBendAmount);
    const double bendEventDuration = duration / numBendEvents;

    for (int i = 1; i <= numBendEvents; i++)
    {
        const double timestamp = startTime + bendEventDuration * i;
        if (startBendAmount < releaseBendAmount)
            bends.push_back(BendEventInfo(timestamp, startBendAmount + i));
        else
            bends.push_back(BendEventInfo(timestamp, startBendAmount - i));
    }
}

MidiPlayer::BendEventInfo::BendEventInfo(double timestamp,
                                         uint8_t pitchBendAmount)
    : timestamp(timestamp), pitchBendAmount(pitchBendAmount)
{
}

void MidiPlayer::changePlaybackSpeed(int newPlaybackSpeed)
{
    myPlaybackSpeed = newPlaybackSpeed;
}

/// Generates slides for the given note
void MidiPlayer::generateSlides(std::vector<BendEventInfo> &bends,
                                double startTime, double noteDuration,
                                double currentTempo, const Note &note,
                                const Note *nextNote)
{
    const int SLIDE_OUT_OF_STEPS = 5;

    const double SLIDE_BELOW_BEND =
        floor(BendEvent::DEFAULT_BEND -
              SLIDE_OUT_OF_STEPS * 2 * BendEvent::BEND_QUARTER_TONE);

    const double SLIDE_ABOVE_BEND =
        floor(BendEvent::DEFAULT_BEND +
              SLIDE_OUT_OF_STEPS * 2 * BendEvent::BEND_QUARTER_TONE);

    if (note.hasProperty(Note::ShiftSlide) ||
        note.hasProperty(Note::LegatoSlide) ||
        note.hasProperty(Note::SlideOutOfDownwards) ||
        note.hasProperty(Note::SlideOutOfUpwards))
    {
        uint8_t bendAmount = BendEvent::DEFAULT_BEND;

        if (note.hasProperty(Note::ShiftSlide) || note.hasProperty(Note::LegatoSlide))
        {
            if (nextNote)
            {
                bendAmount =
                    floor(BendEvent::DEFAULT_BEND +
                          (nextNote->getFretNumber() - note.getFretNumber()) *
                              2 * BendEvent::BEND_QUARTER_TONE);
            }
            else
            {
                // Treat as a slide out of downwards.
                bendAmount = SLIDE_BELOW_BEND;
            }
        }
        else if (note.hasProperty(Note::SlideOutOfDownwards))
            bendAmount = SLIDE_BELOW_BEND;
        else if (note.hasProperty(Note::SlideOutOfUpwards))
            bendAmount = SLIDE_ABOVE_BEND;

        // Start the slide in the last part of the note duration, to make it
        // somewhat more realistic-sounding.
        const double slideDuration = noteDuration / 3.0;
        generateGradualBend(bends, startTime + noteDuration - slideDuration,
                            slideDuration, BendEvent::DEFAULT_BEND, bendAmount);

        // Reset pitch wheel after note is played.
        bends.push_back(
            BendEventInfo(startTime + noteDuration, BendEvent::DEFAULT_BEND));
    }

    if (note.hasProperty(Note::SlideIntoFromAbove) ||
        note.hasProperty(Note::SlideIntoFromBelow))
    {
        uint8_t bendAmount = note.hasProperty(Note::SlideIntoFromAbove)
                                 ? SLIDE_ABOVE_BEND
                                 : SLIDE_BELOW_BEND;

        // Slide over a 16th note.
        const double slideDuration = currentTempo / 4.0;
        generateGradualBend(bends, startTime, slideDuration, bendAmount,
                            BendEvent::DEFAULT_BEND);
    }
}

#if 0
void MidiPlayer::generateTremoloBar(std::vector<BendEventInfo>& bends, double startTime,
                                    double noteDuration, double currentTempo, const Position* position)
{
    uint8_t type = 0, duration = 0, pitch = 0;
    position->GetTremoloBar(type, duration, pitch);

    const uint8_t resultantPitch = floor(BendEvent::DEFAULT_BEND -
                                         pitch * BendEvent::BEND_QUARTER_TONE);

    // drop the pitch over the note duration
    if (type == Position::diveAndRelease || type == Position::diveAndHold)
    {
        generateGradualBend(bends, startTime, noteDuration,
                            BendEvent::DEFAULT_BEND, resultantPitch);
    }

    // move from active pitch to resultant pitch over the note duration
    if (type == Position::returnAndHold || type == Position::returnAndRelease)
    {
        generateGradualBend(bends, startTime, noteDuration,
                            activePitchBend, resultantPitch);
    }

    if (type == Position::dip || type == Position::invertedDip)
    {
        // dip for either a 32nd note, or half the note duration (for very short notes)
        const double dipDuration = std::min(noteDuration / 2.0, currentTempo / 8.0);

        // pitch that would be used for an inverted dip (bending up instead of down)
        const uint8_t invertedDipPitch = floor(BendEvent::DEFAULT_BEND +
                                               pitch * BendEvent::BEND_QUARTER_TONE);

        // select the correct pitch to dip to (allows us to reuse the following lines of
        // code for both types of dips)
        const uint8_t dipPitch = (type == Position::dip) ? resultantPitch : invertedDipPitch;

        // quickly drop to the specified pitch and then return
        generateGradualBend(bends, startTime, dipDuration,
                            BendEvent::DEFAULT_BEND, dipPitch);
        generateGradualBend(bends, startTime + dipDuration, dipDuration,
                            dipPitch, BendEvent::DEFAULT_BEND);
    }

    if (type == Position::diveAndRelease || type == Position::returnAndRelease
             || type == Position::release)
    {
        // make sure we return to the default pitch, regardless of where the resultant pitch was
        bends.push_back(BendEventInfo(startTime + noteDuration, BendEvent::DEFAULT_BEND));
        activePitchBend = BendEvent::DEFAULT_BEND;
    }
    else if (type == Position::diveAndHold || type == Position::returnAndHold)
    {
        activePitchBend = resultantPitch;
    }
    else
    {
        activePitchBend = BendEvent::DEFAULT_BEND;
    }
}
#endif
