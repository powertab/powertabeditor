#include "midiplayer.h"

#include <QSettings>
#include <QDebug>

#include <list>
#include <algorithm>

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

using std::shared_ptr;
using std::unique_ptr;

MidiPlayer::MidiPlayer(Caret* caret) :
    caret(caret)
{
    QSettings settings;
    rtMidiWrapper.initialize(settings.value("midi/preferredPort", 0).toInt());

    isPlaying = false;
    currentSystemIndex = 0;

    initNaturalHarmonics();
}

MidiPlayer::~MidiPlayer()
{
    isPlaying = false;

    wait();
}

/// Compares the start times of two midi events
struct CompareEventTimestamps
{
    bool operator()(const unique_ptr<MidiEvent>& event1, const unique_ptr<MidiEvent>& event2)
    {
        return event1->getStartTime() < event2->getStartTime();
    }
};

void MidiPlayer::run()
{
    isPlaying = true;

    currentSystemIndex = caret->getCurrentSystemIndex();
    quint32 startPos = caret->getCurrentPositionIndex();

    // go through each system, generate a list of the notes (midi events) from each staff
    // then, sort notes by their start time, and play them in order
    for (; currentSystemIndex < caret->getCurrentScore()->GetSystemCount(); ++currentSystemIndex)
    {
        std::list<unique_ptr<MidiEvent>> eventList;
        generateEventsForSystem(currentSystemIndex, eventList);
        generateMetronome(currentSystemIndex, eventList);

        eventList.sort(CompareEventTimestamps());

        playMidiEvents(eventList, startPos);

        if (startPos > 0)
        {
            startPos = 0;
        }

        if (!isPlaying)
        {
            return;
        }
    }
}

// Generates a list of all notes in the given system, by iterating through each position in each staff of the system
void MidiPlayer::generateEventsForSystem(uint32_t systemIndex, std::list<unique_ptr<MidiEvent> >& eventList) const
{
    shared_ptr<System> system = caret->getCurrentScore()->GetSystem(systemIndex);

    for (quint32 i = 0; i < system->GetStaffCount(); i++)
    {
        Staff* staff = system->GetStaff(i);
        shared_ptr<Guitar> guitar = caret->getCurrentScore()->GetGuitar(i);

        for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
        {
            double startTime = 0; // each note in the staff is given a start time relative to the first note of the staff

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

                    Note* note = position->GetNote(k);

                    // find the pitch of the note
                    const quint32 openStringPitch = guitar->GetTuning().GetNote(note->GetString()) + guitar->GetCapo();
                    quint32 pitch = openStringPitch + note->GetFretNumber();

                    if (note->IsNaturalHarmonic())
                    {
                        pitch = getNaturalHarmonicPitch(openStringPitch, note->GetFretNumber());
                    }

                    // figure out the velocity
                    PlayNoteEvent::VelocityType velocity = PlayNoteEvent::DEFAULT_VELOCITY;
                    if (note->IsMuted())
                    {
                        velocity = PlayNoteEvent::MUTED_VELOCITY;
                    }
                    if (note->IsGhostNote())
                    {
                        velocity = PlayNoteEvent::GHOST_VELOCITY;
                    }

                    // if this note is not tied to the previous note, play the note
                    if (!note->IsTied())
                    {
                        unique_ptr<PlayNoteEvent> noteEvent(new PlayNoteEvent(i, startTime, duration, pitch,
                                                                              positionIndex, systemIndex, guitar,
                                                                              note->IsMuted(), velocity));
                        eventList.push_back(std::move(noteEvent));
                    }

                    // vibrato events
                    if (position->HasVibrato() || position->HasWideVibrato())
                    {
                        VibratoEvent::VibratoType type = position->HasVibrato() ? VibratoEvent::NORMAL_VIBRATO :
                                                                                  VibratoEvent::WIDE_VIBRATO;

                        // add vibrato event, and an event to turn of the vibrato after the note is done
                        eventList.push_back(unique_ptr<VibratoEvent>(new VibratoEvent(i, startTime, positionIndex, systemIndex,
                                                                                      VibratoEvent::VIBRATO_ON, type)));

                        eventList.push_back(unique_ptr<VibratoEvent>(new VibratoEvent(i, startTime + duration, positionIndex,
                                                                                      systemIndex, VibratoEvent::VIBRATO_OFF)));
                    }

                    bool tiedToNextNote = false;
                    // check if this note is tied to the next note
                    {
                        Note* nextNote = staff->GetAdjacentNoteOnString(Staff::NextNote, position, note, voice);
                        if (nextNote && nextNote->IsTied())
                        {
                            tiedToNextNote = true;
                        }
                    }

                    // end the note, unless we are tied to the next note
                    if (!note->HasTieWrap() && !tiedToNextNote)
                    {
                        const double noteLength = position->IsStaccato() ? duration / 2.0 : duration;

                        unique_ptr<StopNoteEvent> stopEvent(new StopNoteEvent(i, startTime + noteLength,
                                                                              positionIndex, systemIndex, pitch));
                        eventList.push_back(std::move(stopEvent));
                    }
                }

                startTime += duration;
            }
        }
    }
}

// The events are already in order of occurrence, so just play them one by one
// startPos is used to identify the starting position to begin playback from
void MidiPlayer::playMidiEvents(std::list<unique_ptr<MidiEvent> >& eventList, quint32 startPos)
{
    uint32_t currentPosition = 0;
    uint32_t currentSystem = 0;

    while (!eventList.empty())
    {
        if (!isPlaying)
        {
            return;
        }

        unique_ptr<MidiEvent> activeEvent = std::move(eventList.front());
        eventList.pop_front();

        const uint32_t eventPosition = activeEvent->getPositionIndex();

        if (eventPosition < startPos)
        {
            continue;
        }

        activeEvent->performEvent(rtMidiWrapper);

        // if we've moved to a new position, move the caret
        if (eventPosition > currentPosition)
        {
            currentPosition = eventPosition;
            emit playbackPositionChanged(currentPosition);
        }

        const uint32_t eventSystemIndex = activeEvent->getSystemIndex();
        if (eventSystemIndex != currentSystem)
        {
            currentSystem = eventSystemIndex;
            emit playbackSystemChanged(currentSystem);
        }

        // add delay between this event and the next one
        if (!eventList.empty())
        {
            const int sleepDuration = eventList.front()->getStartTime() - activeEvent->getStartTime();
            Q_ASSERT(sleepDuration >= 0);

            usleep(1000 * sleepDuration);
        }
        else
        {
            usleep(1000 * activeEvent->getDuration());
        }
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

double MidiPlayer::calculateNoteDuration(Position* currentPosition) const
{
    const double tempo = getCurrentTempo(currentPosition->GetPosition());

    return currentPosition->GetDuration() * tempo;
}

double MidiPlayer::getWholeRestDuration(shared_ptr<System> system, Staff* staff, Position* position, double originalDuration) const
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
void MidiPlayer::initNaturalHarmonics()
{
    naturalHarmonicPitches[3] = 31;
    naturalHarmonicPitches[4] = naturalHarmonicPitches[9] = 28;
    naturalHarmonicPitches[16] = naturalHarmonicPitches[28] = 28;
    naturalHarmonicPitches[5] = naturalHarmonicPitches[24] = 24;
    naturalHarmonicPitches[7] = naturalHarmonicPitches[19] = 19;
    naturalHarmonicPitches[12] = 12;
}

// Add the pitch offset for the natural harmonic to the pitch of the open string
quint8 MidiPlayer::getNaturalHarmonicPitch(const quint8 openStringPitch, const quint8 fret) const
{
    return openStringPitch + naturalHarmonicPitches[fret];
}

// Generates the metronome ticks
void MidiPlayer::generateMetronome(uint32_t systemIndex, std::list<std::unique_ptr<MidiEvent> >& eventList) const
{
    shared_ptr<System> system = caret->getCurrentScore()->GetSystem(systemIndex);

    std::vector<Barline*> barlines;
    system->GetBarlines(barlines);
    barlines.pop_back(); // don't need the end barline

    double startTime = 0;

    for (size_t i = 0; i < barlines.size(); i++)
    {
        Barline* barline = barlines.at(i);
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

            eventList.push_back( unique_ptr<MetronomeEvent>( new MetronomeEvent(METRONOME_CHANNEL, startTime, duration,
                                                                                position, systemIndex, velocity)));

            startTime += duration;

            eventList.push_back( unique_ptr<StopNoteEvent> ( new StopNoteEvent(METRONOME_CHANNEL, startTime, position,
                                                                               systemIndex, MetronomeEvent::METRONOME_PITCH)));
        }
    }
}
