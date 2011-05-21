#ifndef BENDEVENT_H
#define BENDEVENT_H

#include "midievent.h"

class Note;

class BendEvent : public MidiEvent
{
public:
    BendEvent(uint8_t channel, double startTime, uint32_t positionIndex,
              uint32_t systemIndex, uint8_t bendAmount);

    void performEvent(RtMidiWrapper& sequencer) const;

    static const uint8_t PITCH_BEND_RANGE;
    static const uint8_t DEFAULT_BEND;
    static const double BEND_QUARTER_TONE;

private:
    const uint8_t bendAmount;
};

#endif // BENDEVENT_H
