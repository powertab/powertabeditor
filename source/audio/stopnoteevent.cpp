#include "stopnoteevent.h"

#include <rtmidiwrapper.h>
#include <QDebug>

StopNoteEvent::StopNoteEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                             uint32_t systemIndex, uint8_t pitch) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    pitch(pitch)
{
}

void StopNoteEvent::performEvent(RtMidiWrapper& sequencer) const
{
    qDebug() << "Stop Note: " << systemIndex << ", " << positionIndex << " at " << startTime;

    sequencer.stopNote(channel, pitch);
}
