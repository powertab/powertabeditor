#include "volumechangeevent.h"

#include "rtmidiwrapper.h"

VolumeChangeEvent::VolumeChangeEvent(uint8_t channel, double startTime,
                                     uint32_t positionIndex, uint32_t systemIndex,
                                     uint8_t newVolume) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    newVolume(newVolume)
{
}

void VolumeChangeEvent::performEvent(RtMidiWrapper& sequencer) const
{
    sequencer.setVolume(channel, newVolume);
}
