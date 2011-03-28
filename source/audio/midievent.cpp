#include "midievent.h"

MidiEvent::MidiEvent(uint8_t channel, double startTime, double duration, uint32_t positionIndex) :
    channel(channel),
    startTime(startTime),
    duration(duration),
    positionIndex(positionIndex)
{
}

uint32_t MidiEvent::getPositionIndex() const
{
    return positionIndex;
}

double MidiEvent::getStartTime() const
{
    return startTime;
}

double MidiEvent::getDuration() const
{
    return duration;
}
