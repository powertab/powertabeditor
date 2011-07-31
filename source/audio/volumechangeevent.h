#ifndef VOLUMECHANGEEVENT_H
#define VOLUMECHANGEEVENT_H

#include "midievent.h"

class VolumeChangeEvent : public MidiEvent
{
public:
    VolumeChangeEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                      uint32_t systemIndex, uint8_t newVolume);

    void performEvent(RtMidiWrapper&) const;

private:
    const uint8_t newVolume;
};

#endif // VOLUMECHANGEEVENT_H
