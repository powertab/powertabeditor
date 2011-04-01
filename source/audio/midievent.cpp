#include "midievent.h"

MidiEvent::MidiEvent(uint8_t channel, double startTime, double duration,
                     uint32_t positionIndex, uint32_t systemIndex) :
    channel(channel),
    startTime(startTime),
    duration(duration),
    positionIndex(positionIndex),
    systemIndex(systemIndex)
{
}

uint32_t MidiEvent::getPositionIndex() const
{
    return positionIndex;
}

uint32_t MidiEvent::getSystemIndex() const
{
    return systemIndex;
}

double MidiEvent::getStartTime() const
{
    return startTime;
}

double MidiEvent::getDuration() const
{
    return duration;
}
