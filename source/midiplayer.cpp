#include "midiplayer.h"

#include <QSettings>
#include <QDebug>

#include <boost/next_prior.hpp>

#include <painters/caret.h>

#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/system.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>

#include <audio/midievent.h>
#include <audio/playnoteevent.h>
#include <audio/vibratoevent.h>
#include <audio/stopnoteevent.h>
#include <audio/metronomeevent.h>
#include <audio/letringevent.h>
#include <audio/repeatcontroller.h>

using std::shared_ptr;
using std::unique_ptr;

MidiPlayer::MidiPlayer(Caret* caret) :
    caret(caret)
{
    QSettings settings;
    rtMidiWrapper.initialize(settings.value("midi/preferredPort", 0).toInt());

    isPlaying = false;
    currentSystemIndex = 0;

    initHarmonicPitches();
}

MidiPlayer::~MidiPlayer()
{
    isPlaying = false;

    wait();
}

void MidiPlayer::run()
{
    isPlaying = true;

    uint32_t startSystemIndex = caret->getCurrentSystemIndex();
    uint32_t startPos = caret->getCurrentPositionIndex();

    boost::ptr_list<MidiEvent> eventList;
    double timeStamp = 0;

    // go through each system, generate a list of the notes (midi events) from each staff
    // then, sort notes by their start time, and play them in order
    for (currentSystemIndex = 0; currentSystemIndex < caret->getCurrentScore()->GetSystemCount(); ++currentSystemIndex)
    {
        generateMetronome(currentSystemIndex, timeStamp, eventList);

        timeStamp = generateEventsForSystem(currentSystemIndex, timeStamp, eventList);
    }

    eventList.sort();

    playMidiEvents(eventList, startSystemIndex, startPos);
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

/// Generates a list of all notes in the given system, by iterating through each position in each staff of the system
/// @return The timestamp of the end of the last event in the system
double MidiPlayer::generateEventsForSystem(uint32_t systemIndex, const double systemStartTime,
                                           boost::ptr_list<MidiEvent>& eventList) const
{
    double endTime = systemStartTime;

    shared_ptr<const System> system = caret->getCurrentScore()->GetSystem(systemIndex);

    for (quint32 i = 0; i < system->GetStaffCount(); i++)
    {
        const Staff* staff = system->GetStaff(i);
        shared_ptr<const Guitar> guitar = caret->getCurrentScore()->GetGuitar(i);

        for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
        {
            // each note in the staff is given a start time relative to the first note of the staff
            double startTime = systemStartTime;
            
            bool letRingActive = false;

            for (quint32 j = 0; j < staff->GetPositionCount(voice); j++)
            {
                Position* position = staff->GetPosition(voice, j);

                double duration = calculateNoteDuration(position); // each note at a position has the same duration

                if (position->IsRest())
                {
                    // for whole rests, they must last for the entire bar, regardless of time signature
                    if (position->GetDurationType() == 1)
                    {
                        duration = getWholeRestDuration(system, staff, position, duration);
                    }

                    startTime += duration;
                    endTime = std::max(endTime, startTime);
                    continue;
                }

                if (position->IsAcciaccatura()) // grace note
                {
                    duration = GRACE_NOTE_DURATION;
                    startTime -= duration;
                }

                const quint32 positionIndex = position->GetPosition();

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

                for (quint32 k = 0; k < position->GetNoteCount(); k++)
                {
                    // for arpeggios, delay the start of each note a small amount from the last,
                    // and also adjust the duration correspondingly
                    if (position->HasArpeggioDown() || position->HasArpeggioUp())
                    {
                        startTime += ARPEGGIO_OFFSET;
                        duration -= ARPEGGIO_OFFSET;
                    }

                    const Note* note = position->GetNote(k);

                    uint32_t pitch = getActualNotePitch(note, guitar);

                    const PlayNoteEvent::VelocityType velocity = getNoteVelocity(position, note);

                    // if this note is not tied to the previous note, play the note
                    if (!note->IsTied())
                    {
                        eventList.push_back(new PlayNoteEvent(i, startTime, duration, pitch,
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

                    // vibrato events
                    if (position->HasVibrato() || position->HasWideVibrato())
                    {
                        VibratoEvent::VibratoType type = position->HasVibrato() ? VibratoEvent::NORMAL_VIBRATO :
                                                                                  VibratoEvent::WIDE_VIBRATO;

                        // add vibrato event, and an event to turn of the vibrato after the note is done
                        eventList.push_back(new VibratoEvent(i, startTime, positionIndex, systemIndex,
                                                             VibratoEvent::VIBRATO_ON, type));

                        eventList.push_back(new VibratoEvent(i, startTime + duration, positionIndex,
                                                             systemIndex, VibratoEvent::VIBRATO_OFF));
                    }
                    
                    // let ring events
                    if (position->HasLetRing() && !letRingActive)
                    {
                        eventList.push_back(new LetRingEvent(i, startTime, positionIndex, systemIndex,
                                                             LetRingEvent::LET_RING_ON));
                        letRingActive = true;
                    }
                    else if (!position->HasLetRing() && letRingActive)
                    {
                        eventList.push_back(new LetRingEvent(i, startTime, positionIndex, systemIndex,
                                                             LetRingEvent::LET_RING_OFF));
                        letRingActive = false;
                    }

                    // tremolo picking
                    if (position->HasTremoloPicking())
                    {
                        // tremolo picking is done using 32nd notes
                        const double tremPickNoteDuration = getCurrentTempo(position->GetPosition()) / 8.0;
                        const int numNotes = duration / tremPickNoteDuration;

                        for (int k = 0; k < numNotes; ++k)
                        {
                            const double currentStartTime = startTime + k * tremPickNoteDuration;

                            eventList.push_back(new StopNoteEvent(i, currentStartTime, positionIndex,
                                                                  systemIndex, pitch));

                            eventList.push_back(new PlayNoteEvent(i, currentStartTime, tremPickNoteDuration, pitch,
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

                        eventList.push_back(new StopNoteEvent(i, startTime + noteLength,
                                                              positionIndex, systemIndex, pitch));
                    }
                }

                startTime += duration;

                endTime = std::max(endTime, startTime);
            }
        }
    }

    return endTime;
}

// The events are already in order of occurrence, so just play them one by one
// startPos is used to identify the starting position to begin playback from
void MidiPlayer::playMidiEvents(boost::ptr_list<MidiEvent>& eventList,
                                uint32_t startSystem, uint32_t startPos)
{
    RepeatController repeatController(caret->getCurrentScore());

    uint32_t currentPosition = 0;
    uint32_t currentSystem = 0;

    auto activeEvent = eventList.begin();

    while (activeEvent != eventList.end())
    {
        if (!isPlaying)
        {
            return;
        }

        const uint32_t eventPosition = activeEvent->getPositionIndex();
        const uint32_t eventSystemIndex = activeEvent->getSystemIndex();

        if (eventSystemIndex < startSystem || eventPosition < startPos)
        {
            ++activeEvent;
            continue;
        }
        else
        {
            startPos = startSystem = 0;
        }

        // if we've moved to a new position, move the caret
        if (eventPosition > currentPosition)
        {
            currentPosition = eventPosition;
            emit playbackPositionChanged(currentPosition);
        }

        if (eventSystemIndex != currentSystem)
        {
            currentSystem = eventSystemIndex;
            currentPosition = 0;
            emit playbackSystemChanged(currentSystem);
        }

        uint32_t newSystem = 0, newPos = 0;
        if (repeatController.checkForRepeat(currentSystem, currentPosition, newSystem, newPos))
        {
            qDebug() << "Moving to: " << newSystem << ", " << newPos;
            qDebug() << "From position: " << currentSystem << ", " << currentPosition
                     << " at " << activeEvent->getStartTime();

            startSystem = newSystem;
            startPos = newPos;
            currentPosition = currentSystem = 0;
            emit playbackSystemChanged(startSystem);
            emit playbackPositionChanged(startPos);
            activeEvent = eventList.begin();
            continue;
        }

        activeEvent->performEvent(rtMidiWrapper);

        // add delay between this event and the next one
        auto nextEvent = boost::next(activeEvent);
        if (nextEvent != eventList.end())
        {
            const int sleepDuration = nextEvent->getStartTime() - activeEvent->getStartTime();
            Q_ASSERT(sleepDuration >= 0);

            usleep(1000 * sleepDuration);
        }
        else
        {
            usleep(1000 * activeEvent->getDuration());
        }

        ++activeEvent;
    }
}

// Finds the active tempo marker
TempoMarker* MidiPlayer::getCurrentTempoMarker(const quint32 positionIndex) const
{
    Score* currentScore = caret->getCurrentScore();

    TempoMarker* currentTempoMarker = NULL;

    // find the active tempo marker
    for(quint32 i = 0; i < currentScore->GetTempoMarkerCount(); i++)
    {
        TempoMarker* temp = currentScore->GetTempoMarker(i);
        if (temp->GetSystem() <= currentSystemIndex &&
            temp->GetPosition() <=  positionIndex &&
            !temp->IsAlterationOfPace()) // TODO - properly support alterations of pace
        {
            currentTempoMarker = temp;
        }
    }

    return currentTempoMarker;
}

double MidiPlayer::getCurrentTempo(const quint32 positionIndex) const
{
    TempoMarker* tempoMarker = getCurrentTempoMarker(positionIndex);

    double bpm = TempoMarker::DEFAULT_BEATS_PER_MINUTE; // default tempo in case there is no tempo marker in the score
    double beatType = TempoMarker::DEFAULT_BEAT_TYPE;

    if (tempoMarker != NULL)
    {
        bpm = tempoMarker->GetBeatsPerMinute();
        Q_ASSERT(bpm != 0);

        beatType = tempoMarker->GetBeatType();
    }

    // convert bpm to millisecond duration
    return (60.0 / bpm * 1000.0 * (TempoMarker::quarter / beatType));
}

double MidiPlayer::calculateNoteDuration(const Position* currentPosition) const
{
    const double tempo = getCurrentTempo(currentPosition->GetPosition());

    return currentPosition->GetDuration() * tempo;
}

double MidiPlayer::getWholeRestDuration(shared_ptr<const System> system, const Staff* staff, 
                                        const Position* position, double originalDuration) const
{
    Barline* prevBarline = system->GetPrecedingBarline(position->GetPosition());

    // if the whole rest is not the only item in the bar, treat it like a regular rest
    if (!staff->IsOnlyPositionInBar(position, system))
    {
        return originalDuration;
    }

    const TimeSignature& currentTimeSignature = prevBarline->GetTimeSignatureConstRef();

    double tempo = getCurrentTempo(position->GetPosition());
    double beatDuration = currentTimeSignature.GetBeatAmount();
    double duration = tempo * 4.0 / beatDuration;
    int numBeats = currentTimeSignature.GetBeatsPerMeasure();
    duration *= numBeats;

    return duration;
}

// initialize the mapping of frets to pitch offsets (counted in half-steps or frets)
// e.g. The natural harmonic at the 7th fret is an octave and a fifth - 19 frets - above the pitch of the open string
void MidiPlayer::initHarmonicPitches()
{
    harmonicPitches[3] = 31;
    harmonicPitches[4] = harmonicPitches[9] = 28;
    harmonicPitches[16] = harmonicPitches[28] = 28;
    harmonicPitches[5] = harmonicPitches[24] = 24;
    harmonicPitches[7] = harmonicPitches[19] = 19;
    harmonicPitches[12] = 12;
}

quint8 MidiPlayer::getHarmonicPitch(const quint8 basePitch, const quint8 fretOffset) const
{
    return basePitch + harmonicPitches[fretOffset];
}

// Generates the metronome ticks
void MidiPlayer::generateMetronome(uint32_t systemIndex, double startTime,
                                   boost::ptr_list<MidiEvent>& eventList) const
{
    shared_ptr<System> system = caret->getCurrentScore()->GetSystem(systemIndex);

    std::vector<const Barline*> barlines;
    system->GetBarlines(barlines);
    barlines.pop_back(); // don't need the end barline

    for (size_t i = 0; i < barlines.size(); i++)
    {
        const Barline* barline = barlines.at(i);
        const TimeSignature& timeSig = barline->GetTimeSignatureConstRef();

        const quint8 numPulses = timeSig.GetPulses();
        const quint8 beatsPerMeasure = timeSig.GetBeatsPerMeasure();
        const quint8 beatValue = timeSig.GetBeatAmount();

        // figure out duration of pulse
        const double tempo = getCurrentTempo(barline->GetPosition());
        double duration = tempo * 4.0 / beatValue;
        duration *= beatsPerMeasure / numPulses;

        const quint32 position = barline->GetPosition();

        for (quint8 j = 0; j < numPulses; j++)
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

    // insert an empty event for the last barline of the system, to trigger any repeat events for that bar
    eventList.push_back(new StopNoteEvent(METRONOME_CHANNEL, startTime,
                                          system->GetEndBarConstRef().GetPosition(),
                                          systemIndex, MetronomeEvent::METRONOME_PITCH));
}

uint32_t MidiPlayer::getActualNotePitch(const Note* note, shared_ptr<const Guitar> guitar) const
{
    const Tuning& tuning = guitar->GetTuningConstRef();
    
    const quint32 openStringPitch = tuning.GetNote(note->GetString()) + guitar->GetCapo();
    quint32 pitch = openStringPitch + note->GetFretNumber();
    
    if (note->IsNaturalHarmonic())
    {
        pitch = getHarmonicPitch(openStringPitch, note->GetFretNumber());
    }
    
    if (note->HasTappedHarmonic())
    {
        uint8_t tappedFret = 0;
        note->GetTappedHarmonic(tappedFret);
        pitch = getHarmonicPitch(pitch, tappedFret - note->GetFretNumber());
    }
    
    if (note->HasArtificialHarmonic())
    {
        uint8_t key = 0, keyVariation = 0, octaveDiff = 0;
        note->GetArtificialHarmonic(key, keyVariation, octaveDiff);
        
        pitch = (midi::GetMidiNoteOctave(pitch) + octaveDiff + 2) * 12 + key;
    }
    
    return pitch;
}
