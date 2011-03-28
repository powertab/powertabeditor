#ifndef VIBRATOEVENT_H
#define VIBRATOEVENT_H

#include "midievent.h"

class VibratoEvent : public MidiEvent
{
public:
    enum VibratoType
    {
        NORMAL_VIBRATO,
        WIDE_VIBRATO
    };

    enum EventType
    {
        VIBRATO_ON,
        VIBRATO_OFF
    };

    VibratoEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                 EventType eventType, VibratoType vibratoType = NORMAL_VIBRATO);

    void performEvent(RtMidiWrapper& sequencer);

protected:
    EventType eventType;
    VibratoType vibratoType;
};

#endif // VIBRATOEVENT_H
