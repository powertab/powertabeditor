#include "midiplayer.h"

#include <QSettings>
#include <QTimer>
#include <QSignalMapper>

#include "painters/caret.h"
#include "powertabdocument/score.h"
#include "powertabdocument/guitar.h"

MidiPlayer::MidiPlayer(Caret* caret)
{
    this->caret = caret;

    QSettings settings;
    rtMidiWrapper.initialize(settings.value("midi/preferredPort", 0).toInt());

    signalMapper = new QSignalMapper();

    isPlaying = false;

    songTimers.resize(8);
    for (int i = 0; i < songTimers.size(); ++i)
    {
        songTimers[i] = new QTimer;
        connect(songTimers.at(i),SIGNAL(timeout()),signalMapper,SLOT(map()));
        signalMapper->setMapping(songTimers.at(i), i);
    }
    connect(signalMapper, SIGNAL(mapped(const int)), this, SLOT(playbackSong(int)), Qt::DirectConnection);
}

MidiPlayer::~MidiPlayer()
{
    delete signalMapper;
    for (int i = 0; i < songTimers.size(); i++)
    {
        delete songTimers.at(i);
    }
}

void MidiPlayer::play()
{
    previousPositionInStaff.clear();

    for (quint32 i = 0; i < caret->getCurrentSystem()->GetStaffCount(); ++i)
    {
        playNotesAtCurrentPosition(i);
    }
}

void MidiPlayer::stop()
{
    for (int i = 0; i < songTimers.size(); i++)
    {
        songTimers.at(i)->stop();
    }

    QMultiMap<unsigned int, NoteHistory>::iterator i = oldNotes.begin();
    for (; i != oldNotes.end(); ++i)
    {
        rtMidiWrapper.stopNote(i.key(), i.value().pitch);
    }
    oldNotes.clear();
}

void MidiPlayer::playNotesAtCurrentPosition(int staff)
{
    quint32 position_index = previousPositionInStaff.value(staff, -1) + 1; // get position
    previousPositionInStaff.insert(staff, position_index); // update

    // retrieve the current position
    //qDebug() << "Staff: " << staff << " Position " << position_index;
    Position* position = caret->getCurrentSystem()->GetStaff(staff)->GetPosition(0,position_index);

    if (!position) // check for invalid position (caused by invalid position index)
    {
        return;
    }
    // stop all previous notes except for notes that are to be tied
    {
        QList<unsigned int> notesToBeKept;
        notesToBeKept.reserve(position->GetNoteCount());
        for (unsigned int i = 0; i < position->GetNoteCount(); ++i)
        {
            if (position->GetNote(i)->IsTied())
            {
                notesToBeKept.push_back(position->GetNote(i)->GetString());
            }
        }
        QMutableMapIterator<unsigned int, NoteHistory> i(oldNotes);
        while(i.hasNext())
        {
            i.next();
            if (i.key() == (unsigned int)staff && !notesToBeKept.contains(i.value().stringNum))
            {
                songTimers.at(staff)->stop();
                rtMidiWrapper.stopNote(staff, i.value().pitch);
                i.remove();
            }
        }
    }

    if (!position->IsRest())
    {
        for (unsigned int i = 0; i < position->GetNoteCount(); ++i)
        // loop through all notes in the current position on the current staff
        {
            if (!position->GetNote(i)->IsTied())
            {
                Guitar* guitar = caret->getCurrentScore()->GetGuitar(staff);
                Note* note = position->GetNote(i);

                unsigned int pitch = guitar->GetTuning().GetNote(note->GetString()) + guitar->GetCapo();
                pitch += note->GetFretNumber();

                rtMidiWrapper.setVolume(staff,guitar->GetInitialVolume());
                rtMidiWrapper.setPan(staff,guitar->GetPan());
                rtMidiWrapper.setPatch(staff,guitar->GetPreset());


                rtMidiWrapper.playNote(staff, pitch ,127);
                NoteHistory noteHistory = {pitch, note->GetString()};
                oldNotes.insertMulti(staff, noteHistory );
            }
        }
    }

    double tempo = getCurrentTempo();

    double duration = position->GetDurationType();
    duration = tempo * 4.0 / duration;
    duration += position->IsDotted() * 0.5 * duration;
    duration += position->IsDoubleDotted() * 0.75 * duration;
    songTimers.at(staff)->start(duration);
}

void MidiPlayer::playbackSong(int staff)
{
    // make sure all staves are finished before switching to the next system
    bool okayToSwitchStaves = true;
    // the caret should move along with the staff that is furthest along in playback
    quint32 staffWithMaxPosition = 0;

    QMap<quint32, quint32>::const_iterator i = previousPositionInStaff.constBegin();
    while (i != previousPositionInStaff.constEnd())
    {
        Staff* staff = caret->getCurrentSystem()->GetStaff(i.key());
        if (i.value() < staff->GetPositionCount(0)) // check if a staff is not finished
        {
            okayToSwitchStaves = false;
        }
        if (previousPositionInStaff.value(staffWithMaxPosition) <= i.value())
        {
            staffWithMaxPosition = i.key();
        }
        ++i;
    }

    // only advance the caret if we are in the staff that is furthest along
    if(previousPositionInStaff.value(staff) == previousPositionInStaff.value(staffWithMaxPosition))
    {
        if (!caret->moveCaretHorizontal(1))
        {
            if (!caret->moveCaretSection(1) && okayToSwitchStaves)
            {
                stop();
                return;
            }
            else
            {
                previousPositionInStaff.clear();
                // once we move to a new system, start playing all each of the staves for that system
                for (unsigned int j = 0; j < caret->getCurrentSystem()->GetStaffCount(); ++j)
                {
                    playNotesAtCurrentPosition(j);
                }
                return;
            }
        }
    }

    // play next note for this staff
    playNotesAtCurrentPosition(staff);
}

double MidiPlayer::getCurrentTempo()
{
    quint32 currentSystemIndex = caret->getSystemIndex();
    Score* currentScore = caret->getCurrentScore();
    double bpm = 0;

    for(quint32 i = 0; i < currentScore->GetTempoMarkerCount(); i++)
    {
        TempoMarker* tempoMarker = currentScore->GetTempoMarker(i);
        if (tempoMarker->GetSystem() <= currentSystemIndex)
        {
            bpm = tempoMarker->GetBeatsPerMinute();
        }
    }

    // convert bpm to millisecond duration
    return (60.0 / bpm * 1000.0);
}
