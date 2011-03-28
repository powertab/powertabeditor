#include "stopnoteevent.h"

#include <rtmidiwrapper.h>

StopNoteEvent::StopNoteEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint8_t pitch) :
    MidiEvent(channel, startTime, 0, positionIndex),
    pitch(pitch)
{
}

void StopNoteEvent::performEvent(RtMidiWrapper& sequencer)
{
    sequencer.stopNote(channel, pitch);
}
