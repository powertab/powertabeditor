#include "bendevent.h"

#include <rtmidiwrapper.h>

BendEvent::BendEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                     uint32_t systemIndex, uint8_t bendAmount) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    bendAmount(bendAmount)
{
}

void BendEvent::performEvent(RtMidiWrapper& sequencer) const
{
    sequencer.setPitchBend(channel, bendAmount);
}
