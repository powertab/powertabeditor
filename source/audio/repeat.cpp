#include "repeat.h"

Repeat::Repeat(const SystemLocation& startBarLocation) :
    startBarLocation(startBarLocation),
    activeRepeat(1)
{
}

Repeat::Repeat():
    activeRepeat(1)
{
}

/// Adds a new end bar to the repeat
void Repeat::addRepeatEnd(const SystemLocation& location, const RepeatEnd& endBar)
{
    endBars[location] = endBar;
}

/// Performs a repeat event if possible
/// @return The playback position to shift to
SystemLocation Repeat::performRepeat(const SystemLocation& currentLocation)
{
    auto repeatEnd = endBars.find(currentLocation);

    if (repeatEnd == endBars.end()) // no repeat end bar at the current location
    {
        return currentLocation;
    }
    else if (repeatEnd->second.performRepeat()) // repeat event performed
    {
        activeRepeat++;
        return startBarLocation;
    }
    else // repeat end bar exists, but no repeat event was performed
    {
        activeRepeat = 1; // reset
        return currentLocation;
    }
}

RepeatEnd::RepeatEnd() :
    repeatCount(0),
    remainingRepeats(0)
{
}

RepeatEnd::RepeatEnd(uint8_t repeatCount) :
    repeatCount(repeatCount),
    remainingRepeats(repeatCount - 1)
{
}

bool RepeatEnd::performRepeat()
{
    if (remainingRepeats != 0)
    {
        remainingRepeats--;
        return true;
    }
    else
    {
        remainingRepeats = repeatCount - 1;
        return false;
    }
}
