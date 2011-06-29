#include "midievent.h"

#include <boost/test/floating_point_comparison.hpp>

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

/// Compares by timestamp, then by system index, then by position index.
bool MidiEvent::operator<(const MidiEvent& event) const
{
    using namespace boost::test_tools;

    // compare timestamps using a floating point comparison
    if (check_is_close(startTime, event.startTime, fpc::percent_tolerance(0.001)))
    {
        if (systemIndex == event.systemIndex)
        {
            return positionIndex < event.positionIndex;
        }
        else
        {
            return systemIndex < event.systemIndex;
        }
    }
    else
    {
        return startTime < event.startTime;
    }
}
