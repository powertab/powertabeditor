#include "stopnoteevent.h"

#include <audio/rtmidiwrapper.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

StopNoteEvent::StopNoteEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                             uint32_t systemIndex, uint8_t pitch) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    pitch(pitch)
{
}

void StopNoteEvent::performEvent(RtMidiWrapper& sequencer) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Stop Note: " << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    sequencer.stopNote(channel, pitch);
}
