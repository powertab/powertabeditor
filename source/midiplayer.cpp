#include "midiplayer.h"

#include <QSettings>

#include <list>
#include <algorithm>

#include <painters/caret.h>
#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>

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
    mutex.lock();
    isPlaying = false;
    mutex.unlock();

    wait();
}

void MidiPlayer::run()
{
    isPlaying = true;

    currentSystemIndex = caret->getCurrentSystemIndex();
    int startPos = caret->getCurrentPositionIndex();

    // go through each system, generate a list of the notes (midi events) from each staff
    // then, sort notes by their start time, and play them in order
    for (; currentSystemIndex < caret->getCurrentScore()->GetSystemCount(); ++currentSystemIndex)
    {
        std::list<NoteInfo> noteList;
        generateNotesInSystem(currentSystemIndex, noteList);

        noteList.sort();

        playNotesInSystem(noteList, startPos);

        if (startPos > 0)
        {
            startPos = -1;
        }

        if (!isPlaying)
        {
            return;
        }

        emit playbackSystemChanged();
    }
}

// Generates a list of all notes in the given system, by iterating through each position in each staff of the system
void MidiPlayer::generateNotesInSystem(int systemIndex, std::list<NoteInfo>& noteList) const
{
    System* system = caret->getCurrentScore()->GetSystem(systemIndex);

    for (quint32 i = 0; i < system->GetStaffCount(); i++)
    {
        Staff* staff = system->GetStaff(i);
        Guitar* guitar = caret->getCurrentScore()->GetGuitar(i);

        double startTime = 0; // each note in the staff is given a start time relative to the first note of the staff

        for (quint32 j = 0; j < staff->GetPositionCount(0); j++)
        {
            Position* position = staff->GetPosition(0, j);

            const double duration = calculateNoteDuration(position); // each note at a position has the same duration
            const quint32 positionIndex = position->GetPosition(); // only keep track of position for the first staff

            for (quint32 k = 0; k < position->GetNoteCount(); k++)
            {
                Note* note = position->GetNote(k);

                // find the pitch of the note
                const quint32 openStringPitch = guitar->GetTuning().GetNote(note->GetString()) + guitar->GetCapo();
                quint32 pitch = openStringPitch + note->GetFretNumber();

                // fill in the note info structure
                NoteInfo noteInfo;
                noteInfo.channel = i;
                noteInfo.messageType = position->IsRest() ? REST : PLAY_NOTE;
                noteInfo.pitch = pitch;
                noteInfo.guitar = guitar;
                noteInfo.duration = duration;
                noteInfo.startTime = startTime;
                noteInfo.position = positionIndex;
                noteInfo.isMuted = note->IsMuted();
                noteInfo.velocity = DEFAULT_VELOCITY;

                if (note->IsMuted())
                {
                    noteInfo.velocity = MUTED_VELOCITY;
                }
                if (note->IsGhostNote())
                {
                    noteInfo.velocity = GHOST_VELOCITY;
                }

                if (note->IsNaturalHarmonic())
                {
                    noteInfo.pitch = getNaturalHarmonicPitch(openStringPitch, note->GetFretNumber());
                }

                if (note->IsTied()) // if the note is tied, delete the previous note-off event
                {
                    NoteInfo temp;
                    temp.channel = i;
                    temp.messageType = STOP_NOTE;
                    temp.pitch = pitch;
                    auto prevNoteEnd = std::find(noteList.rbegin(), noteList.rend(), temp);
                    if (prevNoteEnd != noteList.rend())
                    {
                        noteList.erase(--prevNoteEnd.base());
                    }
                }
                else // otherwise, if the note is not tied, add a new note
                {
                    noteList.push_back(noteInfo);
                }

                if (!note->HasTieWrap())
                {
                    // now, add the note-off event, scheduled for after the note's duration has ended
                    noteInfo.startTime += duration;
                    noteInfo.duration = 0;
                    noteInfo.messageType = STOP_NOTE;
                    noteList.push_back(noteInfo);
                }
            }

            if (position->IsRest())
            {
                NoteInfo noteInfo;
                noteInfo.messageType = REST;
                noteInfo.duration = duration;
                noteInfo.position = positionIndex;
                noteInfo.startTime = startTime;

                // for whole rests, they must last for the entire bar, regardless of time signature
                if (position->GetDurationType() == 1)
                {
                    noteInfo.duration = getWholeRestDuration(system, staff, position, noteInfo.duration);
                }

                noteList.push_back(noteInfo);

                startTime += noteInfo.duration;
            }
            else
            {
                startTime += duration;
            }
        }
    }
}

// The notes are already in order of occurrence, so just play them one by one
// startPos is used to identify the starting position to begin playback from
void MidiPlayer::playNotesInSystem(std::list<NoteInfo>& noteList, int startPos)
{
    quint8 currentPosition = 0;

    while (!noteList.empty())
    {
        if (!isPlaying)
        {
            return;
        }

        NoteInfo noteInfo = noteList.front();
        noteList.pop_front();

        if (noteInfo.position < startPos)
        {
            continue;
        }

        if (noteInfo.messageType == PLAY_NOTE)
        {
            // grab the patch/pan/volume immediately before playback to allow for real-time mixing
            if (noteInfo.isMuted)
            {
                rtMidiWrapper.setPatch(noteInfo.channel, midi::MIDI_PRESET_ELECTRIC_GUITAR_MUTED);
            }
            else
            {
                rtMidiWrapper.setPatch(noteInfo.channel, noteInfo.guitar->GetPreset());
            }

            rtMidiWrapper.setPan(noteInfo.channel, noteInfo.guitar->GetPan());
            rtMidiWrapper.setVolume(noteInfo.channel, noteInfo.guitar->GetInitialVolume());

            rtMidiWrapper.playNote(noteInfo.channel, noteInfo.pitch, noteInfo.velocity);
        }
        else if (noteInfo.messageType == STOP_NOTE)
        {
            rtMidiWrapper.stopNote(noteInfo.channel, noteInfo.pitch);
        }

        // if we've moved to a new position, move the caret
        if (noteInfo.position > currentPosition)
        {
            currentPosition = noteInfo.position;
            emit playbackPositionChanged(currentPosition);
        }

        // pause for the required time between notes
        if (!noteList.empty())
        {
            usleep(1000 * (noteList.front().startTime - noteInfo.startTime));
        }
        else
        {
            usleep(1000 * noteInfo.duration);
        }
    }
}

TempoMarker* MidiPlayer::getCurrentTempoMarker() const
{
    Score* currentScore = caret->getCurrentScore();

    TempoMarker* currentTempoMarker = NULL;

    // find the active tempo marker
    for(quint32 i = 0; i < currentScore->GetTempoMarkerCount(); i++)
    {
        TempoMarker* temp = currentScore->GetTempoMarker(i);
        if (temp->GetSystem() <= currentSystemIndex)
        {
            currentTempoMarker = temp;
        }
    }

    return currentTempoMarker;
}

double MidiPlayer::getCurrentTempo() const
{
    TempoMarker* tempoMarker = getCurrentTempoMarker();

    double bpm = TempoMarker::DEFAULT_BEATS_PER_MINUTE; // default tempo in case there is no tempo marker in the score
    double beatType = TempoMarker::DEFAULT_BEAT_TYPE;

    if (tempoMarker != NULL)
    {
        bpm = tempoMarker->GetBeatsPerMinute();
        beatType = tempoMarker->GetBeatType();
    }

    // convert bpm to millisecond duration
    return (60.0 / bpm * 1000.0 * (TempoMarker::quarter / beatType));
}

double MidiPlayer::calculateNoteDuration(Position* currentPosition) const
{
    double tempo = getCurrentTempo();

    double duration = currentPosition->GetDurationType();
    duration = tempo * 4.0 / duration;
    duration += currentPosition->IsDotted() * 0.5 * duration;
    duration += currentPosition->IsDoubleDotted() * 0.75 * duration;

    // adjust for irregular groupings (triplets, etc)
    if (currentPosition->HasIrregularGroupingTiming())
    {
        quint8 notesPlayed = 0;
        quint8 notesPlayedOver = 0;
        currentPosition->GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

        // for example, with triplets we have 3 notes played in the time of 2,
        // so each note is 2/3 of its normal duration
        duration *= (double)notesPlayedOver / (double)notesPlayed;
    }

    return duration;
}

double MidiPlayer::getWholeRestDuration(System* system, Staff* staff, Position* position, double originalDuration) const
{
    Barline* prevBarline = system->GetPrecedingBarline(position->GetPosition());

    // if the whole rest is not the only item in the bar, treat it like a regular rest
    if (!staff->IsOnlyPositionInBar(position, system))
    {
        return originalDuration;
    }

    const TimeSignature& currentTimeSignature = prevBarline->GetTimeSignatureConstRef();

    double tempo = getCurrentTempo();
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
