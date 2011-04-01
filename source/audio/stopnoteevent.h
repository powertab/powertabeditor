#ifndef STOPNOTEEVENT_H
#define STOPNOTEEVENT_H

#include "midievent.h"

class StopNoteEvent : public MidiEvent
{
public:
    StopNoteEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex, uint8_t pitch);

    void performEvent(RtMidiWrapper& sequencer) const;

protected:
    uint8_t pitch;
};

#endif // STOPNOTEEVENT_H
