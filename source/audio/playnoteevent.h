#ifndef PLAYNOTEEVENT_H
#define PLAYNOTEEVENT_H

#include "midievent.h"

#include <memory>

class Guitar;

class PlayNoteEvent : public MidiEvent
{
public:
    enum VelocityType
    {
        DEFAULT_VELOCITY = 127,
        MUTED_VELOCITY = 60,
        GHOST_VELOCITY = 50,
        PALM_MUTED_VELOCITY = 112
    };

    PlayNoteEvent(uint8_t channel, double startTime, double duration, uint8_t pitch, uint32_t positionIndex,
                  uint32_t systemIndex, std::shared_ptr<const Guitar> guitar, bool isMuted, VelocityType velocity);

    void performEvent(RtMidiWrapper& sequencer) const;

protected:
    uint8_t pitch;
    std::shared_ptr<const Guitar> guitar;
    bool isMuted;
    VelocityType velocity;
};

#endif // PLAYNOTEEVENT_H
