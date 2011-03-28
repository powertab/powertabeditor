#ifndef METRONOMEEVENT_H
#define METRONOMEEVENT_H

#include "midievent.h"

class MetronomeEvent : public MidiEvent
{
public:
    enum VelocityType
    {
        STRONG_ACCENT = 127,
        WEAK_ACCENT = 80,
        METRONOME_OFF = 0
    };

    static const uint8_t METRONOME_PITCH;

    MetronomeEvent(uint8_t channel, double startTime,
                   double duration, uint32_t positionIndex, VelocityType velocity);

    void performEvent(RtMidiWrapper& sequencer);

protected:
    VelocityType velocity;
};

#endif // METRONOMEEVENT_H
