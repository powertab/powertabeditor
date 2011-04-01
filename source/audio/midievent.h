#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <stdint.h>

class RtMidiWrapper;

class MidiEvent
{
public:
    static const uint8_t NUM_CHANNELS = 16;

    MidiEvent(uint8_t channel, double startTime, double duration, uint32_t positionIndex, uint32_t systemIndex);
    virtual ~MidiEvent() {}

    virtual void performEvent(RtMidiWrapper& sequencer) = 0;

    uint32_t getPositionIndex() const;
    uint32_t getSystemIndex() const;
    double getDuration() const;
    double getStartTime() const;

protected:
    uint8_t channel;
    double startTime;
    double duration;
    uint32_t positionIndex; ///< position in the staff that the event occurred at
    uint32_t systemIndex; ///< system that the event occurred in
};

#endif // MIDIEVENT_H
