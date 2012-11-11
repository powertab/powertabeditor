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

#include <QSettings>
#include <app/settings.h>
#include <QDebug>

#include <audio/rtmidiwrapper.h>

#include <boost/next_prior.hpp>
#include <boost/foreach.hpp>

#include <painters/caret.h>

#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/system.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>
#include <powertabdocument/harmonics.h>
#include <powertabdocument/timesignature.h>
#include <powertabdocument/barline.h>
#include <powertabdocument/dynamic.h>

#include <audio/midievent.h>
#include <audio/playnoteevent.h>
#include <audio/vibratoevent.h>
#include <audio/stopnoteevent.h>
#include <audio/metronomeevent.h>
#include <audio/letringevent.h>
#include <audio/bendevent.h>
#include <audio/repeatcontroller.h>
#include <audio/restevent.h>
#include <audio/volumechangeevent.h>

/// A MIDI event that does nothing, but is useful for triggering a position
/// change.
class DummyEvent : public MidiEvent
{
public:
    DummyEvent(uint8_t channel, double startTime, double duration,
               uint32_t positionIndex, uint32_t systemIndex) :
        MidiEvent(channel, startTime, duration, positionIndex, systemIndex)
    {
    }

    virtual void performEvent(RtMidiWrapper&) const
    {
    }
};

using boost::shared_ptr;

MidiPlayer::MidiPlayer(Caret* caret, int playbackSpeed) :
    caret(caret),
    isPlaying(false),
    activePitchBend(BendEvent::DEFAULT_BEND),
    playbackSpeed(playbackSpeed)
{
}

MidiPlayer::~MidiPlayer()
{
    mutex.lock();
    isPlaying = false;
    mutex.unlock();

    wait();
}

void MidiPlayer::run()
{
    mutex.lock();
    isPlaying = true;
    mutex.unlock();

    const SystemLocation startLocation(caret->getCurrentSystemIndex(),
                                       caret->getCurrentPositionIndex());

    boost::ptr_list<MidiEvent> eventList;
    generateEvents(caret->getCurrentScore(), eventList);

    // Sort the events by their start time, and play them in order.
    eventList.sort();

    playMidiEvents(eventList, startLocation);
}

void MidiPlayer::generateEvents(const Score* score,
                                boost::ptr_list<MidiEvent>& eventList)
{
    double timeStamp = 0;

    for (uint32_t i = 0; i < score->GetSystemCount(); ++i)
    {
        shared_ptr<const System> system = score->GetSystem(i);

        std::vector<shared_ptr<const Barline> > barlines;
        system->GetBarlines(barlines);

        for (size_t j = 0; j < barlines.size() - 1; ++j)
        {
            shared_ptr<const Barline> leftBar = barlines[j];
            shared_ptr<const Barline> rightBar = barlines[j+1];

            const double barStartTime = timeStamp;

            for (uint32_t k = 0; k < system->GetStaffCount(); ++k)
            {
                shared_ptr<const Staff> staff = system->GetStaff(k);

                for (uint32_t voice = 0; voice < Staff::NUM_STAFF_VOICES;
                     ++voice)
                {
                    std::vector<Position*> positions;
                    staff->GetPositionsInRange(positions, voice,
                                               leftBar->GetPosition(),
                                               rightBar->GetPosition() - 1);

                    double endTime = generateEventsForBar(i, positions, score,
                                                          system, staff, k,
                                                          voice, barStartTime,
                                                          eventList);
                    timeStamp = std::max(timeStamp, endTime);
                }
            }

            // Don't add in the metronome for empty bars, but add an empty event
            // for the left bar so that repeats, etc. are triggered.
            if (timeStamp == barStartTime)
            {
                eventList.push_back(new DummyEvent(METRONOME_CHANNEL, timeStamp, 0,
                                                   leftBar->GetPosition(), i));
                continue;
            }

            // Add metronome ticks for the bar.
            timeStamp = generateMetronome(i, system, leftBar, barStartTime,
                                          timeStamp, eventList);
        }

        // Add event at the end bar's position in order to trigger any
        // repeats or alternate endings. We don't need this for any other bars
        // since there are metronome events at the other bars.
        eventList.push_back(new DummyEvent(METRONOME_CHANNEL, timeStamp, 0,
                                           barlines.back()->GetPosition(), i));
    }
}

/// Returns the appropriate note velocity type for the given position/note
PlayNoteEvent::VelocityType getNoteVelocity(const Position* position, const Note* note)
{
    if (note->IsGhostNote())
    {
        return PlayNoteEvent::GHOST_VELOCITY;
    }
    if (note->IsMuted())
    {
        return PlayNoteEvent::MUTED_VELOCITY;
    }
    if (position->HasPalmMuting())
    {
        return PlayNoteEvent::PALM_MUTED_VELOCITY;
    }

    return PlayNoteEvent::DEFAULT_VELOCITY;
}

/// Generates a list of all notes in the given bar (list of positions).
/// @return The timestamp of the end of the last event in the bar.
double MidiPlayer::generateEventsForBar(
        uint32_t systemIndex, const std::vector<Position*>& positions,
        const Score* score, boost::shared_ptr<const System> system,
        boost::shared_ptr<const Staff> staff, uint32_t staffIndex,
        const uint32_t voice, const double barStartTime,
        boost::ptr_list<MidiEvent>& eventList)
{
    activePitchBend = BendEvent::DEFAULT_BEND;
    double startTime = barStartTime;
    bool letRingActive = false;

    const uint32_t channel = staffIndex; // 1 channel per guitar/staff.
    shared_ptr<const Guitar> guitar = score->GetGuitar(channel);

    for (size_t i = 0; i < positions.size(); ++i)
    {
        Position* position = positions[i];

        const uint32_t positionIndex = position->GetPosition();
        const SystemLocation location(systemIndex, positionIndex);
        const double currentTempo = getCurrentTempo(location);

        // Each note at a position has the same duration.
        double duration = calculateNoteDuration(systemIndex, position);

        if (position->IsRest())
        {
            // for whole rests, they must last for the entire bar, regardless of time signature
            if (position->GetDurationType() == 1)
            {
                duration = getWholeRestDuration(system, staff, systemIndex, position, duration);

                // extend for multi-bar rests
                if (position->HasMultibarRest())
                {
                    uint8_t measureCount = 0;
                    position->GetMultibarRest(measureCount);
                    duration *= measureCount;
                }
            }

            eventList.push_back(new RestEvent(channel, startTime, duration,
                                              positionIndex, systemIndex));
            startTime += duration;
            continue;
        }

        if (position->IsAcciaccatura()) // grace note
        {
            duration = GRACE_NOTE_DURATION;
            startTime -= duration;
        }

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
            VibratoEvent::VibratoType type = position->HasVibrato() ? VibratoEvent::NORMAL_VIBRATO :
                                                                      VibratoEvent::WIDE_VIBRATO;

            // add vibrato event, and an event to turn of the vibrato after the note is done
            eventList.push_back(new VibratoEvent(channel, startTime, positionIndex, systemIndex,
                                                 VibratoEvent::VIBRATO_ON, type));

            eventList.push_back(new VibratoEvent(channel, startTime + duration, positionIndex,
                                                 systemIndex, VibratoEvent::VIBRATO_OFF));
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

        for (uint32_t j = 0; j < position->GetNoteCount(); j++)
        {
            // for arpeggios, delay the start of each note a small amount from the last,
            // and also adjust the duration correspondingly
            if (position->HasArpeggioDown() || position->HasArpeggioUp())
            {
                startTime += ARPEGGIO_OFFSET;
                duration -= ARPEGGIO_OFFSET;
            }

            const Note* note = position->GetNote(j);

            uint32_t pitch = getActualNotePitch(note, guitar);

            const PlayNoteEvent::VelocityType velocity = getNoteVelocity(position, note);

            // if this note is not tied to the previous note, play the note
            if (!note->IsTied())
            {
                eventList.push_back(new PlayNoteEvent(channel, startTime, duration, pitch,
                                                      positionIndex, systemIndex, guitar,
                                                      note->IsMuted(), velocity));
            }
            // if the note is tied, make sure that the pitch is the same as the previous note,
            // so that the Stop Note event works correctly with harmonics
            else
            {
                const Note* prevNote = staff->GetAdjacentNoteOnString(Staff::PrevNote, position, note, voice);

                // TODO - deal with ties that wrap across systems
                if (prevNote)
                {
                    pitch = getActualNotePitch(prevNote, guitar);
                }
            }

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

            bool tiedToNextNote = false;
            // check if this note is tied to the next note
            {
                const Note* nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note, voice);
                if (nextNote && nextNote->IsTied())
                {
                    tiedToNextNote = true;
                }
            }

            // end the note, unless we are tied to the next note
            if (!note->HasTieWrap() && !tiedToNextNote)
            {
                double noteLength = duration;

                if (position->IsStaccato())
                {
                    noteLength /= 2.0;
                }
                else if (position->HasPalmMuting())
                {
                    noteLength /= 1.15;
                }

                eventList.push_back(new StopNoteEvent(channel, startTime + noteLength,
                                                      positionIndex, systemIndex, pitch));
            }
        }

        startTime += duration;
    }

    return startTime;
}

// The events are already in order of occurrence, so just play them one by one
// startLocation is used to identify the starting position to begin playback from
void MidiPlayer::playMidiEvents(boost::ptr_list<MidiEvent>& eventList, SystemLocation startLocation)
{
    RtMidiWrapper rtMidiWrapper;

    // set the port for RtMidi
    QSettings settings;
    rtMidiWrapper.initialize(
                settings.value(Settings::MIDI_PREFERRED_API,
                               Settings::MIDI_PREFERRED_API_DEFAULT).toInt(),
                settings.value(Settings::MIDI_PREFERRED_PORT,
                               Settings::MIDI_PREFERRED_PORT_DEFAULT).toInt());

    // set pitch bend settings for each channel to one octave
    for (uint8_t i = 0; i < midi::NUM_MIDI_CHANNELS_PER_PORT; i++)
    {
        rtMidiWrapper.setPitchBendRange(i, BendEvent::PITCH_BEND_RANGE);
    }

    RepeatController repeatController(caret->getCurrentScore());

    SystemLocation currentLocation;

    typedef boost::ptr_list<MidiEvent>::const_iterator MidiEventIterator;
    MidiEventIterator activeEvent = eventList.begin();

    while (activeEvent != eventList.end())
    {
        {
            QMutexLocker locker(&mutex);
            Q_UNUSED(locker);

            if (!isPlaying)
            {
                return;
            }
        }

        const SystemLocation eventLocation(activeEvent->getSystemIndex(), activeEvent->getPositionIndex());

#if defined(LOG_MIDI_EVENTS)
        qDebug() << "Playback location: " << eventLocation.getSystemIndex() << ", " << eventLocation.getPositionIndex();
#endif

        // if we haven't reached the starting position yet, keep going
        if (eventLocation < startLocation)
        {
            ++activeEvent;
            continue;
        }
        // if we just reached the starting position, update the system index explicitly
        // to avoid the "currentPosition = 0" effect of a normal system change
        else if (eventLocation == startLocation)
        {
            emit playbackSystemChanged(startLocation.getSystemIndex());
            currentLocation.setSystemIndex(startLocation.getSystemIndex());
            startLocation = SystemLocation(0, 0);
        }

        // if we've moved to a new position, move the caret
        if (eventLocation.getPositionIndex() > currentLocation.getPositionIndex())
        {
            currentLocation.setPositionIndex(eventLocation.getPositionIndex());
            emit playbackPositionChanged(currentLocation.getPositionIndex());
        }

        // moving on to a new system, so we need to reset the position to 0 to ensure
        // playback begins at the start of the staff
        if (eventLocation.getSystemIndex() != currentLocation.getSystemIndex())
        {
            currentLocation.setSystemIndex(eventLocation.getSystemIndex());
            currentLocation.setPositionIndex(0);
            emit playbackSystemChanged(currentLocation.getSystemIndex());
        }

        SystemLocation newLocation;
        if (repeatController.checkForRepeat(currentLocation, newLocation))
        {
            qDebug() << "Moving to: " << newLocation.getSystemIndex()
                     << ", " << newLocation.getPositionIndex();
            qDebug() << "From position: " << currentLocation.getSystemIndex()
                     << ", " << currentLocation.getPositionIndex()
                     << " at " << activeEvent->getStartTime();

            startLocation = newLocation;
            currentLocation = SystemLocation(0, 0);
            emit playbackSystemChanged(startLocation.getSystemIndex());
            emit playbackPositionChanged(startLocation.getPositionIndex());
            activeEvent = eventList.begin();
            continue;
        }

        activeEvent->performEvent(rtMidiWrapper);

        // add delay between this event and the next one
        MidiEventIterator nextEvent = boost::next(activeEvent);
        if (nextEvent != eventList.end())
        {
            const int sleepDuration = abs(nextEvent->getStartTime() - activeEvent->getStartTime());

            mutex.lock();
            const double speedShiftFactor = 100.0 / playbackSpeed; // slow down or speed up playback
            mutex.unlock();

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

// Finds the active tempo marker
boost::shared_ptr<TempoMarker> MidiPlayer::getCurrentTempoMarker(
        const SystemLocation& location) const
{
    const Score* currentScore = caret->getCurrentScore();

    Score::TempoMarkerPtr currentTempoMarker;

    // find the active tempo marker
    for(quint32 i = 0; i < currentScore->GetTempoMarkerCount(); i++)
    {
        Score::TempoMarkerPtr temp = currentScore->GetTempoMarker(i);
        if (temp->GetSystem() <= location.getSystemIndex() &&
            temp->GetPosition() <=  location.getPositionIndex() &&
            !temp->IsAlterationOfPace()) // TODO - properly support alterations of pace
        {
            currentTempoMarker = temp;
        }
    }

    return currentTempoMarker;
}

/// Returns the current tempo (duration of a quarter note in milliseconds).
double MidiPlayer::getCurrentTempo(const SystemLocation& location) const
{
    Score::TempoMarkerPtr tempoMarker = getCurrentTempoMarker(location);

    double bpm = TempoMarker::DEFAULT_BEATS_PER_MINUTE; // default tempo in case there is no tempo marker in the score
    uint8_t beatType = TempoMarker::DEFAULT_BEAT_TYPE;

    if (tempoMarker)
    {
        bpm = tempoMarker->GetBeatsPerMinute();
        Q_ASSERT(bpm != 0);

        beatType = tempoMarker->GetBeatType();
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

    // convert bpm to millisecond duration
    return (60.0 / (bpm * factor) * 1000.0);
}

double MidiPlayer::calculateNoteDuration(uint32_t systemIndex,
                                         const Position* currentPosition) const
{
    const double tempo = getCurrentTempo(
                SystemLocation(systemIndex, currentPosition->GetPosition()));

    return currentPosition->GetDuration() * tempo;
}

double MidiPlayer::getWholeRestDuration(
        shared_ptr<const System> system, shared_ptr<const Staff> staff,
        uint32_t systemIndex, const Position* position, double originalDuration) const
{
    System::BarlineConstPtr prevBarline = system->GetPrecedingBarline(position->GetPosition());

    // if the whole rest is not the only item in the bar, treat it like a regular rest
    if (!staff->IsOnlyPositionInBar(position, system))
    {
        return originalDuration;
    }

    const TimeSignature& currentTimeSignature = prevBarline->GetTimeSignature();

    const double tempo = getCurrentTempo(
                SystemLocation(systemIndex, position->GetPosition()));

    double beatDuration = currentTimeSignature.GetBeatAmount();
    double duration = tempo * 4.0 / beatDuration;
    int numBeats = currentTimeSignature.GetBeatsPerMeasure();
    duration *= numBeats;

    return duration;
}

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

uint32_t MidiPlayer::getActualNotePitch(const Note* note, shared_ptr<const Guitar> guitar) const
{
    const Tuning& tuning = guitar->GetTuning();
    
    const quint32 openStringPitch = tuning.GetNote(note->GetString()) + guitar->GetCapo();
    quint32 pitch = openStringPitch + note->GetFretNumber();
    
    if (note->IsNaturalHarmonic())
    {
        pitch = openStringPitch + Harmonics::getPitchOffset(note->GetFretNumber());
    }
    
    if (note->HasTappedHarmonic())
    {
        uint8_t tappedFret = 0;
        note->GetTappedHarmonic(tappedFret);
        pitch = pitch + Harmonics::getPitchOffset(tappedFret - note->GetFretNumber());
    }
    
    if (note->HasArtificialHarmonic())
    {
        uint8_t key = 0, keyVariation = 0, octaveDiff = 0;
        note->GetArtificialHarmonic(key, keyVariation, octaveDiff);
        
        pitch = (midi::GetMidiNoteOctave(pitch) + octaveDiff + 2) * 12 + key;
    }
    
    return pitch;
}

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
    mutex.lock();
    playbackSpeed = newPlaybackSpeed;
    mutex.unlock();
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
