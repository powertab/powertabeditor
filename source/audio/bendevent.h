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

    static const uint8_t DEFAULT_BEND = 64;
    static const double BEND_QUARTER_TONE = 2.625; // Assuming a 12-step pitch bend range

private:
    const uint8_t bendAmount;
};

#endif // BENDEVENT_H
