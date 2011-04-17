#ifndef LETRINGEVENT_H
#define LETRINGEVENT_H

#include "midievent.h"

class LetRingEvent : public MidiEvent
{
public:
    enum EventType
    {
        LET_RING_ON,
        LET_RING_OFF
    };

    LetRingEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                 EventType eventType);
    
    void performEvent(RtMidiWrapper& sequencer) const;
    
private:
    const EventType eventType;
};

#endif // LETRINGEVENT_H
