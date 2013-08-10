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
#include <audio/midievent.h>
#include <audio/midioutputdevice.h>
#include <audio/playnoteevent.h>
#include <audio/repeatcontroller.h>
#include <audio/restevent.h>
#include <audio/stopnoteevent.h>
#include <boost/foreach.hpp>
#include <QDebug>
#include <QSettings>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils.h>

static const int METRONOME_CHANNEL = 15;

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

    virtual void performEvent(MidiOutputDevice &) const
    {
    }
};

MidiPlayer::MidiPlayer(const Score &score, int startSystem, int startPosition,
                       int speed)
    : myScore(score),
      myStartSystem(startSystem),
      myStartPosition(startPosition),
      myIsPlaying(false),
      myActivePitchBend(BendEvent::DEFAULT_BEND),
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

    boost::ptr_list<MidiEvent> eventList;
    generateEvents(eventList);

    // Sort the events by their start time, and play them in order.
    eventList.sort();
    playMidiEvents(eventList);
}

void MidiPlayer::setIsPlaying(bool set)
{
    QMutexLocker lock(&myMutex);
    myIsPlaying = set;
}

bool MidiPlayer::isPlaying() const
{
    QMutexLocker lock(&myMutex);
    return myIsPlaying;
}

void MidiPlayer::generateEvents(boost::ptr_list<MidiEvent> &eventList)
{
    double time = 0;

    int systemIndex = 0;
    BOOST_FOREACH(const System &system, myScore.getSystems())
    {
        BOOST_FOREACH(const Barline &leftBar, system.getBarlines())
        {
            const Barline *rightBar = system.getNextBarline(
                        leftBar.getPosition());
            if (!rightBar)
                break;

            const double barStartTime = time;

            int staffIndex = 0;
            BOOST_FOREACH(const Staff &staff, system.getStaves())
            {
                for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
                {
                    const double endTime = generateEventsForBar(system,
                                systemIndex, staff, staffIndex, voice,
                                leftBar.getPosition(), rightBar->getPosition(),
                                barStartTime, eventList);

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
                eventList.push_back(new DummyEvent(METRONOME_CHANNEL, time, 0,
                                                   leftBar.getPosition(),
                                                   systemIndex));
                continue;
            }

            // Add metronome ticks for the bar.
#if 0
            time = generateMetronome(systemIndex, system, leftBar, barStartTime,
                                     time, eventList);
#endif
        }

        // Add event at the end bar's position in order to trigger any
        // repeats or alternate endings. We don't need this for any other bars
        // since there are metronome events at the other bars.
        eventList.push_back(
                    new DummyEvent(METRONOME_CHANNEL, time, 0,
                                   system.getBarlines().back().getPosition(),
                                   systemIndex));

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

double MidiPlayer::generateEventsForBar(
        const System &system, int systemIndex, const Staff &staff,
        int staffIndex, int voice, int leftPos, int rightPos,
        const double barStartTime, boost::ptr_list<MidiEvent> &eventList)
{
    myActivePitchBend = BendEvent::DEFAULT_BEND;
    double startTime = barStartTime;
    bool letRingActive = false;

    BOOST_FOREACH(const Position &pos, staff.getPositionsInRange(
                      voice, leftPos, rightPos))
    {
        const int position = pos.getPosition();
        const double currentTempo = getCurrentTempo(systemIndex, position);

        // Each note at a position has the same duration.
        double duration = calculateNoteDuration(systemIndex, position);

        const PlayerChange *currentPlayers = ScoreUtils::getCurrentPlayers(
                    myScore, systemIndex, pos.getPosition());
        if (!currentPlayers)
            continue;

        const std::vector<ActivePlayer> activePlayers =
                currentPlayers->getActivePlayers(staffIndex);
        if (activePlayers.empty())
            continue;

        if (pos.isRest())
        {
            // For whole rests, they must last for the entire bar, regardless
            // of time signature.
            if (pos.getDurationType() == Position::WholeNote)
            {
                duration = getWholeRestDuration(system, systemIndex, staff,
                                                voice, position, duration);

                // Extend for multi-bar rests.
                if (pos.hasMultiBarRest())
                    duration *= pos.getMultiBarRestCount();
            }

            BOOST_FOREACH(const ActivePlayer &player, activePlayers)
            {
                eventList.push_back(
                            new RestEvent(player.getPlayerNumber(), startTime,
                                          duration, position, systemIndex));
            }

            startTime += duration;
            continue;
        }

        // Handle grace notes.
        if (pos.hasProperty(Position::Acciaccatura))
        {
            duration = GraceNoteDuration;
            startTime -= duration;
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

        // vibrato events (these apply to all notes in the position)
        if (position->HasVibrato() || position->HasWideVibrato())
        {
            VibratoEvent::VibratoType type = position->HasVibrato() ? VibratoEvent::NormalVibrato :
                                                                      VibratoEvent::WideVibrato;

            // add vibrato event, and an event to turn of the vibrato after the note is done
            eventList.push_back(new VibratoEvent(channel, startTime, positionIndex, systemIndex,
                                                 VibratoEvent::VibratoOn, type));

            eventList.push_back(new VibratoEvent(channel, startTime + duration, positionIndex,
                                                 systemIndex, VibratoEvent::VibratoOff));
        }

        // dynamics
        {
            Score::DynamicPtr dynamic = score->FindDynamic(systemIndex, channel, positionIndex);
            if (dynamic)
            {
                eventList.push_back(new VolumeChangeEvent(channel, startTime,
                                                          positionIndex, systemIndex,
                                                          dynamic->GetStaffVolume()));
            }
        }

        // let ring events (applied to all notes in the position)
        if (position->HasLetRing() && !letRingActive)
        {
            eventList.push_back(new LetRingEvent(channel, startTime, positionIndex, systemIndex,
                                                 LetRingEvent::LET_RING_ON));
            letRingActive = true;
        }
        else if (!position->HasLetRing() && letRingActive)
        {
            eventList.push_back(new LetRingEvent(channel, startTime, positionIndex, systemIndex,
                                                 LetRingEvent::LET_RING_OFF));
            letRingActive = false;
        }
        // Make sure that we end the let ring after the last position in the bar.
        else if (letRingActive && position == positions.back())
        {
            eventList.push_back(new LetRingEvent(channel, startTime + duration, positionIndex, systemIndex,
                                                 LetRingEvent::LET_RING_OFF));
            letRingActive = false;
        }
#else
        Q_UNUSED(currentTempo);
        Q_UNUSED(letRingActive);
#endif
        BOOST_FOREACH(const Note &note, pos.getNotes())
        {
            // For arpeggios, delay the start of each note a small amount from
            // the last, and also adjust the duration correspondingly.
            if (pos.hasProperty(Position::ArpeggioDown) ||
                pos.hasProperty(Position::ArpeggioUp))
            {
                startTime += ArpeggioOffset;
                duration -= ArpeggioOffset;
            }

            // Pick a tuning from one of the active players.
            // TODO - should we handle cases where different tunings are used
            // by players in the same staff?
            const int playerIndex = activePlayers.front().getPlayerNumber();
            const Tuning &tuning = myScore.getPlayers()[playerIndex].getTuning();
            int pitch = getActualNotePitch(note, tuning);

            const PlayNoteEvent::VelocityType velocity = getNoteVelocity(position, note);

            // If this note is not tied to the previous note, play the note.
            if (!note.hasProperty(Note::Tied))
            {
                BOOST_FOREACH(const ActivePlayer &activePlayer, activePlayers)
                {
                    const Player &player = myScore.getPlayers()[
                            activePlayer.getPlayerNumber()];
                    const Instrument &instrument = myScore.getInstruments()[
                            activePlayer.getInstrumentNumber()];

                    eventList.push_back(
                            new PlayNoteEvent(activePlayer.getPlayerNumber(),
                                              startTime, duration, pitch,
                                              position, systemIndex, player,
                                              instrument,
                                              note.hasProperty(Note::Muted),
                                              velocity));
                }
            }
            // If the note is tied, make sure that the pitch is the same as the
            // previous note, so that the Stop Note event works correctly with
            // harmonics.
            else
            {
#if 0
                const Note* prevNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position, note, voice);

                // TODO - deal with ties that wrap across systems
                if (prevNote)
                {
                    pitch = getActualNotePitch(prevNote, guitar);
                }
#endif
            }

#if 0
            // generate all events that involve pitch bends
            {
                std::vector<BendEventInfo> bendEvents;

                if (note->HasSlide())
                {
                    generateSlides(bendEvents, startTime, duration, currentTempo, note);
                }

                if (note->HasBend())
                {
                    generateBends(bendEvents, startTime, duration, currentTempo, note);
                }

                // only generate tremolo bar events once, since they apply to all notes in
                // the position
                if (position->HasTremoloBar() && j == 0)
                {
                    generateTremoloBar(bendEvents, startTime, duration, currentTempo, position);
                }

                BOOST_FOREACH(const BendEventInfo& event, bendEvents)
                {
                    eventList.push_back(new BendEvent(channel, event.timestamp, positionIndex,
                                                      systemIndex, event.pitchBendAmount));
                }
            }

            // Perform tremolo picking or trills - they work identically, except trills alternate between two pitches
            if (position->HasTremoloPicking() || note->HasTrill())
            {
                // each note is a 32nd note
                const double tremPickNoteDuration = currentTempo / 8.0;
                const int numNotes = duration / tremPickNoteDuration;

                // find the other pitch to alternate with (this is just the same pitch for tremolo picking)
                uint32_t otherPitch = pitch;
                if (note->HasTrill())
                {
                    uint8_t otherFret = 0;
                    note->GetTrill(otherFret);
                    otherPitch = pitch + (otherFret - note->GetFretNumber());
                }

                for (int k = 0; k < numNotes; ++k)
                {
                    const double currentStartTime = startTime + k * tremPickNoteDuration;

                    eventList.push_back(new StopNoteEvent(channel, currentStartTime, positionIndex,
                                                          systemIndex, pitch));

                    // alternate to the other pitch (this has no effect for tremolo picking)
                    std::swap(pitch, otherPitch);

                    eventList.push_back(new PlayNoteEvent(channel, currentStartTime, tremPickNoteDuration, pitch,
                                                          positionIndex, systemIndex, guitar,
                                                          note->IsMuted(), velocity));
                }
            }
#endif
            bool tiedToNextNote = false;
#if 0
            // check if this note is tied to the next note
            {
                const Note* nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note, voice);
                if (nextNote && nextNote->IsTied())
                {
                    tiedToNextNote = true;
                }
            }
#endif

            // End the note, unless we are tied to the next note.
            if (!tiedToNextNote)
            {
                double noteLength = duration;

                // Shorten the note duration for certain effects.
                if (pos.hasProperty(Position::Staccato))
                    noteLength /= 2.0;
                else if (pos.hasProperty(Position::PalmMuting))
                    noteLength /= 1.15;

                BOOST_FOREACH(const ActivePlayer &player, activePlayers)
                {
                    eventList.push_back(
                                new StopNoteEvent(player.getPlayerNumber(),
                                                  startTime + noteLength,
                                                  position, systemIndex, pitch));
                }
            }
        }

        startTime += duration;
    }

    return startTime;
}

void MidiPlayer::playMidiEvents(const boost::ptr_list<MidiEvent> &eventList)
{
    SystemLocation startLocation(myStartSystem, myStartPosition);

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

    RepeatController repeatController(myScore);

    SystemLocation currentLocation;
    SystemLocation prevLocation;

    typedef boost::ptr_list<MidiEvent>::const_iterator MidiEventIterator;
    MidiEventIterator activeEvent = eventList.begin();

    while (activeEvent != eventList.end())
    {
        if (!isPlaying())
            return;

        const SystemLocation eventLocation(activeEvent->getSystem(),
                                           activeEvent->getPosition());

#if defined(LOG_MIDI_EVENTS)
        qDebug() << "Playback location: " << eventLocation.getSystem() << ", "
                 << eventLocation.getPosition();
#endif

        // If we haven't reached the starting position yet, keep going.
        if (eventLocation < startLocation)
        {
            ++activeEvent;
            continue;
        }
        // If we just reached the starting position, update the system index
        // explicitly to avoid the "currentPosition = 0" effect of a normal
        // system change.
        else if (eventLocation == startLocation)
        {
            emit playbackSystemChanged(startLocation.getSystem());
            currentLocation.setSystem(startLocation.getSystem());
            prevLocation = currentLocation;
            startLocation = SystemLocation(0, 0);
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
            emit playbackSystemChanged(startLocation.getSystem());
            emit playbackPositionChanged(startLocation.getPosition());
            activeEvent = eventList.begin();
            continue;
        }

        activeEvent->performEvent(device);

        // Add delay between this event and the next one.
        MidiEventIterator nextEvent = boost::next(activeEvent);
        if (nextEvent != eventList.end())
        {
            const int sleepDuration = abs(nextEvent->getStartTime() -
                                          activeEvent->getStartTime());

            myMutex.lock();
            // Slow down or speed up playback.
            const double speedShiftFactor = 100.0 / myPlaybackSpeed;
            myMutex.unlock();

            if (sleepDuration)
            {
                usleep(1000 * sleepDuration * speedShiftFactor);
            }
        }
        else // last note
        {
            usleep(1000 * activeEvent->getDuration());
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
    const TempoMarker *lastMarker = NULL;

    int i = 0;
    BOOST_FOREACH(const System &system, myScore.getSystems())
    {
        if (i > systemIndex)
            break;

        BOOST_FOREACH(const TempoMarker &marker, system.getTempoMarkers())
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

double MidiPlayer::calculateNoteDuration(int system, const Position &pos) const
{
    const double tempo = getCurrentTempo(system, pos.getPosition());
    return pos.getDurationTime() * tempo;
}

double MidiPlayer::getWholeRestDuration(const System &system, int systemIndex,
                                        const Staff &staff, int voice,
                                        const Position &pos,
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
        const Position *otherPos = ScoreUtils::findByPosition(
                    staff.getVoice(voice), i);

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

#if 0
/// Generates metronome ticks for a bar.
/// @param notesEndTime The timestamp of the last note event in the bar.
double MidiPlayer::generateMetronome(uint32_t systemIndex,
                                     shared_ptr<const System> system,
                                     shared_ptr<const Barline> barline,
                                     double startTime, const double notesEndTime,
                                     boost::ptr_list<MidiEvent>& eventList) const
{
    const TimeSignature& timeSig = barline->GetTimeSignature();

    uint8_t numPulses = timeSig.GetPulses();
    Q_ASSERT(timeSig.IsValidPulses(numPulses));

    const uint8_t beatsPerMeasure = timeSig.GetBeatsPerMeasure();
    const uint8_t beatValue = timeSig.GetBeatAmount();
    const uint32_t position = barline->GetPosition();

    // Figure out duration of pulse.
    const double tempo = getCurrentTempo(SystemLocation(systemIndex, position));
    double duration = tempo * 4.0 / beatValue;
    duration *= beatsPerMeasure / numPulses;

    // Check for multi-bar rests, as we need to generate more metronome events
    // to fill the extra bars.
    uint8_t measureCount = 0;
    uint8_t repeatCount = 1;
    if (system->HasMultiBarRest(barline, measureCount))
    {
        repeatCount = measureCount;
    }

    // If there are too many notes in the bar, add some more metronome pulses.
    // If there are too few notes, we don't remove any metronome pulses.
    if (repeatCount == 1)
    {
        numPulses = std::max<uint8_t>(numPulses,
                                      (notesEndTime - startTime) / duration);
    }

    for (uint8_t repeat = 0; repeat < repeatCount; repeat++)
    {
        for (uint8_t j = 0; j < numPulses; j++)
        {
            MetronomeEvent::VelocityType velocity = (j == 0) ? MetronomeEvent::STRONG_ACCENT :
                                                               MetronomeEvent::WEAK_ACCENT;

            eventList.push_back(new MetronomeEvent(METRONOME_CHANNEL, startTime, duration,
                                                   position, systemIndex, velocity));

            startTime += duration;

            eventList.push_back(new StopNoteEvent(METRONOME_CHANNEL, startTime, position,
                                                  systemIndex, MetronomeEvent::METRONOME_PITCH));
        }
    }

    startTime = std::max(startTime, notesEndTime);

    return startTime;
}
#endif

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

    // TODO - implement this for the new file format.
#if 0
    if (note->HasArtificialHarmonic())
    {
        uint8_t key = 0, keyVariation = 0, octaveDiff = 0;
        note->GetArtificialHarmonic(key, keyVariation, octaveDiff);
        
        pitch = (midi::GetMidiNoteOctave(pitch) + octaveDiff + 2) * 12 + key;
    }
#endif
    
    return pitch;
}

#if 0
/// Generates bend events for the given note
void MidiPlayer::generateBends(std::vector<BendEventInfo>& bends, double startTime,
                               double duration, double currentTempo, const Note* note)
{
    uint8_t type = 0, bentPitch = 0, releasePitch = 0, bendDuration = 0, drawStartPoint = 0, drawEndPoint = 0;
    note->GetBend(type, bentPitch, releasePitch, bendDuration, drawStartPoint, drawEndPoint);

    const uint8_t bendAmount = floor(BendEvent::DEFAULT_BEND + bentPitch * BendEvent::BEND_QUARTER_TONE);
    const uint8_t releaseAmount = floor(BendEvent::DEFAULT_BEND + releasePitch * BendEvent::BEND_QUARTER_TONE);

    // perform a pre-bend
    if (type == Note::preBend || type == Note::preBendAndRelease || type == Note::preBendAndHold)
    {
        bends.push_back(BendEventInfo(startTime, bendAmount));
    }

    // perform a normal (gradual) bend
    if (type == Note::normalBend || type == Note::bendAndHold)
    {
        if (bendDuration == 0) // default - bend over 32nd note
        {
            generateGradualBend(bends, startTime, currentTempo / 8.0, BendEvent::DEFAULT_BEND, bendAmount);
        }
        else if (bendDuration == 1) // bend over current note duration
        {
            generateGradualBend(bends, startTime, duration, BendEvent::DEFAULT_BEND, bendAmount);
        }
        // TODO - implement bends that stretch over multiple notes
    }

    // for a "bend and release", bend up to bent pitch, for half the note duration
    if (type == Note::bendAndRelease)
    {
        generateGradualBend(bends, startTime, duration / 2, BendEvent::DEFAULT_BEND, bendAmount);
    }

    // bend back down to the release pitch
    if (type == Note::preBendAndRelease)
    {
        generateGradualBend(bends, startTime, duration, bendAmount, releaseAmount);
    }
    else if (type == Note::bendAndRelease)
    {
        generateGradualBend(bends, startTime + duration / 2, duration / 2, bendAmount, releaseAmount);
    }
    else if (type == Note::gradualRelease)
    {
        generateGradualBend(bends, startTime, duration, activePitchBend, releaseAmount);
    }

    // reset to the release pitch bend value
    if (type == Note::preBend || type == Note::immediateRelease || type == Note::normalBend)
    {
        bends.push_back(BendEventInfo(startTime + duration, releaseAmount));
    }

    if (type == Note::bendAndHold || type == Note::preBendAndHold)
    {
        activePitchBend = bendAmount;
    }
    else
    {
        activePitchBend = releaseAmount;
    }
}

/// Generates a series of BendEvents to perform a gradual bend over the given duration
/// Bends the note from the startBendAmount to the releaseBendAmount over the note duration
void MidiPlayer::generateGradualBend(std::vector<BendEventInfo>& bends, double startTime, double duration,
                                     int startBendAmount, int releaseBendAmount) const
{
    const int numBendEvents = abs(startBendAmount - releaseBendAmount);
    const double bendEventDuration = duration / numBendEvents;

    for (int i = 1; i <= numBendEvents; i++)
    {
        const double timestamp = startTime + bendEventDuration * i;
        if (startBendAmount < releaseBendAmount)
        {
            bends.push_back(BendEventInfo(timestamp, startBendAmount + i));
        }
        else
        {
            bends.push_back(BendEventInfo(timestamp, startBendAmount - i));
        }
    }
}

MidiPlayer::BendEventInfo::BendEventInfo(double timestamp, uint8_t pitchBendAmount) :
    timestamp(timestamp),
    pitchBendAmount(pitchBendAmount)
{
}

void MidiPlayer::changePlaybackSpeed(int newPlaybackSpeed)
{
    // playback speed may be changed via the main thread during playback
    myMutex.lock();
    playbackSpeed = newPlaybackSpeed;
    myMutex.unlock();
}

/// Generates slides for the given note
void MidiPlayer::generateSlides(std::vector<BendEventInfo>& bends, double startTime,
                               double noteDuration, double currentTempo, const Note* note)
{
    const int SLIDE_OUT_OF_STEPS = 5;

    const double SLIDE_BELOW_BEND = floor(BendEvent::DEFAULT_BEND -
                                               SLIDE_OUT_OF_STEPS * 2 * BendEvent::BEND_QUARTER_TONE);

    const double SLIDE_ABOVE_BEND = floor(BendEvent::DEFAULT_BEND +
                                          SLIDE_OUT_OF_STEPS * 2 * BendEvent::BEND_QUARTER_TONE);

    if (note->HasSlideOutOf())
    {
        int8_t steps = 0;
        uint8_t type = 0;
        note->GetSlideOutOf(type, steps);

        uint8_t bendAmount = BendEvent::DEFAULT_BEND;

        switch(type)
        {
        case Note::slideOutOfLegatoSlide:
        case Note::slideOutOfShiftSlide:
            bendAmount = floor(BendEvent::DEFAULT_BEND +
                               steps * 2 * BendEvent::BEND_QUARTER_TONE);
            break;

        case Note::slideOutOfDownwards:
            bendAmount = SLIDE_BELOW_BEND;
            break;

        case Note::slideOutOfUpwards:
            bendAmount = SLIDE_ABOVE_BEND;
            break;

        default:
            Q_ASSERT("Unexpected slide type");
            break;
        }

        // start the slide in the last half of the note duration, to make it somewhat more realistic-sounding
        const double slideDuration = noteDuration / 2.0;
        generateGradualBend(bends, startTime + slideDuration, slideDuration,
                            BendEvent::DEFAULT_BEND, bendAmount);

        // reset pitch wheel after note
        bends.push_back(BendEventInfo(startTime + noteDuration, BendEvent::DEFAULT_BEND));
    }

    if (note->HasSlideInto())
    {
        uint8_t type = 0;
        note->GetSlideInto(type);

        uint8_t bendAmount = BendEvent::DEFAULT_BEND;

        switch(type)
        {
        case Note::slideIntoFromBelow:
            bendAmount = SLIDE_BELOW_BEND;
            break;

        case Note::slideIntoFromAbove:
            bendAmount = SLIDE_ABOVE_BEND;
            break;

        default:
            qDebug() << "Unsupported Slide Into type";
            break;
        }

        // slide over a 16th note
        const double slideDuration = currentTempo / 4.0;
        generateGradualBend(bends, startTime, slideDuration, bendAmount, BendEvent::DEFAULT_BEND);
    }
}

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
